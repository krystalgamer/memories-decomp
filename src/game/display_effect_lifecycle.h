#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H

#include "../types.h"
#include "menu_record.h"

/* The lifecycle helpers take the MenuRecord they arm, reset and tear down:
 * the records they are handed are elements of gDisplayEffect_aMenuRecords
 * (script_scene.c, duel_effect_text.c), and gDisplayEffect_apfnSteps calls
 * DisplayEffect_FinishStepWhenIdle with the same MenuRecord *. */

/* Zero on the first mark; one when bit 0x80 was already present. */
s32 DisplayEffect_MarkInitialized(MenuRecord *record);
/* Partial state/position/delay reset, with one RNG call on every invocation. */
void DisplayEffect_ResetRecordState(MenuRecord *record);
/* Exactly three addressable slots, released and nulled from index 2 to 0. */
void DisplayEffect_ReleaseObjectRow(struct DisplayObject **objects);
/* Deactivate the fixed record and clear only its three primary object slots.
 * This does not free arena storage, clear the step or reset shared selectors. */
void DisplayEffect_ReleaseRecord(MenuRecord *record);

/* gText_DisplayEffectRecord moved to menu_record.h with the MenuRecord * type it now
 * carries; it points into gDisplayEffect_aMenuRecords, which is declared there too. */
void DisplayEffect_FinishStepWhenIdle(MenuRecord *record);

#endif
