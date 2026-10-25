#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_POSITION_TWEEN_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_POSITION_TWEEN_H

#include "../types.h"
#include "menu_record.h"

/* gDisplayEffect_apfnSteps display effect step: the easing sweep. It runs the record's six
 * tail halfwords -- 0x40 and 0x42 the destination, 0x44 and 0x46 the distance
 * still to travel, 0x48 the eased angle and 0x4A its per-tick step -- and
 * hands the result to the position helpers as x and y.
 *
 * Every offset belongs to MenuRecord, the callback table's argument type. */
void DisplayEffect_UpdatePositionTween(MenuRecord *record);

#endif
