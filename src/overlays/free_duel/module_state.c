#include "../../types.h"
#include "../../game/display_object.h"
#include "module_state.h"

u8 gFreeDuel_abGridAvailable[FREE_DUEL_GRID_STATE_CAPACITY] = {0};
DisplayObject *gFreeDuel_pThumbWidget = 0;
static u32 sFreeDuel_dwUnknown105C = 0;
DisplayObject *gFreeDuel_apSparklePool[FREE_DUEL_SPARKLE_POOL_CAPACITY] = {0};
DisplayObject *gFreeDuel_pCursorWidget = 0;
u8 gFreeDuel_bScreenFlags = 0;
