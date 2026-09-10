
#include "../types.h"
#include "card_constants.h"
#include "func_80036C14.h"
#include "duel_card.h"
#include "text_constants.h"
#include "duel_effect.h"
#include "func_80037DA4.h"

extern s16 gDuel_wSelectedCardID;

void func_80037DA4(u8 *object)
{
    u8 **stream;
    /* Pinned: the branch on the opcode byte flips local-alloc's choice for
       this pair, so without it the slot pointer and the current pointer come
       out in each other's registers. Pinning the current pointer is enough;
       the slot pointer then follows into $v1. */
    register u8 *current __asm__("$2");
    s32 op;
    s32 id;
    s32 n;
    s32 kind;
    s32 stats;
    s32 type;
    u8 *text;
    u8 **slot;

    stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    object[0x62] = 0;
    current = *stream;
    op = current[0];
    *stream = current + 1;
    n = 0;
    if (op & 0x10) {
        object[0x54] = D_8009B320;
        return;
    }
    if (op & 0x20) {
        id = gDuel_wSelectedCardID + 0x8000;
    } else if (op & 0x40) {
        id = gDuel_wSelectedCardID + 0xD100;
    } else {
        kind = op & 0xF;
        id = 0;
        switch (kind) {
        case 0:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            break;
        case 1:
            stats = gDuel_adwCardStats[gDuel_wSelectedCardID - 1];
            id = (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            type = (stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            id += 0x17;
            if ((u32)(type - CARD_TYPE_MAGIC) < CARD_NON_MONSTER_TYPE_COUNT) {
                object[0x62] = type;
            }
            break;
        case 2:
            id = (gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
                  CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                 CARD_STAT_GUARDIAN_STAR_MASK;
            id += 0x17;
            if (id == 0x17) {
                n = 1;
            }
            break;
        }
        if (!(op & 0x80)) {
            goto plain;
        }
        id += 0x8300;
    }
    object[0x58]++;
    n = id;
    if (id > 0xCFFF) {
        text = (u8 *)((u32)D_801C0000 & 0xFFFF0000) + D_801C0000[id - 0xD000];
    } else if (id > 0x7FFF) {
        text = (u8 *)((u32)D_801D5800 & 0xFFFF0000) + D_801D5800[id - 0x8000];
    } else {
        if (id >= 0x500) {
            n = id - 0x100;
        }
        text = (u8 *)((u32)D_801B0000 & 0xFFFF0000) + D_801C0000[n];
    }
store:
    slot = &((u8 **)object)[*(s8 *)(object + 0x58)];
    *slot = text;
    return;
plain:
    *(u16 *)(object + 0x34) |= 0x80;
    if ((u8)n == 0) {
        func_80036C14(object, id);
    }
    *(u16 *)(object + 0x34) &= 0xFF7F;
    *(u16 *)(object + 0x38) += 0x10;
}
