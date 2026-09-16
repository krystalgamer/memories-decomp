#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_MENU_RECORD_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_MENU_RECORD_H

#include "../types.h"
#include "menu_record.h"

/* Ticks one menu record's display effect: the flag byte at 0x32, the two
 * object slots at 0x04 and 0x08, the delay halfword at 0x3E and the retry
 * counter at 0x3B.
 *
 * This took u8 * because none of that was expressible. All of it is now:
 * 0x04 and 0x08 are grid[0][1] and grid[0][2] -- #3551 established that
 * grid[4][3] is four DisplayPositionGroup children arrays -- and 0x3E is
 * field_3E, named from the DisplayEffectState view that already had it.
 * Both consumers hold the record as MenuRecord and no longer cast. */
void DisplayEffect_UpdateMenuRecord(MenuRecord *p, s32 n);

#endif
