#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_PROPERTY_TRANSITIONS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_PROPERTY_TRANSITIONS_H

#include "../types.h"
#include "display_object.h"

/* These are installed in the update slot by the field-action controller,
 * not called directly. Each clears the callback and active latch on completion. */
/* Steps three colour bytes toward the halfword targets by field_60. */
void DisplayObject_UpdateColorTransition(DisplayObject *object);
/* Cosine-interpolates the saved position and advances rotation over the
 * configured duration, then snaps both to their targets. */
void DisplayObject_UpdatePositionRotationTransition(DisplayObject *o);

#endif
