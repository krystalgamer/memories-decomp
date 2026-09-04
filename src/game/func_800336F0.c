#include "../types.h"
#include "input.h"

extern u8 D_8009B24B;
extern u16 gDuel_wViewerCardID;
extern u8 D_8009B254;
extern u16 D_8009B394;
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B3A4;

extern void func_80031574(s32, s32, s32, s32, s32);
extern void func_80031E5C(u8 *);
extern void func_80031F7C(u8 *, s32);
extern void func_800320BC(u8 *, s32);
extern void func_80032B38(u8 *);
extern s32 func_800330BC(u8 *);
extern s32 func_80033500(u8 *);
extern void SD_SEPlayFull(s32);

void func_800336F0(u8 *p)
{
    u8 *e;
    u8 *q;
    s32 r;
    u32 c;

    e = p + (p[0x6342] * 0x2D4C + 4);
    func_80032B38(p);
    if (func_800330BC(e) != 0) {
        return;
    }

    if ((D_8009B398 & 0x10) != 0) {
        r = func_80033500(e);
        if (r != 0) {
            D_8009B24B = 0x14;
            gDuel_wViewerCardID = r;
            D_8009B254 = 2;
        }
        return;
    }

    if (D_8009B3A4 == PAD_DIRECTION_RIGHT) {
        *(s32 *)(p + 0x5AA4) = 0x140;
        *(s16 *)(p + 0x633E) = 1;
        *(s16 *)(p + 0x6340) = 3;
        return;
    }

    if ((D_8009B398 & 0x20) != 0) {
        *(s16 *)(p + 0x633E) = 4;
        *(s16 *)(p + 0x6340) = 2;
        return;
    }

    if ((D_8009B394 & 0xC0) == 0) {
        return;
    }

    r = func_80033500(e);
    c = 1;
    if ((u32)(r - 0x11) < 5) {
        c = (p + r)[0x5AC4] < c;
    }

    if (r != 0 && c != 0) {
        q = p + r;
        if (*(s32 *)(p + 0x5AA0) < 0x28 &&
            q[0x5D97] != 0 &&
            q[0x5AC4] < 3) {
            SD_SEPlayFull(7);
            func_800320BC(p, r);
            func_80031F7C(p, r);
            func_80031E5C(p);
            func_80031574(r, 3, 0x18, 0x11C, 0xC);
            return;
        }
    }

    SD_SEPlayFull(9);
}
