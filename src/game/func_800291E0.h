#ifndef MEMORIES_DECOMP_FUNC_800291E0_H
#define MEMORIES_DECOMP_FUNC_800291E0_H

#include "../types.h"

/* Builds the display objects for slot `index` of the D_800EA0E8
 * effect-resource records and returns the first of them. Its body is still the
 * target's own words -- func_800291E0.c holds it as an asm block -- so u8 * is
 * what its three callers agreed on rather than a recovered type;
 * func_800283F4.c casts the result to DisplayObject *, which is the closest
 * thing to evidence there is.
 *
 * src/overlays/password/shop_setup.c declares it again and cannot consume this
 * header -- overlay sources are a separate build -- and spells the return
 * PasswordCardPreviewView *, a fourth view of the same object. */
u8 *func_800291E0(s32 index, s32 arg1, s32 arg2);

/* Releases both display-object words of one record through func_8004036C and
 * zeroes them. */
void func_80029528(s32 index);

#endif
