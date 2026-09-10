#ifndef MEMORIES_DECOMP_DIALOG_READ_CHOICE_INPUT_H
#define MEMORIES_DECOMP_DIALOG_READ_CHOICE_INPUT_H

#include "../types.h"

/* One frame of choice-cursor input on a dialog's text-box record. Returns 1
 * when the repeat pad held a direction or R1 -- whether or not the cursor
 * actually moved, because a clamped edge still counts as handled and returns
 * before the cursor sound -- and 0 when it held none, which is the caller's
 * signal to look at the other buttons.
 *
 * R1 wraps past the last choice to the first; up and down clamp. The record
 * parameter is only forwarded to Dialog_HighlightChoice, which takes the same
 * `u8 *record` view in dialog_highlight_choice.h; func_8002EE94
 * (src/candidates/func_8002EE94.c) holds the same object as
 * DuelEffectChannel * and casts. */
s32 Dialog_ReadChoiceInput(u8 *record);

#endif
