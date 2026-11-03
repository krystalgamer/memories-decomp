#include "../../types.h"
#include "../duel_effect_entry_occupancy.h"
/* This build's entries at 0x800EBF70-0x800F0430 (#2602); its battle models
 * belong to text_box_runtime.c. */
#define DUEL_EFFECT_ENTRY_STORAGE_TYPE EuropeanDuelEffectEntry
#define DUEL_EFFECT_ENTRY_STORAGE_COUNT EUROPEAN_DUEL_EFFECT_ENTRY_COUNT
#define DUEL_EFFECT_PROCESS_ENTRIES_NO_BATTLE_MODELS

#define DUEL_EFFECT_PROCESS_ENTRY_TYPE EuropeanDuelEffectEntry
#include "../duel_effect_process_entries.c"
