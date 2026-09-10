#include "../types.h"
#include "main_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "file_transfer.h"
#include "input.h"
#include "display_object.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "display_object_transition.h"

void func_8004365C(DisplayObject *a, DisplayObject *b)
{
    DisplayObject *x;
    DisplayObject *y;
    s32 i;
    s32 hi;
    s32 c;
    s32 v;

    x = (DisplayObject *)0;

    if (a != (DisplayObject *)0) {
        x = func_800400AC(func_8004002C(), 2);
        func_800428A8(x, 0, 0, 0, 0, a->field_69, a->field_66, 0x20D,
                      D_801AF000);
        x->attribute = a->attribute;
        func_800428EC((u8 *)x, -1);
        x->flags = x->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        x->attribute = x->attribute | (GsALON | GsATWO);
        a->attribute = a->attribute | (GsALON | GsAONE);
    }

    b->field_48.h.field_48 = 0xA0;
    b->field_48.h.field_4A = 0x78;
    b->attribute = b->attribute & ~GsROTOFF;

    y = func_800400AC(func_8004002C(), 2);
    func_800428A8(y, 0, 0, 0, 0, b->field_69, b->field_66, 0x20D, D_801AF000);
    y->attribute = b->attribute;
    func_800428EC((u8 *)y, -1);
    y->flags = y->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    y->attribute = y->attribute | (GsALON | GsATWO);
    y->field_48.word = b->field_48.word;
    b->attribute = b->attribute | (GsALON | GsAONE);

    i = 0;
    hi = 0x80;

    do {
        c = ((i << 16) | (i << 8)) | i;
        b->field_0C = c;
        y->field_0C = c;
        if (a != (DisplayObject *)0) {
            c = hi - i;
            v = c * 0x60 + 0x1000;
            c = c | ((c << 16) | (c << 8));
            y->field_44.h.field_46 = v;
            y->field_44.h.field_44 = v;
            b->field_44.h.field_46 = v;
            b->field_44.h.field_44 = v;
            a->field_0C = c;
            x->field_0C = c;
        }
        i += 8;
        func_80012D4C();
    } while (i < 0x81);

    b->field_44.word = 0x10001000;
    b->attribute = (b->attribute | GsROTOFF) & ~(GsALON | GsATWO | GsAONE);
    func_8004036C(a);
    func_8004036C(x);
    func_8004036C(y);
}

void func_800438B8(s32 count)
{
    s32 found = 0;

    for (;;) {
        func_80012D4C();
        if (!found &&
            (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
              D_8009B134) == 0))
            found = 1;
        if ((gInput_wPad1Pressed &
             (PAD_BUTTON_START | PAD_BUTTON_CONFIRM_MASK)) && found)
            count = 0;
        count--;
        if (count >= 0)
            continue;
        if (!found) {
            count = 0;
            continue;
        }
        break;
    }
}
