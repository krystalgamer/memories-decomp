#include "../../types.h"

/* SLPM-86398 build of src/game/file_transfer_flags.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4 gJapanese_FileTransferFlags
#define D_8009B10C gJapanese_D_8009B10C
#define D_8009B134 gJapanese_FileSecondaryRequest

#include "../file_transfer_flags.c"
