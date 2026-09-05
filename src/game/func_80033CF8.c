#include "../types.h"

struct Rec {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
};
extern struct Rec *D_8009B310;
extern u32 D_8009B314;
extern long SquareRoot0(long);
extern s32 func_800899A0(s32, s32);

void func_80033CF8(s32 dx, s32 dy, s32 dz) {
    struct Rec *p;
    s32 q;
    s32 v;

    D_8009B310->f2 = SquareRoot0(dx * dx + dz * dz);
    q = func_800899A0(dx, dz) / 16;
    if (q >= 256) {
        q = 255;
    }
    v = q << 8;

    p = D_8009B310;
    p->f0 = v | (dy >> 4);
    p->f4 = D_8009B314;
    p->f6 = 0;

    D_8009B310 = p + 1;
    D_8009B314 = D_8009B314 + 1;
}
