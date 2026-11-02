#include "../../types.h"
#define DUEL_EFFECT_PROCESS_ENTRIES_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800EB158-0x800ECD88 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 D_800EB158_slpm_86398[0x1C30];
#include "duel_effect_channel.h"
#include "../duel_effect_entry_occupancy.h"

#define VERSION_JAPAN
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_PROCESS_RANGE_START(channel) \
    (((JapaneseDuelEffectChannel *)(channel))->entry_range_start)
#include "../duel_effect_process_entries.c"
