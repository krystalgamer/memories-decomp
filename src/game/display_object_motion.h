#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_MOTION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_MOTION_H

#include "display_object.h"

/* The ritual controller installs this as DisplayObjectCallback (u8 *).
 * Keep that measured callback view separate from the typed direct-call and
 * definition view; the callback receives the same display-record address. */
#ifdef DISPLAY_OBJECT_MOTION_BYTE_CALLBACK
void DisplayObject_UpdateCosineSlide(u8 *object);
#else
void DisplayObject_UpdateCosineSlide(DisplayObject *object);
#endif
void func_8001ED20(DisplayObject *object);

#endif
