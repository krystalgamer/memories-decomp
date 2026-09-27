#include "../../types.h"

/* SLES-03947 build of src/game/model_load_monster_merge.c. The US build reaches these objects
 * through second .data views named *_abs; the European build names the
 * objects only. The US source is included as is. */
#define D_8009B0F4_abs D_8009B0F4

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x5D4
#define MODEL_AUX_FILE_START_SECTOR 0x2A8

#include "../model_load_monster_merge.c"
