#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

extern s16 D_8009B1D2;
extern s16 D_8009B1AC;
extern s16 D_8009B1AE;
extern u8 D_80090A4C[];
extern u16 D_8009B220;
extern u8 D_8009B1D5;
extern u8 D_800907D8[];
extern u8 D_801A7AD8[];

extern s16 func_800181EC(u8 *arg0);
extern s32 func_80024E24(void);
extern u8 *func_8002C68C(s32 arg0);
extern void SD_SEPlayFull(s32 sound_id);
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
    rb = D_801A7AD8;
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
    e = D_801A7AD8 + D_800907D8[
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
