#ifndef MEMORIES_DECOMP_MODEL_SLOT_PROPERTIES_H
#define MEMORIES_DECOMP_MODEL_SLOT_PROPERTIES_H

#include "../psyq/libgte.h"
#include "model.h"

/* Preserve fingerprinted overlay sources while emitting canonical symbols. */
#define func_800593D0 Model_TransformPartVertex
#define func_80059520 Model_GetSlotTintDescriptor
#define func_800595C8 Model_SetSlotAmbient

s32 Model_TransformPartVertex(s32 slot, s32 part, s32 vertex, VECTOR *out);
ModelSlotS32Quad *Model_GetSlotScale(s32 index);
void Model_SetSlotScale(s32 index, ModelSlotS32Quad *source);
u8 *Model_GetSlotTintDescriptor(s32 index);
void Model_SetSlotTintTarget(
    s32 slot,
    s32 mode,
    s32 target0,
    s32 target1,
    s32 target2
);
void Model_SetSlotAmbient(s32 index, s32 x, s32 y, s32 z);

#endif
