#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_SET_RESOURCE_VARIANT_FROM_SIGN_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_SET_RESOURCE_VARIANT_FROM_SIGN_H

#include "../types.h"
#include "display_object_config.h"

/* Facing-result callback installed by the card and icon factories at +0x10.
   DisplayObject_ProjectRotationFacing supplies the projected-area word.
   The actual signature retains the narrow config view and full u32 bits;
   the body stores only their sign at +0x69. */
void DisplayObject_SetResourceVariantFromSign(
    DisplayObjectConfig *object,
    u32 value
);

#endif
