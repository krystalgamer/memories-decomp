#ifndef MEMORIES_DECOMP_FUNC_80041C8C_H
#define MEMORIES_DECOMP_FUNC_80041C8C_H

#include "../types.h"
#include "display_object.h"

/* The stream dispatcher consumes only ctx. func_80041D60 nevertheless emits
 * the retail four-argument call sequence: script and attribute are selected
 * on the first-time path or ambient on the cooldown path, and ctx is repeated
 * in $a3. */
#ifdef FUNC_80041C8C_WIDE_CALL
void func_80041C8C(
    DisplayObject *ctx, s32 script, s32 attribute, DisplayObject *ctx_again
);
#else
void func_80041C8C(u8 *ctx);
#endif

#endif
