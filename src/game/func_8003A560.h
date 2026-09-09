#ifndef MEMORIES_DECOMP_FUNC_8003A560_H
#define MEMORIES_DECOMP_FUNC_8003A560_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

/* The wider display-effect view owned by this callback. The lifecycle helpers
 * consume the same record through their narrower DisplayEffectState view. */
typedef struct {
    u8 pad_00[0x30];
    s8 field_30;
    u8 field_31;
    u8 field_32;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x04];
    u8 field_3C;
    u8 pad_3D;
    u16 field_3E;
} DisplayEffectVramState;

typedef struct {
    u8 image[0x18000];
    u8 clut[0x400];
    u8 extra[0x800];
    RECT image_rect;
    RECT clut_rect;
} DisplayEffectVramSlot;

typedef char DisplayEffectVramSlot_size_must_be_0x18C10[
    sizeof(DisplayEffectVramSlot) == 0x18C10 ? 1 : -1
];

void func_8003A560(DisplayEffectVramState *state);

#endif
