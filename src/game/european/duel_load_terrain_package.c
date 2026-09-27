#include "../../types.h"

/* SLES-03947 build of src/game/duel_load_terrain_package.c. The European
 * WA.MRG gives each terrain package 0xF0 sectors from 0x1B88 (US: 0xEB from
 * 0x16C6). */

#define DUEL_TERRAIN_LOAD_FIRST_SECTOR 0x1B88
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT 0xF0
/* value * 0xF0 */
#define DUEL_TERRAIN_LOAD_OFFSET(value) (((value) * 15) * 16)

/* As in duel_init_scene.c. */
#define Duel_LoadPackageStage func_800170C0

#include "../duel_load_terrain_package.c"
