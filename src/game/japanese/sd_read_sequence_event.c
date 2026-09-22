#include "../../types.h"

/* SLPM-86398 build of src/game/sd_read_sequence_event.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_80011484 gJapanese_D_80011484

#include "../sd_read_sequence_event.c"
