#ifndef MEMORIES_DECOMP_DUEL_CARD_OBJECT_HELPERS_H
#define MEMORIES_DECOMP_DUEL_CARD_OBJECT_HELPERS_H

#include "../types.h"

/* The two fields func_800181EC reads out of a card's display object: the
   flag byte at 0x22 and the type byte at 0x68. It is a view of those two
   offsets, not a claim about the rest of the record, which is why the gaps
   are still padding. */
typedef struct {
    char pad_00[0x22];
    u8 flag;
    char pad_23[0x45];
    u8 type;
} CardObject;

/* Returns the card's display value. The result is s32, not the s16 two of
   the three callers used to declare: the definition returns int, and every
   call site stores the result straight into a 16-bit field, so the narrowing
   they were describing happens at the store rather than at the return. */
s32 func_800181EC(CardObject *object);

#endif
