#include "../../types.h"

/* SLPM-86398 build of src/game/duel_scene_battle.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_8009B170 gJapanese_D_8009B170
#define D_8009B1B4 gJapanese_D_8009B1B4
#define D_8009B21C gJapanese_D_8009B21C
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B369 gJapanese_DuelFlag
#define D_8009B374 gJapanese_DuelBgmId2
#define D_801A7AD8 gJapanese_DuelCardRecords
#define func_8001944C DuelCard_CaptureRoundedTexture
#define func_8001F0D0 Duel_SelectAttackTrap
#define func_8001F364 Duel_UpdateTrapPresentation

#include "../duel_scene_battle.c"
