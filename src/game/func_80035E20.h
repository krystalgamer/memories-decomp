#ifndef MEMORIES_DECOMP_FUNC_80035E20_H
#define MEMORIES_DECOMP_FUNC_80035E20_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws a duel-effect channel's text glyph, icon or rotating quad from
 * scratchpad packets. Nothing calls it directly: func_800391E4 stores its
 * address in a display object's field_4C, and the object's draw pass
 * supplies both arguments. */
void func_80035E20(DisplayObject *obj, GsOT *ot);

#endif
