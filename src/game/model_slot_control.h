#ifndef MEMORIES_DECOMP_MODEL_SLOT_CONTROL_H
#define MEMORIES_DECOMP_MODEL_SLOT_CONTROL_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model.h"

/* Per-frame animation tick for one model slot, selected by index into
 * gModel_aSlots. It returns immediately when the slot is inactive, otherwise
 * advances or rewinds the frame counter and, at the end of an animation with
 * no hold pending, switches to the queued one. Model_UpdateScene in
 * model_scene_states.c drives slots 0, 1 and 2 with it once per frame. It
 * operates on a gModel_aSlots record like the other Model_ slot functions,
 * so it carries that owner. */
void Model_UpdateSlotAnimation(s32 index);

/* A loaded module's control entry follows its first word. */
typedef s32 (*ModelControlHandler)(u8 *context, s32 command);

/* Non-owning view of the signed command words at +0xD08 through +0xD10.
 * Entries 0/1 are the first/second-star module commands; entry 2 is
 * primary_module_command. Preserve unchecked selection and this view. */
typedef struct {
    u8 prefix[0xD08];
    s32 commands[3];
} ModelControlCommandView;

typedef char ModelControlCommandOffset[
    (u32)&((ModelControlCommandView *)0)->commands ==
        (u32)&((ModelSlot *)0)->metadata + 0x10 ? 1 : -1
];
typedef char ModelControlCommandMembersMatchSlot[
    (u32)&((ModelControlCommandView *)0)->commands[0] ==
        (u32)&((ModelSlot *)0)->metadata.first_star_module_command &&
    (u32)&((ModelControlCommandView *)0)->commands[1] ==
        (u32)&((ModelSlot *)0)->metadata.second_star_module_command &&
    (u32)&((ModelControlCommandView *)0)->commands[2] ==
        (u32)&((ModelSlot *)0)->metadata.primary_module_command &&
    sizeof(((ModelControlCommandView *)0)->commands) == 12 ? 1 : -1
];
typedef char ModelControlCommandViewFitsSlot[
    sizeof(ModelControlCommandView) <= sizeof(ModelSlot) ? 1 : -1
];

void Model_UpdateSlotControl(s32 index);

void Model_InitSlot(s32 slot, u8 *hmd, s32 size, s32 flags);

/* One model slot's reset, the initializer half of this unit: it zeroes the
 * slot's first 0x388 words through Model_FillWords and then writes the
 * defaults -- MODEL_FIXED_HALF at +0xDA0/+0xDA4/+0xDA8, MODEL_FIXED_ONE at
 * +0xDB0/+0xDB4/+0xDB8, 0x80 at +0xDC0..+0xDC2, and the mode byte at +0xE16.
 *
 * The layout pass below is its consumer: the unit's own note records that the
 * two are contiguous, 0x8005611C ending exactly where 0x80056250 begins, and
 * that the pass switches on the same +0xE16 the reset writes. */
void Model_ResetSlot(s32 arg0);

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * Model_SetSlotPartSpeeds before Model_ApplyTextureTint draws it.
 *
 * arg1 is a pointer used only as a non-null gate. Both callers now take it
 * from high_memory_addresses.h: the scalar D_80010000 or the indexed
 * prefix's payload_bases[0], rather than a private integer declaration. */
void Model_LoadSlot(s32 arg0, u8 *arg1, s32 arg2, s32 arg3);

/* Starts an animation on one of the gModel_aSlots slots. `index` picks the slot,
 * at a stride of 0xE20, and `anim` picks an entry within it.
 *
 * Two things a prototype cannot say. It may do nothing at all: an entry whose
 * length is zero returns immediately, and so does a slot that already has an
 * animation running when `flag` is zero. And `flag` only means anything in
 * that second case -- when something is already playing it commits the queued
 * animation and re-enters as (index, 0, 0), which is why the defining file
 * needs this declaration before the definition rather than only its callers.
 *
 * All three callers already spelled it exactly this way, as did the file's own
 * forward declaration. */
void Model_ControlSlotAnimation(s32 index, s32 anim, s32 flag);

#endif
