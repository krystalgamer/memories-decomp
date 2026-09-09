#ifndef MEMORIES_DECOMP_DUEL_CARD_DISPLAY_STATE_H
#define MEMORIES_DECOMP_DUEL_CARD_DISPLAY_STATE_H

#include "../types.h"

/* The fields these three touch on a duel card's display object. This is not
 * the whole display record: the callers hold wider views of the same memory
 * under their own names. */
typedef struct {
    u8 pad_00[4];
    u8 field_04;
} DuelCardDisplayData;

typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x02];
    u32 color;
    u8 pad_10[0x11];
    u8 field_21;
    u8 field_22;
    u8 pad_23[0x44];
    u8 field_67;
    u8 pad_68[0x02];
    u8 card_index;
} DuelCardDisplayObject;

void func_80017DB4(DuelCardDisplayObject *object);
void func_80017E3C(DuelCardDisplayObject *object);

/* func_80018004.c does not consume this header: it declares and calls
 * func_80017F04 with the record alone, and arg1/arg2 arrive as whatever
 * retail left in a1/a2. See the note beside the definition. */
u8 *func_80017F04(u8 *arg0, s32 arg1, s32 arg2);

#endif
