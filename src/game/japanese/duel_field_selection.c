#include "../../types.h"

/* SLPM-86398 build of src/game/duel_field_selection.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gDuel_abFieldGridCardRecordIndices gJapanese_DuelFieldGridCardRecordIndices
#define gDuel_pActiveSelectionRecord gJapanese_DuelActiveSelectionRecord
#define gDuel_aCardRecords gJapanese_DuelCardRecords

#include "../duel_field_selection.c"
