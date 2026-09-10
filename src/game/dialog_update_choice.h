#ifndef MEMORIES_DECOMP_DIALOG_UPDATE_CHOICE_H
#define MEMORIES_DECOMP_DIALOG_UPDATE_CHOICE_H

#include "../types.h"
#include "duel_effect.h"

/* Entries 0 and 1 of the text-box state callback table D_80090E64
 * (text_box_state_callbacks.c): one frame of a choice prompt.
 *
 * On the first frame it creates the pulsing highlight object into the
 * record's field_30. After that it takes a choice either from
 * gDialog_bInputState, which the script arms, or from the pad through
 * Dialog_ReadChoiceInput plus a confirm press. A choice whose bit is clear
 * in gDialog_bChoiceEnabled is refused with sound 9. An accepted one clears
 * state byte 0x51 and, unless 0x40 is set in D_8009B34C, releases the
 * highlight and moves the record to state 3. */
void Dialog_UpdateChoice(DuelEffectChannel *p);

#endif
