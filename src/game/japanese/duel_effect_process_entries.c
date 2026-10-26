#include "../../types.h"
#include "duel_effect_channel.h"
#include "../duel_effect_entry_occupancy.h"

#define VERSION_JAPAN
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_PROCESS_RANGE_START(channel) \
    (((JapaneseDuelEffectChannel *)(channel))->entry_range_start)
#include "../duel_effect_process_entries.c"
