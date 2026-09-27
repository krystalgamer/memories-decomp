#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_OCCUPANCY_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_OCCUPANCY_H

#include "../types.h"

#define JAPANESE_DUEL_EFFECT_ENTRY_COUNT 300

typedef struct {
    /* Word alignment preserves whole-record copies during compaction. */
    u32 pad_00[4];
    u8 pad_10;
    u8 flags_11;
    u8 field_12;
    u8 field_13;
    u8 pad_14;
    u8 field_15;
    u8 pad_16[2];
} JapaneseDuelEffectEntry;

typedef char JapaneseDuelEffectEntry_size_must_be_0x18[
    sizeof(JapaneseDuelEffectEntry) == 0x18 ? 1 : -1
];
typedef char JapaneseDuelEffectEntry_alignment_must_be_4[
    sizeof(struct { u8 lead; JapaneseDuelEffectEntry entry; }) == 0x1C ? 1 : -1
];

/* The European build's entries: 800 of them, 0x16 bytes each, with the
   marker flag at 0xF and the channel byte at 0x10 (DuelEffect_ClearMatching-
   Marker and DuelEffect_ResetEntryMarkers read them there). Only those two
   bytes are named; they keep the US field names. */
#define EUROPEAN_DUEL_EFFECT_ENTRY_COUNT 800

typedef struct {
    u8 pad_00[0xF];
    u8 flags_11;
    u8 field_12;
    u8 pad_11[5];
} EuropeanDuelEffectEntry;

typedef char EuropeanDuelEffectEntry_size_must_be_0x16[
    sizeof(EuropeanDuelEffectEntry) == 0x16 ? 1 : -1
];

void DuelEffect_ClearOccupancyValue(s32 value);
void DuelEffect_ResetOccupancy(void);
s32 func_80035D10(void);
void DuelEffect_ClearMatchingMarker(s32 value);
void DuelEffect_ResetEntryMarkers(void);

#endif
