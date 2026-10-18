#include "../../types.h"

/* SLPM-86398 build of src/game/duel_magic_effect_dispatch.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gDuel_abFieldGridCardRecordIndices gJapanese_DuelFieldGridCardRecordIndices
#define D_8009AF40 gJapanese_D_8009AF40
#define gDuel_aCardRecords gJapanese_DuelCardRecords

#include "../duel_magic_effect_dispatch.c"
