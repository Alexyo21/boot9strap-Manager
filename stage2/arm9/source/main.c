/*
*   main.c
*/

#include <stdio.h>
#include "types.h"
#include "memory.h"
#include "crypto.h"
#include "i2c.h"
#include "fs.h"
#include "firm.h"
#include "utils.h"
#include "buttons.h"
#include "../build/bundled.h"
#include "hid.h"

u8 *top_screen, *bottom_screen;

static void (*const itcmStub)(Firm *firm, bool isNand) = (void (*const)(Firm *, bool))0x01FF8000;
static volatile Arm11Operation *operation = (volatile Arm11Operation *)0x1FF80204;

static void invokeArm11Function(Arm11Operation op)
{
    while(*operation != ARM11_READY);
    *operation = op;
    while(*operation != ARM11_READY); 
}

static void loadFirm(bool isNand, bool bootOnce, bool bootfirm, u32 index)
{
    Firm *firmHeader = (Firm *)0x080A0000;
    const char *firmName = bootOnce ? "bootonce.firm" : "boot.firm";

    if(fileRead(firmHeader, firmName, 0x200, 0) != 0x200) return;

    bool isPreLockout = ((firmHeader->reserved2[0] & 2) != 0),
         isScreenInit = ((firmHeader->reserved2[0] & 1) != 0);

    Firm *firm;
    u32 maxFirmSize;

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
	
	if(bootfirm)
	{
		char path[125];
		snprintf(path, 125, "BS9/%s", tab[index]);
		fileRead(firm, path, 0, maxFirmSize);
		
	}else{
		if(fileRead(firm, firmName, 0, maxFirmSize) < calculatedFirmSize || !checkSectionHashes(firm)) mcuPowerOff();
		if(bootOnce) fileDelete(firmName);
    }
	
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

//Function Boot9Strap Manager
static void BS9Manager(void)
{
	
	//screenInit arm11
	invokeArm11Function(INIT_SCREENS);
	i2cWriteRegister(I2C_DEV_MCU, 0x22, 0x2A);
	top_screen = (u8 *)0x18300000;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ;
	bottom_screen = (u8 *)0x18346500;
	
	
	//menu
	u32 count = GetDirList("/BS9");//opendir list firm folder "sd:/BS9"
	u32 index = 0;
	while (true) 
	{
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 200 - ((25 * 8) / 2), 10, true, "Boot9Strap Manager v1.2.1");//Header
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 262, 230, true, "Power: Power off");
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 230, true, "A: Boot Payload");
		for (u32 i = 0; i < count; i++) 
		{
			if(i != index)
			{
				DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 200 - ((countnamefirm[i] * 8) / 2), 50 + (i*13 + 2), true, "%s", tab[i]);
			}
			if(i == index)
			{
				DrawStringFColor(COLOR_SELECT, COLOR_BLACK, 200 - ((countnamefirm[i] * 8) / 2), 50 + (i*13 + 2), true, "%s", tab[i]);
			}
		}
		u32 pad_state = InputWait();
				
		if (pad_state & BUTTON_A) {
			loadFirm(true, false, true, index);
		} else if (pad_state & BUTTON_DOWN) {
			index = (index == count - 1) ? 0 : index + 1;	
			} else if (pad_state & BUTTON_UP) {
			index = (index == 0) ? count - 1 : index - 1;	
		} else if (pad_state & BUTTON_POWER) {
			mcuPowerOff();	
		}
		
	}

	
}

void main(void)
{
    setupKeyslots();

    if(mountSd())
    {
        /* I believe this is the canonical secret key combination. */
        if(HID_PAD == NTRBOOT_BUTTONS)
        {
            fileWrite((void *)0x08080000, "boot9strap/boot9.bin", 0x10000);
            fileWrite((void *)0x08090000, "boot9strap/boot11.bin", 0x10000);
            fileWrite((void *)0x10012000, "boot9strap/otp.bin", 0x100);

            /* Wait until buttons are not held, for compatibility. */
            while(HID_PAD & NTRBOOT_BUTTONS);
            wait(1000ULL);
        }
		u32 index = 0;
		if(HID_PAD & BUTTON_L1)BS9Manager();
        loadFirm(false, true, false, index);
        loadFirm(false, false, false, index);
        unmountSd();
    }

    if(mountCtrNand())
    {
        /* Wait until buttons are not held, for compatibility. */
        if(HID_PAD == NTRBOOT_BUTTONS)
        {
            while(HID_PAD & NTRBOOT_BUTTONS);
            wait(1000ULL);
        }
		u32 index = 0;
		if(HID_PAD & BUTTON_L1)BS9Manager();
        loadFirm(true, false, false, index);
    }

    mcuPowerOff();
}
