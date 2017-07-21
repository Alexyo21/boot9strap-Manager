/*
*   fs.c
*/
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"

#include "firm.h"
#include "i2c.h"

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

#define SET_PIXEL(buffer, x, y, rgb)\
{\
	u32 offset = (SCREEN_HEIGHT * x + SCREEN_HEIGHT - y ) * BYTES_PER_PIXEL;\
	*((u8*)buffer + offset++) = rgb >> 16;\
	*((u8*)buffer + offset++) = rgb >> 8;\
	*((u8*)buffer + offset++) = rgb & 0xFF;;\
}

//drawimage(button, 80, 30 + (i*13),240, 11);
void drawimage(char* data, int posX, int posY,int sizeX, int sizeY)
{
	u8 r,g,b;
	int dir = 18;
	for(int i = sizeY; 0 < i; i--)
	{	 
		
		for(int j = 0; j < sizeX; j++)	
		{
			
			b = (data[dir++]);
			g = (data[dir++]);
			r = (data[dir++]);
			SET_PIXEL(TOP_SCREEN, (posX+j), (posY+i), RGB(r,g,b));
			
			
		}			
	}
	
}
