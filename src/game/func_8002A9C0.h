#ifndef MEMORIES_DECOMP_FUNC_8002A9C0_H
#define MEMORIES_DECOMP_FUNC_8002A9C0_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Per-frame fade and projection callback installed by func_8002ABB4. It sits
 * in the same (DisplayObject *, GsOT *) draw-callback slot func_80031874's
 * header describes, and hands the table straight to func_8005B260. */
void func_8002A9C0(DisplayObject *object, GsOT *ordering_table);

#endif
