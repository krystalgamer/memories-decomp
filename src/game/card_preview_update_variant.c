#include "../types.h"
#include "display_object_config.h"
#include "display_object.h"
#include "card_preview_callbacks.h"
#define DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_AMBIENT_ARGS
#include "display_object_update_command_stream.h"

void CardPreview_UpdateVariant(DisplayObject *source, s32 facing_area)
{
    DisplayObjectConfig *object = (DisplayObjectConfig *)source;
    int requested_variant = 2;

    if (facing_area >= 0) {
        requested_variant = source->field_6A;
    }
    if (requested_variant != object->resource_variant) {
        DisplayObject_SetResourceVariant(object, requested_variant);
        DisplayObject_UpdateCommandStream((DisplayObject *)object);
        gDisplayObject_fRenderRetry = 1;
    }
}
