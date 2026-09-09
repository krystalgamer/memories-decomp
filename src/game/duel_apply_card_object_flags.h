#ifndef MEMORIES_DECOMP_DUEL_APPLY_CARD_OBJECT_FLAGS_H
#define MEMORIES_DECOMP_DUEL_APPLY_CARD_OBJECT_FLAGS_H

#include "../types.h"
#include "duel_card_display_state.h"

/* Re-derives a duel card object's visual state from its own card record:
 * D_801A7AD8[object->card_index] is the record, and every field this writes
 * comes from that record's flags. Face-down sets field_22, defense-position
 * sets field_21, DUEL_CARD_FLAG_USED_THIS_TURN dims the colour, and field_67
 * is cleared unless DUEL_CARD_FLAG_DISPLAY_MARKER asks to keep it.
 *
 * The thing a prototype cannot say is why every caller calls it: the object
 * carries no state of its own here, so a caller that has just changed the
 * record's flags must call this or the object keeps showing the old ones.
 * All four do exactly that -- func_8001898C after clearing
 * DUEL_CARD_FLAG_USED_THIS_TURN on every occupied record, func_8001825C
 * after restoring two ranges of records it re-set up, func_80025B28 after
 * clearing the field-effect bits, and func_800229F4 in the shared reset block
 * of the card flip animation.
 *
 * `DuelCardDisplayObject *` is the definition's own view. The callers hold
 * three others of the same memory -- DuelCardTurnObject, a local struct Obj,
 * and a raw u8 * read out of the record's first word -- which is why the four
 * local declarations this header replaces disagreed: two said u8 *, one u8 *
 * with a named argument, one Obj *. The pointer value is identical in every
 * case, so the call sites cast and the resident image is unchanged. */
void Duel_ApplyCardObjectFlags(DuelCardDisplayObject *object);

#endif
