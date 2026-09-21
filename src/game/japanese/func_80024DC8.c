#define gDuel_wBgmId gJapanese_DuelBgmId
#define D_8009B374 gJapanese_DuelBgmId2
#define D_8009B360 gJapanese_DuelPlayerState
#define gDuel_bOpponentID gJapanese_DuelOpponentId
#define D_8009B370 gJapanese_DuelValue0
#define D_8009B372 gJapanese_DuelValue1
#define gDuel_bTerrain gJapanese_DuelTerrain
#define D_8009B369 gJapanese_DuelFlag
#define D_8009B26C gJapanese_bActiveMainMode
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "../../types.h"
#include "../func_80024DC8.h"
#include "../duel_side_state.h"
#include "../../unmatched.h"
#include "../main_mode_state.h"
#include "../ai_opponent_data.h"
#include "../duel_terrain_boost.h"

void func_80024DC8(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    gJapanese_DuelBgmId = 0x7270;
    gJapanese_DuelBgmId2 = 0x7280;
    gJapanese_DuelPlayerState = arg0;
    gJapanese_DuelOpponentId = arg1;
    gJapanese_DuelValue0 = arg2;
    gJapanese_DuelValue1 = arg3;
    gJapanese_DuelTerrain = 0;
    gJapanese_DuelFlag = 0;
    gJapanese_bActiveMainMode = 3;
}
