#include "../../types.h"

/* SLPM-86398 build of src/game/frontend_package_stages.c. Values that differ
 * in the Japanese release; the shared source names each with an #ifndef
 * default. */
#define VERSION_JAPAN
#define FREE_DUEL_PACKAGE_START_SECTOR 0x1E93
#define NAME_ENTRY_PACKAGE_START_SECTOR 0x1EEA
#define PASSWORD_PACKAGE_START_SECTOR 0x1F3A
#define CAMPAIGN_MAP_PACKAGE_START_SECTOR 0x1FE4
#define OPTIONS_PACKAGE_START_SECTOR 0x2120
#define OPTIONS_PACKAGE_LOAD_SECOND_BLOCK 0
#define GAME_OVER_PACKAGE_START_SECTOR 0x2152
#include "../frontend_package_stages.c"
