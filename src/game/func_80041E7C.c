#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/inline_c.h"

struct Out {
    s16 f0;
    s16 f2;
    s32 f4;
};

struct Mat {
    char pad14[0x14];
    s32 f14;
    s32 f18;
    s32 f1C;
};

struct Vec308 {
    s16 f0;
    s16 f2;
    s16 f4;
    char pad8[0x8 - 0x6];
    s16 f8;
    char pad12[0x12 - 0xA];
    s16 f12;
};

struct Vec310 {
    char pad2[0x2];
    s16 f2;
    s16 f4;
};

struct Vec318 {
    s16 f0;
    char pad4[0x4 - 0x2];
    s16 f4;
};

extern void func_80088C50(struct Vec308 *, struct Mat *);
extern void GsSetLsMatrix(struct Mat *);
extern s32 func_80089CF0(struct Vec308 *, struct Vec310 *, struct Vec318 *,
                        struct Vec318 *);

s32 func_80041E7C(u32 arg0, s32 arg1, s32 arg2, struct Out *arg3)
{
    struct Vec308 *v308;
    struct Mat *mtx;
    s32 otz;

    arg3->f0 = arg1;
    arg3->f2 = arg2;
    arg3->f4 = 0;

    SetGeomOffset(arg1, arg2);
    SetGeomScreen(0x12C);

    mtx = (struct Mat *)0x1F8002D0;
    v308 = (struct Vec308 *)0x1F800308;

    v308->f0 = (s16)((arg0 & 0xFF) * 0x10);
    v308->f2 = (s16)((arg0 >> 4) & 0xFF0);
    v308->f4 = (s16)((arg0 >> 0xC) & 0xFF0);

    func_80088C50(v308, mtx);

    mtx->f14 = 0;
    mtx->f18 = 0;
    mtx->f1C = 0x12C;

    GsSetLsMatrix(mtx);

    {
        register struct Vec318 *v318 __asm__("$7") =
            (struct Vec318 *)0x1F800318;
        struct Vec310 *v310 = (struct Vec310 *)0x1F800310;

        v318->f0 = 0;
        v308->f0 = 0;
        v308->f8 = 0x200;
        v310->f2 = 0;
        v308->f2 = 0;
        v308->f12 = 0x200;
        v318->f4 = 0;
        v310->f4 = 0;
        v308->f4 = 0;

        func_80089CF0(v308, v310, v318, v318);
    }

    {
        s32 *p = &otz;
        gte_stopz(p);
    }

    return otz;
}
