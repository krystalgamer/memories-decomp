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
void func_80039FF8(DisplayEffectState *object);

#endif
