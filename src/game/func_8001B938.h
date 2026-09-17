#ifndef MEMORIES_DECOMP_FUNC_8001B938_H
#define MEMORIES_DECOMP_FUNC_8001B938_H

#include "../types.h"
#include "duel_selection_layout.h"

/* Fills the selection record for the hand slot the cursor is on, writing the
 * card's staging bytes and grid position into it (func_8001B938.c:27). */
void func_8001B938(DuelSelectionRecord *selection);

/* Restores the hand display after a scripted pick: the slot search runs inside
 * a one-pass do/while, which is the shape the inventory records for this
 * function (func_8001B938.c:92). */
void func_8001BAF0(void);

#endif
