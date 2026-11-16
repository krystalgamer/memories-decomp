#include "../../types.h"

/* SLPM-86398 build of src/game/trade_screen_widgets.c. CardList_CreateDeckBoxStatRows
 * places both deck-box rows at the given x, with no +2, and reads the
 * viewport through the Japanese addresses. */
#define CARD_LIST_ROW_X_BIAS 0
#define gGraphics_sViewportY_abs (*(s16 *)0x8009B038)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009B036)
#include "../trade_screen_widgets.c"
