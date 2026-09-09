#include "../types.h"
#include "display_object.h"
#include "display_object_config.h"
#include "display_object_api.h"
#include "display_object_helpers.h"

DisplayObject *func_80019564(DisplayObjectConfigView *source)
{
    DisplayObject *object =
        (DisplayObject *)func_800400AC(func_8004002C(), 1);

    func_80040510(
        (DisplayObjectConfigView *)object,
        source->x,
        source->y,
        0x8C,
        0xC4,
        0,
        0,
        0x15,
        0,
        0
    );
    object->field_18 = 0x46;
    object->field_48.h.field_48 = 0x46;
    object->field_1A = 0x62;
    object->field_48.h.field_4A = 0x62;
    func_80042918(object);
    object->attribute |= 0x02000000;
    return object;
}
