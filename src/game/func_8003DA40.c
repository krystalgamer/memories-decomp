#define D_8009AF74_IN_DATA
#include "../types.h"
#include "display_object.h"
#include "display_object_core.h"
#include "duel_effect.h"
#include "dialog_callback_state.h"
#include "text_channel_state.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "text_box_runtime.h"
#include "func_80039794.h"
#include "display_object_interpolation.h"
#include "duel_effect_create_channel.h"
#include "func_8003DA40.h"
#include "dialog_layout.h"

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
