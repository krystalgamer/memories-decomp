#define D_8009B09C gJapanese_FrameCounter
#include "../../types.h"
#include "../graphics_frame.h"
#include "../display_object.h"
#include "../widget_update_pulse_colour.h"

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->field_0C)

void Widget_UpdatePulseColour(DisplayObject *object)
{
    DisplayObject *q = object;
    s32 c = 0;
    s32 t = gJapanese_FrameCounter & 0x7F;

    if (t >= 0x40)
        t = 0x7F - t;
    t = t * 2 + 0x80;
    if (DISPLAY_OBJECT_COLOR_BYTES(q)[0] != 0)
        c = t;
    if (DISPLAY_OBJECT_COLOR_BYTES(q)[1] != 0)
        c |= t << 8;
    if (DISPLAY_OBJECT_COLOR_BYTES(q)[2] != 0)
        c |= t << 16;
    q->field_3C.word = c;
    q->field_44.word = c;
    c = (c & 0xFCFCFC) / 4;
    q->field_2C.word = c;
    q->field_34.word = c;
    q->field_4C = c;
    *(s32 *)&q->field_54 = c;
}
