#include "../../types.h"

#define D_8009C02B_IN_DATA

#define VERSION_EUROPE

/* The European request: 0x51 sectors from 0x2997. */
#define CARD_LIST_GRAPHICS_REQUEST_FILE_ID 0x2997
#define CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT 0x51
#include "../card_list_setup.c"
