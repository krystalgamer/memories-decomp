#ifndef MEMORIES_DECOMP_DUEL_BATTLE_STATS_H
#define MEMORIES_DECOMP_DUEL_BATTLE_STATS_H

#include "../types.h"
#include "display_object.h"

/* Returns the left card's position-selected, guardian-adjusted battle stat
 * minus the right card's position-selected effective stat. Equal stats yield
 * 0 if either card defends, otherwise -1; negative values are not normalized.
 * With `right` null it returns `left`'s effective attack instead.
 *
 * Both operands are duel card display objects, and the only offset either is
 * reached at is 0x6A -- the gDuel_aCardRecords index. That is the same use
 * Duel_DrawCardFrame makes of the byte through this record's field_6A, and the
 * evidence for calling these DisplayObject rather than something narrower.
 *
 * The field selectors now take canonical DuelCardRecord pointers and use
 * this same DisplayObject view, rather than separate one-pointer and
 * 0x6B-byte prefix records. DuelScene_UpdateBattle also consumes the result
 * as a damage magnitude, tie marker, or direct-attack value. */
s32 Duel_CompareBattleStrength(DisplayObject *left, DisplayObject *right);

#endif
