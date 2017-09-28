#include <stdio.h>

#include "sha.h"
#include "hashfile.h"

#include "../fs.h"
#include "../draw.h"
#include "../fatfs/ff.h"
#include "../memory.h"
#include "../manager.h"

u32 GetHashFromFile(const char* filename, u32 offset, u8* hash)
{
    // uses the standard buffer, so be careful
    u8* buffer = BUFFER_ADDRESS;
    
    if (!Open_File(filename,FA_READ))
        return HASH_FAILED;
    u64 size = Size_File();
	
	
    
	sha_init(SHA_256_MODE);
    for (u64 i = 0; i < size; i += BUFFER_MAX_SIZE) {
        u32 read_bytes = min(BUFFER_MAX_SIZE, (size - i));
		
        //fonctionne pas a regarder
		if (size >= 0x100000) DrawStringFColor(COLOR_WHITE, COLOR_BLACK, 10, 10, true,"Verifying dump via .SHA... %3llu%%", (i * 100) / size);
		
        if(!Read_File(buffer, read_bytes, offset + i)) {
            Close_File();
            return HASH_FAILED;
        }
        sha_update(buffer, read_bytes);
    }
    sha_get(hash);
   
    Close_File();
    
    return HASH_VERIFIED;
}

u32 CheckHashFromFile(const char* filename, u32 offset, const u8* hash)
{
    u8 digest[32];
    
    if (GetHashFromFile(filename, offset, digest) != 0)
		return HASH_FAILED;
	
	return (memcmp(hash, digest, 32) == 0) ? HASH_VERIFIED : HASH_FAILED; 
}

u32 HashVerifyFile(const char* filename)
{
    char hashname[64];
    u8 hash[32];
    
    snprintf(hashname, 64, "%s.sha", filename);
    if (!Open_File(hashname,FA_READ))
		return HASH_NOT_FOUND;
	Read_File(hash, 32, 0);
	Close_File();
	
	return CheckHashFromFile(filename, 0, hash);
}


u32 CheckNandDumpIntegrity(const char* path) {
   u32 hash_res = HashVerifyFile(path);
    if (hash_res == HASH_FAILED) {
        return HASH_FAILED;
    }
    return hash_res;
 }