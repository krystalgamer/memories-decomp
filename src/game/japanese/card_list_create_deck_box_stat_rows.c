#include "../../types.h"

/* The Japanese build places both deck-box rows at the given x, with no +2. */
#define CARD_LIST_ROW_X_BIAS 0
#define gGraphics_sViewportY_abs (*(s16 *)0x8009B038)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009B036)
#include "../card_list_create_deck_box_stat_rows.c"
