#include "../types.h"

extern s16 gDuel_wSelectedCardID;
extern u8 D_8009B320;
extern u8 D_800EB15C[];
extern s32 D_801D5608[];

s32 func_8002A6B8(u8 *arg0);
s32 func_80029EB0(u8 *arg0, s32 arg1);
u8 *TextBox_Create(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
void func_80039A60(u8 *arg0);

void func_8002A2F4(u8 *p)
{
    s32 *q = D_801D5608;
    s32 r;
    s32 t;
    s32 n;
    s32 mode;
    u8 *o;

    mode = 0;
    r = func_8002A6B8(p);
    gDuel_wSelectedCardID = r;
    t = (s16)r;
    q[0] = t;
    n = t;

    if (n != 0) {
        r = func_80029EB0(p, n);
        mode = 5;
        if ((r & 0x80) == 0) {
            gDuel_wSelectedCardID = 0;
        }
    }

    o = TextBox_Create(1, mode, 0x10, 0xCA, 0x120, 0x30);
    D_8009B320 = o[0x54];
    if (*(p + (n << 2) + 0x56) & 1) {
        D_8009B320 = 4;
    }
    func_80039A60(D_800EB15C);
}
