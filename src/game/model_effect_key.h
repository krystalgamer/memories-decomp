#ifndef MEMORIES_DECOMP_MODEL_EFFECT_KEY_H
#define MEMORIES_DECOMP_MODEL_EFFECT_KEY_H

#include "../types.h"
#include "../ygo_types.h"

#define MODEL_EFFECT_KEY_OFFSET(member) ((u32)&(((Key *)0)->member))

typedef struct ModelEffectKey {
    ModelEffectEndpoint requested[2];
    ModelEffectEndpoint resolved[2];
    s16 magnitude;
    u16 radius;
    u16 progress;
    u8 ready;
    u8 pad_27;
} Key;

typedef char ModelEffectKey_size_must_be_0x28[
    sizeof(Key) == 0x28 ? 1 : -1
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
