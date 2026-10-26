#include "../types.h"
#include "display_object_config.h"
#include "display_object_set_resource_variant_from_sign.h"

void DisplayObject_SetResourceVariantFromSign(
    DisplayObjectConfig *object,
    u32 value
)
{
    object->resource_variant = value >> 31;
}
