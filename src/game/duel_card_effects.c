#include "../types.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "sound.h"

struct Obj {
    s16 field0;
    s16 field2;
    u8 pad[0x1A - 4];
    s16 field1A;
};

extern s16 D_8009B1D2;
extern DuelSideState *D_8009B1C8;
extern u16 D_8009B220;
extern u16 D_8009B210;
extern s16 D_8009B22A;
extern u8 D_8009B1D5;
extern u8 gDuel_abLifePointRecoveryUnits[DUEL_LIFE_POINT_EFFECT_COUNT];
extern u8 gDuel_abDirectDamageUnits[DUEL_LIFE_POINT_EFFECT_COUNT];

s32 func_80024E24(void);
s32 func_8001F364(s32);
s32 func_80025028(s32);
void *func_8002C68C(s32);

/* Runs the table-driven LP change phases. Recovery values are scaled by 100,
   added to the selected side's life points, and capped at its maximum; the
   alternate path subtracts the same values and floors the result at zero. */
void func_800250C8(void) {
    s32 s0;
    s32 s1;
    s32 flag;
    u16 v1;
    struct Obj *obj;

    s0 = D_8009B1D2;
    s1 = s0 - DUEL_LIFE_POINT_RECOVERY_FIRST_CARD_ID;
    if (func_80024E24() == 0) {
        if (func_80025028(DUEL_BAD_REACTION_TO_SIMOCHI_CARD_ID) != 0) {
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
            v1 = D_8009B1C8->life_points.unsigned_value +
                 (*p) * DUEL_LIFE_POINT_RECOVERY_SCALE;
            D_8009B1C8->life_points.unsigned_value = v1;
            if (D_8009B1C8->max_life_points < (s16) v1) {
                D_8009B1C8->life_points.unsigned_value =
                    (u16) D_8009B1C8->max_life_points;
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
        v1 = D_8009B1C8->life_points.unsigned_value -
             (*p) * DUEL_LIFE_POINT_RECOVERY_SCALE;
        D_8009B1C8->life_points.unsigned_value = v1;
        if ((s16) v1 < 0) {
            D_8009B1C8->life_points.unsigned_value = 0;
        }
block_14:
        D_8009B220 = 0;
    }
}

/* Applies one of the five direct-damage cards. The selected table value is
   scaled by 10 and taken off the target side's life points, clamped at zero. */
void func_8002525C(void) {
    s32 unit;
    s32 flags;
    u16 remaining;
    struct Obj *obj;
    DuelSideState *p;

    unit = D_8009B1D2 - DUEL_DIRECT_DAMAGE_FIRST_CARD_ID;
    if (func_80024E24() == 0) {
        if (func_80025028(DUEL_GOBLIN_FAN_CARD_ID) != 0) {
            unit = DUEL_LIFE_POINT_EFFECT_COUNT;
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
        remaining = p->life_points.unsigned_value -
                    gDuel_abDirectDamageUnits[unit] *
                        DUEL_DIRECT_DAMAGE_SCALE;
        p->life_points.unsigned_value = remaining;
        if ((s16) remaining < 0) {
            p->life_points.unsigned_value = 0;
        }
        D_8009B220 = 0;
    }
}

#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"

extern s16 D_8009B1D2;
extern s16 D_8009B1AC;
extern s16 D_8009B1AE;
extern u8 D_80090A4C[];
extern u16 D_8009B220;
extern u8 D_8009B1D5;
extern u8 D_800907D8[];
extern u8 D_801A7AD8_bytes[] asm("D_801A7AD8");

extern s16 func_800181EC(u8 *arg0);
extern s32 func_80024E24(void);
extern s32 Duel_CalcCardStats(u8 *arg0);
extern void func_80024954(u8 *arg0);

void func_8002538C(void) {
    u8 *p;
    u8 *e;
    u8 *tb;
    u8 *cb;
    u8 *rb;
    s16 *sp;
    s32 i;
    s32 n;
    s16 m;
    s32 x;
    s32 ix;

    if (func_80024E24() == 0) {
        p = func_8002C68C(0xF);
        *(s16 *)(p + 0) = 0;
        *(s16 *)(p + 2) = 0;
        *(s16 *)(p + 4) = 0;
        SD_SEPlayFull(0x22);
        i = 0;
        cb = D_80090A4C;
        x = D_8009B1D2;
        while (1) {
            if (*(u8 *)(i + (s32)cb) + 0x258 == x) {
                break;
            }
            i += 2;
        }
        *(s16 *)(p + 0x1A) = i / 2;
        m = D_80090A4C[i + 1];
        n = m;
        D_8009B1AC = m;
        if (n >= 0x15) {
            D_8009B1AC = n * CARD_STAT_SCALE;
            D_8009B220 = D_8009B220 | 1;
        }
        D_8009B1AE = 5;
        return;
done:
        D_8009B220 = 0;
        return;
    }

    tb = D_800907D8;
    sp = &D_8009B1AE;
    rb = D_801A7AD8_bytes;
    goto head;

arm:
    if ((Duel_CalcCardStats(e) & 0xFFFF) >= D_8009B1AC) {
        goto hit;
    }
next:
    D_8009B1AE = D_8009B1AE + 1;
    if (D_8009B1AE >= DUEL_FIELD_SIDE_ZONE_COUNT) {
        goto done;
    }
head:
    ix = D_8009B1AE + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    e = (u8 *)(*(u8 *)(ix + (s32)tb) * DUEL_CARD_RECORD_SIZE +
        (s32)rb);
    if ((*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_OCCUPIED) == 0) {
        goto next;
    }
    if ((D_8009B220 & 1) != 0) {
        goto arm;
    }
    if (*(u8 *)(*(s32 *)e + 0x68) != sp[-1]) {
        goto next;
    }

hit:
    e = D_801A7AD8_bytes + D_800907D8[
        D_8009B1AE + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ] * DUEL_CARD_RECORD_SIZE;
    p = func_8002C68C(0xB);
    *(u16 *)(p + 0) = *(u16 *)(*(s32 *)e + 0x30);
    *(u16 *)(p + 2) = *(u16 *)(*(s32 *)e + 0x32);
    *(u16 *)(p + 4) = *(u16 *)(*(s32 *)e + 0x34);
    *(s16 *)(p + 0x1A) = func_800181EC(*(u8 **)e);
    func_80024954(e);
    SD_SEPlayFull(0x1F);
}

#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"

extern volatile u16 D_8009B112 __attribute__((section(".data")));
extern u8 *D_8009B17C;
extern s16 D_8009B1D2;
extern u8 D_8009B1D5;
extern s16 D_8009B20C[4];
extern u16 D_8009B220;
extern u8 D_800907D8[];
extern DuelFieldPosition D_80090800[];
extern u8 D_801A7B64[];

extern void func_80019BA0(u8 *arg0, u8 arg1, s16 arg2, s16 arg3);
extern s32 func_80024E24(void);
extern u8 *func_8002C604(s32 arg0);

void DuelEffect_UpdateFieldMarker(void) {
    DuelCardRecord *r;
    u8 *p;
    u8 *e;
    u8 *t;
    s32 f;
    s32 c;
    s32 v;
    s32 n;
    s32 w;

    if (func_80024E24() == 0) {
        D_8009B20C[1] = -1;
    }

    f = D_8009B220;

    if ((f & 0x40) != 0) {
        if ((f & 0x20) == 0) {
            if (D_8009B17C[0x1D] != 0) {
                n = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_ROW_SIZE;
                c = D_800907D8[D_8009B20C[1] + n];
                r = &D_801A7AD8[c];
                if ((*(s32 *)&r->terrain_modifier & 0x88000000) ==
                    0x88000000) {
                    func_80019BA0((u8 *)r->object, 0xC0, 0, 6);
                    r->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
                }
                D_8009B220 = D_8009B220 | 0x20;
            }
        }

        if ((D_8009B17C[0x1C] & 0x80) != 0) {
            return;
        }
        D_8009B220 = D_8009B220 & 0xFF9F;
        return;
    }

    v = *(u16 *)&D_8009B20C[1] + 1;
    D_8009B20C[1] = v;
    if ((s16)v >= DUEL_FIELD_ROW_SIZE) {
        D_8009B220 = 0;
        return;
    }

    p = func_8002C604(0xC);
    t = (u8 *)D_80090800;
    e = (
        (D_8009B20C[1] + DUEL_FIELD_ROW_SIZE) *
            sizeof(DuelFieldPosition) +
        D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES
    ) + t;
    w = *(u16 *)(e + 0);
    D_8009B17C = p;
    *(s16 *)(p + 2) = 0;
    *(s16 *)(p + 0) = w;
    *(s16 *)(p + 4) = *(u16 *)(e + 2);
    SD_SEPlayFull(0x20);

    D_8009B220 = D_8009B220 | 0x40;
}

void func_800257A0(void) {
    u8 *e;
    u8 *p;
    u8 *q;
    s32 i;
    s32 f;
    s32 g;

    if (func_80024E24() == 0) {
        func_8003FF88(0x8020);
        return;
    }

    f = D_8009B220;

    if ((f & 0x20) == 0) {
        if ((D_8009B112 & 0x4000) == 0) {
            return;
        }
        D_8009B220 = f | 0x20;
        e = func_8002C68C(0x11);
        if (D_8009B1D2 == DUEL_DRAGON_CAPTURE_JAR_CARD_ID) {
            g = D_8009B220;
            *(s16 *)(e + 0x1A) = 1;
            D_8009B220 = g | 0x40;
        }
        *(s16 *)(e + 0) = 0;
        *(s16 *)(e + 2) = 0;
        *(s16 *)(e + 4) = 0;
        return;
    }

    if ((D_8009B112 & 0x4000) != 0) {
        return;
    }

    if ((f & 0x40) != 0) {
        i = DUEL_FIELD_ROW_SIZE;
        while (1) {
            p = D_800907D8[
                i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ] * DUEL_CARD_RECORD_SIZE + D_801A7AD8_bytes;
            if ((*(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0 &&
                (*(u8 **)p)[0x68] == 0) {
                func_80024954(p);
            }
            i++;
            if (i >= DUEL_FIELD_SIDE_ZONE_COUNT) {
                break;
            }
        }
    } else {
        p = D_801A7B64;
        q = p + DUEL_CARD_SIDE_RECORD_COUNT * DUEL_CARD_RECORD_SIZE;
        i = 0;
        do {
            if ((*(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                func_80024954(p);
            }
            if ((*(u16 *)(q + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                func_80024954(q);
            }
            i++;
            p += DUEL_CARD_RECORD_SIZE;
            q += DUEL_CARD_RECORD_SIZE;
        } while (i < DUEL_FIELD_SIDE_ZONE_COUNT);
    }

    D_8009B220 = 0;
}
