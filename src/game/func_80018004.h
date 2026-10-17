#ifndef MEMORIES_DECOMP_FUNC_80018004_H
#define MEMORIES_DECOMP_FUNC_80018004_H

#include "../types.h"
#include "duel_card.h"
#include "duel_card_display_state.h"

/* Spawns the display object for one card record and marks it if the duel is
 * showing card markers: gDuel_pActiveSideState's field_1F non-zero sets
 * DUEL_CARD_FLAG_DISPLAY_MARKER on the record and copies the card data's
 * field_04 + 1 into the object's field_67, or 0xFF when that byte is negative.
 *
 * The declaration is the interesting part. The definition takes the record
 * alone, but all three callers pass three arguments -- record, x, y -- and
 * that is not a mistake in any of them: func_80018004 hands the record to
 * func_80017F04, whose own prototype is the ambient-argument arm of
 * duel_card_display_state.h, so the two position words the caller left in
 * $a1/$a2 reach func_80017F04 untouched and become the object's initial
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
DuelCardDisplayObject *func_80018004(
    DuelCardRecord *card, s32 x, s32 y
);
#else
DuelCardDisplayObject *func_80018004(DuelCardRecord *card);
#endif

#endif
