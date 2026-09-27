#include "../../types.h"

/* SLES-03947 build of src/game/duel_result_runtime.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_DUEL_RESULT_UPDATE_ORBIT_SPRITE
#define VERSION_EUROPE_FUNC_80020EE8
#define VERSION_EUROPE_DUEL_SCENE_UPDATE_RESULT_OUTRO
#define VERSION_EUROPE_DUEL_SHOW_RESULT_PAGE

/* One 0x22-sector outro package per text language, from 0x2290; the
   duelist data from 0x2218, as in duel_init_scene.c. */
#define D_8009C02B_IN_DATA
#define DUEL_RESULT_OUTRO_START_SECTOR (0x2290 + D_8009C02B * 0x22)
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_RESULT_PAGE_TEXT_Y 0x38
#define DUEL_RESULT_PAGE_TEXT_HEIGHT 0xF0
/* This build names the loader words directly. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134
/* As in duel_scene_battle.c. */
#define func_800472A8 func_80047534

#include "../duel_result_runtime.c"
