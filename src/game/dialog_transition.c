#define D_8009AF74_IN_DATA
#define GDIALOG_CHOICE_IN_DATA
#include "../types.h"
#include "../unmatched.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "display_object_core.h"

#include "duel_effect.h"
#include "dialog_callback_state.h"
#include "text_channel_state.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "text_box_runtime.h"
#include "text_box_update_channels.h"
#include "duel_effect_create_channel.h"
#include "dialog_transition.h"
#include "dialog_update_message_transition.h"
#include "dialog_layout.h"
#include "dialog_choice.h"

#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DISPLAY_OBJECT_POSITION_VIEW(object) \
    ((DisplayObjectPosition *)(object))
#define DISPLAY_OBJECT_SNAPSHOT_VIEW(object) \
    ((DisplayObjectSnapshot *)(object))

/* Regional values, which the European wrappers define for themselves: the
   spotlight's centre y (120 in the US build, 128 in the European one) and
   the channel flag Dialog_UpdateOpenTransition sets while its text box is up (4 in the US
   build, 8 in the European one). */
#ifndef DIALOG_TRANSITION_SPOTLIGHT_Y
#define DIALOG_TRANSITION_SPOTLIGHT_Y 120
#endif
#ifndef DIALOG_TRANSITION_CHANNEL_FLAG
#define DIALOG_TRANSITION_CHANNEL_FLAG 4
#endif

void Dialog_UpdateSpotlightContraction(MenuRecord *record)
{
    DisplayObject *object;
    s32 flags;

    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        object->field_30.h.field_30 = 160;
        object->field_30.h.field_32 = DIALOG_TRANSITION_SPOTLIGHT_Y;
        object->field_48.h.field_48 = 128;
        object->field_48.h.field_4A = 224;
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset(object, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 3));
        object->field_4C = (s32)DisplayObject_RenderSpotlightMask;
        record->grid[0][1] = (s32)object;
    }
    flags = gDialog_bCallbackState;
    object = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    if (flags & 64) {
        gDialog_bCallbackState = 0;
    } else {
        s32 x = *(u16 *)&object->field_48.h.field_48 - 8;
        s32 y = *(u16 *)&object->field_48.h.field_4A - 8;

        object->field_48.h.field_4A = y;
        object->field_48.h.field_48 = x;
        if ((s16)x <= 0) {
            gDialog_bCallbackState = flags | 64;
            object->field_48.h.field_48 = 0;
            object->field_48.h.field_4A = 64;
        }
    }
}

