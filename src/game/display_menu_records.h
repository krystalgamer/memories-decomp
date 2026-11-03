#ifndef GAME_DISPLAY_MENU_RECORDS_H
#define GAME_DISPLAY_MENU_RECORDS_H

#include "../types.h"
#include "menu_record.h"

/* Script_OpShowMenu selects element 0 or 1 from gDisplayEffect_aMenuRecords using operand bit
 * 7, arms it, then polls its display_effect_step on subsequent ticks. */
extern MenuRecord *gScript_pMenuRecord;

#endif
