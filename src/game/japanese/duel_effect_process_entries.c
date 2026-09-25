#include "../../types.h"
#include "duel_effect_channel.h"
#include "../duel_effect_entry_occupancy.h"

#define VERSION_JAPAN
#define DuelEffect_ProcessEntries func_800393D8
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_PROCESS_RANGE_START(channel) \
    (((JapaneseDuelEffectChannel *)(channel))->range_start_5C)
#define D_800EB288 gJapanese_DuelEffectEntries
#define D_80090F58 gJapanese_DuelEffectEntryHandlers
#include "../duel_effect_process_entries.c"
