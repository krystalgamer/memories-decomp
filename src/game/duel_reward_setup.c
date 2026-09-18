#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#define D_8009B118_IN_DATA
#include "card_constants.h"
#include "../unmatched.h"
#include "graphics_frame.h"
#include "duel_reward_setup.h"
#include "../ygo_types.h"

void func_80032184(FileTransferDescriptor *p, s32 mode) {
    s32 one;
    s32 w;
    s32 v;
    s32 t0;
    s32 t1;
    s32 c;
    s32 n;
    s32 u;
    s16 *g;
    s32 m;
    s32 m2v;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    if (mode == 3) {
        goto m3;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    *(s16 *)&p->field_30.h.counter = 0x300;
    *(s16 *)&p->field_30.h.field_32 = 0x100;
    *(u16 *)&p->w = 0x40;
    t0 = D_8009B0F4_abs;
    *(u16 *)&p->h = 0x10;
    D_8009B0F4_abs = t0 & m;
    D_8009B0F4_abs = D_8009B0F4_abs | 0x10000;
    p->done = 2;
    v = D_8009B118;
    w = 0x20000;
    *(s32 *)&p->phase_size = w;
    p->value_08 = v;
    v += 0x800;
    p->value_0C = v;
    return;

m1:
    m = 0xFFDDFFFF;
    *(s16 *)&p->field_30.h.counter = 0x340;
    *(u16 *)&p->w = 0x40;
    t1 = D_8009B0F4_abs;
    *(u16 *)&p->h = 0x10;
    D_8009B0F4_abs = t1 & m;
    u = D_8009B0F4_abs;
    n = 0x10000;
    p->field_30.h.field_32 = 0;
    D_8009B0F4_abs = u | n;
    p->done = 2;
    v = D_8009B118;
    w = 0x4000;
    *(s32 *)&p->phase_size = w;
    p->value_08 = v;
    v += 0x800;
    p->value_0C = v;
    return;

m2:
    m2v = 0xFFDCFFFF;
    *(s32 *)&p->phase_size = 4 * FILE_SECTOR_SIZE;
    D_8009B0F4_abs = D_8009B0F4_abs & m2v;
    p->value_0C = D_8009B118;
    p->value_08 = D_8009B118;
    p->done = 1;
    return;

m3:
    g = (s16 *)D_800E9D70;
    c = 0x100;
    g[0] = c;
    g[1] = 0xF0;
    g[2] = c;
    g[3] = 0x10;
    LoadImage2((RECT *)g, (u32 *)D_8009B118);
}

void func_80032328(void)
{
    File_RequestAsyncTransfer(0, 0, 0x2189, 0x4C, func_80032184, 0, 0);
    File_WaitForTransfers();
}

/*
 * Reward-drop compaction. It matches all 136 bytes under split-address
 * lowering -- recorded at gcc_2_8_1_g0_split and byte-identical at this
 * unit's g8_split_comm -- which keeps the table's high half in a temporary
 * before publishing its address; without splitting, the first two words
 * change. Neither loop needs pins.
 *
 * The chest base and reverse cursor use target-width address arithmetic:
 * the chest is a separate object below the drop table, and the last reverse
 * decrement runs after slot zero without dereferencing the resulting address.
 */
void func_80032370(void)
{
    s16 *source;
    u8 *base;
    s32 i;
    u32 current;
    s16 *destination;

    source = gDuel_awRecentCardDrops;
    base = (u8 *)((u32)source - DUEL_RECENT_DROPS_CHEST_DISTANCE);
    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = (u32)(source + DUEL_RECENT_CARD_DROP_COUNT - 1);
    for (; i >= 0; i--, current -= sizeof(*source)) {
        if (*(s16 *)current != 0 && base[*(s16 *)current - 1] == 0)
            *(s16 *)current = 0;
    }
    destination = source;
    for (i = 0; i < DUEL_RECENT_CARD_DROP_COUNT; i++, source++) {
        if (*source) {
            if (source != destination) {
                *destination = *source;
                *source = 0;
            }
            destination++;
        }
    }
}
