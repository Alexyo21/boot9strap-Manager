#include <stdio.h>

#include "manager.h"
#include "hid.h"
#include "draw.h"
#include "utils.h"
#include "firm.h"
#include "fs.h"

#include "fatfs/sdmmc/sdmmc.h"
#include "fatfs/ff.h"
#include "memory.h"
#include "crypto/sha.h"
#include "crypto/hashfile.h"

#include "bmp/bitmap.h"


//Function Boot9Strap Manager
void BS9Manager(bool NANDorSD)
{
	
	InitScreen();
	ClearScreenF(true, false, COLOR_BLACK);
	
	
	BMP_Header* bmp = (BMP_Header*)0x18100000;
	u32 no_bmp = 0;
	
	//opendir list firm folder "sd:/BS9"
	u32 count = GetDirList("/BS9");
	
	
	u32 index = 0;
	u32 pos = 0;
	
	bool refresh_page = false;
	bool refresh_b9sm = true;
	
	//count mumber Page
	u32 page = 0;
	u32 c_page = 0;
	if(count < 40)c_page = 3;
	if(count < 30)c_page = 2;
	if(count < 20)c_page = 1;
	if(count < 10)c_page = 0;
	
	//mount memory bmp image
	if(Read_BMP(&bmp[BMP_BG_TOP], "/img/bg_top.bmp") != 0)no_bmp |= NO_BG_TOP|NO_CURSER_L|NO_CURSER_R;
	if(Read_BMP(&bmp[BMP_BG_BOT], "/img/bg_bot.bmp") != 0)no_bmp |= NO_BG_BOT;
	if(Read_BMP(&bmp[BMP_CURSER_L], "/img/curser_L.bmp") != 0)no_bmp |= NO_CURSER_L;
	if(Read_BMP(&bmp[BMP_CURSER_R], "/img/curser_R.bmp") != 0)no_bmp |= NO_CURSER_R;
	if(Read_BMP(&bmp[BMP_BUTTON_A], "/img/button_A.bmp") != 0)no_bmp |= NO_BUTTON_A;
	if(Read_BMP(&bmp[BMP_BUTTON_B], "/img/button_B.bmp") != 0)no_bmp |= NO_BUTTON_B;
	if(Read_BMP(&bmp[BMP_BUTTON_X], "/img/button_X.bmp") != 0)no_bmp |= NO_BUTTON_X;
	if(Read_BMP(&bmp[BMP_BUTTON_Y], "/img/button_Y.bmp") != 0)no_bmp |= NO_BUTTON_Y;
	if(Read_BMP(&bmp[BMP_BUTTON_DIR], "/img/button_D.bmp") != 0)no_bmp |= NO_BUTTON_DIR;
	
	
	
	
	
	while (true) 
	{
		
		if(refresh_b9sm)
		{
			
			bmp[BMP_BG_TOP].height =  240;
			bmp[BMP_BG_TOP].width = 400;
			if(!(no_bmp & NO_BG_TOP))Draw_BMP(&bmp[BMP_BG_TOP], TOP, 0, 0);
			if(!(no_bmp & NO_BG_BOT))Draw_BMP(&bmp[BMP_BG_BOT], BOT, 0, 0);
			
			if(!(no_bmp & NO_BUTTON_A))
			{
				Draw_BMP(&bmp[BMP_BUTTON_A], BOT, 10, 10);
				DrawStringFColor(COLOR_BLACK, COLOR_TRANSPARENT, 60, 25, false, "Boot Firm");
			} else DrawStringFColor(COLOR_WHITE, COLOR_TRANSPARENT, 60, 25, false, "A: Boot Firm");
			
			if(!(no_bmp & NO_BUTTON_X))
			{
				Draw_BMP(&bmp[BMP_BUTTON_X], BOT, 10, 90);
				DrawStringFColor(COLOR_BLACK, COLOR_TRANSPARENT, 60, 105, false, "Dump Boot9/11 and OTP");
			} else DrawStringFColor(COLOR_WHITE, COLOR_TRANSPARENT, 60, 105, false, "X: Dump Boot9/11 and OTP");
			
			if(!(no_bmp & NO_BUTTON_Y))
			{
				Draw_BMP(&bmp[BMP_BUTTON_Y], BOT, 10, 130);
				DrawStringFColor(COLOR_BLACK, COLOR_TRANSPARENT, 60, 145, false, "Menu Nand Dump/Restore");
			} else DrawStringFColor(COLOR_WHITE, COLOR_TRANSPARENT, 60, 145, false, "Y: Menu Nand Dump/Restore");
			
			if(!(no_bmp & NO_BUTTON_DIR))
			{
				Draw_BMP(&bmp[BMP_BUTTON_DIR], BOT, 10, 170);
				DrawStringFColor(COLOR_BLACK, COLOR_TRANSPARENT, 60, 185, false, "Selection Firm");
			} else DrawStringFColor(COLOR_WHITE, COLOR_TRANSPARENT, 60, 185, false, "Pad-D up/Down: Selection Firm");
			
			refresh_b9sm = false;
		}
		
		if(refresh_page)
		{
			for (u32 i = 0; i < 10; i++) 
			{
				
				//delete name payload
				bmp[BMP_BG_TOP].height =  8;
				bmp[BMP_BG_TOP].width = countnamefirm[pos-10+i] * 8;
				if(!(no_bmp & NO_BG_TOP))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, 200 - ((countnamefirm[pos-10+i] * 8) / 2), 50 + (i*15+2));
				bmp[BMP_BG_TOP].width = countnamefirm[pos+10+i] * 8;
				if(!(no_bmp & NO_BG_TOP))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, 200 - ((countnamefirm[pos+10+i] * 8) / 2), 50 + (i*15+2));
				
				
			}
			
			//delete position curseur
			//curser LEFT - GAUCHE
			bmp[BMP_BG_TOP].width = bmp[BMP_CURSER_L].width;
			bmp[BMP_BG_TOP].height = bmp[BMP_CURSER_L].height;
			if(!(no_bmp & NO_CURSER_L))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, (200 - ((countnamefirm[pos-10+index] * 8) / 2))-(bmp[BMP_CURSER_L].width + 10), (50 + (index*15+2))-(bmp[BMP_CURSER_L].height / 2));
			if(!(no_bmp & NO_CURSER_L))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, (200 - ((countnamefirm[pos+10+index] * 8) / 2))-(bmp[BMP_CURSER_L].width + 10), (50 + (index*15+2))-(bmp[BMP_CURSER_L].height / 2));
			
			//curser RIGHT - DROITE
			bmp[BMP_BG_TOP].width = bmp[BMP_CURSER_R].width;
			bmp[BMP_BG_TOP].height = bmp[BMP_CURSER_R].height;
			if(!(no_bmp & NO_CURSER_R))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, 200 + ((countnamefirm[pos-10+index] * 8) / 2)+10, (50 + (index*15+2))-(bmp[BMP_CURSER_R].height / 2));
			if(!(no_bmp & NO_CURSER_R))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, 200 + ((countnamefirm[pos+10+index] * 8) / 2)+10, (50 + (index*15+2))-(bmp[BMP_CURSER_R].height / 2));
			
			refresh_page = false;
			index = 0;
		}
		
		for (u32 i = 0; i < 10; i++) 
		{
			//delete position curseur
			//curser LEFT - GAUCHE
			bmp[BMP_BG_TOP].width = bmp[BMP_CURSER_L].width;
			bmp[BMP_BG_TOP].height = bmp[BMP_CURSER_L].height;
			if(!(no_bmp & NO_CURSER_L))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, (200 - ((countnamefirm[i+pos] * 8) / 2))-(bmp[BMP_CURSER_L].width + 10), (50 + (i*15+2))-(bmp[BMP_CURSER_L].height / 2));
			//curser RIGHT - DROITE
			bmp[BMP_BG_TOP].width = bmp[BMP_CURSER_R].width;
			bmp[BMP_BG_TOP].height = bmp[BMP_CURSER_R].height;
			if(!(no_bmp & NO_CURSER_R))Draw_BMP(&bmp[BMP_BG_TOP], DELETE_TOP, 200 + ((countnamefirm[i+pos] * 8) / 2)+10, (50 + (i*15+2))-(bmp[BMP_CURSER_R].height / 2));
			
		}
		
		for (u32 i = 0; i < 10; i++) 
		{
			
			if(i+pos != index+pos)
			{
				DrawStringFColor(COLOR_BLACK, COLOR_TRANSPARENT, 200 - ((countnamefirm[i+pos] * 8) / 2), 50 + (i*15 + 2), true, "%s", tab[i+pos]);	
				if(no_bmp & NO_BG_TOP)DrawStringFColor(COLOR_WHITE, COLOR_TRANSPARENT, 200 - ((countnamefirm[i+pos] * 8) / 2), 50 + (i*15 + 2), true, "%s", tab[i+pos]);
			}
			
			if(i+pos == index+pos)
			{
				DrawStringFColor(COLOR_SELECT, COLOR_TRANSPARENT, 200 - ((countnamefirm[i+pos] * 8) / 2), 50 + (i*15 + 2), true, "%s", tab[i+pos]);
				if(no_bmp & NO_BG_TOP)DrawStringFColor(COLOR_SELECT, COLOR_TRANSPARENT, 200 - ((countnamefirm[i+pos] * 8) / 2), 50 + (i*15 + 2), true, "%s", tab[i+pos]);
			}
			
		}
		
		//position curseur
		
		//curser LEFT - GAUCHE
		bmp[BMP_BG_TOP].width = bmp[BMP_CURSER_L].width;
		bmp[BMP_BG_TOP].height = bmp[BMP_CURSER_L].height;
		if(!(no_bmp & NO_CURSER_L))Draw_BMP(&bmp[BMP_CURSER_L], TOP, (200 - ((countnamefirm[index+pos] * 8) / 2))-(bmp[BMP_CURSER_L].width + 10), (50 + (index*15+2))-(bmp[BMP_CURSER_L].height / 2) );
		//curser RIGHT - DROITE
		bmp[BMP_BG_TOP].width = bmp[BMP_CURSER_R].width;
		bmp[BMP_BG_TOP].height = bmp[BMP_CURSER_R].height;
		if(!(no_bmp & NO_CURSER_R))Draw_BMP(&bmp[BMP_CURSER_R], TOP, 200 + ((countnamefirm[index+pos] * 8) / 2)+10, (50 + (index*15+2))-(bmp[BMP_CURSER_R].height / 2) );
		
		
		DrawStringFColor(COLOR_RED, COLOR_TRANSPARENT, 200 - ((18 * 8) / 2), 10, true, "Boot9Strap Manager");
		if(c_page > 0)DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 30, 80, true, "P - %d", page);
		
		u32 pad_state = InputWait();
			
		if (pad_state & BUTTON_A) {
			
			loadFirm(NANDorSD, true, pos+index);
			
		} else if (pad_state & BUTTON_DOWN) {
			
			index = (index+pos == count-1) ? 0 : (index+pos+1 == 10)? 0 : index + 1;	
		
		} else if (pad_state & BUTTON_UP) {
			
			index = (index == 0) ? ((pos+10 > count) ? ((count-1)-pos) : 9) : index - 1;	
		
		} else if (pad_state & BUTTON_X) {
			DumpBoot9_11_OTP(NANDorSD);	
		} else if (pad_state & BUTTON_Y) {
			Menu_Dump_Restore();
			refresh_b9sm = true;
		} else if (pad_state & BUTTON_POWER) {
			mcuPowerOff();	
		} else if (pad_state & BUTTON_R1) {
			Screenshot();
		} else if (pad_state & BUTTON_L1) {
			
			page = (page == c_page) ? 0 : page + 1;
			
			if(page == 0)pos=0;
			if(page == 1)pos=10;
			if(page == 2)pos=20;
			if(page == 3)pos=30;
			
			refresh_page = true;
		}
		
	}
	
}

