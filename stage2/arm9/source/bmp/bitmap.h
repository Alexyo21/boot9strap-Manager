#pragma once

#include "../types.h"

#define BMP_VALID 0
#define ERROR_SIG 1
#define ERROR_BPP 2
#define ERROR_COM 3
#define ERROR_PAL 4

#define TOP 				(1<<0)
#define BOT 				(1<<1)
#define DELETE_TOP 			(1<<2)
#define DELETE_BOT			(1<<3)


typedef struct 
{
	char signature[2];
	int size;				
	int rsv;				
	int offsetim;			
	int size_imhead;		 
	
	int width;			
	int height;				
	
	short nbplans; 			// toujours 1
	short bpp;				//bit Pixel
	int compression;		
	int sizeim;				
	int hres;				
	int vres;				
	int cpalette;			// color palette
	int cIpalette;			
	
	char pixel[400*240*3];
	
} __attribute__((packed)) BMP_Data;
//BMP_Header* bmp = (BMP_Header*)0x18100000; boot9strap
//BMP_Header* bmp = (BMP_Header*)0x23100000; payload .firm

u32 Read_Draw_BMP(u32 screen, const char* path, int pos_width, int pos_height);

u32 Read_BMP(BMP_Data* bmp, const char* path);
void Draw_BMP(BMP_Data* bmp, u32 screen,  int pos_width, int pos_height);








