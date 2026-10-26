#ifndef MEMORIES_DECOMP_DUEL_CARD_DISPLAY_OBJECTS_H
#define MEMORIES_DECOMP_DUEL_CARD_DISPLAY_OBJECTS_H

#include "../types.h"
#include "duel_card.h"
#include "duel_card_display_state.h"

/* Creates a hand-card display object and marks it if the duel is
 * showing card markers: nonzero gDuel_pActiveSideState->card_view_mode sets
 * DUEL_CARD_FLAG_DISPLAY_MARKER on the record and copies data byte +0x04
 * plus one into field_67, or 0xFF when the view mode is negative.
 *
 * The declaration is the interesting part. The definition takes the record
 * alone, but all three callers pass three arguments -- record, x, y -- and
 * that is not a mistake in any of them: Duel_CreateHandCardDisplayObject hands the record to
 * Duel_CreateCardDisplayObject through an unprototyped cast in
 * duel_init_scene.c, so the two position words the caller left in
 * $a1/$a2 reach Duel_CreateCardDisplayObject untouched and become the object's initial
 * position. Narrowing a call site to one argument would delete its argument
 * setup and change the image.
 *
 * So a caller that makes the three-argument call defines
 * FUNC_80018004_AMBIENT_POSITION_ARGS and gets the explicit wide declaration,
 * while the defining unit gets the real prototype and is checked against it.
 *
 * The return type is a card-rendering view of the shared display-pool slot.
 * Callers storing it in hand slots cast to the canonical DisplayObject;
 * duel_draw_resolution.c also uses that wider record for +0x6C. The narrow
 * return view and both measured argument-count arms below remain unchanged. */
#ifdef FUNC_80018004_AMBIENT_POSITION_ARGS
DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(
    DuelCardRecord *card, s32 x, s32 y
);
#else
DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(DuelCardRecord *card);
#endif

/* Re-derives a duel card object's visual state from its own card record:
 * gDuel_aCardRecords[object->card_index] is the record, and every field this writes
 * comes from that record's flags. Face-down sets field_22, defense-position
 * sets field_21, DUEL_CARD_FLAG_USED_THIS_TURN dims the colour, and field_67
 * is cleared unless DUEL_CARD_FLAG_DISPLAY_MARKER asks to keep it.
 *
 * The thing a prototype cannot say is why every caller calls it: the object
 * carries no state of its own here, so a caller that has just changed the
 * record's flags must call this or the object keeps showing the old ones.
 * All four do exactly that -- DuelScene_UpdateDrawPhase after clearing
 * DUEL_CARD_FLAG_USED_THIS_TURN on every occupied record, DuelScene_UpdateResume
 * after restoring two ranges of records it re-set up,
 * DuelEffect_UpdateRevealCard after
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
