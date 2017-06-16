/*
*   This file is part of Luma3DS
*   Copyright (C) 2016 Aurora Wright, TuxSH
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b of GPLv3 applies to this file: Requiring preservation of specified
*   reasonable legal notices or author attributions in that material or in the Appropriate Legal
*   Notices displayed by works containing it.
*/

/*
*   waitInput function based on code by d0k3 https://github.com/d0k3/Decrypt9WIP/blob/master/source/hid.c
*/

#pragma once

#include "types.h"

#define TICKS_PER_SEC       67027964ULL
#define REG_TIMER_CNT(i)    *(vu16 *)(0x10003002 + 4 * i)
#define REG_TIMER_VAL(i)    *(vu16 *)(0x10003000 + 4 * i)

void mcuPowerOff(void);
void wait(u64 amount);
void error(const char *fmt, ...);


//timer
// see: https://www.3dbrew.org/wiki/TIMER_Registers
#define TIMER_VAL0  ((vu16*)0x10003000)
#define TIMER_VAL1  ((vu16*)0x10003004)
#define TIMER_VAL2  ((vu16*)0x10003008)
#define TIMER_VAL3  ((vu16*)0x1000300C)
#define TIMER_CNT0  ((vu16*)0x10003002)
#define TIMER_CNT1  ((vu16*)0x10003006)
#define TIMER_CNT2  ((vu16*)0x1000300A)
#define TIMER_CNT3  ((vu16*)0x1000300E)

#define TIMER_COUNT_UP  0x0004
#define TIMER_ACTIVE    0x0080
//#define TICKS_PER_SEC   67027964ULL

void timer_start( void );
void timer_stop( void );
u64 timer_ticks( void );
u64 timer_msec( void );
u64 timer_sec( void );