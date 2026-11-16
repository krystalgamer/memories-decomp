#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/trade_screen_widgets.c: the European
 * build with these images' viewport addresses for CardList_CreateDeckBoxStatRows. */
#define gGraphics_sViewportY_abs (*(s16 *)0x8009C4C2)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009C4C0)
#include "../european/trade_screen_widgets.c"
