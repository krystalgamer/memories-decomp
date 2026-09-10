#include "../types.h"
#include "func_80024DC8.h"
#include "duel_side_state.h"

extern s8 gDuel_bOpponentID;
extern u8 gDuel_bTerrain;
extern u16 D_8009B370;
extern u16 D_8009B372;
extern u16 D_8009B374;
extern u8 D_8009B26C;

void func_80024DC8(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    D_8009B36A = 0x7270;
    D_8009B374 = 0x7280;
    D_8009B360 = arg0;
    gDuel_bOpponentID = arg1;
    D_8009B370 = arg2;
    D_8009B372 = arg3;
    gDuel_bTerrain = 0;
    D_8009B369 = 0;
    D_8009B26C = 3;
}
