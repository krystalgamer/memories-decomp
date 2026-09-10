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

/* Eight adjacent handlers of gDuelEffect_apfnGroupHandler plus the transition
 * callback installed by func_80025BEC. They share the D_801A7AD8 field walk,
 * D_8009B220 phase state, effect-object requests and completion gates.
 *
 * func_800250C8 runs the table-driven LP change phases -- recovery values
 * scaled by 100 -- func_8002525C applies one of the five direct-damage cards,
 * func_8002538C and func_800257A0 are the two remaining LP arms, and
 * DuelEffect_UpdateFieldMarker repositions the field marker object from the
 * card record it follows. All five are reached only as table entries, so they
 * take no arguments and report through D_8009B220 like every other handler. */
void func_800250C8(void);
void func_8002525C(void);
void func_8002538C(void);
void func_800257A0(void);
void DuelEffect_UpdateFieldMarker(void);
void func_8002596C(void);
void func_80025B28(DuelFieldEffectObject *object);
void func_80025BEC(void);
void func_80025D30(void);

#endif
