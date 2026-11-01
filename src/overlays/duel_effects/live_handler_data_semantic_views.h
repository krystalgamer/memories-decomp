#ifndef MEMORIES_DECOMP_DUEL_LIVE_HANDLER_DATA_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_LIVE_HANDLER_DATA_SEMANTIC_VIEWS_H

#include "tile_effect.h"
#include "effect_10.h"
#include "gather_effect.h"
#include "effect_17.h"

extern TileEffectDescriptor gDuelEffect_DestroyFlamesConfigs[2];
extern GsIMAGE gDuelEffect_DestroyFlamesTextureImage;
extern DuelEffect10Config gDuelEffect_TerrainChangeConfigs[6];
extern VECTOR gDuelEffect_TerrainChangeInitialScale;
extern GsIMAGE gDuelEffect_TerrainChangeTextureImages[21];
extern GatherEffectDescriptor gDuelEffect_GuardianStarBonusConfig;
extern VECTOR gDuelEffect_GuardianStarBonusInitialScale;
extern DuelEffect17Config gDuelEffect_BoardDestructionConfigs[2];
extern VECTOR gDuelEffect_BoardDestructionInitialScale;

#endif
