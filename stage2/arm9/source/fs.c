/*
*   fs.c
*/
#include <stdio.h>
#include <string.h>

#include "fs.h"
#include "hid.h"
#include "utils.h"
#include "fatfs/ff.h"
#include "draw.h"

static FATFS fs;
static FIL file;

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

bool Open_File(const char* path, u32 flag)
{
	if (*path == '/')
        path++;
    if(f_open(&file, path, flag) == FR_OK) {
		
		f_lseek(&file, 0);
		f_sync(&file);
		return true;
		
	} else	return false;
}

bool Read_File(void* buf, u32 size, u32 foffset)
{
	if(!size) size = f_size(&file);
	unsigned int bytes_read = 0;
	
	f_lseek(&file, foffset);
	if (f_read(&file, buf, size, &bytes_read) == FR_OK)
		return (u32)bytes_read;
	
	else return false;
}

bool Write_File(const void *buffer, u32 size, u32 foffset)
{
	unsigned int bytes_written;
	f_lseek(&file, foffset);
	f_write(&file, buffer, size, &bytes_written);
	f_sync(&file);
	return (u32)bytes_written == size;
}

u32 Size_File()
{
    return f_size(&file);
}

void Seek_File(u32 offset)
{
    f_lseek(&file, offset);
}

void Sync_File()
{
    f_sync(&file);
}

void Close_File()
{
    f_close(&file);
}

bool fileOpen(const char* path, u32 flag)
{
	u32 ret = false;
	if (*path == '/')
        path++;
    if(f_open(&file, path, flag) == FR_OK) 
		ret = true;
	f_close(&file);
	return ret;
}

u32 fileRead(void *dest, const char *path, u32 size, u32 maxSize)
{
    FIL file;
    u32 ret = 0;
	if (*path == '/')
        path++;
    if(f_open(&file, path, FA_READ) != FR_OK) return ret;

    if(!size) size = f_size(&file);
    if(!maxSize || size <= maxSize)
        f_read(&file, dest, size, (unsigned int *)&ret);
    f_close(&file);

    return ret;
}

bool fileWrite(const void *buffer, const char *path,  size_t size)
{
    FIL file;
	if (*path == '/')
        path++;
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
	DIR ptdir;
	
	if(f_opendir(&ptdir, path) != FR_OK)return false;
	
	int count = 0;
	
	while (f_readdir(&ptdir, &fno) == FR_OK) {
		
		
		if (fno.fname[0] == 0) break;
		if ((fno.fname[0] != '.') && !(fno.fattrib & AM_DIR))
		{
			memcpy(tab[count], fno.fname, 64);
			
			int i = 0;
			while (fno.fname[i] !='\0')
				i++;
			
			
			countnamefirm[count] = i;
			count++;
		}
    }
	f_closedir(&ptdir);
    return count;
}


// ShowUnlockSequence(1);
bool ShowUnlockSequence(u32 seqlvl) 
{
    const int seqcolors[7] = { COLOR_WHITE, COLOR_BRIGHTGREEN, COLOR_BRIGHTYELLOW,
        COLOR_ORANGE, COLOR_BRIGHTBLUE, COLOR_RED, COLOR_DARKRED };
    const u32 sequences[7][5] = {
        { BUTTON_RIGHT, BUTTON_DOWN, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_A },
        { BUTTON_LEFT, BUTTON_DOWN, BUTTON_RIGHT, BUTTON_UP, BUTTON_A },   
        { BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_UP, BUTTON_A },
        { BUTTON_LEFT, BUTTON_UP, BUTTON_RIGHT, BUTTON_UP, BUTTON_A },
        { BUTTON_RIGHT, BUTTON_DOWN, BUTTON_LEFT, BUTTON_DOWN, BUTTON_A },
        { BUTTON_DOWN, BUTTON_LEFT, BUTTON_UP, BUTTON_LEFT, BUTTON_A },
        { BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_A }
    };
    //  '\x18' UP - '\x19' DOWN - '\x1A' RIGHT - '\x1B' LEFT
	const char seqsymbols[7][5] = { 
        { '\x1A', '\x19', '\x1A', '\x19', 'A' },
        { '\x1B', '\x19', '\x1A', '\x18', 'A' },
        { '\x1B', '\x1A', '\x19', '\x18', 'A' },
        { '\x1B', '\x18', '\x1A', '\x18', 'A' },
        { '\x1A', '\x19', '\x1B', '\x19', 'A' },
        { '\x19', '\x1B', '\x18', '\x1B', 'A' },
        { '\x18', '\x19', '\x1B', '\x1A', 'A' }
    };
    const u32 len = 5;
    
	u32 color_off = COLOR_GREY;
    u32 color_on = seqcolors[seqlvl];
    u32 lvl = 0;
	
    ClearScreenF(true, false, COLOR_BLACK);
    DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 200 - ((24 * 8) / 2), 50, true, "enter the sequence below");
	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 200 - ((21 * 8) / 2), 60, true, "or press B to cancel.");

	DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 200 - ((23 * 8) / 2), 80, true, "To proceed, enter this:");
    
    while (true) {
        for (u32 n = 0; n < len; n++) {
            DrawStringFColor((lvl > n) ? color_on : color_off, COLOR_BLACK, 106 + (n*4*8), 100, true, "<%c>", seqsymbols[seqlvl][n]);
        }
        if (lvl == len)
            break;
        u32 pad_state = InputWait();
        if (!(pad_state & BUTTON_ANY))
            continue;
        else if (pad_state & sequences[seqlvl][lvl])
            lvl++;
        else if (pad_state & BUTTON_B)
            break;
        else if (lvl == 0 || !(pad_state & sequences[seqlvl][lvl-1]))
            lvl = 0;
    }
    
    ClearScreenF(true, true, COLOR_BLACK);
    
    return (lvl >= len);
}

