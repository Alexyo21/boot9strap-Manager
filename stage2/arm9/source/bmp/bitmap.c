

#include "bitmap.h"
#include "../fs.h"
#include "../draw.h"
#include "../fatfs/ff.h"
#include "../memory.h"

//u32 Read_BMP(u32 screen, const char* path, int pos_height, int pos_width)
u32 Read_BMP(BMP_Header* header, const char* path)
{
	//int dir = 0;
	//BMP_Header header;
	
	if (!Open_File(path, FA_READ))
		return 1;
	//Read_File(&header, 0, 0);
	Read_File(header, 0, 0);
	
	if (header->signature[0]!='B' || header->signature[1]!='M'){
		Close_File();
		return ERROR_SIG;}
	if (header->bpp!=24){
		Close_File();
		return ERROR_BPP;}
	if (header->compression!=0){
		Close_File();
		return ERROR_COM;}
	if (header->cpalette!=0 || header->cIpalette!=0){
		Close_File();
		return ERROR_PAL;}
		
	
	
	/*	
	for(int i = header->height; i > 0; i--)
	{	 
		
		for(int j = 0; j < header->width; j++)	
		{
			u32 offset = (SCREEN_HEIGHT*(pos_height+j)+SCREEN_HEIGHT-(pos_width+i))*BYTES_PER_PIXEL;
			
			if(screen & TOP)
			{
				*((u8*)TOP_SCREEN + offset++) = header->data.pixel[dir++];
				*((u8*)TOP_SCREEN + offset++) = header->data.pixel[dir++];
				*((u8*)TOP_SCREEN + offset++) = header->data.pixel[dir++];
			} 
			if(screen & TOP && screen & BOT)dir = dir-3;
			if(screen & BOT)
			{
				
				*((u8*)BOT_SCREEN + offset++) = header->data.pixel[dir++];
				*((u8*)BOT_SCREEN + offset++) = header->data.pixel[dir++];
				*((u8*)BOT_SCREEN + offset++) = header->data.pixel[dir++];
			}
			
		}			
	}
	*/
	
	Close_File();
	return 0;
}


