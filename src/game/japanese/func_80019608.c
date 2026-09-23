#include "../../types.h"

/* SLPM-86398 build of src/game/func_80019608.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_801A7AD8 gJapanese_DuelCardRecords
#define func_8001944C DuelCard_CaptureRoundedTexture

#include "../func_80019608.c"
