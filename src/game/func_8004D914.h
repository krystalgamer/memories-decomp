#ifndef MEMORIES_DECOMP_FUNC_8004D914_H
#define MEMORIES_DECOMP_FUNC_8004D914_H

#include "../types.h"

/* Relinks the command list of one model slot's animation row tables.
 *
 * It returns immediately unless the slot has both a command list at +0xDD8
 * and a nonzero channel count at +0xE1B, then walks that count of channels by
 * MODEL_SLOT_ROW_COUNT rows, rewriting halfword links in the list so entries
 * point at one another rather than at a key.
 *
 * model_slot_row_tables.c describes the family this belongs to: the reset
 * fills the keys with 0xFFFF and stores the command list at 0xDD8, and the
 * walk later claims those keys. This is the pass between them. */
void func_8004D914(s32 slot);

#endif
