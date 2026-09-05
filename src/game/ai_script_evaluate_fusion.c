#include "../types.h"

extern s32 D_800F5B98[];
extern u8 D_800F5BE8[];
extern s16 D_800F5C88[];
extern u8 D_800F5C8B[];

extern s32 Duel_GetBaseCardStat(s32, s32);
extern s32 func_8007058C(void);
extern s32 Ai_GetHandSize(void);
extern void Ai_CompleteFusion(s32);

void AiScript_EvaluateFusion(void)
{
    u8 *r;
    s32 a;
    s32 b;
    s32 c;
    s32 k;
    s32 x;
    s32 y;
    s32 i;
    s32 n;

    a = D_800F5B98[func_8007058C()];
    b = D_800F5B98[func_8007058C()] + 1;
    c = D_800F5B98[func_8007058C()];
    k = func_8007058C();
    n = Ai_GetHandSize();

    r = (u8 *)D_800F5BE8;
    r[0x9C] = n;
    r[0x9D] = b;
    *(s16 *)(r + 0xA0) = 0;
    r[0xA2] = 0;
    r[0xA3] = 0;
    r[0x9E] = c;

    for (i = 0; i < r[0x9C]; i++) {
        r[i + 0xAA] = 0;
    }

    x = Duel_GetBaseCardStat(a, 0);
    y = Duel_GetBaseCardStat(a, 1);

    if (y < x) {
        D_800F5C88[0] = Duel_GetBaseCardStat(a, 0);
    } else {
        D_800F5C88[0] = Duel_GetBaseCardStat(a, 1);
    }

    Ai_CompleteFusion(a);

    if (D_800F5C8B[0] != 0) {
        D_800F5B98[k] = 0;
    } else {
        D_800F5B98[k] = 1;
    }
}
