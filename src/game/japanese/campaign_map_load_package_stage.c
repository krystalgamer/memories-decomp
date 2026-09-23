#include "../../types.h"

/* SLPM-86398 build of src/game/campaign_map_load_package_stage.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define CAMPAIGN_MAP_PACKAGE_START_SECTOR 0x1FE4

#include "../campaign_map_load_package_stage.c"
