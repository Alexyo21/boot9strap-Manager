/*
*   fs.c
*/
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"
#include "fs.h"
#include "firm.h"
#include "i2c.h"
#include "fatfs/ff.h"

#include "bmp/bitmap.h"

void InitScreen(void)
{
	//screenInit arm11
	invokeArm11Function(INIT_SCREENS);
	i2cWriteRegister(I2C_DEV_MCU, 0x22, 0x2A);
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


void ClearScreen(u8* screen, int color)
{
    int width = (screen == TOP_SCREEN) ? SCREEN_WIDTH_TOP : SCREEN_WIDTH_BOT;
    if (color == COLOR_TRANSPARENT) color = COLOR_BLACK;
    for (int i = 0; i < (width * SCREEN_HEIGHT); i++) {
        *(screen++) = color >> 16;  // B
        *(screen++) = color >> 8;   // G
        *(screen++) = color & 0xFF; // R
    }
}

void ClearScreenF(bool clear_top, bool clear_bottom, int color)
{
    if (clear_top) ClearScreen(TOP_SCREEN, color);
    if (clear_bottom) ClearScreen(BOT_SCREEN, color);
}



void Screenshot()
{
    u8* buffer = (u8*) 0x18000000; // careful, this area is used by other functions in Decrypt9
    u8* buffer_t = buffer + (400 * 240 * 3);
    u8 bmp_header[54] = {
        0x42, 0x4D, 0x36, 0xCA, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
        0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xCA, 0x08, 0x00, 0x12, 0x0B, 0x00, 0x00, 0x12, 0x0B, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    static u32 n = 0;
    for (; n < 1000; n++) 
	{
		char filename[16];
		snprintf(filename, 16, "snap%03i.bmp", (int) n);
            
		if (!Open_File(filename, FA_READ)) 
		{
			Open_File(filename, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
			break;
		}
		Close_File();
	}
	
	if (n >= 1000)
		return;
   
    
    memset(buffer, 0x1F, 400 * 240 * 3 * 2);
    for (u32 x = 0; x < 400; x++)
        for (u32 y = 0; y < 240; y++)
            memcpy(buffer_t + (y*400 + x) * 3, TOP_SCREEN + (x*240 + y) * 3, 3);
    for (u32 x = 0; x < 320; x++)
        for (u32 y = 0; y < 240; y++)
            memcpy(buffer + (y*400 + x + 40) * 3, BOT_SCREEN + (x*240 + y) * 3, 3);
    
    Write_File(bmp_header, 54, 0);
	
	Write_File(buffer, 400 * 240 * 3 * 2, 54);
    Close_File();
}

