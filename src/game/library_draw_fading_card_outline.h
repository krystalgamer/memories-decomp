#ifndef MEMORIES_DECOMP_LIBRARY_DRAW_FADING_CARD_OUTLINE_H
#define MEMORIES_DECOMP_LIBRARY_DRAW_FADING_CARD_OUTLINE_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Fade/projection callback installed by Library_CreateFadingCardOutline in
 * the same (DisplayObject *, GsOT *) draw-callback slot func_80031874's
 * header describes, and hands the table straight to func_8005B260. */
void Library_DrawFadingCardOutline(DisplayObject *object, GsOT *ordering_table);

#endif
