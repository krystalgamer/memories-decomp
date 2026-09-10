#ifndef MEMORIES_DECOMP_DUEL_BATTLE_STATS_H
#define MEMORIES_DECOMP_DUEL_BATTLE_STATS_H

#include "../types.h"
#include "display_object.h"

/* Compares two duel cards' display objects by battle outcome, for the field
 * scans at the end of ai_turn_action.c: it returns the attack or defence
 * difference, 0 when neither can win outright, and -1 when `left` loses.
 * With `right` null it returns `left`'s own attack instead.
 *
 * Both operands are duel card display objects, and the only offset either is
 * reached at is 0x6A -- the D_801A7AD8 index. That is the same use
 * func_80016784 makes of the byte through this record's field_6A, and the
 * evidence for calling these DisplayObject rather than something narrower.
 *
 * The former duel_card_selection.c declared this function itself, using its
 * own DuelSelectionObject -- a 0x6B view whose header states it never leaves
 * that translation unit. Nothing compared that spelling against the u8 *
 * the definition actually took. */
s32 func_8001EFD4(DisplayObject *left, DisplayObject *right);

#endif
