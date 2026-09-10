#ifndef MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H
#define MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H

#include "../types.h"
#include "duel_effect.h"

/* Dialog_OpenChoice builds the choice-list object for a dialog record and hands
 * it back -- the caller stores it at the record's +0x30 and releases it through
 * func_8004036C when the state ends. func_800374A8, declared with the other
 * D_80090E64 callbacks, is the state that drives it.
 *
 * Four files declared Dialog_OpenChoice under three spellings between them,
 * `u8 *(u8 *)`, `u8 *(void *)` and `void *(void *)`. Every caller passes the
 * same record pointer and stores the result in a void * slot, so the
 * differences were views of one signature.
 *
 * The parameter is now the record's own type. The five fields this function
 * reads -- field_3C, field_3E, field_40, field_42 and field_59 -- are all
 * declared in DuelEffectChannel at those offsets and at these widths, and
 * every caller, func_80028310.c included, holds a DuelEffectChannel *.
 *
 * The return stays u8 *. It is a display object, not this record, and every
 * caller stores it into a void * slot. */
u8 *Dialog_OpenChoice(DuelEffectChannel *record);

/* The per-frame half of the same state: Dialog_OpenChoice builds the choice
 * list once, and this runs it, reading the cursor input and repainting the
 * entries. dialog_choice.h describes the byte the two share.
 *
 * text_box_state_callbacks.c installs it in two adjacent D_80090E64 slots and
 * declared it itself until now -- while already including this header, which
 * is what an incomplete unit header costs. */
void Dialog_UpdateChoice(DuelEffectChannel *object);

#endif
