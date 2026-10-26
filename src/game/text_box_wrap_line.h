#ifndef MEMORIES_DECOMP_TEXT_BOX_WRAP_LINE_H
#define MEMORIES_DECOMP_TEXT_BOX_WRAP_LINE_H

#include "../types.h"
#include "duel_effect.h"

/* Advances an object's scroll position by one step and reports whether it has
 * reached the end.
 *
 * When the counter pen_x reaches its limit rect_width the counter resets and
 * the position pen_y moves on by the step held in line_advance_y. If that would
 * carry the position past the bound rect_height the step is taken back off and 1
 * is returned, leaving the position at the last value that still fits.
 *
 * pen_x and pen_y are u16 in the record but this function compares them
 * signed, so those two reads keep an explicit `*(s16 *)&` at the use.
 *
 * The return is a flag, not a count. */
s32 TextBox_WrapLineIfNeeded(DuelEffectChannel *object);

#endif
