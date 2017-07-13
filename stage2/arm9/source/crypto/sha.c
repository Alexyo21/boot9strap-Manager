#include "sha.h"
#include "../memory.h"


void sha_init(u32 mode)
{
    while(*REG_SHA_CNT & 1);
    *REG_SHA_CNT = mode | SHA_CNT_OUTPUT_ENDIAN | SHA_NORMAL_ROUND;
}

void sha_update(const void* src, u32 size)
{    
    const u32* src32 = (const u32*)src;
    
    while(size >= 0x40) {
        while(*REG_SHA_CNT & 1);
        for(u32 i = 0; i < 4; i++) {
            *REG_SHA_INFIFO = *src32++;
            *REG_SHA_INFIFO = *src32++;
            *REG_SHA_INFIFO = *src32++;
            *REG_SHA_INFIFO = *src32++;
        }
        size -= 0x40;
    }
    while(*REG_SHA_CNT & 1);
    memcpy((void*)REG_SHA_INFIFO, src32, size);
}

void sha_get(void* res) {
    *REG_SHA_CNT = (*REG_SHA_CNT & ~SHA_NORMAL_ROUND) | SHA_FINAL_ROUND;
    while(*REG_SHA_CNT & SHA_FINAL_ROUND);
    while(*REG_SHA_CNT & 1);
    memcpy(res, (void*)REG_SHA_HASH, (256 / 8));
}

void sha(void* res, const void* src, u32 size, u32 mode) {
    sha_init(mode);
    sha_update(src, size);
    sha_get(res);
}
