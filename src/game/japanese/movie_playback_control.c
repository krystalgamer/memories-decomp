#include "../../types.h"

/* SLPM-86398 build of src/game/movie_playback_control.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B144 gJapanese_D_8009B144
#define D_8009B318 gJapanese_D_8009B318

#include "../movie_playback_control.c"
