#include "../types.h"
#include "display_object.h"
#include "display_object_brightness.h"
#include "duel_effect_states.h"

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)

void DebugMenu_DimPrimaryDisplayObject(void)
{
    DISPLAY_OBJECT_COLOR_BYTES(TEXT_CHANNEL(1)->render_object)[0] =
        DISPLAY_OBJECT_COLOR_BYTES(TEXT_CHANNEL(1)->render_object)[1] =
        DISPLAY_OBJECT_COLOR_BYTES(TEXT_CHANNEL(1)->render_object)[2] = 0x40;
}

void DebugMenu_RestorePrimaryDisplayObject(void)
{
    DISPLAY_OBJECT_COLOR_BYTES(TEXT_CHANNEL(1)->render_object)[0] =
        DISPLAY_OBJECT_COLOR_BYTES(TEXT_CHANNEL(1)->render_object)[1] =
        DISPLAY_OBJECT_COLOR_BYTES(TEXT_CHANNEL(1)->render_object)[2] = 0x80;
}
