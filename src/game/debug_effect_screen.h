#ifndef MEMORIES_DECOMP_DEBUG_EFFECT_SCREEN_H
#define MEMORIES_DECOMP_DEBUG_EFFECT_SCREEN_H

#include "../types.h"
#include "duel_card.h"

/* The developer effect-preview screen. DuelScene_UpdateEffectPreview is the gDuel_apfnSceneStateHandler duel
 * phase entry for it: it runs the preview builder and then prints the
 * "EFFECT = %2d %2d" line with one of two divider strings, chosen by the
 * coordinate-axis byte in gDebugEffect_abPreviewState.
 *
 * The other two functions this unit defines are reached only from within it. */
extern u8 gDebugEffect_abPreviewState[6];
extern u8 gJapanese_DebugEffectPreviewState[6];
extern u8 *D_8009B180;
extern u8 *D_8009B184;
extern DuelCardRecord D_801A7B80[];

void DuelScene_UpdateEffectPreview(void);

#endif
