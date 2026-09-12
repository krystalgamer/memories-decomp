#ifndef MEMORIES_DECOMP_MODEL_SLOT_SETUP_H
#define MEMORIES_DECOMP_MODEL_SLOT_SETUP_H

#include "../types.h"

#ifdef MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS
void func_8004CB0C(s32 slot, s32 arg1, s32 arg2, s32 arg3);
#else
void func_8004CB0C(void);
#endif

/* One model slot's reset, the initializer half of this unit: it zeroes the
 * slot's first 0x388 words through func_8005B5FC and then writes the
 * defaults -- MODEL_FIXED_HALF at +0xDA0/+0xDA4/+0xDA8, MODEL_FIXED_ONE at
 * +0xDB0/+0xDB4/+0xDB8, 0x80 at +0xDC0..+0xDC2, and the mode byte at +0xE16.
 *
 * The layout pass below is its consumer: the unit's own note records that the
 * two are contiguous, 0x8005611C ending exactly where 0x80056250 begins, and
 * that the pass switches on the same +0xE16 the reset writes. */
void func_8005611C(s32 arg0);

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * func_8005A468 before func_800582C0 draws it.
 *
 * arg1 is a pointer used only as a non-null gate. Both callers now take it
 * from high_memory_addresses.h: the scalar D_80010000 or the indexed
 * prefix's payload_bases[0], rather than a private integer declaration. */
void func_80056250(s32 arg0, u8 *arg1, s32 arg2, s32 arg3);

#endif
