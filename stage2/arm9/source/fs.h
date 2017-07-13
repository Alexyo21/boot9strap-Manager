/*
*   fs.h
*/

#pragma once

#include "types.h"
bool mountSd(void);
void unmountSd(void);
bool mountCtrNand(void);

bool DebugOpenFile(const char *path);
u32 fileRead(void *dest, const char *path, u32 size, u32 maxSize);
bool fileWrite(const void *buffer, const char *path, u32 size);
bool fileDelete(const char *path);

char tab[50][32];
u32 countnamefirm[50];
u32 GetDirList(char* path);

bool ShowUnlockSequence(u32 seqlvl);

