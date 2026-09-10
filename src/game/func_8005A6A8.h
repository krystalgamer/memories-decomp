#ifndef MEMORIES_DECOMP_FUNC_8005A6A8_H
#define MEMORIES_DECOMP_FUNC_8005A6A8_H

#include "../types.h"

/* Distance-to-camera fade and pan for one sound source at (arg0, arg1). The
 * square root of the distance from the listener record at D_800F56F0, minus
 * 700, sets the volume byte in *arg2 (0xFF down to 0x10). The signed
 * projection onto the listener direction, divided by 16 and clamped to
 * +-0x7F, sets the pan in *arg3. */
void func_8005A6A8(s32 arg0, s32 arg1, u8 *arg2, u8 *arg3);

#endif
