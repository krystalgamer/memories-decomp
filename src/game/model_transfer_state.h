#ifndef MEMORIES_DECOMP_MODEL_TRANSFER_STATE_H
#define MEMORIES_DECOMP_MODEL_TRANSFER_STATE_H

#include "../ygo_types.h"
#include "model_effect_key.h"

void Model_ResetEffectKeyQueue(void);
s32 Model_IsEffectKeyQueueEmpty(void);
s32 Model_GetCurrentEffectKeyAddress(void);
/* Refreshes slot-backed kinds 2/3; a null argument selects the current key. */
void Model_RefreshEffectKeySlotValues(Key *key);

#endif
