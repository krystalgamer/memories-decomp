#include "../types.h"
#include "display_object_config.h"
#include "func_80041D60.h"

extern s32 D_8009B424;

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
