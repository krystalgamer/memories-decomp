#ifndef MEMORIES_DECOMP_FUNC_80016784_H
#define MEMORIES_DECOMP_FUNC_80016784_H

#include "../types.h"
#include "display_object.h"

/* Draws one duel card's frame from its display object.
 *
 * The record is the canonical DisplayObject: this function reads the
 * attribute at 0x04, the flags at 0x08, the colour word at 0x0C, 0x14, the
 * word at 0x20 and the four tail bytes 0x67 to 0x6A, and every one of those
 * offsets is already named on that record. 0x6A is the index into
 * D_801A7AD8 that duel_card_display_state.c writes there, which is what
 * says this object is a duel card's.
 *
 * The second argument is passed straight through to func_80042188, which
 * takes it as a word in every declaration of it in the tree. One caller
 * hands it an element of the void * array D_800E9D98, so a pointer travels
 * through this word-sized channel; that conversion belongs at the call site
 * rather than in a declaration claiming the parameter is a pointer. */
void func_80016784(DisplayObject *object, s32 arg1, s32 arg2, s32 arg3);

#endif
