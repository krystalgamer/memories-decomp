#ifndef MEMORIES_DECOMP_DUEL_EFFECT_SOUND_COMMANDS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_SOUND_COMMANDS_H

#include "../types.h"
#include "duel_effect.h"

/* Four gText_SecondaryCommandHandlers entries. Text_OpenCampaignMap reads one byte from the object's
 * current stream into gCampaignMap_bEntryLocation and puts the scene mode at 5.
 *
 * Text_NoOpSecondary12 and Text_NoOpSecondary13 take NO argument at all -- they are defined
 * `void (void)` and their bodies are empty -- while the table's entry type is
 * `void (*)(DuelEffectChannel *)`. The table casts them, which is the same
 * shape #3073 settled on for Text_EmitGuardianStarGlyph: the real prototype lives here and
 * the cast is at the one place that needs it. They are also two distinct
 * addresses rather than one entry repeated. */
void Text_NoOpSecondary12(void);
void Text_NoOpSecondary13(void);
void Text_OpenCampaignMap(DuelEffectChannel *object);
void Text_ApplyFadeCommand(DuelEffectChannel *object);


#endif
