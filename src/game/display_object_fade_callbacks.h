#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_FADE_CALLBACKS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_FADE_CALLBACKS_H

#include "../types.h"
#include "duel_effect.h"

/* The last four entries of the duel effect command table tent_SecondaryTextCommandTable
 * (duel_effect_command_table.c), in table order: DuelEffect_UpdateGlyphConceal,
 * DuelEffect_UpdateGlyphReveal, DuelEffect_UpdateGlyphConceal again, func_80039C94.
 *
 * Each is a two-phase fade over the record's colour bytes, keyed on
 * DisplayObjectFade_MarkInitialized for its first-frame setup and on
 * DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE in byte 0x13 for the phase.
 *
 *   DuelEffect_UpdateGlyphReveal  Raises byte 4, then byte 5, by 16 per elapsed frame
 *                  (D_8009B0D8), copying them into bytes 6 and 7. When byte 5
 *                  passes 0x7F it clears bytes 0x13 to 0x15 itself.
 *   DuelEffect_UpdateGlyphConceal  Starts the colour word at +4 at 0x80808080 and lowers
 *                  bytes 4/5, then 6/7, to zero at the same rate.
 *   func_80039C94  Counts byte 4 down from a delay derived from the
 *                  halfwords at +0xC and +0xE, then raises bytes 8 to 0xA
 *                  by 4 a frame until they reach 0x40.
 *
 * The last two end by releasing the record's occupancy slot through
 * DisplayObjectFade_ReleaseChannel. */
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *record);
void func_80039C94(DuelEffectChannel *record);

#endif
