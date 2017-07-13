#include <stdio.h>

#include "manager.h"
#include "hid.h"
#include "draw.h"
#include "utils.h"
#include "firm.h"
#include "fs.h"
#include "i2c.h"

#include "fatfs/sdmmc/sdmmc.h"
#include "fatfs/ff.h"
#include "memory.h"
#include "crypto/sha.h"


//#include "buttonimage.h"

u8 *top_screen, *bottom_screen;


//Function Boot9Strap Manager
void BS9Manager(bool NANDorSD)
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
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 200 - ((25 * 8) / 2), 10, true, "Boot9Strap Manager v1.2.2");//Header
		
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 210, true, "Y: Dump SysNand");
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 220, true, "X: Dump Boot9/11 and OTP         ");
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 230, true, "A: Boot Payload");
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 262, 230, true, "Power: Power off");
		for (u32 i = 0; i < count; i++) 
		{
			if(i != index)
			{
				//draw button image
				//drawimage(button, 80, 50 + (i*13),240, 13);
				//DrawStringFColor(COLOR_WHITE, COLOR_TRANSPARENT, 200 - ((countnamefirm[i] * 8) / 2), 50 + (i*13 + 2), true, "%s", tab[i]);
				DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 200 - ((countnamefirm[i] * 8) / 2), 50 + (i*13 + 2), true, "%s", tab[i]);
				
			}
			if(i == index)
			{
				//draw button image
				//drawimage(button, 80, 50 + (i*13),240, 13);
				//DrawStringFColor(COLOR_SELECT, COLOR_TRANSPARENT, 200 - ((countnamefirm[i] * 8) / 2), 50 + (i*13 + 2), true, "%s", tab[i]);
				
				DrawStringFColor(COLOR_SELECT, COLOR_BLACK, 200 - ((countnamefirm[i] * 8) / 2), 50 + (i*13 + 2), true, "%s", tab[i]);
				
			}
		}
		u32 pad_state = InputWait();
			
		if (pad_state & BUTTON_A) {
			
			loadFirm(NANDorSD, true, index);
			
		} else if (pad_state & BUTTON_DOWN) {
			index = (index == count - 1) ? 0 : index + 1;	
			} else if (pad_state & BUTTON_UP) {
			index = (index == 0) ? count - 1 : index - 1;	
		} else if (pad_state & BUTTON_X) {
			DumpBoot9_11_OTP(NANDorSD);	
		} else if (pad_state & BUTTON_Y) {
			DumpNand();	
		} else if (pad_state & BUTTON_POWER) {
			mcuPowerOff();	
		}
		
	}

	
}

void DumpBoot9_11_OTP(bool NANDorSD)
{
	if(!NANDorSD)
	{
		
		fileWrite((void *)0x08080000, "boot9strap/boot9.bin", 0x10000);
		fileWrite((void *)0x08090000, "boot9strap/boot11.bin", 0x10000);
		fileWrite((void *)0x10012000, "boot9strap/otp.bin", 0x100);
		DrawStringFColor(COLOR_GREEN, COLOR_BLACK, 10, 220, true, "X: Dump Boot9/11 and OTP Succes !");
	}
    /* Wait until buttons are not held, for compatibility. */
    while(HID_PAD & NTRBOOT_BUTTONS);
    wait(1000ULL);
	
	
}


#define BUFFER_ADDRESS  ((u8*) 0x18000000)
#define BUFFER_MAX_SIZE ((u32) (1 * 1024 * 1024))
#define NAND_SECTOR_SIZE 0x200
#define SECTORS_PER_READ (BUFFER_MAX_SIZE / NAND_SECTOR_SIZE)
static FIL file;


u32 DumpNand(void)
{
	ClearScreenF(true, true, COLOR_BLACK);
	char path[64];
	snprintf(path, 64, "SYSNAND.bin");
	u8* buffer = BUFFER_ADDRESS;
	u32 result = 0;
	u32 nand_size = getMMCDevice(0)->total_size * NAND_SECTOR_SIZE;
   
	u32 button;
	if(DebugOpenFile(path))
	{
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 10, true, "The file <%s> exists",path);
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 20, true, "Press <A> to replace it");
		button = InputWait();
		
		if(button & BUTTON_A){
			if(!ShowUnlockSequence(1))return 1;
		} else {
			result = 4;
		}
	}
	
	if(result == 0)
	{
		//open file to write
		if (f_open(&file, path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS) != 0)
			return -1;
		f_lseek(&file, 0);
		f_sync(&file);
		
		
		sha_init(SHA_256_MODE);
		u64 n_sectors = nand_size / NAND_SECTOR_SIZE;
		
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 10, true, "Dump %s: %d.MB", path, nand_size / (1024 * 1024));
		DrawStringFColor(COLOR_GREYGREEN, COLOR_BLACK, 10, 40, true, "Press <B> to cancel operation");
		
		for (u64 i = 0; i < n_sectors; i += SECTORS_PER_READ) {
			
			u32 read_sectors = min(SECTORS_PER_READ, (n_sectors - i));
			
			//progress
			DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 20, true, "Dump : %3llu%%", (i * 100) / n_sectors);
			
			
			//read sectors nand 1mb
			if (sdmmc_nand_readsectors(i, read_sectors, buffer) != 0)  {   
				result = 1;
				break;
			}
			
			//Write the nand sectors to the file
			UINT bytes_written = 0;
			f_lseek(&file, i * NAND_SECTOR_SIZE);
			if (f_write(&file, buffer, NAND_SECTOR_SIZE * read_sectors, &bytes_written) != 0) { 
				result = 2;
				break;
			}
			f_sync(&file);
			sha_update(buffer, NAND_SECTOR_SIZE * read_sectors);
			
			
			if (HID_PAD & BUTTON_B)
			{
				DrawStringFColor(COLOR_GREYGREEN, COLOR_BLACK, 10, 60, true, "Press <A> to cancel operation");
				if (InputWait() & BUTTON_A) {
					result = 3;
					break;
					
				} else {
					DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 60, true, "Continuing operation...      ");
				}
			}
			
		}
		f_close(&file);
	}
	
	if (result == 0)
	{
		char hashname[64];
        u8 shasum[32];
        sha_get(shasum);
		DrawStringFColor(COLOR_GREEN, COLOR_BLACK, 10, 70, true, "Dump Nand Succes !");
		
        DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 80, true, "NAND dump SHA256: %08X...", getbe32(shasum));
		
		snprintf(hashname, 64, "%s.sha", path);
		u32 ret = fileWrite(shasum, hashname, 32);
		DrawStringFColor((ret == 0) ? COLOR_RED : COLOR_GREEN, COLOR_BLACK, 10, 90, true, 
			"Store to %s: %s", hashname, (ret == 0) ? "failed": "ok");
		
	}
	if (result == 1)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 70, true, "Read sectors nand failed ! !");
	if (result == 2)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 70, true, "sector write to the file failed  !");
	if (result == 3 || result == 4)
		DrawStringFColor(COLOR_ORANGE, COLOR_BLACK, 10, (result == 3) ? 70 : 40, true, "(cancelled by user)");
	
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 110, true, "Press any key to return to manager");
	InputWait();
	ClearScreenF(true, true, COLOR_BLACK);
    
	return result;
}

	
