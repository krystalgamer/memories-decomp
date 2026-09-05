#include "../types.h"

/* Runs the table-driven LP change phases. Recovery values are scaled by 100,
   added to the selected side's life points, and capped at its maximum; the
   alternate path subtracts the same values and floors the result at zero. */

struct Obj {
    s16 field0;
    s16 field2;
    u8 pad[0x1A - 4];
    s16 field1A;
};

struct DuelSideState {
    u8 pad[0x14];
    u16 life_points;
    s16 maximum_life_points;
};

extern s16 D_8009B1D2;
extern struct DuelSideState *D_8009B1C8;
extern u16 D_8009B220;
extern u16 D_8009B210;
extern s16 D_8009B22A;
extern u8 gDuel_abLifePointRecoveryUnits[5];

s32 func_80024E24(void);
s32 func_8001F364(s32);
s32 func_80025028(s32);
struct Obj *func_8002C68C(s32);
void SD_SEPlayFull(u32);

void func_800250C8(void) {
    s32 s0;
    s32 s1;
    s32 flag;
    u16 v1;
    struct Obj *obj;

    s0 = D_8009B1D2;
    s1 = s0 - 0x152;
    if (func_80024E24() == 0) {
        if (func_80025028(0x2B0) != 0) {
            s1 = s0 - 0x14D;
        }
        obj = func_8002C68C(5);
        obj->field0 = 0xA0;
        obj->field2 = 0x78;
        obj->field1A = s1;
        SD_SEPlayFull(0x14);
        return;
    }
    flag = D_8009B220;
    if (!(flag & 0x40)) {
        D_8009B220 = flag | 0x60;
        if (D_8009B22A == 0) {
            u8 *p = &gDuel_abLifePointRecoveryUnits[s1];
            v1 = D_8009B1C8->life_points + (*p) * 0x64;
            D_8009B1C8->life_points = v1;
            if (D_8009B1C8->maximum_life_points < (s16) v1) {
                D_8009B1C8->life_points =
                    (u16) D_8009B1C8->maximum_life_points;
            }
            goto block_14;
        }
        D_8009B210 = 0;
        goto block_9;
    }
block_9:
    if (D_8009B220 & 0x20) {
        if (func_8001F364(flag) == 0) {
            D_8009B220 &= 0xFFDF;
            obj = func_8002C68C(9);
            obj->field0 = 0xA0;
            obj->field2 = 0x78;
            obj->field1A = s1;
        }
    } else {
        u8 *p = &gDuel_abLifePointRecoveryUnits[s1];
        v1 = D_8009B1C8->life_points - (*p) * 0x64;
        D_8009B1C8->life_points = v1;
        if ((s16) v1 < 0) {
            D_8009B1C8->life_points = 0;
        }
block_14:
        D_8009B220 = 0;
    }
}
