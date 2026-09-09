#ifndef MEMORIES_DECOMP_DIALOG_CHOICE_H
#define MEMORIES_DECOMP_DIALOG_CHOICE_H

#include "../types.h"

/* The dialog choice prompt's shared state.
 *
 * dialog_highlight_choice.h beside this one is the per-TU header for
 * Dialog_HighlightChoice, in the shape #2495 asks for. This is the state that
 * the three dialog sources and the two text-stream sources share.
 *
 *   gDialog_bChoiceEnabled  One bit per choice, tested as
 *                           `(gDialog_bChoiceEnabled >> gDialog_bChoice) & 1`
 *                           and set from `d & 0xF`, so four choices at most.
 *   gDialog_bChoiceCount    How many are offered; read into an s32 and set to
 *                           4 and 7 at different prompts.
 *
 * func_8002EE94.c also clears gDialog_bChoiceCount, spelled with a .data
 * section attribute. It does not include this header, so the two never meet
 * and no guarded arm is needed; if it ever does, that is what would go here.
 *
 * NOT HERE, ON PURPOSE
 *
 * gDialog_bChoice, the selected index these two are read against, is declared
 * eleven times in four different ways: plain s8, s8 with a .data section
 * attribute, u8 in dialog_read_choice_input.c, and `s8 [9]` in
 * duel_scene_update.c. The section attribute alone would need a guarded arm,
 * and the array spelling has to be understood before any of it can be shared.
 * It is a bigger question than these two and wants its own change.
 */
extern u8 gDialog_bChoiceEnabled;
extern s8 gDialog_bChoiceCount;

#endif
