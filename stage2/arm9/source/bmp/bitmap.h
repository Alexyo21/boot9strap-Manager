#pragma once

#include "../types.h"

#define ERROR_SIG 1
#define ERROR_BPP 2
#define ERROR_COM 3
#define ERROR_PAL 4

#define TOP 		(1<<0)
#define BOT 		(1<<1)
#define DELETE_TOP 	(1<<2)
#define DELETE_BOT	(1<<3)

#define BMP_BG_TOP 			0
#define BMP_BG_BOT 			1
#define BMP_CURSER_L		2//LEFT - GAUCHE
#define BMP_CURSER_R 		3//RIGHT - DROITE
#define BMP_BUTTON_A 		4
#define BMP_BUTTON_B 		5
#define BMP_BUTTON_X 		6
#define BMP_BUTTON_Y 		7
#define BMP_BUTTON_R 		8
#define BMP_BUTTON_L 		9
#define BMP_BUTTON_HOME 	10
#define BMP_BUTTON_POWER 	11
#define BMP_BUTTON_START 	12
#define BMP_BUTTON_SELECT	13
#define BMP_BUTTON_DIR		14

#define BMP_MAX 14


#define NO_BG_TOP 			(1<<0)
#define NO_BG_BOT 			(1<<1)
#define NO_CURSER_L			(1<<2)//LEFT - GAUCHE
#define NO_CURSER_R 		(1<<3)//RIGHT - DROITE
#define NO_BUTTON_A 		(1<<4)
#define NO_BUTTON_B 		(1<<5)
#define NO_BUTTON_X 		(1<<6)
#define NO_BUTTON_Y 		(1<<7)
#define NO_BUTTON_R 		(1<<8)
#define NO_BUTTON_L 		(1<<9)
#define NO_BUTTON_HOME 		(1<<10)
#define NO_BUTTON_POWER 	(1<<11)
#define NO_BUTTON_START 	(1<<12)
#define NO_BUTTON_SELECT	(1<<13)
#define NO_BUTTON_DIR		(1<<14)


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
	
} __attribute__((packed)) BMP_Header;


//u32 Read_BMP(BMP_Header *header, u32 screen, const char* path, int pos_height, int pos_width);
u32 Read_BMP(BMP_Header* header, const char* path);

void Draw_BMP(BMP_Header* header, u32 screen,  int pos_width, int pos_height);






