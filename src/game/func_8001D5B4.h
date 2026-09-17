#ifndef MEMORIES_DECOMP_FUNC_8001D5B4_H
#define MEMORIES_DECOMP_FUNC_8001D5B4_H

#include "../types.h"
#include "duel_grid.h"

/* Steps the field cursor and reports whether it is still moving
   (func_8001D5B4.c:8). Both duel_grid.h and duel_cursor_status.h discuss this
   function's view of the cursor bytes in their own prose; neither declared it,
   and that prose is unchanged. */
s32 func_8001D5B4(DuelFieldCursor *cursor);

#endif
