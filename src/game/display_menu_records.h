#ifndef GAME_DISPLAY_MENU_RECORDS_H
#define GAME_DISPLAY_MENU_RECORDS_H

#include "../types.h"
#include "menu_record.h"

extern MenuRecord gDisplayEffect_aMenuRecords[];

/* Script_OpShowMenu selects element 0 or 1 from D_800EB010 using operand bit
 * 7, arms it, then polls its display_effect_step on subsequent ticks. */
extern MenuRecord *gScript_pMenuRecord;

#endif
