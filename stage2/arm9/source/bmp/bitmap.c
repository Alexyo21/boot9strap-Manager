

#include "bitmap.h"
#include "../fs.h"
#include "../draw.h"
#include "../fatfs/ff.h"
#include "../memory.h"

int Multiple4(int width)
{	
	for(int pix = 1; pix < 397; pix+=4)
	{
		if(width == pix)return 1;
		if(pix > width)break;
	}
	
	for(int pix = 2; pix < 398; pix+=4)
	{
		if(width == pix)return 2;
		if(pix > width)break;
	}
	
	for(int pix = 3; pix < 399; pix+=4)
	{
		if(width == pix)return 3;
		if(pix > width)break;
	}
	
	return 0;
}

u32 Read_Draw_BMP(u32 screen, const char* path, int pos_width, int pos_height)
{
	int nextpixel = 0;
	BMP_Data bmp;
	
	if (!Open_File(path, FA_READ))
		return 1;
	Read_File(&bmp, 0, 0);
	
	if (bmp.signature[0]!='B' || bmp.signature[1]!='M'){
		Close_File();
		return ERROR_SIG;}
	if (bmp.bpp!=24){
		Close_File();
		return ERROR_BPP;}
	if (bmp.compression!=0){
		Close_File();
		return ERROR_COM;}
	if (bmp.cpalette!=0 || bmp.cIpalette!=0){
		Close_File();
		return ERROR_PAL;}
		
		
	for(int i = bmp.height; i > 0; i--)
	{	 
		
		for(int j = 0; j < bmp.width; j++)	
		{
			u32 offset = (SCREEN_HEIGHT*(pos_width+j)+SCREEN_HEIGHT-(pos_height+i))*BYTES_PER_PIXEL;
			
			if(screen & TOP)
			{
				//if super fuchsia transparence (r,255,0xFF)(g,0,0x00)(b,239,0xEF)
				if(	bmp.pixel[nextpixel+0] == 0xEF && bmp.pixel[nextpixel+1] == 0x00 && bmp.pixel[nextpixel+2] == 0xFF){
					
					nextpixel+=3;
					
				} else {//else draw pixel
					
					*((u8*)TOP_SCREEN + offset++) = bmp.pixel[nextpixel++];
					*((u8*)TOP_SCREEN + offset++) = bmp.pixel[nextpixel++];
					*((u8*)TOP_SCREEN + offset++) = bmp.pixel[nextpixel++];
				}
				
			} 
			if(screen & TOP && screen & BOT)nextpixel-=3;
			if(screen & BOT)
			{
				//if super fuchsia transparence (r,255,0xFF)(g,0,0x00)(b,239,0xEF)
				if(	bmp.pixel[nextpixel+0] == 0xEF && bmp.pixel[nextpixel+1] == 0x00 && bmp.pixel[nextpixel+2] == 0xFF){
					
					nextpixel+=3;
					
				} else {//else draw pixel
					
					*((u8*)BOT_SCREEN + offset++) = bmp.pixel[nextpixel++];
					*((u8*)BOT_SCREEN + offset++) = bmp.pixel[nextpixel++];
					*((u8*)BOT_SCREEN + offset++) = bmp.pixel[nextpixel++];
				}
				
			}
			
		}	
			
		//multiple 4
		nextpixel += Multiple4(bmp.width);
	}
	
	
	Close_File();
	return BMP_VALID;
}

u32 Read_BMP(BMP_Data* bmp, const char* path)
{
	
	if (!Open_File(path, FA_READ))
		return 1;
	Read_File(bmp, 0, 0);
	
	if (bmp->signature[0]!='B' || bmp->signature[1]!='M'){
		Close_File();
		return ERROR_SIG;}
	if (bmp->bpp!=24){
		Close_File();
		return ERROR_BPP;}
	if (bmp->compression!=0){
		Close_File();
		return ERROR_COM;}
	if (bmp->cpalette!=0 || bmp->cIpalette!=0){
		Close_File();
		return ERROR_PAL;}
		
	
	Close_File();
	return BMP_VALID;
}


void Draw_BMP(BMP_Data* bmp, u32 screen, int pos_width, int pos_height)
{
	
	
	int nextpixel = 0;
	for(int i = bmp->height; i > 0; i--)
	{	 
		
		for(int j = 0; j < bmp->width; j++)	
		{
			u32 offset = (SCREEN_HEIGHT*(pos_width+j)+SCREEN_HEIGHT-(pos_height+i))*BYTES_PER_PIXEL;
			
			int retpixel = ((((SCREEN_HEIGHT-pos_height)-i)*SCREEN_WIDTH_TOP)+(pos_width+j))*BYTES_PER_PIXEL;
			
			
			
			if(screen & TOP){
				//if super fuchsia transparence (r,255,0xFF)(g,0,0x00)(b,239,0xEF)
				if(	bmp->pixel[nextpixel+0] == 0xEF && bmp->pixel[nextpixel+1] == 0x00 && bmp->pixel[nextpixel+2] == 0xFF){
					
					nextpixel+=3;
					
				} else {//else draw pixel
					
					*((u8*)TOP_SCREEN + offset++) = bmp->pixel[nextpixel++];
					*((u8*)TOP_SCREEN + offset++) = bmp->pixel[nextpixel++];
					*((u8*)TOP_SCREEN + offset++) = bmp->pixel[nextpixel++];
				}
			}
			if(screen & TOP && screen & BOT)nextpixel-=3;
			if(screen & BOT){
				//if super fuchsia transparence (r,255,0xFF)(g,0,0x00)(b,239,0xEF)
				if(	bmp->pixel[nextpixel+0] == 0xEF && bmp->pixel[nextpixel+1] == 0x00 && bmp->pixel[nextpixel+2] == 0xFF){
					
					nextpixel+=3;
					
				} else {//else draw pixel
					
					*((u8*)BOT_SCREEN + offset++) = bmp->pixel[nextpixel++];
					*((u8*)BOT_SCREEN + offset++) = bmp->pixel[nextpixel++];
					*((u8*)BOT_SCREEN + offset++) = bmp->pixel[nextpixel++];
				}
			}
			
			if(screen & DELETE_TOP || screen & DELETE_BOT){
				
				if(screen & DELETE_TOP){
					*((u8*)TOP_SCREEN + offset++) = bmp->pixel[retpixel+0];
					*((u8*)TOP_SCREEN + offset++) = bmp->pixel[retpixel+1];
					*((u8*)TOP_SCREEN + offset++) = bmp->pixel[retpixel+2];
				}
				
				if(screen & DELETE_BOT){
					*((u8*)BOT_SCREEN + offset++) = bmp->pixel[retpixel+0];
					*((u8*)BOT_SCREEN + offset++) = bmp->pixel[retpixel+1];
					*((u8*)BOT_SCREEN + offset++) = bmp->pixel[retpixel+2];
				}
			}
			
			
			
		}
		
			//multiple 4
			nextpixel += Multiple4(bmp->width);
	}
}
	

