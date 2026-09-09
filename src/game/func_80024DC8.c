#include "../types.h"
#include "func_80024DC8.h"

extern s8 D_8009B360;
extern s8 gDuel_bOpponentID;
extern u8 gDuel_bTerrain;
extern u8 D_8009B369;
extern u16 D_8009B36A;
extern u16 D_8009B370;
extern u16 D_8009B372;
extern u16 D_8009B374;
extern u8 D_8009B26C;

void func_80024DC8(s8 arg0, s8 arg1, u16 arg2, u16 arg3)
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
