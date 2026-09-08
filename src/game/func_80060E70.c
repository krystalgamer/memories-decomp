#include "../types.h"
#include "card_constants.h"
#include "card_list_rows.h"

typedef struct {
    u8 pad_00[0x28];
    void *child;
    u8 pad_2C[8];
    u16 field34;
    u8 pad_36[4];
    s16 field3A;
    u16 field3C;
    u8 pad_3E[0x16];
    u8 field54;
} Box;

typedef struct {
    u8 *obj;
    s16 f4;
    s16 f6;
    s16 f8;
    u16 fA[7];
} Slot;

extern Slot D_801A8000[];
extern s32 D_801D5608[];
extern s32 gDuel_adwCardStats[];
extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
extern Box *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(void *);

void func_80060E70(u16 *e, s32 idx, s32 flag, s32 ignored)
{
    Slot *s;
    Box *box;
    s32 i;
    s32 y;
    s32 style;
    s32 base;
    register s32 id asm("$3");
    u16 *q;
    Slot *w;
    s32 *dst;
    register s32 *stats asm("$8");
    register s32 offset asm("$2");

    s = &D_801A8000[idx];
    base = 0x18;
    s->f8 = flag;
    s->obj[0x68] = flag;
    if (flag != 0) {
        base = 0x1A;
    }
    i = 0;
    dst = D_801D5608;
    w = s;
    y = 0;
    q = e + 1;
    for (i = 0; i < 7; i++) {
        w->fA[0] = e[0];
        style = e[0];
        if (style != 0) {
            id = style;
            offset = id - 1;
            offset <<= 2;
            stats = gDuel_adwCardStats;
            style = base;
            if (((*(s32 *)((u8 *)stats + offset) >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                style = base + 1;
            }
            D_801D5608[0] = id;
            dst[1] = q[0];
            gDuel_wSelectedCardID = id;
        }
        box = TextBox_Create(idx + 1, style, s->f4, s->f6, 0x120, 0xB0);
        box->field3A = y;
        if (q[0] == 0) {
            box->field54 = 4;
            w->fA[0] |= 0x8000;
        }
        if (i != 0) {
            box->field34 |= 0x40;
        }
        func_80039A14(box);
        q += 2;
        e += 2;
        w = (Slot *)((u8 *)w + 2);
        y += 0x16;
    }
}
