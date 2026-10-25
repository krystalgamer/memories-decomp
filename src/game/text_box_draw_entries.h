#ifndef MEMORIES_DECOMP_TEXT_BOX_DRAW_ENTRIES_H
#define MEMORIES_DECOMP_TEXT_BOX_DRAW_ENTRIES_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws a duel-effect channel's text glyph, icon or rotating quad from
 * scratchpad packets. Nothing calls it directly: TextBox_RefreshDisplayObjects stores its
 * address in a display object's field_4C, and the object's draw pass
 * supplies both arguments. */
void TextBox_DrawEntries(DisplayObject *obj, GsOT *ot);

#endif
