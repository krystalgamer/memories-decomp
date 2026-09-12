#ifndef YUGIOH_GAME_DISPLAY_EFFECT_STEP_TABLE_H
#define YUGIOH_GAME_DISPLAY_EFFECT_STEP_TABLE_H

#include "../types.h"
#include "menu_record.h"

/* The display effect step table at 0x80090F68.
 *
 * Each callback receives the MenuRecord selected from D_800EB010. Left
 * unsized deliberately: its consumer indexes it with `& 0x1F`, which permits
 * thirty-two entries against the thirteen defined, so a bound here would
 * contradict the call site rather than describe it. */
extern void (*D_80090F68[])(MenuRecord *);

#endif
