#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/inline_c.h"

struct Out {
    s16 f0;
    s16 f2;
    s32 f4;
};

struct Mat {
    s16 m[3][3];
    s16 pad12;
    s32 t[3];
};

struct Vec308 {
    s16 f0;
    s16 f2;
    s16 f4;
    char pad6[0x8 - 0x6];
    s16 f8;
    char pad10[0x12 - 0xA];
    s16 f12;
};

struct VecScale {
    s32 x;
    s32 y;
    s32 z;
};

struct Vec310 {
    char pad0[0x2];
    s16 f2;
    s16 f4;
};

struct Vec318 {
    s16 f0;
    char pad2[0x4 - 0x2];
    s16 f4;
};

struct Obj {
    char pad0[0x4];
    u32 field4;
    char pad8[0x10 - 0x8];
    s32 field10;
    char pad14[0x20 - 0x14];
    u8 field20;
    u8 field21;
    u8 field22;
    char pad23[0x44 - 0x23];
    s16 field44;
    s16 field46;
    char pad48[0x65 - 0x48];
    u8 field65;
};

typedef void (*ObjCallback)(struct Obj *, s32);

extern void SetGeomOffset(long a0, long a1);
extern void SetGeomScreen(long a0);
extern void func_80088C50(struct Vec308 *a0, struct Mat *a1);
extern void func_80087670(struct Mat *a0, struct VecScale *a1);
extern void func_800855D0(struct Mat *a0);
extern s32 func_80089CF0(
    struct Vec308 *a0,
    struct Vec310 *a1,
    struct Vec318 *a2,
    struct Vec318 *a3
);
extern void func_800877B0(struct Mat *a0);

s32 func_80041F90(struct Obj *obj, s32 arg1, s32 arg2, struct Out *out) {
    struct Mat *mtx = (struct Mat *)0x1F8002D0;
    struct Vec308 *v308 = (struct Vec308 *)0x1F800308;
    s32 otz;

    out->f0 = arg1;
    out->f2 = arg2;
    out->f4 = obj->field65;

    SetGeomOffset(arg1, arg2);
    SetGeomScreen(0x12C);

    v308->f0 = (s16)(obj->field20 * 0x10);
    v308->f2 = (s16)(obj->field21 * 0x10);
    v308->f4 = (s16)(obj->field22 * 0x10);
    func_80088C50(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = 0x12C;

    if (!(obj->field4 & 0x8000000)) {
        ((struct VecScale *)v308)->x = obj->field44;
        ((struct VecScale *)v308)->y = obj->field46;
        ((struct VecScale *)v308)->z = 0x1000;
        func_80087670(mtx, (struct VecScale *)0x1F800308);
    }

    func_800855D0(mtx);

    {
        register struct Vec318 *v318 asm("a3") =
            (struct Vec318 *)0x1F800318;
        struct Vec310 *v310 = (struct Vec310 *)0x1F800310;
        s32 *otzp;

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

        otzp = &otz;
        gte_stopz(otzp);

        {
            s32 cb = obj->field10;
            if (cb != 0) {
                if (cb < 0) {
                    ((ObjCallback)cb)(obj, otz);
                }
                if (otz >= 0) {
                    return otz;
                }

                mtx->m[0][0] = -mtx->m[0][0];
                mtx->m[1][0] = -mtx->m[1][0];
                mtx->m[2][0] = -mtx->m[2][0];
                mtx->m[0][2] = -mtx->m[0][2];
                mtx->m[1][2] = -mtx->m[1][2];
                mtx->m[2][2] = -mtx->m[2][2];
                func_800877B0(mtx);

                func_80089CF0(
                    v308,
                    v310,
                    (struct Vec318 *)0x1F800318,
                    v318
                );

                otzp = &otz;
                gte_stopz(otzp);
            }
        }
    }

    return otz;
}
