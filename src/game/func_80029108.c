#include "../types.h"
#include "display_object_config.h"
#include "display_object.h"
#include "card_preview_callbacks.h"
#define FUNC_80041D60_AMBIENT_ARGS
#include "func_80041D60.h"

void func_80029108(DisplayObjectConfig *object, int arg1)
{
    int value = 2;

    if (arg1 >= 0) {
        value = object->field_6A;
    }
    if (value != object->field_69) {
        func_80040410(object, value);
        func_80041D60((DisplayObject *)object);
        D_8009B424 = 1;
    }
}
