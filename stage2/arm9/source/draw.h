/*
*   fs.h
*/

#pragma once

#include "types.h"

#define BYTES_PER_PIXEL 3
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH_TOP 400
#define SCREEN_WIDTH_BOT 320

#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)

#define BG        RGB(0xE4, 0xE4, 0xE4)


#define COLOR_SELECT        RGB(0xD7, 0x9A, 0x28)
#define COLOR_BLACK         RGB(0x00, 0x00, 0x00)
#define COLOR_WHITE         RGB(0xFF, 0xFF, 0xFF)
#define COLOR_RED           RGB(0xFF, 0x00, 0x00)
#define COLOR_GREEN         RGB(0x00, 0xFF, 0x00)
#define COLOR_BLUE          RGB(0xFF, 0x00, 0xFF)
#define COLOR_GREY          RGB(0x77, 0x77, 0x77)
#define COLOR_PURPLE        RGB(0x66, 0x00, 0xFF)
#define COLOR_TRANSPARENT   RGB(0xFF, 0x00, 0xEF) // otherwise known as 'super fuchsia' (r,255)(g,0)(b,239)
#define COLOR_ORANGE        RGB(0xFF, 0xA5, 0x00)

#define COLOR_BRIGHTRED     RGB(0xFF, 0x30, 0x30)
#define COLOR_DARKRED       RGB(0x80, 0x00, 0x00)
#define COLOR_BRIGHTYELLOW  RGB(0xFF, 0xFF, 0x30)
#define COLOR_BRIGHTGREEN   RGB(0x30, 0xFF, 0x30)
#define COLOR_BRIGHTBLUE    RGB(0x30, 0x30, 0xFF)
#define COLOR_GREYGREEN     RGB(0xA0, 0xFF, 0xA0)

#define TOP_SCREEN (u8 *)0x18300000
#define BOT_SCREEN (u8 *)0x18346500

void InitScreen(void);

void DrawStringFColor(int colorfont, int colorbg, int x, int y, bool use_top, const char *format, ...);
void ClearScreenF(bool clear_top, bool clear_bottom, int color);

void drawimage(char* data, int posX, int posY,int sizeX, int sizeY);

void Screenshot();

