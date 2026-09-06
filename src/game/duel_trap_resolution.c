#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

extern u8 D_800907D8[];
extern u8 gDuel_abTrapAttackThresholds[DUEL_ATTACK_TRAP_COUNT];
extern u8 D_8009B1B8;
extern u8 D_8009B1D5;
extern u16 D_8009B22A;
extern u8 D_8015C424[];
extern u8 D_801A7AD8_raw[] asm("D_801A7AD8");

s32 Duel_CalcCardStats(u8 *arg0);

s32 func_8001F0D0(u8 *p) {
    s32 i;
    s32 off1;
    u8 *b1;
    s32 n;
    u8 *tbl2;
    u8 *rec2;
    u8 *b2;
    s32 off2;
    s32 h2;
    u8 *e;
    s32 id;
    s32 sx;
    s32 sx2;
    s32 th;
    s32 sel;
    s32 off3;
    u8 *b3;
    u8 *q;
    u8 *tb;
    s32 v;
    u8 *w;
    s32 off4;
    u8 *b4;
    s32 j2;
    u8 *tbl3;
    u8 *rec3;
    s32 h3;
    s32 k;

    i = 0;
    off1 = 0x18000;
    b1 = D_8015C424;
    for (; i < DUEL_ATTACK_TRAP_COUNT; i++) {
        *(u16 *)(b1 + i * 2 + off1 + 0x3C68) = 0;
    }
    n = 0;
    i = n;
    tbl2 = D_800907D8;
    rec2 = D_801A7AD8_raw;
    b2 = D_8015C424;
    off2 = 0x18000;
    h2 = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    for (; i < DUEL_FIELD_ROW_SIZE; i++) {
        e = (u8 *)(*(u8 *)(i + h2 + (s32)tbl2) *
            DUEL_CARD_RECORD_SIZE + (s32)rec2);
        if ((*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            id = *(u16 *)(e + 0xC);
            if ((u32)(id - DUEL_ATTACK_TRAP_FIRST_CARD_ID) <
                DUEL_ATTACK_TRAP_COUNT) {
                sx = (s16)id;
                th = sx - DUEL_ATTACK_TRAP_FIRST_CARD_ID;
                n++;
                *(u16 *)(b2 + th * 2 + off2 + 0x3C68) = id;
                sx2 = sx - 0x299;
                *(u16 *)(b2 + sx2 * 2 + off2 + 0x3C68) = (*(u8 **)e)[0x6A];
            }
        }
    }
    if (n != 0) {
        do {
            th = Duel_CalcCardStats(
                D_801A7AD8_raw + p[0x6A] * DUEL_CARD_RECORD_SIZE
            ) & 0xFFFF;
        } while (0);
        sel = -1;
        off3 = 0x18000;
        b3 = D_8015C424;
        q = b3 + 0xA;
        i = DUEL_ATTACK_TRAP_COUNT - 1;
        tb = gDuel_abTrapAttackThresholds;
        do {
            if (*(u16 *)(q + off3 + 0x3C68) != 0) {
                v = *(u8 *)(i + (s32)tb);
                if (v * DUEL_ATTACK_TRAP_THRESHOLD_SCALE < th) {
                    break;
                }
                sel = i;
            }
            i--;
            q -= 2;
        } while (i >= 0);
        if (sel >= 0) {
            off4 = 0x18000;
            D_8009B22A = sel + DUEL_ATTACK_TRAP_FIRST_CARD_ID;
            b4 = D_8015C424;
            j2 = sel + 0x10;
            D_8009B1B8 = *(u8 *)(b4 + j2 * 2 + off4 + 0x3C68);
            return 1;
        }
        if (0) {
        hit:
            w = *(u8 **)e;
            D_8009B22A = v;
            D_8009B1B8 = w[0x6A];
            return 1;
        }
    }
    i = 0;
    tbl3 = D_800907D8;
    rec3 = D_801A7AD8_raw;
    h3 = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    k = DUEL_FAKE_TRAP_CARD_ID;
    for (; i < DUEL_FIELD_ROW_SIZE; i++) {
        e = (u8 *)(*(u8 *)(i + h3 + (s32)tbl3) *
            DUEL_CARD_RECORD_SIZE + (s32)rec3);
        if ((*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            v = *(s16 *)(e + 0xC);
            if (v == k) {
                goto hit;
            }
        }
    }
    return 0;
}

extern u16 D_8009B162;
extern u16 D_8009B210;
extern u16 D_8009B1D0;
extern s16 D_800F284A[];
extern u8 D_800E9FF0[];

void func_80022D94(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
u8 *func_8002C68C(s32 arg0);
void func_80024954(DuelCardRecord *arg0);
void SD_SEPlayFull(s32 arg0);

/* Four-state presentation sequencer on the D_8009B210 mode byte: mode 0
 * starts the first screen effect and arms the 0x14-frame counter; mode 1
 * copies the selected card's position into a type-8 effect object, updates
 * the card record and plays the SE when that counter expires; mode 2 starts
 * the second screen effect; mode 3 waits once more, advances the opposing
 * side's state byte at +6 and completes. Returns 1 while busy. */
s32 func_8001F364(void) {
    u8 *e;
    u8 *g;
    u8 *p;
    u8 *q;
    u8 *r;
    s32 one;
    s32 v;
    u16 t;
    u8 *q34;
    u16 *d;

    if (D_8009B162 != 0) {
        return 1;
    }

    one = 1;
    v = D_8009B210 & 0xF;

    if (v == one) {
        goto m1;
    }
    if (v < 2) {
        if (v == 0) {
            goto m0;
        }
        return 1;
    }
    if (v == 2) {
        goto m2;
    }
    if (v == 3) {
        goto m3;
    }

    return 1;

m0:
    func_80022D94(0x10, 0x208, 0x200, D_800F284A[0],
                  0xB2 - D_8009B1D5 * 0x164);
    D_8009B162 = 0x10;
    D_8009B210 = one;
    D_8009B1D0 = 0x14;
    do {
    return 1;

m1:
    t = D_8009B1D0 - 1;
    D_8009B1D0 = t;
    if ((s16)t <= 0) {
    r = D_8015C424;
    g = r + D_8009B1B8 * 0x1C + 0x48000;
    p = *(u8 **)(g + 0x36B4);
    e = func_8002C68C(8);
    *(u16 *)(e + 0) = *(u16 *)(p + 0x30);
    *(u16 *)(e + 2) = *(u16 *)(p + 0x32);
    q34 = p + 0x34;
    *(d = (u16 *)(e + 4)) = *(u16 *)q34;
    func_80024954(&D_801A7AD8[p[0x6A]]);
    SD_SEPlayFull(0x17);
    D_8009B210 = 2;
    }
    return 1;

m2:
    func_80022D94(0x10, 0x258, 0x100, D_800F284A[0], 0);
    D_8009B162 = 0x10;
    D_8009B210 = 3;
    D_8009B1D0 = 0x14;
    } while (0);
    return 1;

m3:
    t = D_8009B1D0 - 1;
    D_8009B1D0 = t;
    if ((s16)t > 0) {
        return 1;
    }
    q = (u8 *)D_800E9FF0 + (D_8009B1D5 ^ 1) * 0x20;
    q[6] = q[6] + 1;
    return 0;
}
