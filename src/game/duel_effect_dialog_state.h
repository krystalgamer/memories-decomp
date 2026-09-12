#ifndef MEMORIES_DECOMP_DUEL_EFFECT_DIALOG_STATE_H
#define MEMORIES_DECOMP_DUEL_EFFECT_DIALOG_STATE_H

#include "../types.h"

extern u16 gDuel_wEffectDialogTextID;

/* gDuelEffect_apfnStateHandler entry: runs the effect's dialog box. It opens
 * the box on the first frame, waits for TEXT_BOX_FLAG_DONE, then either opens
 * the choice list or waits for a confirm press before completing the state. */
void DuelEffect_UpdateDialogState(void);

#endif
