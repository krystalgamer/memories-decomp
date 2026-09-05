#include "../types.h"

extern u8 D_8009B24B;
extern u16 gDuel_wViewerCardID;
extern u8 D_8009B254;
extern u16 D_8009B394;
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B3A4;

extern void func_80031574(s32, s32, s32, s32, s32);
extern void func_80031E5C(u8 *);
extern void func_80031EE4(u8 *, s32);
extern void func_8003201C(u8 *);
extern void func_80032B38(u8 *);
extern void func_80032C48(u8 *);
extern s32 func_800330BC(u8 *);
extern s32 func_80033500(u8 *);
extern void SD_SEPlayFull(s32);

void func_8003353C(u8 *p) {
    u8 *e;
    s32 r;

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

    if (D_8009B3A4 == 0x8000) {
        *(s16 *)(p + 0x633E) = 1;
        *(s32 *)(p + 0x5AA4) = 0;
        *(s16 *)(p + 0x6340) = 2;
        return;
    }

    if ((D_8009B398 & 0x20) != 0) {
        *(s16 *)(p + 0x633E) = 4;
        *(s16 *)(p + 0x6340) = 3;
        return;
    }

    if ((D_8009B394 & 0xC0) != 0) {
        r = func_80033500(e);
        if (r != 0) {
            SD_SEPlayFull(7);
            *(e + 0xD - -((*(s16 *)(e + 0x2D3C) +
                             *(s8 *)(e + 0x2D48)) * 0x10)) = 0;
            func_80032C48(p + 0x2D50);
            func_8003201C(p);
            func_80031EE4(p, r);
            func_80031E5C(p);
            func_80031574(r, 0x234, 0x16, 0x162, 0xA);
            return;
        }
        SD_SEPlayFull(9);
    }
}
