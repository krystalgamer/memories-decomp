#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_FADE_CALLBACKS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_FADE_CALLBACKS_H

#include "../types.h"
#include "duel_effect.h"

/* The last four entries of the duel effect command table gText_SecondaryCommandHandlers
 * (duel_effect_command_table.c), in table order: DuelEffect_UpdateGlyphConceal,
 * DuelEffect_UpdateGlyphReveal, DuelEffect_UpdateGlyphConceal again, DuelEffect_UpdateDelayedGlyphRotation.
 *
 * The paired shade/geometry transitions and delayed three-axis rotation use
 * DisplayObjectFade_MarkInitialized for their first-invocation setup and
 * DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE in byte 0x13 for the phase.
 *
 *   DuelEffect_UpdateGlyphReveal  Raises byte 4, then byte 5, by 16 per elapsed frame
 *                  (D_8009B0D8), copying them into bytes 6 and 7. When byte 5
 *                  passes 0x7F it clears bytes 0x13 to 0x15 itself.
 *   DuelEffect_UpdateGlyphConceal  Starts the colour word at +4 at 0x80808080 and lowers
 *                  bytes 4/5, then 6/7, to zero at the same rate.
 *   DuelEffect_UpdateDelayedGlyphRotation counts the position-derived byte
 *   delay down, then raises rotation components +8..+A by four per invocation
 *   until0x40. The renderer multiplies them by16; these are not color bytes.
 *
 * The last two end by releasing the record's occupancy slot through
 * DisplayObjectFade_ReleaseChannel. */
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *record);
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record);

#endif
