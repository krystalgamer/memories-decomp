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
 *                           and set from the options byte's low four bits.
 *                           This does not clamp the independent choice count.
 *   gDialog_bChoiceCount    How many are offered; read into an s32 and set to
 *                           4 and 7 at different prompts.
 *
 * Script_OpSavePrompt also clears gDialog_bChoiceCount.
 *
 * WAS NOT HERE, AND NOW IS
 *
 * gDialog_bChoice used to be listed here as deliberately left out: declared
 * eleven times in four ways, with the array spelling unexplained. It is no
 * longer left out: every source now shares the one declaration below.
 */
extern u8 gDialog_bChoiceEnabled;
extern s8 gDialog_bChoiceCount;

/* The selected index the other two are read against. #3149 left this out
 * because it is declared eleven times in four spellings; sorting those into
 * addressing groups, as #3154 sets out, reduces it to one question that has
 * now been measured.
 *
 * Of the five that reach it from small data, four spelled it s8 and
 * Dialog_ReadChoiceInput (dialog_choice_cursor.c) uses a u8 local. That was an abstention rather
 * than a disagreement: every use there is `u8 choice = gDialog_bChoice`,
 * assigning straight into a u8 local, so the load's signedness does not
 * survive. Taking s8 there builds byte for byte. DuelScene_Update's former
 * `s8 [9]` spelling, read only at [0], builds byte-identically as the
 * scalar now that every unit assembles at -G0. */
extern s8 gDialog_bChoice;

/* The prompt's input state, the third member of that same shared set.
 * Text_HandleChoiceCommand arms it during setup -- 1 if the optional
 * options byte's 0x80 bit is set, otherwise 0 -- and Dialog_UpdateChoice is
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
 * Dialog_UpdateChoice tests DIALOG_CHOICE_INPUT_CONFIRMED before taking the
 * choice index from the low three bits, and treats
 * DIALOG_CHOICE_INPUT_CANCELLED as completion without selecting an index.
 * The confirm/cancel meanings are also established by the live choice-input
 * trace recorded in notes/research/Unchiga_Symbols/findings.md. */
#define DIALOG_CHOICE_INPUT_CONFIRMED 0x40
#define DIALOG_CHOICE_INPUT_CANCELLED 0x80

extern u8 gDialog_bInputState;

#endif
