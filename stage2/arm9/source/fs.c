/*
*   fs.c
*/

#include "fs.h"

#include "hid.h"
#include "utils.h"

#include <string.h>
#include "fatfs/ff.h"
#include "draw.h"

static FATFS fs;

bool mountSd(void)
{
    return f_mount(&fs, "0:", 1) == FR_OK;
}

void unmountSd(void)
{
    f_mount(NULL, "0:", 1);
}

bool mountCtrNand(void)
{
    return f_mount(&fs, "1:", 1) == FR_OK && f_chdrive("1:") == FR_OK;
}

u32 fileRead(void *dest, const char *path, u32 size, u32 maxSize)
{
    FIL file;
    u32 ret = 0;

    if(f_open(&file, path, FA_READ) != FR_OK) return ret;

    if(!size) size = f_size(&file);
    if(!maxSize || size <= maxSize)
        f_read(&file, dest, size, (unsigned int *)&ret);
    f_close(&file);

    return ret;
}

bool fileWrite(const void *buffer, const char *path, u32 size)
{
    FIL file;

    switch(f_open(&file, path, FA_WRITE | FA_OPEN_ALWAYS))
    {
        case FR_OK:
        {
            unsigned int written;
            f_write(&file, buffer, size, &written);
            f_truncate(&file);
            f_close(&file);

            return (u32)written == size;
        }
        case FR_NO_PATH:
            for(u32 i = 1; path[i] != 0; i++)
                if(path[i] == '/')
                {
                    char folder[i + 1];
                    memcpy(folder, path, i);
                    folder[i] = 0;
                    f_mkdir(folder);
                }

            return fileWrite(buffer, path, size);
        default:
            return false;
    }
}

bool fileDelete(const char *path)
{
    return f_unlink(path) == FR_OK;
}

u32 GetDirList(char* path)
{
   
	FILINFO fno;
	DIR pdir;
	char* list = (char*) 0x20001000;
	fno.lfname = list;
	
	if(f_opendir(&pdir, path) != FR_OK)return false;
	
	
	int count = 0;
	
	while (f_readdir(&pdir, &fno) == FR_OK) {
		
		
		if (fno.fname[0] == 0) break;
		if ((fno.fname[0] != '.') && !(fno.fattrib & AM_DIR))
			memcpy(tab[count], fno.fname, 32);
		int i = 0;
		while (fno.fname[i] !='\0')
		{
			i++;
		}
		DrawStringFColor(COLOR_SELECT, COLOR_BLACK, 10, 10, true, "%s", tab[0][1]);
			
		countnamefirm[count] = i;
		count++;
    }
	f_closedir(&pdir);
    return count;
}

void DumpBoot9_11_OTP(bool NANDorSD)
{
	if(NANDorSD)
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
