#ifndef MEMORIES_DECOMP_LIBRARY_CREATE_FADING_CARD_OUTLINE_H
#define MEMORIES_DECOMP_LIBRARY_CREATE_FADING_CARD_OUTLINE_H

#include "../types.h"
#include "display_object.h"

/* Creates one fading outline snapshot using selected source transform and
 * dimension words. Colour starts at zero; the draw callback supplies its blue
 * fade. The returned pool object uses the original narrowed depth delta. */
DisplayObject *Library_CreateFadingCardOutline(DisplayObject *src, int add);

#endif
