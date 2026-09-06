#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

extern u8 D_800907D8[];
extern u8 gDuel_abTrapAttackThresholds[DUEL_ATTACK_TRAP_COUNT];
extern u8 D_8009B1B8;
extern u8 D_8009B1D5;
extern u16 D_8009B22A;
extern u8 D_8015C424[];
extern u8 D_801A7AD8[];

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
    rec2 = D_801A7AD8;
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
                D_801A7AD8 + p[0x6A] * DUEL_CARD_RECORD_SIZE
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
    rec3 = D_801A7AD8;
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
