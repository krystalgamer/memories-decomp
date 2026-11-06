#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_MOTION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_MOTION_H

#include "display_object.h"

/* Callers that install this as a DisplayObjectCallback (u8 *) cast it; the
 * callback receives the same display-record address. */
void DisplayObject_UpdateCosineSlide(DisplayObject *object);

#endif