u32 Menu_Dump_Restore()
{
	u32 count = 3;
    u32 index = 0;
	
	const char *options[3] = {	"DUMP NAND FULL",
								"RESTORE NAND (full)",
								"RESTORE NAND (keep hax)"
	};
	
	ClearScreenF(true, true, COLOR_BLACK);
	
	while (true) {
        
		DrawStringFColor(COLOR_BLUE, COLOR_BLACK, 10,10, true, "Menu Nand Dump/Restore.");
		DrawStringFColor(COLOR_GREYGREEN, COLOR_BLACK, 10,90, true, "Press <B> to cancel operation.");
		for (u32 i = 0; i < count; i++) 
		{
			if(i != index)
			{
				DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 30 + (i*10 + 2), true, "%s", options[i]);
			}
			if(i == index)
			{
				DrawStringFColor(COLOR_SELECT, COLOR_BLACK, 10, 30 + (i*10 + 2), true, "%s", options[i]);
			}
		}
		u32 pad_state = InputWait();
        
		if (pad_state & BUTTON_DOWN) { 
            index = (index == count - 1) ? 0 : index + 1;	
		} else if (pad_state & BUTTON_UP) {
            index = (index > 0) ? index - 1 : count - 1;
        } else if (pad_state & BUTTON_A) {
			
			if(index == 0) DumpNand();
			if(index == 1) RestoreNand(FULL_NAND);
			if(index == 2) RestoreNand(KEEP_HAX);
			
		} else if (pad_state & BUTTON_B) {
            DrawStringFColor(COLOR_ORANGE, COLOR_BLACK, 10, 70, true, "(cancelled by user)");
            break;
        }else if (pad_state & BUTTON_R1) {
			Screenshot();	
		}
    }
	
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 90, true, "Press any key to return to manager.");
	InputWait();
	ClearScreenF(true, true, COLOR_BLACK);
    return 0;
}

