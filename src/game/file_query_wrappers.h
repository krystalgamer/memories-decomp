#ifndef MEMORIES_DECOMP_FILE_QUERY_WRAPPERS_H
#define MEMORIES_DECOMP_FILE_QUERY_WRAPPERS_H

#include "../types.h"

/* The size of host file `value`, a path pointer passed as an int, found by
 * opening it through the SN fileserver and seeking to its end. Returns -1
 * when the open fails. */
int func_8005988C(int value);

/* Reads the whole of host file `first` to `second` through func_80059908 and
 * returns the byte count, or 0 on failure. model_texture_upload.c loads a TIM
 * to 0x80400000 with it. */
s32 func_800598E4(int first, int second);

#endif
