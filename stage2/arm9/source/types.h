/*
*   types.h
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//Common data types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef unsigned int	uint;
typedef unsigned char	uchar;

#define CFG9_SYSPROT9   (*(vu8 *)0x10000000)
#define CFG9_SYSPROT11  (*(vu8 *)0x10000001)
#define min(a,b) \
    (((a) < (b)) ? (a) : (b))
#define getbe16(d) \
    ((((u8*)d)[0]<<8) | ((u8*)d)[1])
#define getbe32(d) \
    ((((u32) getbe16(d))<<16) | ((u32) getbe16(d+2)))
	
typedef enum
{
    INIT_SCREENS = 0,
    WAIT_BOOTROM11_LOCKED,
    PREPARE_ARM11_FOR_FIRMLAUNCH,
    ARM11_READY
} Arm11Operation;
