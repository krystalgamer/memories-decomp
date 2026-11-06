#include "../../types.h"
#include "../duel_effect_entry_occupancy.h"
/* This build's entries at 0x800EB158-0x800ECD78 (#2602). */
#define DUEL_EFFECT_ENTRY_STORAGE_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_ENTRY_STORAGE_COUNT JAPANESE_DUEL_EFFECT_ENTRY_COUNT

#define VERSION_JAPAN
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE JapaneseDuelEffectEntry
#include "../duel_effect_process_entries.c"
