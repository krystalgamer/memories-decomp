#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_OCCUPANCY_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_OCCUPANCY_H

#include "../types.h"

typedef struct {
    u8 pad_00[0x11];
    u8 flags_11;
    u8 field_12;
    u8 pad_13[5];
} JapaneseDuelEffectEntry;

typedef char JapaneseDuelEffectEntry_size_must_be_0x18[
    sizeof(JapaneseDuelEffectEntry) == 0x18 ? 1 : -1
];

void func_80035CA8(s32 value);
void DuelEffect_ResetOccupancy(void);
s32 func_80035D10(void);
void DuelEffect_ClearMatchingMarker(s32 value);
void DuelEffect_ResetEntryMarkers(void);

#endif
