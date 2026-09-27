#include "../../types.h"

#define D_8009C02B_IN_DATA
#include "../duel_effect_resource_setup.h"

#define GAME_OVER_PACKAGE_START_SECTOR \
    (D_8009C02B * 0x32 + 0x289D)

#include "../file_request_game_over_package.c"