void DumpBoot9_11_OTP(bool NANDorSD)
{
	if(!NANDorSD)
	{
		
		fileWrite((void *)0x08080000, "boot9strap/boot9.bin", 0x10000);
		fileWrite((void *)0x08090000, "boot9strap/boot11.bin", 0x10000);
		fileWrite((void *)0x10012000, "boot9strap/otp.bin", 0x100);
		DrawStringFColor(COLOR_GREEN, COLOR_TRANSPARENT, 60, 105, false, "Dump Boot9/11 and OTP Succes !");
		
	}
    /* Wait until buttons are not held, for compatibility. */
    while(HID_PAD & NTRBOOT_BUTTONS);
    wait(1000ULL);
	
	
}


//static FIL file;


u32 DumpNand(void)
{
	ClearScreenF(true, true, COLOR_BLACK);
	char path[64];
	snprintf(path, 64, "SYSNAND.bin");
	u8* buffer = BUFFER_ADDRESS;
	u32 result = 0;
	u32 nand_size = getMMCDevice(0)->total_size * NAND_SECTOR_SIZE;
   
	if(Open_File(path, FA_READ))
	{
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 10, true, "The file <%s> exists",path);
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 20, true, "Press <A> to replace it");
		u32 button = InputWait();
		
		if(button & BUTTON_A){
			if(!ShowUnlockSequence(1))
				result = 6;
		} else {
			result = 4;
		}
		Close_File();
	}
	
	
	
	if(result == 0)
	{
		while(1)
		{
			//open file to write
			//if (f_open(&file, path, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) != 0)
			if (Open_File(path, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) != true)
			{
				result = 5;
				break;
			}
			//f_lseek(&file, 0);
			//f_sync(&file);
			
			
			sha_init(SHA_256_MODE);
			u64 n_sectors = nand_size / NAND_SECTOR_SIZE;
			
			DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 10, true, "Dump %s: %d.MB.", path, nand_size / (1024 * 1024));
			DrawStringFColor(COLOR_GREYGREEN, COLOR_BLACK, 10, 40, true, "Press <B> to cancel operation.");
			
			for (u64 i = 0; i < n_sectors; i += SECTORS_PER_READ) {
				
				u32 read_sectors = min(SECTORS_PER_READ, (n_sectors - i));
				
				//progress
				DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 20, true, "Dump SysNand: %3llu%%", (i * 100) / n_sectors);
				
				
				//read sectors nand 1mb
				if (sdmmc_nand_readsectors(i, read_sectors, buffer) != 0)  {   
					result = 1;
					Close_File();
					//f_close(&file);
					break;
				}
				
			//Write the nand sectors to the file
				
				//UINT bytes_written = 0;
				//f_lseek(&file, i * NAND_SECTOR_SIZE);
				//if (f_write(&file, buffer, NAND_SECTOR_SIZE * read_sectors, &bytes_written) != 0) { 
				if (Write_File(buffer, NAND_SECTOR_SIZE * read_sectors, i * NAND_SECTOR_SIZE) != true) { 
					result = 2;
					Close_File();
					//f_close(&file);
					break;
				}
				//f_sync(&file);
				sha_update(buffer, NAND_SECTOR_SIZE * read_sectors);
				
				
				if (HID_PAD & BUTTON_B)
				{
					DrawStringFColor(COLOR_GREYGREEN, COLOR_BLACK, 10, 60, true, "Press <A> to cancel operation.");
					if (InputWait() & BUTTON_A) {
						result = 3;
						Close_File();
						//f_close(&file);
						break;
						
					} else {
						DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 60, true, "Continuing operation...      ");
					}
				}
				
			}
			
			Close_File();
			//progress
			DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 20, true, "Dump SysNand: 100%");
			//f_close(&file);
			break;
		}
	}
	
	if (result == 0)
	{
		char hashname[64];
        u8 shasum[32];
        sha_get(shasum);
		DrawStringFColor(COLOR_GREEN, COLOR_BLACK, 10, 20, true, "Dump SysNand: Succes !");
		
		snprintf(hashname, 64, "%s.sha", path);
		u32 ret = fileWrite(shasum, hashname, 32);
		
		DrawStringFColor((ret == 0) ? COLOR_RED : COLOR_GREEN, COLOR_BLACK, 10, 40, true, 
			"Store to %s: %s", hashname, (ret == 0) ? "failed   ": "ok   ");
		
		DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 50, true, "SysNAND dump SHA256: %08X...", getbe32(shasum));
		
	}
	
	if (result == 1)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 70, true, "Read sectors nand failed !");
	if (result == 2)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 70, true, "sector write to the file failed !");
	if (result == 3 || result == 4)
		DrawStringFColor(COLOR_ORANGE, COLOR_BLACK, 10, (result == 3) ? 70 : 40, true, "(cancelled by user)");
	if (result == 5)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 70, true, "Open file <Sysnand.bin> failed !");
	if (result == 6)
		DrawStringFColor(COLOR_ORANGE, COLOR_BLACK, 10, 20, true, "Show Unlock Sequence cancelled by user");
	
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, (result == 0) ? 70 : 110, true, "Press any key to return to manager.");
	InputWait();
	ClearScreenF(true, true, COLOR_BLACK);
    
	return result;
}

	// see: http://3dbrew.org/wiki/Flash_Filesystem
