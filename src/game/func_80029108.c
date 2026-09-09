#include "../types.h"
#include "display_object_config.h"

extern s32 D_8009B424;
/* One argument on purpose: func_80041D60 takes three, and reads the
 * second and third on its cooldown path without assigning them. The
 * retail image calls it this way; see that file. */
extern void func_80041D60(DisplayObjectConfig *);

void func_80029108(DisplayObjectConfig *object, int arg1)
{
    int value = 2;

    if (arg1 >= 0) {
        value = object->field_6A;
    }
    if (value != object->field_69) {
        func_80040410(object, value);
        func_80041D60(object);
        D_8009B424 = 1;
    }
}
