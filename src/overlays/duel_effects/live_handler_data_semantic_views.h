#ifndef MEMORIES_DECOMP_DUEL_LIVE_HANDLER_DATA_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_LIVE_HANDLER_DATA_SEMANTIC_VIEWS_H

#include "tile_effect.h"
#include "effect_10.h"
#include "gather_effect.h"
#include "effect_17.h"

extern TileEffectDescriptor gDuelEffect_DestroyFlamesConfigs[2] __asm__("D_8015A648");
extern GsIMAGE gDuelEffect_DestroyFlamesTextureImage __asm__("D_8015A62C");
extern DuelEffect10Config gDuelEffect_TerrainChangeConfigs[6] __asm__("D_8015AB14");
extern VECTOR gDuelEffect_TerrainChangeInitialScale __asm__("D_80146138");
extern GsIMAGE gDuelEffect_TerrainChangeTextureImages[21] __asm__("D_8015A8C8");
extern GatherEffectDescriptor gDuelEffect_GuardianStarBonusConfig __asm__("D_8015A5F8");
extern VECTOR gDuelEffect_GuardianStarBonusInitialScale __asm__("D_80146024");
extern DuelEffect17Config gDuelEffect_BoardDestructionConfigs[2] __asm__("D_8015A60C");
extern VECTOR gDuelEffect_BoardDestructionInitialScale __asm__("D_80146034");

#endif
