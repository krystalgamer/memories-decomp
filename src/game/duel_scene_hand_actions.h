#ifndef MEMORIES_DECOMP_DUEL_SCENE_HAND_ACTIONS_H
#define MEMORIES_DECOMP_DUEL_SCENE_HAND_ACTIONS_H

#include "../types.h"

/* Duel scene-state 4, the hand navigation and card play phase, and the state
 * private to it. DuelScene_UpdateHandActions (duel_scene_hand_actions.c) is
 * the only source in the tree that names any of the three globals below;
 * duel_scene_callbacks.c takes the prototype for the phase callback table.
 *
 * Every access to all three is gp-relative in retail: 5, 3 and 4 relocations,
 * none through %hi/%lo. So these are the plain scalar declarations. That the
 * distinction is real rather than an artefact of the pattern is visible in
 * the same function, where D_800EAE88 is reached through %hi/%lo and
 * gp-relatively never.
 *
 * A fourth byte the function uses, the scripted path's target slot at
 * 0x8009B20A, is not here: it falls inside D_8009B208[8], which
 * duel_scene_resume.h already declares, and the unit reads it as
 * D_8009B208[2].
 */

/* Whether the scripted path has a card to stack onto the hand. The AI step
 * clears it, and raises it when the selection's field1 byte D_800EAE88[1] (ai.h)
 * is non-zero; the following step keeps bit 0 and tests the result, and the
 * delayed step tests bit 0 again to decide whether the hand-stack child is
 * spawned.
 *
 * The two loads are lhu and the three stores are sh. Both reads are masked
 * with 1, and GCC loads unsigned for a mask that discards the sign, so the
 * declared sign does not show in the object code and the stores cannot settle
 * it either; s16 is kept from the declaration the unit matched with. The next
 * named symbol is D_8009B1A0, two bytes on, so the declared width fills the
 * gap exactly. */
extern s16 D_8009B19E;

/* The cursor into the AI's pending selection list D_800EAE88 (ai.h). The
 * scripted step sets it to -1 through a signed lvalue cast, and each following
 * step pre-increments it and reads D_800EAE88 at the new index, so the walk
 * starts at the list's first entry.
 *
 * Declared u8, as the private declaration had it: the single load is lbu and
 * the two stores are sb, and the signed views are casts at the use sites --
 * the arrangement duel_hand.h records for D_8009B1EC, where the declaration
 * stays unsigned and the cast at the use is what keeps the signed read. The
 * gap to the next named symbol is six bytes, and D_8009B1E4 below sits inside
 * it, so that gap is not evidence of a wider object. */
extern u8 D_8009B1E2;

/* The step delay for that walk. Each selection entry loads it with 6, every
 * tick takes one off, and the step runs once the signed value reaches zero or
 * below, reloading 6.
 *
 * Declared u16. The single load is lhu and the three stores are sh; the one
 * signed read is spelled (s16) at the bound test, which is where the private
 * declaration put it. */
extern u16 D_8009B1E4;

/* "Com Error SelCard\n", the scripted path's complaint about a selection byte
 * below 0xB. It stays in generated .rodata: retail keeps the string at
 * 0x80010060, four strings and three other units' tables ahead of this
 * function's jump table at 0x80010148, so one object cannot emit both. */
extern const char D_80010060[];

/* The hand card's display object as DuelScene_UpdateHandActions reads it.
 * The position pairs are unions because retail copies them as words, and the
 * members are limited to what that function touches; the canonical record is
 * DisplayObject in display_object.h. */
typedef struct {
    u8 pad_00[8];
    u16 flags;                  /* 0x08 */
    u8 pad_0A[2];
    u32 color;                  /* 0x0C */
    u8 pad_10[6];
    u8 depth;                   /* 0x16 */
    u8 pad_17[0xA];
    u8 face;                    /* 0x21 */
    u8 pad_22[2];
    void (*update)();           /* 0x24 */
    union { struct { s16 x, y; } xy; s32 word; } target;  /* 0x28 */
    union { struct { u16 x, y; } xy; s32 word; } saved;   /* 0x2C */
    union { struct { u16 x, y; } xy; s32 word; } pos;     /* 0x30 */
    u8 pad_34[2];
    u16 home_x;                 /* 0x36 */
    u16 home_y;                 /* 0x38 */
    u8 pad_3A[0x22];
    u8 icon_state;              /* 0x5C */
    u8 pad_5D[3];
    s16 step;                   /* 0x60 */
    u8 pad_62[6];
    u8 kind;                    /* 0x68 */
    u8 pad_69;
    u8 card_index;              /* 0x6A */
    u8 pad_6B;
    u8 field_6C;                /* 0x6C */
} HandCardObject;

typedef char HandCardObject_field_6C_offset_must_be_0x6C[
    (u32)&(((HandCardObject *)0)->field_6C) == 0x6C ? 1 : -1
];

void DuelScene_UpdateHandActions(void);

#endif
