#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H

#include "../types.h"
#include "menu_record.h"

/* Fields the display-effect lifecycle helpers read and write while arming,
 * resetting and tearing down an effect slot.
 *
 * This is a narrower view of MenuRecord in menu_record.h: the records these
 * helpers are handed are elements of D_800EB010, and every offset the two
 * spell in common agrees -- 0x30, 0x32, 0x33 (display_effect_step there),
 * 0x34 and now 0x36. The view stays because the helpers' own signatures are
 * written against it; what it adds over the full record is nothing, and a
 * consumer that needs the tail should take MenuRecord, as DisplayEffect_UpdatePositionTween in
 * display_effect_update_callbacks.c does. */
typedef struct {
    u8 pad_00[0x31];
    u8 resource_entry_index;
    u8 variant_update_flags;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x06];
    u16 variant_countdown;
} DisplayEffectState;

/* Zero on the first mark; one when bit 0x80 was already present. */
s32 DisplayEffect_MarkInitialized(DisplayEffectState *object);
/* Partial state/position/delay reset, with one RNG call on every invocation. */
void DisplayEffect_ResetRecordState(DisplayEffectState *object);
/* Exactly three addressable slots, released and nulled from index 2 to 0. */
void DisplayEffect_ReleaseObjectRow(void **objects);
/* Deactivate the fixed record and clear only its three primary object slots.
 * This does not free arena storage, clear the step or reset shared selectors. */
void DisplayEffect_ReleaseRecord(MenuRecord *record);

/* gText_DisplayEffectRecord moved to menu_record.h with the MenuRecord * type it now
 * carries; it points into D_800EB010, which is declared there too. */
void DisplayEffect_FinishStepWhenIdle(DisplayEffectState *object);

#endif
