#include "../../types.h"

/* SLES-03947 build of src/game/duel_result_runtime.c. */

#define VERSION_EUROPE

/* One 0x22-sector outro package per text language, from 0x2290; the
   duelist data from 0x2218, as in duel_init_scene.c. */
#define D_8009C02B_IN_DATA
#define DUEL_RESULT_OUTRO_START_SECTOR (0x2290 + gText_bLanguageIndex * 0x22)
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_RESULT_PAGE_TEXT_Y 0x38
#define DUEL_RESULT_PAGE_TEXT_HEIGHT 0xF0
/* As in duel_scene_battle.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_STARCHIP_Y 210
#define DUEL_RESULT_RANK_Y 40

#include "../duel_result_runtime.c"
