#ifndef MEMORIES_DECOMP_MODEL_SLOT_PROPERTIES_H
#define MEMORIES_DECOMP_MODEL_SLOT_PROPERTIES_H

#include "../psyq/libgte.h"
#include "model.h"

s32 func_800593D0(s32 slot, s32 part, s32 vertex, VECTOR *out);
ModelSlotS32Quad *func_8005949C(s32 index);
void func_800594C0(s32 index, ModelSlotS32Quad *source);
u8 *func_80059520(s32 index);
void func_80059590(
    s32 index,
    s32 fourth,
    s32 first,
    s32 second,
    s32 third
);
void func_800595C8(s32 index, s32 x, s32 y, s32 z);

#endif
