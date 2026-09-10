#ifndef MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H
#define MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H

#include "../types.h"
#include "duel_effect.h"

/* Dialog_OpenChoice builds the choice-list object for a dialog record and hands
 * it back -- the caller stores it at the record's +0x30 and releases it through
 * func_8004036C when the state ends. func_800374A8 is the D_80090E64 state that
 * drives it.
 *
 * Four files declared Dialog_OpenChoice under three spellings between them,
 * `u8 *(u8 *)`, `u8 *(void *)` and `void *(void *)`. Every caller passes the
 * same record pointer and stores the result in a void * slot, so the
 * differences were views of one signature.
 *
 * The parameter is now the record's own type. The five fields this function
 * reads -- field_3C, field_3E, field_40, field_42 and field_59 -- are all
 * declared in DuelEffectChannel at those offsets and at these widths, and two
 * of the three callers already held a DuelEffectChannel * and cast it away.
 * func_80028310.c keeps its own `void *` spelling: it does not include this
 * header, and a void * parameter accepts the record unchanged.
 *
 * The return stays u8 *. It is a display object, not this record, and every
 * caller stores it into a void * slot. */
void func_800374A8(DuelEffectChannel *object);
u8 *Dialog_OpenChoice(DuelEffectChannel *record);

#endif
