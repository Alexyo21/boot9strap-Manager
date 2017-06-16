/*
*   fs.c
*/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "fs.h"
#include <string.h>
#include "fatfs/ff.h"
#include "font.h"

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
		countnamefirm[count] = i;
		count++;
    }
	f_closedir(&pdir);
    return count;
}


void DrawCharacter(u8* screen, int character, int x, int y, int color, int bgcolor)
{
    for (int yy = 0; yy < 8; yy++) {
        int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_HEIGHT);
        int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * BYTES_PER_PIXEL);
        u8* screenPos = screen + xDisplacement + yDisplacement;

        u8 charPos = font[character * 8 + yy];
        for (int xx = 7; xx >= 0; xx--) {
            if ((charPos >> xx) & 1) {
                *(screenPos + 0) = color >> 16;  // B
                *(screenPos + 1) = color >> 8;   // G
                *(screenPos + 2) = color & 0xFF; // R
            } else if (bgcolor != COLOR_TRANSPARENT) {
                *(screenPos + 0) = bgcolor >> 16;  // B
                *(screenPos + 1) = bgcolor >> 8;   // G
                *(screenPos + 2) = bgcolor & 0xFF; // R
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT;
        }
    }
}

void DrawString(int color, int bgcolor, u8* screen, int x, int y, const char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
        DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
}

void DrawStringFColor(int colorfont, int colorbg, int x, int y, bool use_top, const char *format, ...)
{
    char str[512] = { 0 }; // 512 should be more than enough
    va_list va;

    va_start(va, format);
    vsnprintf(str, 512, format, va);
    va_end(va);

    for (char* text = strtok(str, "\n"); text != NULL; text = strtok(NULL, "\n"), y += 10) {
        if (use_top) {
            DrawString(colorfont, colorbg, TOP_SCREEN, x, y, text);
        } else {
            DrawString(colorfont, colorbg, BOT_SCREEN, x, y, text);
        }
    }
}


