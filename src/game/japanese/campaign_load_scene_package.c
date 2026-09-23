#include "../../types.h"

/* SLPM-86398 build of src/game/campaign_load_scene_package.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009B2A0 gJapanese_D_8009B2A0
#define D_8009B2A4 gJapanese_D_8009B2A4
#define D_800E9D70 gJapanese_D_800E9D70

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define CAMPAIGN_SCENE_PACKAGE_START_SECTOR 0x1E62

#include "../campaign_load_scene_package.c"
