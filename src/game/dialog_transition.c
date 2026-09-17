#define D_8009AF74_IN_DATA
#define GDIALOG_CHOICE_IN_DATA
#include "../types.h"
#include "../unmatched.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "display_object_core.h"

#include "duel_effect.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "func_80039794.h"
#include "duel_effect_create_channel.h"
#include "dialog_transition.h"
#include "dialog_choice.h"
#include "func_80042C08.h"

void func_8003D518(MenuRecord *record)
{
    DisplayObject *object;
    s32 flags;

    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        object->field_30.h.field_30 = 160;
        object->field_30.h.field_32 = 120;
        object->field_48.h.field_48 = 128;
        object->field_48.h.field_4A = 224;
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset((u8 *)object, (s8)(*(u8 *)&D_8009AF74[1] - 3));
        object->field_4C = (s32)func_80042C08;
        record->grid[0][1] = (s32)object;
    }
    flags = D_8009B3C1;
    object = (DisplayObject *)record->grid[0][1];
    if (flags & 64) {
        D_8009B3C1 = 0;
    } else {
        s32 x = *(u16 *)&object->field_48.h.field_48 - 8;
        s32 y = *(u16 *)&object->field_48.h.field_4A - 8;

        object->field_48.h.field_4A = y;
        object->field_48.h.field_48 = x;
        if ((s16)x <= 0) {
            D_8009B3C1 = flags | 64;
            object->field_48.h.field_48 = 0;
            object->field_48.h.field_4A = 64;
        }
    }
}

void func_8003D614(MenuRecord *record)
{
    DisplayObject *object;
    DuelEffectChannel *entry;

    if (!(D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        object = (DisplayObject *)record->grid[0][0];
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
        object->field_60 = 0x400;
    }
    /* The dialog channel's index: the byte at 0x1A, inside grid's third
       row, which MenuRecord does not name. */
    entry = &D_800EB0F8[((u8 *)record)[0x1A]];
    object = (DisplayObject *)record->grid[0][0];
    if (object) {
        *(u16 *)&object->field_60 -= 0x40;
        if (object->field_60 <= 0) {
            DisplayObject_ReleaseIfPresent(object);
            record->grid[0][0] = 0;
        } else {
            Widget_SlideSine(
                (DisplayObjectPosition *)object,
                0x20,
                -0x40,
                object->field_60
            );
            TextBox_SetPos(
                entry,
                *(s16 *)&object->field_30.h.field_30,
                *(s16 *)&object->field_30.h.field_32
            );
        }
    }
    object = (DisplayObject *)record->grid[0][1];
    if (object) {
        *(u16 *)&object->field_48.h.field_4A += 8;
        *(u16 *)&object->field_48.h.field_48 += 8;
        if (object->field_48.h.field_48 >= 0xC0) {
            DisplayObject_ReleaseIfPresent(object);
            record->grid[0][1] = 0;
        }
    }
    if (!record->grid[0][0] && !record->grid[0][1])
        D_8009B3C1 = 0;
}

void func_8003D74C(MenuRecord *record)
{
    DisplayObject *p;
    DuelEffectChannel *r;
    s32 f;
    s32 v;
    s32 t;
    s32 a;
    s32 g;

    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 =
            D_8009B3C1 | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        p->field_30.h.field_30 = 0xA0;
        p->field_30.h.field_32 = 0x78;
        p->field_48.h.field_48 = 0x80;
        p->field_48.h.field_4A = 0xE0;
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset((u8 *)p, (s8)(*(u8 *)&D_8009AF74[1] - 3));
        p->field_4C = (s32)func_80042C08;
        record->grid[0][1] = (s32)p;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(p, 0x20, -0x40, 3, 2, 0, 0xB, 0x20C);
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                   DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset((u8 *)p, (s8)(*(u8 *)&D_8009AF74[1] - 2));
        record->grid[0][0] = (s32)p;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)p);
        a = D_8009B3C7;
        p->field_60 = -0x400;
        r = DuelEffect_CreateChannel((a & 1) | 0xD0, 0);
        r->flags_34 = r->flags_34 | 4;
        do {
            func_80039794();
        } while (r->field_30 == 0);
        TextBox_SetPos(r, *(s16 *)&p->field_30.h.field_30,
                       *(s16 *)&p->field_30.h.field_32);
    }
    f = D_8009B3C1;
    if ((f & 0x20) != 0) {
        D_8009B3C1 = 0;
        return;
    }
    /* The dialog channel's index: the byte at 0x1A, inside grid's third
       row, which MenuRecord does not name. */
    r = &D_800EB0F8[((u8 *)record)[0x1A]];
    if ((f & 0x40) != 0) {
        func_80039794();
        if ((r->flags_34 & TEXT_BOX_FLAG_DONE) != 0) {
            D_8009B3C1 = D_8009B3C1 | 0x20;
            if (gDialog_bChoice == 0) {
                DuelEffect_CreateChannel(0x80D4, 0);
                gDialog_bChoice = 0;
            }
        }
        return;
    }
    p = (DisplayObject *)record->grid[0][0];
    t = *(u16 *)&p->field_60;
    if (p->field_60 < 0) {
        v = t + 0x40;
        p->field_60 = v;
        if ((s16)v >= 0) {
            p->field_30.h.field_30 = 0x20;
            p->field_60 = 0;
            p->field_30.h.field_32 = 0x50;
        } else {
            Widget_SlideSine((DisplayObjectPosition *)p, 0x20, 0x50, (s16)v);
        }
        TextBox_SetPos(r, *(s16 *)&p->field_30.h.field_30,
                       *(s16 *)&p->field_30.h.field_32);
    }
    p = (DisplayObject *)record->grid[0][1];
    p->field_48.h.field_4A = *(u16 *)&p->field_48.h.field_4A - 8;
    p->field_48.h.field_48 = *(u16 *)&p->field_48.h.field_48 - 8;
    if (p->field_48.h.field_48 <= 0) {
        p->field_48.h.field_48 = 0;
        p->field_48.h.field_4A = 0x40;
        if (((DisplayObject *)record->grid[0][0])->field_60 == 0) {
            g = D_8009B3C1;
            r->flags_34 = r->flags_34 & 0xFFFB;
            D_8009B3C1 = g | 0x40;
        }
    }
}
