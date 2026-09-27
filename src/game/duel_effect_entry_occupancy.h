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

/* The European build's entries: 800 of them, 0x16 bytes each. The named
   fields are the ones matched code reads, under their US names: the
   marker flag and channel byte (DuelEffect_ClearMatchingMarker,
   DuelEffect_ResetEntryMarkers) and the fade fields of func_80039AFC and
   func_80039C94. */
#define EUROPEAN_DUEL_EFFECT_ENTRY_COUNT 800

typedef struct {
    u16 code_00;
    u16 field_0C;   /* 0x02 */
    u16 field_0E;   /* 0x04 */
    u8 field_04;    /* 0x06 */
    u8 field_05;
    u8 field_06;
    u8 field_07;
    u8 field_08;    /* 0x0A */
    u8 field_09;
    u8 field_0A;
    u8 pad_0D[2];
    u8 flags_11;    /* 0x0F */
    u8 field_12;    /* 0x10 */
    u8 field_13;    /* 0x11 */
    u8 field_14;
    u8 field_15;    /* 0x13 */
    u8 pad_14[2];
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
