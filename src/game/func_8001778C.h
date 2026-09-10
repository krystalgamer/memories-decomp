#ifndef MEMORIES_DECOMP_FUNC_8001778C_H
#define MEMORIES_DECOMP_FUNC_8001778C_H

#include "../types.h"

/* Clears three fields in every entry of the D_801A7AD8 card record table: the
 * word at +0x00, the word at +0x04, and the halfword at +0x16. It walks all
 * DUEL_CARD_RECORD_COUNT entries and touches nothing else, so it resets the
 * records rather than freeing or reinitialising them.
 *
 * func_800179F4.c is the only consumer and reached it through a local extern
 * that already agreed with this. */
void func_8001778C(void);

#endif
