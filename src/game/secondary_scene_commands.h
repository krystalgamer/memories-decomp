#ifndef MEMORIES_DECOMP_SECONDARY_SCENE_COMMANDS_H
#define MEMORIES_DECOMP_SECONDARY_SCENE_COMMANDS_H

#include "../types.h"

struct DuelEffectChannel;

void Text_NoOpSecondary12(void);
void Text_NoOpSecondary13(void);
void Text_OpenCampaignMap(struct DuelEffectChannel *object);

extern u8 gCampaignMap_bEntryLocation[];

#endif
