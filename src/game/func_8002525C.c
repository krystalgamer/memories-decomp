#include "../types.h"
#include "card_constants.h"

/* Applies one of the five direct-damage cards. gDuel_abDirectDamageUnits holds
   the retail values 5/10/20/50/100; the selected one is scaled by 10 and taken
   off the target side's life points, clamped at zero. This is the direct-damage
   twin of func_800250C8, which runs the same phase structure for LP recovery. */

struct Obj {
    s16 field0;
    s16 field2;
    u8 pad[0x1A - 4];
    s16 field1A;
};

struct DuelSideState {
    u8 pad00[0x14];
    u16 life_points;
    u8 pad16[0xA];
};

extern s16 D_8009B1D2;
extern u16 D_8009B220;
extern u16 D_8009B210;
extern s16 D_8009B22A;
extern u8 D_8009B1D5;
extern struct DuelSideState D_800E9FF0[];
extern u8 gDuel_abDirectDamageUnits[5];

s32 func_80024E24(void);
s32 func_8001F364(s32);
s32 func_80025028(s32);
struct Obj *func_8002C68C(s32);
void SD_SEPlayFull(u32);

void func_8002525C(void) {
    s32 unit;
    s32 flags;
    u16 remaining;
    struct Obj *obj;
    struct DuelSideState *p;

    unit = D_8009B1D2 - 0x157;
    if (func_80024E24() == 0) {
        if (func_80025028(0x2AF) != 0) {
            unit = 5;
        }
        obj = func_8002C68C(6);
        obj->field0 = 0xA0;
        obj->field2 = 0x78;
        obj->field1A = unit;
        SD_SEPlayFull(0x1C);
        return;
    }
    flags = D_8009B220;
    if (!(flags & 0x40)) {
        D_8009B220 = flags | 0x60;
        if (D_8009B22A == 0) {
            p = &D_800E9FF0[D_8009B1D5 ^ 1];
            goto apply;
        }
        D_8009B210 = 0;
    }
    if (D_8009B220 & 0x20) {
        if (func_8001F364(flags) == 0) {
            D_8009B220 &= 0xFFDF;
            obj = func_8002C68C(7);
            obj->field0 = 0xA0;
            obj->field2 = 0x78;
            obj->field1A = unit;
            SD_SEPlayFull(0x1C);
        }
    } else {
        p = &D_800E9FF0[D_8009B1D5];
apply:
        remaining = p->life_points -
                    gDuel_abDirectDamageUnits[unit] *
                        DUEL_DIRECT_DAMAGE_SCALE;
        p->life_points = remaining;
        if ((s16) remaining < 0) {
            p->life_points = 0;
        }
        D_8009B220 = 0;
    }
}
