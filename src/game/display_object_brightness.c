#include "../types.h"
#include "display_object.h"
#include "display_object_brightness.h"

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->field_0C)

void DebugMenu_DimPrimaryDisplayObject(void)
{
    DISPLAY_OBJECT_COLOR_BYTES(D_800EB184[0])[0] =
        DISPLAY_OBJECT_COLOR_BYTES(D_800EB184[0])[1] =
        DISPLAY_OBJECT_COLOR_BYTES(D_800EB184[0])[2] = 0x40;
}

void DebugMenu_RestorePrimaryDisplayObject(void)
{
    DISPLAY_OBJECT_COLOR_BYTES(D_800EB184[0])[0] =
        DISPLAY_OBJECT_COLOR_BYTES(D_800EB184[0])[1] =
        DISPLAY_OBJECT_COLOR_BYTES(D_800EB184[0])[2] = 0x80;
}
