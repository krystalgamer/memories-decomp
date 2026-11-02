#include "../../types.h"
#define DUEL_EFFECT_PROCESS_ENTRIES_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800EBF70-0x800F0430 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 D_800EBF70_sles_03947[0x44C0];
#include "../duel_effect_entry_occupancy.h"

#define DUEL_EFFECT_PROCESS_ENTRY_TYPE EuropeanDuelEffectEntry
#include "../duel_effect_process_entries.c"
