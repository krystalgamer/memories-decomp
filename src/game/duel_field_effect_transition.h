#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_TRANSITION_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_TRANSITION_H

#include "../types.h"

typedef struct DuelFieldEffectObject DuelFieldEffectObject;

/* The display/effect object shared by the two field-wide effect steps. The
 * former private views agreed on every common offset and named complementary
 * fields: the transition owns timer/mark/index while the sweep owns
 * field_1A/flags/count. */
struct DuelFieldEffectObject {
    s16 x;
    s16 y;
    u8 pad_04[0x16];
    s16 field_1A;
    u8 flags;
    u8 count;
    u8 pad_1E[4];
    u8 timer;
    u8 pad_23;
    void (*callback)(DuelFieldEffectObject *);
    u8 pad_28[0x3F];
    u8 mark;
    u8 pad_68[2];
    u8 index;
    u8 pad_6B;
    u8 active;
};

#define DUEL_FIELD_EFFECT_OBJECT_OFFSET(member) \
    ((u32)&(((DuelFieldEffectObject *)0)->member))

typedef char DuelFieldEffectObject_size_must_be_0x70[
    sizeof(DuelFieldEffectObject) == 0x70 ? 1 : -1
];
typedef char DuelFieldEffectObject_callback_offset_must_be_0x24[
    DUEL_FIELD_EFFECT_OBJECT_OFFSET(callback) == 0x24 ? 1 : -1
];
typedef char DuelFieldEffectObject_mark_offset_must_be_0x67[
    DUEL_FIELD_EFFECT_OBJECT_OFFSET(mark) == 0x67 ? 1 : -1
];
typedef char DuelFieldEffectObject_active_offset_must_be_0x6C[
    DUEL_FIELD_EFFECT_OBJECT_OFFSET(active) == 0x6C ? 1 : -1
];

#undef DUEL_FIELD_EFFECT_OBJECT_OFFSET

void func_80025B28(DuelFieldEffectObject *object);

/* gDuelEffect_apfnGroupHandler entry: opens the duel-side effect prompt and,
 * once it is acknowledged, hands every occupied slot of the acting side to the
 * func_80025B28 animation by writing that function into each object's callback
 * slot. */
void func_80025BEC(void);

#endif
