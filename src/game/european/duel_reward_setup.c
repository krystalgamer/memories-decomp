#include "../../types.h"

#define D_8009C02B_IN_DATA

#define VERSION_EUROPE
#define VERSION_EUROPE_DUEL_REWARD_SETUP
#define VERSION_EUROPE_FUNC_80032328

/* The European request: 0x51 sectors from 0x2997. */
#define DUEL_REWARD_REQUEST_FILE_ID 0x2997
#define DUEL_REWARD_REQUEST_SECTOR_COUNT 0x51
#define func_80032184 func_8003236C
#define func_80032328 func_80032568
#define D_8009B0F4_abs D_8009B0F4
#include "../duel_reward_setup.c"
