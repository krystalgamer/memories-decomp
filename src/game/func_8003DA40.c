#define D_8009AF74_IN_DATA
#include "../types.h"
#include "display_object.h"
#include "display_object_core.h"
#include "duel_effect.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "func_80039794.h"
#include "display_object_interpolation.h"
#include "duel_effect_create_channel.h"
#include "func_8003DA40.h"

void func_8003DA40(MenuRecord *record)
{
    DisplayObject *e;
    DuelEffectChannel *q;
    s32 f;
    s32 g;
    s32 v;
    s32 h;

    f = D_8009B3C1;
    if ((f & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 = f | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        e = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(e, 0x20, -0x40, 3, 2, 0, 0xB, 0x20C);
        e->flags = e->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                   DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_80042918(e);
        func_800428EC((u8 *)e, (s8)(*(u8 *)&D_8009AF74[1] - 2));
        record->grid[0][0] = (s32)e;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)e);
        e->field_60 = -0x400;
        q = DuelEffect_CreateChannel(0xD0, 0);
        q->flags_34 = q->flags_34 | 4;
        do {
            func_80039794();
        } while (q->field_30 == 0);
        TextBox_SetPos(q,
                       *(s16 *)&e->field_30.h.field_30,
                       *(s16 *)&e->field_30.h.field_32);
    }

    e = (DisplayObject *)record->grid[0][0];
    /* The dialog channel's index. It is the byte at 0x1A, inside grid's
       third row, and dialog_transition.c reads it the same way; nothing
       matched yet writes it, so MenuRecord has no name for it. */
    q = &D_800EB0F8[((u8 *)record)[0x1A]];
    g = D_8009B3C1;

    if ((g & 0x40) != 0) {
        func_80039794();
        if ((q->flags_34 & 0x2000) != 0) {
            D_8009B3C1 = 0;
        }
    } else {
        v = *(u16 *)&e->field_60 + 0x20;
        e->field_60 = v;
        if ((s16)v >= 0) {
            *(s16 *)&e->field_30.h.field_30 = 0x20;
            *(s16 *)&e->field_30.h.field_32 = 0x50;
            h = q->flags_34;
            D_8009B3C1 = g | 0x40;
            q->flags_34 = h & 0xFFFB;
        } else {
            Widget_SlideSine((DisplayObjectPosition *)e, 0x20, 0x50, (s16)v);
        }
        TextBox_SetPos(q,
                       *(s16 *)&e->field_30.h.field_30,
                       *(s16 *)&e->field_30.h.field_32);
    }
}
