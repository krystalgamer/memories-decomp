#ifndef MEMORIES_DECOMP_DUEL_CARD_EFFECTS_H
#define MEMORIES_DECOMP_DUEL_CARD_EFFECTS_H

#include "../types.h"

typedef struct DuelFieldEffectObject DuelFieldEffectObject;

/* The display/effect object shared by the field-wide transition handlers. */
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

/* Adjacent application handlers from the magic-effect group table plus the
 * per-card reveal callback. They share the D_801A7AD8 field walk,
 * D_8009B220 phase state, effect-object requests and completion gates. */
void DuelEffect_ApplyLifePointRecovery(void);
void DuelEffect_ApplyDirectDamage(void);
void DuelEffect_ApplyMonsterRemoval(void);
void DuelEffect_ApplyBoardDestruction(void);
void DuelEffect_ApplyStopDefense(void);
void DuelEffect_ApplyRaigeki(void);
void DuelEffect_UpdateRevealCard(DuelFieldEffectObject *object);
void DuelEffect_ApplyDarkPiercingLight(void);
void DuelEffect_ApplyStatPenalty(void);

#endif
