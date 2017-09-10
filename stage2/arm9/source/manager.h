
#pragma once

#include "types.h"
#include "crypto/crypto.h"

#define FULL_NAND	(1<<0)
#define KEEP_HAX	(1<<1)

#define BUFFER_ADDRESS  ((u8*) 0x18000000)
#define BUFFER_MAX_SIZE ((u32) (1 * 1024 * 1024))
#define NAND_SECTOR_SIZE 0x200
#define SECTORS_PER_READ (BUFFER_MAX_SIZE / NAND_SECTOR_SIZE)


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




typedef struct {
    char name[16];
    u8  magic[8];
    u32 offset;
    u32 size;
    u32 keyslot;
    u32 mode;
} __attribute__((packed)) PartitionInfo;

void BS9Manager(bool NANDorSD);
u32 Menu_Dump_Restore();

void DumpBoot9_11_OTP(bool NANDorSD);

u32 DumpNand(void);
u32 RestoreNand(u32 param);
