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
 * func_8002EE94 (src/candidates/func_8002EE94.c) also clears
 * gDialog_bChoiceCount, spelled with a .data
 * section attribute. It does not include this header, so the two never meet
 * and no guarded arm is needed; if it ever does, that is what would go here.
 *
 * NOT HERE, ON PURPOSE
 *
 * gDialog_bChoice, the selected index these two are read against, is declared
 * eleven times in four different ways: plain s8, s8 with a .data section
 * attribute, u8 in dialog_read_choice_input.c (now a candidate), and
 * `s8 [9]` in src/candidates/func_80024200.c. The section attribute alone
 * would need a guarded arm, and the array spelling has to be understood
 * before any of it can be shared. It is a bigger question than these two and
 * wants its own change.
 */
extern u8 gDialog_bChoiceEnabled;
extern s8 gDialog_bChoiceCount;

/* The selected index the other two are read against. #3149 left this out
 * because it is declared eleven times in four spellings; sorting those into
 * addressing groups, as #3154 sets out, reduces it to one question that has
 * now been measured.
 *
 * Five sources spell it `s8` with a .data section attribute and one spells it
 * `s8 [9]`; both of those are the lever that keeps it out of -G8 small data,
 * and none of those six includes this header. The card-move presentation
 * also needs an absolute load and selects GDIALOG_CHOICE_IN_DATA here.
 *
 * Of the five that reach it from small data, four spelled it s8 and
 * dialog_read_choice_input.c spelled it u8. That was an abstention rather
 * than a disagreement: every use there is `u8 choice = gDialog_bChoice`,
 * assigning straight into a u8 local, so the load's signedness does not
 * survive. Taking s8 there builds byte for byte.
 */
#ifdef GDIALOG_CHOICE_IN_DATA
extern s8 gDialog_bChoice __attribute__((section(".data")));
#else
extern s8 gDialog_bChoice;
#endif

/* The prompt's input state, the third member of that same shared set.
 * Text_HandleChoiceCommand arms it when the command arrives -- 1 if the
 * command byte's 0x80 bit is set, otherwise 0 -- and Dialog_UpdateChoice is
 * the reader that spends it, clearing it back to 0.
 *
 * Declared u8, and one reader deliberately disagrees with that in place:
 *
 *     f = *(s8 *)&gDialog_bInputState;
 *     g = gDialog_bInputState;
 *
 * Dialog_UpdateChoice takes the same byte twice, once through an explicit
 * s8 cast and once plainly, and both loads are in the matched bytes. The
 * cast is the lever that keeps retail's signed load, so it stays exactly
 * where it is rather than being resolved into the declaration.
 *
 * Two of its arms have no writer. The function tests 0x40 -- taking the
 * choice index from the low three bits and clearing the flag -- and 0x80,
 * but notes/global-usage lists Dialog_UpdateChoice and
 * Text_HandleChoiceCommand as the only accessors of this address in the
 * image, assembly included, and between them they store only 0, 1 and
 * `g & 0xBF`. Recorded as an observation about the code, not a claim about
 * intent. */
extern u8 gDialog_bInputState;

#endif
