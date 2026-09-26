#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_DUEL_RESULT_OUTRO
#define DUEL_RESULT_OUTRO_START_SECTOR 0x1DB6
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x1D3E
#define DuelScene_UpdateResultOutro func_80020D94
#define D_8009B362 gJapanese_D_8009B362
#define D_8009B238 gJapanese_D_8009B238
#define D_8009B21C gJapanese_D_8009B21C
#define D_8009B1E0 gJapanese_DuelResultBgmId
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_80090928 gJapanese_DuelResultSpriteSpecsOpponent
#define D_80090960 gJapanese_DuelResultSpriteSpecsNoOpponent
#define DuelResult_UpdateOrbitSprite func_80020B94
#define func_80020EE8 func_80020D30
#include "../duel_result_runtime.c"
