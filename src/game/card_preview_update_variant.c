#include "../types.h"
#include "display_object_config.h"
#include "display_object.h"
#include "card_preview_callbacks.h"
#define DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_AMBIENT_ARGS
#include "display_object_update_command_stream.h"

void CardPreview_UpdateVariant(CardVariantSource *source, s32 arg1)
{
    DisplayObjectConfig *object = (DisplayObjectConfig *)source;
    int value = 2;

    if (arg1 >= 0) {
        value = source->variant;
    }
    if (value != object->field_69) {
        DisplayObject_SetResourceVariant(object, value);
        DisplayObject_UpdateCommandStream((DisplayObject *)object);
        D_8009B424 = 1;
    }
}
