#include "../../types.h"

/* SLPM-86398 build of src/game/script_op_duel_result.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_8009B280 gJapanese_D_8009B280
#define D_8009B290 gJapanese_D_8009B290
#define D_8009B29C gJapanese_D_8009B29C
#define D_8009B2A0 gJapanese_D_8009B2A0

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR 0x1FB2

#include "../script_op_duel_result.c"
