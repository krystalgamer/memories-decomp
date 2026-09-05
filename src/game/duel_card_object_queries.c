#include "../types.h"

#include "duel_card.h"

typedef struct {
    u32 value;
    u8 pad4[0x12];
    u16 flags;
    u8 pad18[4];
} Entry;

extern u8 D_8009B1D5;
extern Entry D_801A7B64[];
extern u16 Duel_CalcCardStats(Entry *);

void func_8002C938(u32 *output, int alternate)
{
    register int base __asm__("$6") =
        D_8009B1D5 ? DUEL_FIELD_ROW_SIZE : 20;
    register int scaled __asm__("$3");
    register DuelCardRecord *entry __asm__("$3");

    if (alternate) {
        base += DUEL_FIELD_ROW_SIZE;
    }
    scaled = base << 3;
    scaled -= base;
    scaled <<= 2;
    entry = (DuelCardRecord *)((u8 *)D_801A7AD8 + scaled);
    base = 0;
    for (; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
        if (entry->flags & 0x8000) {
            *output++ = (u32)entry->object;
        }
    }
    *output = 0;
}

void func_8002C9B4(u32 *output, int selector)
{
    register Entry *entry __asm__("$16");
    register int base __asm__("$17");
    Entry *other;
    int index;

    if (selector < 0) {
        __asm__ volatile(
            "lui $2,%%hi(D_801A7B64)\n\t"
            "addiu %0,$2,%%lo(D_801A7B64)"
            : "=r"(entry));
        other = entry + DUEL_CARD_SIDE_RECORD_COUNT;
        for (
            index = 0;
            index < DUEL_FIELD_SIDE_ZONE_COUNT;
            index++, entry++, other++
        ) {
            if (entry->flags & 0x8000)
                *output++ = entry->value;
            if (other->flags & 0x8000)
                *output++ = other->value;
        }
        *output = 0;
        return;
    }

    base = D_8009B1D5 ? DUEL_FIELD_ROW_SIZE : 20;
    __asm__ volatile(
        "sll $2,%1,3\n\t"
        "subu $2,$2,%1\n\t"
        "sll $2,$2,2\n\t"
        "lui $3,%%hi(D_801A7AD8)\n\t"
        "addiu $3,$3,%%lo(D_801A7AD8)\n\t"
        "addu %0,$2,$3"
        : "=r"(entry)
        : "r"(base)
        : "$2", "$3");
    if (selector >= 21) {
        for (index = 0; index < DUEL_FIELD_ROW_SIZE; index++, entry++) {
            if ((entry->flags & 0x8000) &&
                (u16)Duel_CalcCardStats(entry) >= selector)
                *output++ = entry->value;
        }
    } else {
        for (index = 0; index < DUEL_FIELD_ROW_SIZE; index++, entry++) {
            if ((entry->flags & 0x8000) &&
                (selector < 0 || *(u8 *)(entry->value + 0x68) == selector))
                *output++ = entry->value;
        }
    }
    *output = 0;
}