static PartitionInfo partitions[] = {
    { "TWLN",    {0xE9, 0x00, 0x00, 0x54, 0x57, 0x4C, 0x20, 0x20}, 0x00012E00, 0x08FB5200, 0x3, AES_CNT_TWLNAND_MODE },
    { "TWLP",    {0xE9, 0x00, 0x00, 0x54, 0x57, 0x4C, 0x20, 0x20}, 0x09011A00, 0x020B6600, 0x3, AES_CNT_TWLNAND_MODE },
    { "AGBSAVE", {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 0x0B100000, 0x00030000, 0x7, AES_CNT_CTRNAND_MODE },
    { "FIRM",    {0x46, 0x49, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x00}, 0x0B130000, 0x00400000, 0x6, AES_CNT_CTRNAND_MODE },
    { "FIRM",    {0x46, 0x49, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x00}, 0x0B530000, 0x00400000, 0x6, AES_CNT_CTRNAND_MODE },
    { "CTRNAND", {0xE9, 0x00, 0x00, 0x43, 0x54, 0x52, 0x20, 0x20}, 0x0B95CA00, 0x2F3E3600, 0x4, AES_CNT_CTRNAND_MODE }, // O3DS
    { "CTRNAND", {0xE9, 0x00, 0x00, 0x43, 0x54, 0x52, 0x20, 0x20}, 0x0B95AE00, 0x41D2D200, 0x5, AES_CNT_CTRNAND_MODE }, // N3DS
    { "CTRNAND", {0xE9, 0x00, 0x00, 0x43, 0x54, 0x52, 0x20, 0x20}, 0x0B95AE00, 0x41D2D200, 0x4, AES_CNT_CTRNAND_MODE }, // NO3DS
    { "CTRFULL", {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 0x0B930000, 0x2F5D0000, 0x4, AES_CNT_CTRNAND_MODE }, // O3DS
    { "CTRFULL", {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 0x0B930000, 0x41ED0000, 0x5, AES_CNT_CTRNAND_MODE }  // N3DS
};

u32 RestoreNand(u32 param)
{
	ClearScreenF(true, true, COLOR_BLACK);
    u8* buffer = BUFFER_ADDRESS;
    u32 nand_size = getMMCDevice(0)->total_size * NAND_SECTOR_SIZE;
    u32 result = 0;
	
	DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 10, true, "<WARNING RISK BRICK>.");
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 30, true, "Press <A> to restore the Sysnand.");
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 40, true, "Press <B> to return to manager.");
	
	u32 button = InputWait();
		
	if(button & BUTTON_A){
		
		if(ShowUnlockSequence(5))
		{
			//verification dump sha
			u32 hash = CheckNandDumpIntegrity("SYSNAND.bin");
			if(hash != HASH_VERIFIED)
			{
				result = (hash == HASH_FAILED)? 7 : 8;
			} else {
				DrawStringFColor(COLOR_GREEN, COLOR_BLACK, 10, 20, true,"Verification passed");
				
			}
			
		}else result = 3;
		
	} else if(button & BUTTON_B) {
		result = 6;
	}
	
	
	
	
    if(result == 0)
	{
		while(1)
		{
			if(Open_File("SYSNAND.bin", FA_READ) == true)
			{
				DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 30, true, "Restoring SysNAND. Size (MB): %u", nand_size / (1024 * 1024));
				
				//verification Size File
				if (Size_File() != nand_size) {
					result = 5;
					Close_File();
					break;
				}
				
				u32 n_sectors = nand_size / NAND_SECTOR_SIZE;
				if (!(param & KEEP_HAX)) { // standard, full restore
					
					
					for (u32 i = 0; i < n_sectors; i += SECTORS_PER_READ) {
						u32 read_sectors = min(SECTORS_PER_READ, (n_sectors - i));
						
						//progress
						DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 40, true, "Restore SysNand : %3llu%%", (i * 100) / n_sectors);
						
						if (!Read_File(buffer, NAND_SECTOR_SIZE * read_sectors, i * NAND_SECTOR_SIZE)) {
							result = 2;
							Close_File();
							break;
						}
						if (sdmmc_nand_writesectors(i, read_sectors, buffer) != 0) {
							result = 1;
							Close_File();
							break;
						}
					}
				} else { // Hax preserving restore
					
					for (u32 section = 0; section < 3; section++) {
						u32 start_sector, end_sector;
						if (section == 0) { // NAND header & sectors until 0x96
							start_sector = 0x00;
							end_sector = 0x96;
						} else if (section == 1) { // TWLN, TWLP & AGBSAVE
							start_sector = partitions[0].offset / NAND_SECTOR_SIZE;
							end_sector = ((partitions[2].offset + partitions[2].size) - partitions[0].offset) / NAND_SECTOR_SIZE;
						} else { // CTRNAND (full size) (FIRM skipped)
							start_sector = 0x0B930000 / NAND_SECTOR_SIZE;
							end_sector = n_sectors;
						}
						
						for (u32 i = start_sector; i < end_sector; i += SECTORS_PER_READ) {
							u32 read_sectors = min(SECTORS_PER_READ, (end_sector - i));
							
							//progress
							DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 40, true, "Restore SysNand : %3llu%%", (i * 100) / n_sectors);
							
							if (!Read_File(buffer, NAND_SECTOR_SIZE * read_sectors, i * NAND_SECTOR_SIZE)) {
								result = 2;
								Close_File();
								break;
							}
							if (sdmmc_nand_writesectors(i, read_sectors, buffer) != 0) {
								result = 1;
								Close_File();
								break;
							}
						}
					}
				}
				Close_File();
				break;
				
			} else {
				result = 4;
				break;
			}
		}
		
	}
	if (result == 0)
		DrawStringFColor(COLOR_GREEN, COLOR_BLACK, 10, 90, true, "Restore SysNand Succes !");
	if (result == 1)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 90, true, "Write sectors nand failed !");
	if (result == 2)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 90, true, "Read sectors to the file failed !");
	if (result == 3)
		DrawStringFColor(COLOR_ORANGE, COLOR_BLACK, 10, 90, true, "(cancelled by user)");
    if (result == 4)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 90, true, "Open file <SYSNAND.bin> failed !");
	if (result == 5)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 90, true, "Failed Small NAND backup !");
	if (result == 6)
		DrawStringFColor(COLOR_ORANGE, COLOR_BLACK, 10, 50, true, "Show Unlock Sequence cancelled by user");
	//hash
	if (result == 7)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 40, true,"Failed, file is corrupt!");
	if (result == 8)
		DrawStringFColor(COLOR_RED, COLOR_BLACK, 10, 40, true,".SHA not found, skipped");
	
	
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10,(result >= 6)? 60 : 130, true, "Press any key to return to manager.");
	InputWait();
	ClearScreenF(true, true, COLOR_BLACK);
    
	return result;
}
