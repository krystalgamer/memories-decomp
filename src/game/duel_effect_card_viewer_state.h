#ifndef MEMORIES_DECOMP_DUEL_EFFECT_CARD_VIEWER_STATE_H
#define MEMORIES_DECOMP_DUEL_EFFECT_CARD_VIEWER_STATE_H

#include "../types.h"

/* gDuelEffect_apfnStateHandler entry: the card-detail effect screen. Builds the
 * portrait, the stat text boxes and the sliding background, and steps the slide
 * each frame. Its body is register-pinned -- see the note in the source -- so
 * it is worth reading before touching. */
void DuelEffect_UpdateCardViewerState(void);

#endif
