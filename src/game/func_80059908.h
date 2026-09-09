#ifndef MEMORIES_DECOMP_FUNC_80059908_H
#define MEMORIES_DECOMP_FUNC_80059908_H

#include "../types.h"

/* Reads `length` bytes of a host file into memory through the SN fileserver,
   in 0x1000-byte chunks. A zero `length` means "to the end of the file".
   Returns the number of bytes transferred, or 0 if the open failed or a chunk
   came up short. */
s32 func_80059908(s32 path, s32 destination, s32 offset, s32 length);

#endif
