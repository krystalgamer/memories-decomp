#ifndef MEMORIES_DECOMP_DUEL_CREATE_HAND_CARD_DISPLAY_OBJECT_H
#define MEMORIES_DECOMP_DUEL_CREATE_HAND_CARD_DISPLAY_OBJECT_H

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
 * Duel_CreateCardDisplayObject, whose own prototype is the ambient-argument arm of
 * duel_card_display_state.h, so the two position words the caller left in
 * $a1/$a2 reach Duel_CreateCardDisplayObject untouched and become the object's initial
 * position. Narrowing a call site to one argument would delete its argument
 * setup and change the image.
 *
 * So the arms mirror the ones this unit itself consumes: a caller that makes
 * the three-argument call defines FUNC_80018004_AMBIENT_POSITION_ARGS and gets
 * the explicit wide declaration, while the defining unit gets the real
 * prototype and is checked against it.
 *
 * The return type is a card-rendering view of the shared display-pool slot.
 * Callers storing it in hand slots cast to the canonical DisplayObject;
 * duel_draw_resolution.c also uses that wider record for +0x6C. The narrow
 * return view and both measured argument-count arms remain unchanged. */
#ifdef FUNC_80018004_AMBIENT_POSITION_ARGS
DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(
    DuelCardRecord *card, s32 x, s32 y
);
#else
DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(DuelCardRecord *card);
#endif

#endif
