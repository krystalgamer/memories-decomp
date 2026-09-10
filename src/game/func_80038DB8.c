#include "../types.h"
#include "duel_effect.h"
#include "func_80036D3C.h"
#include "func_80038DB8.h"

void func_80038DB8(DuelEffectChannel *arg0)
{
    s32 c;
    s32 co;
    s32 no;
    s32 v;

    v = func_80036D3C(arg0);
    c = arg0->stream_58;
    no = (c + 1) * 4;
    co = c * 4;
    *(s32 *)((u8 *)arg0 + no) =
        (*(s32 *)((u8 *)arg0 + co) & 0xFFFF0000) | (v & 0xFFFF);
    arg0->stream_58++;
}
