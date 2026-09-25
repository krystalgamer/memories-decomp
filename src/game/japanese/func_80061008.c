#include "../../types.h"

/* The Japanese build places both deck-box rows at the given x, with no +2. */
#define CARD_LIST_ROW_X_BIAS 0
#define gGraphics_sViewportY_abs (*(s16 *)0x8009B038)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009B036)
#define func_80061008 func_8004FB3C
#include "../func_80061008.c"