void Dialog_UpdateCloseTransition(MenuRecord *record)
{
    DisplayObject *object;
    DuelEffectChannel *entry;

    if (!(gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        object = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
        object->field_60 = 0x400;
    }
    /* The dialog channel's index: the byte at 0x1A, inside grid's third
       row, which MenuRecord does not name. */
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
    entry = (DuelEffectChannel *)((u8 *)gText_aChannels +
        ((u8 *)record)[0x1A] * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
    entry = &gText_aChannels[((u8 *)record)[0x1A]];
#endif
    object = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
    if (object) {
        *(u16 *)&object->field_60 -= 0x40;
        if (object->field_60 <= 0) {
            DisplayObject_ReleaseIfPresent(object);
            record->grid[0][0] = 0;
        } else {
            Widget_SlideSine(
                DISPLAY_OBJECT_POSITION_VIEW(object),
                DIALOG_BOX_X,
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
    object = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    if (object) {
        *(u16 *)&object->field_48.h.field_4A += 8;
        *(u16 *)&object->field_48.h.field_48 += 8;
        if (object->field_48.h.field_48 >= 0xC0) {
            DisplayObject_ReleaseIfPresent(object);
            record->grid[0][1] = 0;
        }
    }
    if (!record->grid[0][0] && !record->grid[0][1])
        gDialog_bCallbackState = 0;
}

void Dialog_UpdateOpenTransition(MenuRecord *record)
{
    DisplayObject *p;
    DuelEffectChannel *r;
    s32 f;
    s32 v;
    s32 t;
    s32 a;
    s32 g;

    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState =
            gDialog_bCallbackState | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        p->field_30.h.field_30 = 0xA0;
        p->field_30.h.field_32 = DIALOG_TRANSITION_SPOTLIGHT_Y;
        p->field_48.h.field_48 = 0x80;
        p->field_48.h.field_4A = 0xE0;
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset(p, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 3));
        p->field_4C = (s32)DisplayObject_RenderSpotlightMask;
        record->grid[0][1] = (s32)p;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(p, DIALOG_BOX_X, -0x40, 3, 2, 0, 0xB, 0x20C);
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                   DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset(p, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 2));
        record->grid[0][0] = (s32)p;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(p));
        a = gMemCardWork_OperationMode;
        p->field_60 = -0x400;
        r = DuelEffect_CreateChannel((a & 1) | 0xD0, 0);
        r->flags_34 = r->flags_34 | DIALOG_TRANSITION_CHANNEL_FLAG;
        do {
            TextBox_UpdateChannels();
        } while (r->field_30 == 0);
        TextBox_SetPos(r, *(s16 *)&p->field_30.h.field_30,
                       *(s16 *)&p->field_30.h.field_32);
    }
    f = gDialog_bCallbackState;
    if ((f & 0x20) != 0) {
        gDialog_bCallbackState = 0;
        return;
    }
    /* The dialog channel's index: the byte at 0x1A, inside grid's third
       row, which MenuRecord does not name. */
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
    r = (DuelEffectChannel *)((u8 *)gText_aChannels +
        ((u8 *)record)[0x1A] * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
    r = &gText_aChannels[((u8 *)record)[0x1A]];
#endif
    if ((f & 0x40) != 0) {
        TextBox_UpdateChannels();
        if ((r->flags_34 & TEXT_BOX_FLAG_DONE) != 0) {
            gDialog_bCallbackState = gDialog_bCallbackState | 0x20;
            if (gDialog_bChoice == 0) {
                DuelEffect_CreateChannel(0x80D4, 0);
                gDialog_bChoice = 0;
            }
        }
        return;
    }
    p = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
    t = *(u16 *)&p->field_60;
    if (p->field_60 < 0) {
        v = t + 0x40;
        p->field_60 = v;
        if ((s16)v >= 0) {
            p->field_30.h.field_30 = DIALOG_BOX_X;
            p->field_60 = 0;
            p->field_30.h.field_32 = 0x50;
        } else {
            Widget_SlideSine(
                DISPLAY_OBJECT_POSITION_VIEW(p), DIALOG_BOX_X, 0x50, (s16)v);
        }
        TextBox_SetPos(r, *(s16 *)&p->field_30.h.field_30,
                       *(s16 *)&p->field_30.h.field_32);
    }
    p = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    p->field_48.h.field_4A = *(u16 *)&p->field_48.h.field_4A - 8;
    p->field_48.h.field_48 = *(u16 *)&p->field_48.h.field_48 - 8;
    if (p->field_48.h.field_48 <= 0) {
        p->field_48.h.field_48 = 0;
        p->field_48.h.field_4A = 0x40;
        if (DISPLAY_OBJECT_VIEW(record->grid[0][0])->field_60 == 0) {
            g = gDialog_bCallbackState;
            r->flags_34 = r->flags_34 & (0xFFFF ^ DIALOG_TRANSITION_CHANNEL_FLAG);
            gDialog_bCallbackState = g | 0x40;
        }
    }
}

/* Regional value: the flags_34 bit set and cleared on the channel (0x4 ->
 * 0x8). The European build (src/game/european/) defines its own. */
#ifndef FUNC_8003DA40_CHANNEL_FLAG
#define FUNC_8003DA40_CHANNEL_FLAG 4
#endif

void Dialog_UpdateMessageTransition(MenuRecord *record)
{
    DisplayObject *e;
    DuelEffectChannel *q;
    s32 f;
    s32 g;
    s32 v;
    s32 h;

    f = gDialog_bCallbackState;
    if ((f & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState = f | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(e, DIALOG_BOX_X, -0x40, 3, 2, 0, 0xB, 0x20C);
        e->flags = e->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                   DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 2));
        record->grid[0][0] = (s32)e;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)e);
        e->field_60 = -0x400;
        q = DuelEffect_CreateChannel(0xD0, 0);
        q->flags_34 = q->flags_34 | FUNC_8003DA40_CHANNEL_FLAG;
        do {
            TextBox_UpdateChannels();
        } while (q->field_30 == 0);
        TextBox_SetPos(q,
                       *(s16 *)&e->field_30.h.field_30,
                       *(s16 *)&e->field_30.h.field_32);
    }

    e = (DisplayObject *)record->grid[0][0];
    /* The dialog channel's index. It is the byte at 0x1A, inside grid's
       third row, and dialog_transition.c reads it the same way; nothing
       matched yet writes it, so MenuRecord has no name for it. */
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
    q = (DuelEffectChannel *)((u8 *)gText_aChannels +
        ((u8 *)record)[0x1A] * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
    q = &gText_aChannels[((u8 *)record)[0x1A]];
#endif
    g = gDialog_bCallbackState;

    if ((g & 0x40) != 0) {
        TextBox_UpdateChannels();
        if ((q->flags_34 & 0x2000) != 0) {
            gDialog_bCallbackState = 0;
        }
    } else {
        v = *(u16 *)&e->field_60 + 0x20;
        e->field_60 = v;
        if ((s16)v >= 0) {
            *(s16 *)&e->field_30.h.field_30 = DIALOG_BOX_X;
            *(s16 *)&e->field_30.h.field_32 = 0x50;
            h = q->flags_34;
            gDialog_bCallbackState = g | 0x40;
            q->flags_34 = h & (0xFFFF ^ FUNC_8003DA40_CHANNEL_FLAG);
        } else {
            Widget_SlideSine((DisplayObjectPosition *)e, DIALOG_BOX_X, 0x50, (s16)v);
        }
        TextBox_SetPos(q,
                       *(s16 *)&e->field_30.h.field_30,
                       *(s16 *)&e->field_30.h.field_32);
    }
}
