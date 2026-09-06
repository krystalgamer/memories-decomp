#include "../types.h"
#include "../psyq/libgte.h"
#include "model.h"

extern u8 D_8009B478;
extern u16 D_8009B47A;
extern u16 D_8009B47C;
typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec32;

extern Vec32 D_800F56F0;
extern Vec32 D_800F56FC;

void func_80058434(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    u8 *b;
    u8 *t;
    s32 c;
    s32 sn;
    s32 r;
    s32 u;
    s32 w;
    s32 z;
    s32 v;

    if (arg0 > 0) {
        b = (u8 *)&D_800F56FC;
        t = b - 0xC;
    } else {
        b = (u8 *)&D_800F56F0;
        t = b + 0xC;
    }

    if (*(s16 *)&D_8009B478 + arg3 >= 0xB) {
        *(u16 *)&D_8009B478 = *(u16 *)&D_8009B478 + arg3;
    }

    if ((arg1 & MODEL_ANGLE_MASK) != 0) {
        v = *(s16 *)&D_8009B47A + arg1 + MODEL_ANGLE_FULL_TURN;
        D_8009B47A = v - v / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if ((arg2 & MODEL_ANGLE_MASK) != 0) {
        v = *(s16 *)&D_8009B47C + arg2 + MODEL_ANGLE_FULL_TURN;
        D_8009B47C = v - v / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if (arg1 != 0 || arg2 != 0 || arg3 != 0) {
        c = rcos(*(s16 *)&D_8009B47C);
        sn = rsin(*(s16 *)&D_8009B47C);
        r = *(s16 *)&D_8009B478;
        u = r * c / MODEL_FIXED_ONE;
        w = r * sn / MODEL_FIXED_ONE;
        c = rcos(*(s16 *)&D_8009B47A);
        sn = rsin(*(s16 *)&D_8009B47A);
        z = u * sn / MODEL_FIXED_ONE;
        u = u * c / MODEL_FIXED_ONE;
        *(s32 *)(t + 0) = *(s32 *)(b + 0) + u * arg0;
        *(s32 *)(t + 4) = *(s32 *)(b + 4) + w * arg0;
        *(s32 *)(t + 8) = *(s32 *)(b + 8) + z * arg0;
    }
}
