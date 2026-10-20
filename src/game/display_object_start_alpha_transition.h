#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_START_ALPHA_TRANSITION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_START_ALPHA_TRANSITION_H

#include "../types.h"
#include "display_object.h"

/* Sets field_21 to start, stores target at 0x28 and the signed step at 0x2A,
   sets field_6C and the clip-test flag, and installs
   DisplayObject_UpdateAlphaTransition as the update callback. */
void DisplayObject_StartAlphaTransition(
    DisplayObject *object, u8 start, s16 target, s16 step
);

#endif
