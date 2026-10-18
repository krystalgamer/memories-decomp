#include "../../types.h"

/* SLPM-86398 build of src/game/ai_turn_action.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gDuel_abFieldGridCardRecordIndices gJapanese_DuelFieldGridCardRecordIndices
#define gDuel_aCardRecords gJapanese_DuelCardRecords

#include "../ai_turn_action.c"
