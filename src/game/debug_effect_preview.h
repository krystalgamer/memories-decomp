#ifndef MEMORIES_DECOMP_DEBUG_EFFECT_PREVIEW_H
#define MEMORIES_DECOMP_DEBUG_EFFECT_PREVIEW_H

#include "../types.h"
#include "duel_card.h"

void DebugEffect_UpdateViewControls(void);
void DebugEffect_UpdatePreview(void);

extern u8 *G32 gDebugEffect_pPreviewCard0;
extern u8 *G32 gDebugEffect_pPreviewCard1;
extern DuelCardRecord gDebugEffect_aPreviewCardRecords[];
extern u8 gDebugEffect_szOperandsFormat[];
extern u8 gDebugEffect_szSecondOperandMarker[];
extern u8 gDebugEffect_szFirstOperandMarker[];

#endif
