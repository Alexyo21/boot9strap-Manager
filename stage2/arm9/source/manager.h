
#pragma once

#include "types.h"
#include "crypto/crypto.h"

#define FULL_NAND	(1<<0)
#define KEEP_HAX	(1<<1)

#define BUFFER_ADDRESS  ((u8*) 0x18000000)
#define BUFFER_MAX_SIZE ((u32) (1 * 1024 * 1024))
#define NAND_SECTOR_SIZE 0x200
#define SECTORS_PER_READ (BUFFER_MAX_SIZE / NAND_SECTOR_SIZE)


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
