#include "../types.h"
#include "graphics_frame.h"
#include "display_object.h"
#include "widget_update_pulse_colour.h"

void Widget_UpdatePulseColour(u8 *arg0)
{
    DisplayObject *q = (DisplayObject *)arg0;
    s32 c = 0;
    s32 t = D_8009B09C & 0x7F;
    if (t >= 0x40)
        t = 0x7F - t;
    t = t * 2 + 0x80;
    if (((u8 *)&q->field_0C)[0] != 0)
        c = t;
    if (((u8 *)&q->field_0C)[1] != 0)
        c |= t << 8;
    if (((u8 *)&q->field_0C)[2] != 0)
        c |= t << 16;
    q->field_3C.word = c;
    q->field_44.word = c;
    c = (c & 0xFCFCFC) / 4;
    q->field_2C.word = c;
    q->field_34.word = c;
    q->field_4C = c;
    *(s32 *)&q->field_54 = c;
}
