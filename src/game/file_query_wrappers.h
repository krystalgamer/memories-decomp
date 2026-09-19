#ifndef MEMORIES_DECOMP_FILE_QUERY_WRAPPERS_H
#define MEMORIES_DECOMP_FILE_QUERY_WRAPPERS_H

#include "../types.h"

/* The size of host file `path`, found by opening it through the SN fileserver
 * and seeking to its end. Returns -1 when the open fails. */
int HostFile_GetSize(char *path);

/* Reads the whole of host file `path` to `destination` through
 * HostFile_ReadRange and returns the byte count, or 0 on failure.
 * model_texture_upload.c loads a TIM to 0x80400000 with it. */
s32 func_800598E4(char *path, char *destination);

/* Reads `length` bytes of a host file into memory through the SN fileserver,
   in 0x1000-byte chunks. A zero `length` means "to the end of the file".
   Returns the number of bytes transferred, or 0 if the open failed or a chunk
   came up short. */
s32 HostFile_ReadRange(
    char *path, char *destination, s32 offset, s32 length
);

#endif
