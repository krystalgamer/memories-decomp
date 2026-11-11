#ifndef MEMORIES_DECOMP_DUEL_EFFECT_PROCESS_ENTRIES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_PROCESS_ENTRIES_H

#include "duel_effect_states.h"

/* Four entry handlers, defined after the primary text commands in
 * duel_effect_command_table.c. Keep this two-argument ABI and unsized spelling. */
extern void (*gDuelEffect_EntryHandlers[])(DuelEffectEntry *, DuelEffectChannel *);
/* Japanese entries are aligned 0x18-byte records with a byte-sized channel
 * range start; North American entries are 0x1C bytes with a halfword range. */
void DuelEffect_ProcessEntries(DuelEffectChannel *);

#endif
