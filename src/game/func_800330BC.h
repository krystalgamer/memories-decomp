#ifndef MEMORIES_DECOMP_FUNC_800330BC_H
#define MEMORIES_DECOMP_FUNC_800330BC_H

#include "../types.h"
#include "card_list_text_boxes.h"

/* One step of the card list's cursor and paging input.
 *
 * It first places the scroll box, deriving a y position from the combined
 * first and cursor rows scaled across 152 pixels by sort_row_count. It then
 * settles any outstanding scroll: while first differs from first_target it
 * moves first one row towards it, refreshes eight rows through
 * func_80031E04 and returns immediately, so a page scroll is animated a row
 * per call rather than jumped. Only once the two agree does it read the pad
 * for paging and sorting.
 *
 * The return value is a handled flag: build_deck_pane_input.c, the only
 * consumer, tests it against zero at both call sites and does no more work
 * when it is set.
 *
 * The definition's spelling is load bearing, and the unit's own comment lists
 * why: a backward goto rather than a loop construct, a shared commit tail
 * reached by goto so the retail block order survives, `row_count - 8`
 * respelled at every use instead of held in a local, and the D_80090DD8
 * lookup kept statement-for-statement. None of that should be tidied without
 * a measurement.
 *
 * The name stays address-based. What the function does is legible, but the
 * list it drives is described by card_list_text_boxes.h rather than named
 * here, and nothing in this unit settles which list screen owns it. */
s32 func_800330BC(CardList *list);

#endif