void Draw_BMP(BMP_Header* header, u32 screen, int pos_width, int pos_height)
{
	
	
	int dir = 0;
	for(int i = header->height; i > 0; i--)
	{	 
		
		for(int j = 0; j < header->width; j++)	
		{
			u32 offset = (SCREEN_HEIGHT*(pos_width+j)+SCREEN_HEIGHT-(pos_height+i))*BYTES_PER_PIXEL;
			
			int dirpixel = ((((SCREEN_HEIGHT-pos_height)-i)*SCREEN_WIDTH_TOP)+(pos_width+j))*BYTES_PER_PIXEL;
			
			if(screen & DELETE_TOP || screen & DELETE_BOT){
				
				if(screen & DELETE_TOP){
					*((u8*)TOP_SCREEN + offset++) = header->pixel[dirpixel+0];
					*((u8*)TOP_SCREEN + offset++) = header->pixel[dirpixel+1];
					*((u8*)TOP_SCREEN + offset++) = header->pixel[dirpixel+2];
				}
				if(screen & DELETE_BOT){
					*((u8*)BOT_SCREEN + offset++) = header->pixel[dirpixel+0];
					*((u8*)BOT_SCREEN + offset++) = header->pixel[dirpixel+1];
					*((u8*)BOT_SCREEN + offset++) = header->pixel[dirpixel+2];
				}
			}
			
			if(screen & TOP){
				//if super fuchsia transparence (r,255,0xFF)(g,0,0x00)(b,239,0xEF)
				if(	header->pixel[dir+0] == 0xEF && header->pixel[dir+1] == 0x00 && header->pixel[dir+2] == 0xFF){
					
					dir+=3;
					
				} else {//else print
					
					*((u8*)TOP_SCREEN + offset++) = header->pixel[dir++];
					*((u8*)TOP_SCREEN + offset++) = header->pixel[dir++];
					*((u8*)TOP_SCREEN + offset++) = header->pixel[dir++];
				}
			}
			
			if(screen & BOT){
				//if super fuchsia transparence (r,255,0xFF)(g,0,0x00)(b,239,0xEF)
				if(	header->pixel[dir+0] == 0xEF && header->pixel[dir+1] == 0x00 && header->pixel[dir+2] == 0xFF){
					
					dir+=3;
					
				} else {//else print
					
					*((u8*)BOT_SCREEN + offset++) = header->pixel[dir++];
					*((u8*)BOT_SCREEN + offset++) = header->pixel[dir++];
					*((u8*)BOT_SCREEN + offset++) = header->pixel[dir++];
				}
			}
			
		}
		//multiple 4
		if(	header->width == 1 || 
			header->width == 5 || 
			header->width == 9 || 
			header->width == 13 || 
			header->width == 17 || 
			header->width == 21 || 
			header->width == 25 || 
			header->width == 29 || 
			header->width == 33 || 
			header->width == 37 || 
			header->width == 41 || 
			header->width == 45 || 
			header->width == 49 || 
			header->width == 53 || 
			header->width == 57 || 
			header->width == 61 || 
			header->width == 65 || 
			header->width == 69 || 
			header->width == 73 || 
			header->width == 77 || 
			header->width == 81 || 
			header->width == 85 || 
			header->width == 89 || 
			header->width == 93 || 
			header->width == 97 || 
			header->width == 101 || 
			header->width == 105 || 
			header->width == 109 || 
			header->width == 113 || 
			header->width == 117 || 
			header->width == 121 || 
			header->width == 125 || 
			header->width == 129 || 
			header->width == 133 || 
			header->width == 137 || 
			header->width == 141 || 
			header->width == 145 || 
			header->width == 149 || 
			header->width == 153 || 
			header->width == 157 || 
			header->width == 161 || 
			header->width == 165 || 
			header->width == 169 || 
			header->width == 173 || 
			header->width == 177 || 
			header->width == 181 || 
			header->width == 185 || 
			header->width == 189 || 
			header->width == 193 || 
			header->width == 197 || 
			header->width == 201 || 
			header->width == 205 || 
			header->width == 209 || 
			header->width == 213 || 
			header->width == 217 || 
			header->width == 221 || 
			header->width == 225 || 
			header->width == 229 || 
			header->width == 233 || 
			header->width == 237 || 
			header->width == 241 || 
			header->width == 245 || 
			header->width == 249 || 
			header->width == 253 || 
			header->width == 257 || 
			header->width == 261 || 
			header->width == 265 || 
			header->width == 269 || 
			header->width == 273 || 
			header->width == 277 || 
			header->width == 281 || 
			header->width == 285 || 
			header->width == 289 || 
			header->width == 293 || 
			header->width == 297 || 
			header->width == 301 || 
			header->width == 305 || 
			header->width == 309 || 
			header->width == 313 || 
			header->width == 317 || 
			header->width == 321 || 
			header->width == 325 || 
			header->width == 329 || 
			header->width == 333 || 
			header->width == 337 || 
			header->width == 341 || 
			header->width == 345 || 
			header->width == 349 || 
			header->width == 353 || 
			header->width == 357 || 
			header->width == 361 || 
			header->width == 365 || 
			header->width == 369 || 
			header->width == 373 || 
			header->width == 377 || 
			header->width == 381 || 
			header->width == 385 || 
			header->width == 389 || 
			header->width == 393 || 
			header->width == 397)dir++;
		
		if(	header->width == 2 || 
			header->width == 6 || 
			header->width == 10 || 
			header->width == 14 || 
			header->width == 18 || 
			header->width == 22 || 
			header->width == 26 || 
			header->width == 30 || 
			header->width == 34 || 
			header->width == 38 || 
			header->width == 42 || 
			header->width == 46 || 
			header->width == 50 || 
			header->width == 54 || 
			header->width == 58 || 
			header->width == 62 || 
			header->width == 66 || 
			header->width == 70 || 
			header->width == 74 || 
			header->width == 78 || 
			header->width == 82 || 
			header->width == 86 || 
			header->width == 90 || 
			header->width == 94 || 
			header->width == 98 || 
			header->width == 102 || 
			header->width == 106 || 
			header->width == 110 || 
			header->width == 114 || 
			header->width == 118 || 
			header->width == 122 || 
			header->width == 126 || 
			header->width == 130 || 
			header->width == 134 || 
			header->width == 138 || 
			header->width == 142 || 
			header->width == 146 || 
			header->width == 150 || 
			header->width == 154 || 
			header->width == 158 || 
			header->width == 162 || 
			header->width == 166 || 
			header->width == 170 || 
			header->width == 174 || 
			header->width == 178 || 
			header->width == 182 || 
			header->width == 186 || 
			header->width == 190 || 
			header->width == 194 || 
			header->width == 198 || 
			header->width == 202 || 
			header->width == 206 || 
			header->width == 210 || 
			header->width == 214 || 
			header->width == 218 || 
			header->width == 222 || 
			header->width == 226 || 
			header->width == 230 || 
			header->width == 234 || 
			header->width == 238 || 
			header->width == 242 || 
			header->width == 246 || 
			header->width == 250 || 
			header->width == 254 || 
			header->width == 258 || 
			header->width == 262 || 
			header->width == 266 || 
			header->width == 270 || 
			header->width == 274 || 
			header->width == 278 || 
			header->width == 282 || 
			header->width == 286 || 
			header->width == 290 || 
			header->width == 294 || 
			header->width == 298 || 
			header->width == 306 || 
			header->width == 310 || 
			header->width == 314 || 
			header->width == 318 || 
			header->width == 322 || 
			header->width == 326 || 
			header->width == 330 || 
			header->width == 334 || 
			header->width == 338 || 
			header->width == 342 || 
			header->width == 346 || 
			header->width == 350 || 
			header->width == 354 || 
			header->width == 358 || 
			header->width == 362 || 
			header->width == 366 || 
			header->width == 370 || 
			header->width == 374 || 
			header->width == 378 || 
			header->width == 382 || 
			header->width == 386 || 
			header->width == 390 || 
			header->width == 394 || 
			header->width == 398)dir+=2;
		
		if(	header->width == 3 || 
			header->width == 7 || 
			header->width == 11 || 
			header->width == 15 || 
			header->width == 19 || 
			header->width == 23 || 
			header->width == 27 || 
			header->width == 31 || 
			header->width == 35 || 
			header->width == 39 || 
			header->width == 43 || 
			header->width == 47 || 
			header->width == 51 || 
			header->width == 55 || 
			header->width == 59 || 
			header->width == 63 || 
			header->width == 67 || 
			header->width == 71 || 
			header->width == 75 || 
			header->width == 79 || 
			header->width == 83 || 
			header->width == 87 || 
			header->width == 91 || 
			header->width == 95 || 
			header->width == 99 || 
			header->width == 103 || 
			header->width == 107 || 
			header->width == 111 || 
			header->width == 115 || 
			header->width == 119 || 
			header->width == 123 || 
			header->width == 127 || 
			header->width == 131 || 
			header->width == 135 || 
			header->width == 139 || 
			header->width == 143 || 
			header->width == 147 || 
			header->width == 151 || 
			header->width == 155 || 
			header->width == 159 || 
			header->width == 163 || 
			header->width == 167 || 
			header->width == 171 || 
			header->width == 175 || 
			header->width == 179 || 
			header->width == 183 || 
			header->width == 187 || 
			header->width == 191 || 
			header->width == 195 || 
			header->width == 199 || 
			header->width == 203 || 
			header->width == 207 || 
			header->width == 211 || 
			header->width == 215 || 
			header->width == 219 || 
			header->width == 223 || 
			header->width == 227 || 
			header->width == 231 || 
			header->width == 235 || 
			header->width == 239 || 
			header->width == 243 || 
			header->width == 247 || 
			header->width == 251 || 
			header->width == 255 || 
			header->width == 259 || 
			header->width == 263 || 
			header->width == 267 || 
			header->width == 271 || 
			header->width == 275 || 
			header->width == 279 || 
			header->width == 283 || 
			header->width == 287 || 
			header->width == 291 || 
			header->width == 295 || 
			header->width == 299 || 
			header->width == 303 || 
			header->width == 307 || 
			header->width == 311 || 
			header->width == 315 || 
			header->width == 319 || 
			header->width == 323 || 
			header->width == 327 || 
			header->width == 331 || 
			header->width == 335 || 
			header->width == 339 || 
			header->width == 343 || 
			header->width == 347 || 
			header->width == 351 || 
			header->width == 355 || 
			header->width == 359 || 
			header->width == 363 || 
			header->width == 367 || 
			header->width == 371 || 
			header->width == 375 || 
			header->width == 379 || 
			header->width == 383 || 
			header->width == 387 || 
			header->width == 391 || 
			header->width == 395 || 
			header->width == 399)dir+=3;
	
	
	
}
