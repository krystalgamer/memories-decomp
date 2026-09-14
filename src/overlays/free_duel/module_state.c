#include "../../types.h"
#include "../../game/display_object.h"
#include "module_state.h"

u8 gFreeDuel_abGridAvailable[FREE_DUEL_GRID_STATE_CAPACITY]
    __attribute__((section(".data"))) = {0};
DisplayObject *gFreeDuel_pThumbWidget
    __attribute__((section(".data"))) = 0;
static u32 sFreeDuel_dwUnknown105C
    __attribute__((section(".data"))) = 0;
DisplayObject *gFreeDuel_apSparklePool[FREE_DUEL_SPARKLE_POOL_CAPACITY]
    __attribute__((section(".data"))) = {0};
DisplayObject *gFreeDuel_pCursorWidget
    __attribute__((section(".data"))) = 0;
u32 gFreeDuel_dwScreenFlagsStorage
    asm("gFreeDuel_bScreenFlags")
    __attribute__((section(".data"))) = 0;
