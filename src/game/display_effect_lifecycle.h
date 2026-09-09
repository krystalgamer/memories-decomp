#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H

#include "../types.h"

/* Fields the display-effect lifecycle helpers read and write while arming,
 * resetting and tearing down an effect slot. Other translation units describe
 * the same memory through their own narrower views. */
typedef struct {
    u8 pad_00[0x31];
    u8 field_31;
    u8 field_32;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x06];
    u16 field_3E;
} DisplayEffectState;

s32 func_80039F1C(DisplayEffectState *object);
void func_80039F44(DisplayEffectState *object);
void func_80039F90(void **objects);
void func_80039FD4(u8 *object);

/* The effect object func_80038EB0 last armed: it stores a pointer into
 * D_800EB010 here at three sites, and the readers walk that record by
 * byte offset (0x30, 0x32, 0x33, 0x3C, 0x40, 0x42 and 0x44 across the
 * three reading units) or hand it to func_80039FD4, whose parameter is
 * u8 *. That is why the pointer is u8 * rather than one of the partial
 * views: MenuRecord names none of 0x33, 0x3C, 0x40, 0x42 or 0x44,
 * DisplayEffectState above stops at 0x40, and func_8003787C.c's one-field
 * view is applied with a cast where it reads. Retail reaches the pointer
 * gp-relative at every one of its thirteen sites. */
extern u8 *D_8009B328;
void func_80039FF8(DisplayEffectState *object);

#endif
