#ifndef MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H
#define MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H

#include "../types.h"

/* Dialog_OpenChoice builds the choice-list object for a dialog record and hands
 * it back -- the caller stores it at the record's +0x30 and releases it through
 * func_8004036C when the state ends. func_800374A8 is the D_80090E64 state that
 * drives it.
 *
 * Four files declared Dialog_OpenChoice under three spellings between them,
 * `u8 *(u8 *)`, `u8 *(void *)` and `void *(void *)`. The definition takes and
 * returns u8 *; every caller passes the same record pointer and stores the
 * result in a void * slot, so the differences are views. */
void func_800374A8(u8 *object);
u8 *Dialog_OpenChoice(u8 *record);

#endif
