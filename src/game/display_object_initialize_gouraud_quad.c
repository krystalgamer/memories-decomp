#include "../types.h"
#include "display_object.h"
#include "display_object_helpers.h"

void DisplayObject_InitializeGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
)
{
    u16 flags = object->flags;

    object->field_54 = 0;
    object->field_4C = 0;
    object->field_44.word = 0;
    object->field_3C.word = 0;
    object->field_34.word = 0;
    object->field_2C.word = 0;
    object->field_10 = 0;
    object->field_20.b.field_21 = 0;
    object->field_20.b.field_20 = 0;
    object->field_20.b.field_22 = 0;
    object->field_1C = 0;
    object->field_1A = 0;
    object->field_18 = 0;
    /* List 4 reads only this low byte to gate a second POLY_G4 submission. */
    *(u8 *)&object->field_5A = has_secondary_quad;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}
