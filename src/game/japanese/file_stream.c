#include "../../types.h"

/* SLPM-86398 build of src/game/file_stream.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0E8 gJapanese_D_8009B0E8
#define D_8009B0F0 gJapanese_D_8009B0F0
#define D_8009B0F4 gJapanese_FileTransferFlags
#define D_8009B0FC gJapanese_D_8009B0FC
#define D_8009B108 gJapanese_D_8009B108
#define D_8009B10C gJapanese_D_8009B10C
#define D_8009B110 gJapanese_D_8009B110
#define D_8009B120 gJapanese_D_8009B120
#define D_8009B124 gJapanese_D_8009B124
#define D_8009B130 gJapanese_D_8009B130
#define D_8009B134 gJapanese_FileSecondaryRequest

#include "../file_stream.c"
