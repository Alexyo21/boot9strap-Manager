/*
*   This file is part of Luma3DS
*   Copyright (C) 2016 Aurora Wright, TuxSH
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b of GPLv3 applies to this file: Requiring preservation of specified
*   reasonable legal notices or author attributions in that material or in the Appropriate Legal
*   Notices displayed by works containing it.
*/

#include <stdio.h>


#include "utils.h"
#include "i2c.h"
#include "fs.h"
#include "types.h"
#include "../build/bundled.h"

#include "firm.h"
#include "memory.h"
#include "crypto/crypto.h"
#include "crypto/sha.h"


static void (*const itcmStub)(Firm *firm, bool isNand) = (void (*const)(Firm *, bool))0x01FF8000;
static volatile Arm11Operation *operation = (volatile Arm11Operation *)0x1FF80204;

void invokeArm11Function(Arm11Operation op)
{
    while(*operation != ARM11_READY);
    *operation = op;
    while(*operation != ARM11_READY); 
}
//====================Load Firm ================================
void loadFirm(bool isNand, bool bootfirm, u32 index)
{
    Firm *firm;
	Firm *firmHeader = (Firm *)0x080A0000;
    u32 maxFirmSize;
	
	char firmname[64];
	
	if(bootfirm)
		snprintf(firmname, 64,"luma/payloads/%s", tab[index]);
	else 
		snprintf(firmname, 64,"boot.firm");
	
	if(fileRead(firmHeader, firmname, 0x200, 0) != 0x200) return;
	
    bool isPreLockout = ((firmHeader->reserved2[0] & 2) != 0),
         isScreenInit = ((firmHeader->reserved2[0] & 1) != 0);
	
    if(!isPreLockout)
    {
        //Lockout
        while(!(CFG9_SYSPROT9  & 1)) CFG9_SYSPROT9  |= 1;
        while(!(CFG9_SYSPROT11 & 1)) CFG9_SYSPROT11 |= 1;
        invokeArm11Function(WAIT_BOOTROM11_LOCKED);
		
        firm = (Firm *)0x20001000;
        maxFirmSize = 0x07FFF000; //around 127MB (although we don't enable ext FCRAM on N3DS, beware!)
    }
    else
    {
        //Uncached area, shouldn't affect performance too much, though
        firm = (Firm *)0x18000000;
        maxFirmSize = 0x300000; //3MB
    }

    u32 calculatedFirmSize = checkFirmHeader(firmHeader, (u32)firm, isPreLockout);

    if(!calculatedFirmSize) mcuPowerOff();
	
	if(fileRead(firm, firmname, 0, maxFirmSize) < calculatedFirmSize || !checkSectionHashes(firm)) mcuPowerOff();
		
	
	//screenInit arm11
    if(isScreenInit)
    {
        invokeArm11Function(INIT_SCREENS);
        i2cWriteRegister(I2C_DEV_MCU, 0x22, 0x2A); //Turn on backlight
    }

    memcpy((void *)itcmStub, itcm_stub_bin, itcm_stub_bin_size);

    //Launch firm
    invokeArm11Function(PREPARE_ARM11_FOR_FIRMLAUNCH);
    itcmStub(firm, isNand);
	
}
//============================================================================================
static __attribute__((noinline)) bool overlaps(u32 as, u32 ae, u32 bs, u32 be)
{
    if(as <= bs && bs <= ae)
        return true;
    if(bs <= as && as <= be)
        return true;
    return false;
}

static __attribute__((noinline)) bool inRange(u32 as, u32 ae, u32 bs, u32 be)
{
   if(as >= bs && ae <= be)
        return true;
   return false;
}

u32 checkFirmHeader(Firm *firmHeader, u32 firmBufferAddr, bool isPreLockout)
{
    if(memcmp(firmHeader->magic, "FIRM", 4) != 0 || firmHeader->arm9Entry == NULL) //Allow for the ARM11 entrypoint to be zero in which case nothing is done on the ARM11 side
        return 0;

    bool arm9EpFound = false,
         arm11EpFound = false;

    u32 size = 0x200;
    for(u32 i = 0; i < 4; i++)		
        size += firmHeader->section[i].size;

    for(u32 i = 0; i < 4; i++)
    {
        FirmSection *section = &firmHeader->section[i];

        //Allow empty sections
        if(section->size == 0)
            continue;

        if((section->offset < 0x200) ||
           (section->address + section->size < section->address) || //Overflow check
           ((u32)section->address & 3) || (section->offset & 0x1FF) || (section->size & 0x1FF) || //Alignment check
           (overlaps((u32)section->address, (u32)section->address + section->size, firmBufferAddr, firmBufferAddr + size)) ||
           ((!inRange((u32)section->address, (u32)section->address + section->size, 0x08000000, 0x08000000 + 0x00100000)) &&
            (!inRange((u32)section->address, (u32)section->address + section->size, 0x18000000, 0x18000000 + 0x00600000)) &&
            (!inRange((u32)section->address, (u32)section->address + section->size, 0x1FF00000, 0x1FFFFC00)) &&
            (!(!isPreLockout && inRange((u32)section->address, (u32)section->address + section->size, 0x20000000, 0x20000000 + 0x8000000)))))
            return 0;

        if(firmHeader->arm9Entry >= section->address && firmHeader->arm9Entry < (section->address + section->size))
            arm9EpFound = true;

        if(firmHeader->arm11Entry >= section->address && firmHeader->arm11Entry < (section->address + section->size))
            arm11EpFound = true;
    }

    return (arm9EpFound && (firmHeader->arm11Entry == NULL || arm11EpFound)) ? size : 0;
}

bool checkSectionHashes(Firm *firm)
{
    for(u32 i = 0; i < 4; i++)
    {
        FirmSection *section = &firm->section[i];

        if(section->size == 0)
            continue;

        __attribute__((aligned(4))) u8 hash[0x20];

        sha(hash, (u8 *)firm + section->offset, section->size, SHA_256_MODE);

        if(memcmp(hash, section->hash, 0x20) != 0)
            return false;
    }

    return true;
}
