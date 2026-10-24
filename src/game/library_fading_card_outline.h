#ifndef MEMORIES_DECOMP_LIBRARY_FADING_CARD_OUTLINE_H
#define MEMORIES_DECOMP_LIBRARY_FADING_CARD_OUTLINE_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Fade/projection callback installed by Library_CreateFadingCardOutline in
 * the same (DisplayObject *, GsOT *) draw-callback slot BuildDeck_DrawCardListPage's
 * header describes, and hands the table straight to Graphics_SubmitDrawModePacket. */
void Library_DrawFadingCardOutline(DisplayObject *object, GsOT *ordering_table);

/* Creates one fading outline snapshot using selected source transform and
 * dimension words. Colour starts at zero; the draw callback supplies its blue
 * fade. The returned pool object uses the original narrowed depth delta. */
DisplayObject *Library_CreateFadingCardOutline(DisplayObject *src, int add);

#endif
