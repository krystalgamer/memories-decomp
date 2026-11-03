#include "../../types.h"

#define TEXT_LANGUAGE_INDEX_IN_DATA
#include "../duel_effect_resource_setup.h"

#define GAME_OVER_PACKAGE_START_SECTOR \
    (gText_bLanguageIndex * 0x32 + 0x289D)

#include "../file_request_game_over_package.c"
