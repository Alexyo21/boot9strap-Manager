/*
*   fs.h
*/

#pragma once

#include "types.h"



bool mountSd(void);
void unmountSd(void);
bool mountCtrNand(void);

bool Open_File(const char *path, u32 flag);
bool Read_File(void* buf, u32 size, u32 foffset);
bool Write_File(const void *buffer, u32 size, u32 foffset);
void Seek_File(u32 offset);
void Sync_File();
u32  Size_File();
void Close_File();

bool fileOpen(const char* path, u32 flag);
u32 fileRead(void *dest, const char *path, u32 size, u32 maxSize);
bool fileWrite(const void *buffer, const char *path, size_t size);
bool fileDelete(const char *path);

char tab[50][64];
u32 countnamefirm[50];
u32 GetDirList(char* path);

bool ShowUnlockSequence(u32 seqlvl);
