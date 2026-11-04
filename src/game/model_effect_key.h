#ifndef MEMORIES_DECOMP_MODEL_EFFECT_KEY_H
#define MEMORIES_DECOMP_MODEL_EFFECT_KEY_H

#include "../types.h"
#include "../ygo_types.h"

#define MODEL_EFFECT_KEY_OFFSET(member) ((u32)&(((Key *)0)->member))

/* Model_RefreshEffectKeySlotValues and Model_CaptureKeyframePose fill a
   resolved record through Model_CopySlotPosition for endpoint kinds 2 and 3:
   the slot's cached X, Y and Z, then the fourth cached_position halfword,
   which stays opaque. */
typedef struct {
    u16 x;
    u16 y;
    u16 z;
    u16 field_6;
} ModelEffectResolvedValues;

typedef struct ModelEffectKey {
    ModelEffectEndpoint requested[2];
    ModelEffectResolvedValues resolved[2];
    s16 magnitude;
    /* The key's length: Model_PrepareEffectKey derives it, Model_UpdateEffectKeyframes steps
       progress up to it (0x4000 marks an open-ended key) and Model_EvaluateKeyframeSpline
       divides by it. It was called radius, which Model_PrepareEffectKey's notes
       already reject. */
    u16 duration;
    u16 progress;
    u8 ready;
    u8 pad_27;
} Key;

typedef char ModelEffectKey_size_must_be_0x28[
    sizeof(Key) == 0x28 ? 1 : -1
];
typedef char ModelEffectKey_alignment_must_support_halfwords[
    __alignof__(Key) >= __alignof__(s16) ? 1 : -1
];
typedef char ModelEffectResolvedValues_size_must_be_0x8[
    sizeof(ModelEffectResolvedValues) == 0x8 ? 1 : -1
];
typedef char ModelEffectKey_resolved_offset_must_be_0x10[
    MODEL_EFFECT_KEY_OFFSET(resolved) == 0x10 ? 1 : -1
];
typedef char ModelEffectKey_magnitude_offset_must_be_0x20[
    MODEL_EFFECT_KEY_OFFSET(magnitude) == 0x20 ? 1 : -1
];
typedef char ModelEffectKey_ready_offset_must_be_0x26[
    MODEL_EFFECT_KEY_OFFSET(ready) == 0x26 ? 1 : -1
];

#undef MODEL_EFFECT_KEY_OFFSET

#endif
