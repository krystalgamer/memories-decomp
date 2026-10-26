#include "../types.h"
#include "display_object_lifecycle.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "sound.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object.h"
#include "duel_card_stat_display.h"
#include "display_object_config.h"
#include "duel_card.h"
#include "card_constants.h"

/* The update callback BuildDeck_CreateCardTransferIcon (below)
   installs: a card's stat panel
   scales up over eight frames, then flies to its target at 0x18/0x1A, waits,
   and scales back down before releasing itself. field_6C carries the phase --
   0x40 while it grows, 0x20 while it shrinks, neither while it travels.

   Two views of one object, on purpose. DisplayObject names the scale word at
   0x44, the countdown at 0x60, the attribute and the phase byte;
   DisplayObjectVelocity names 0x30/0x32 and 0x36/0x38 as position and
   velocity, which is what this function computes there -- it seeds both
   halves with (target - position) << 8 / 12 and then hands the record to
   DisplayObject_StepPositionXY, whose parameter is that view. Taking the
   velocity names from the view that owns them is what
   display_object_helpers.h asks a caller to do rather than pushing one
   reading of 0x36/0x38 onto the shared record, where it has five. */
void BuildDeck_UpdateCardTransferIcon(DisplayObject *o) {
    DisplayObjectVelocity *motion = (DisplayObjectVelocity *)o;
    s32 v;
    s32 t;

    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o) == 0) {
        o->field_44.word = 0;
        o->field_60 = 0;
        o->attribute = o->attribute & ~GsROTOFF;
        o->field_6C = o->field_6C | 0x40;
        SD_SEPlayFull(0xA);
    }

    v = o->field_6C;

    if ((v & 0x40) != 0) {
        t = *(u16 *)&o->field_60 + 1;
        o->field_60 = t;
        o->field_44.h.field_46 = (s16)t * 512;
        o->field_44.h.field_44 = (s16)t * 512;
        if (o->field_60 >= 8) {
            DisplayObject_ResetVelocity(motion);
            motion->velocity_x = ((*(s16 *)&o->field_18 - motion->x) << 8) / 12;
            o->field_6C = o->field_6C & 0xBF;
            o->field_60 = 0xC;
            motion->velocity_y = ((*(s16 *)&o->field_1A - motion->y) << 8) / 12;
        }
    } else if ((v & 0x20) != 0) {
        t = *(u16 *)&o->field_60 - 1;
        o->field_60 = t;
        o->field_44.h.field_46 = (s16)t * 512;
        o->field_44.h.field_44 = (s16)t * 512;
        if (o->field_60 == 0) {
            DisplayObject_ReleaseIfPresent(o);
        }
    } else {
        DisplayObject_StepPositionXY(motion);
        t = *(u16 *)&o->field_60 - 1;
        o->field_60 = t;
        if (o->field_60 <= 0) {
            o->field_30.word = *(s32 *)&o->field_18;
            o->field_60 = 8;
            o->field_6C = o->field_6C | 0x20;
        }
    }
}

/* Regional values: the icon sprite's texture_v (0xC8 -> 0xC0) and clut_x,
 * its CLUT x (0x260 -> 0x2E0). The European build (src/game/european/)
 * defines its own. */
#ifndef DUEL_CARD_TYPE_ICON_FIELD_5D
#define DUEL_CARD_TYPE_ICON_FIELD_5D 0xC8
#endif
#ifndef DUEL_CARD_TYPE_ICON_CX
#define DUEL_CARD_TYPE_ICON_CX 0x260
#endif

#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DISPLAY_OBJECT_UNSIGNED_HALFWORD(field) (*(u16 *)&(field))

/*
 * The object is the return value, not just a discarded temporary: retail
 * moves it to v0 before the final field stores and retains it through exit.
 * Restoring that return removes the artificial pointer-register binding.
 *
 * A scoped table-base capture keeps the full address ahead of the index
 * arithmetic under gcc_2_8_1_g0_split. The final independent flag updates
 * are written in this order to reproduce the target's scheduling; reversing
 * them changes six words. All 380 bytes match without instruction assembly.
 */
DisplayObject *BuildDeck_CreateCardTransferIcon(s32 index, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    s32 table_index = index;
    s32 x = arg1;
    s32 y = arg2;
    DisplayObjectConfigView *object;
    s32 field_18 = arg3;
    s32 bits;
    s32 kind;
    s32 *stats;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
    DisplayObject_ConfigureScreenSprite(
        DISPLAY_OBJECT_VIEW(object), x, y, 0x10, 0x10,
        0, DUEL_CARD_TYPE_ICON_FIELD_5D, 0xB, DUEL_CARD_TYPE_ICON_CX, 0xFC
    );
    do { stats = gDuel_adwCardStats; } while (0);
    table_index--;
    table_index <<= 2;
    table_index += (s32)stats;
    bits = *(s32 *)table_index;
    kind = (bits >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    switch (kind) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        DISPLAY_OBJECT_UNSIGNED_HALFWORD(object->clut_x) += 0x10;
        break;
    case CARD_TYPE_TRAP:
        DISPLAY_OBJECT_UNSIGNED_HALFWORD(object->clut_x) += 0x20;
        break;
    case CARD_TYPE_RITUAL:
        DISPLAY_OBJECT_UNSIGNED_HALFWORD(object->clut_x) += 0x30;
        break;
    }
    DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(object));
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 10);
    {
        DisplayObject *output = DISPLAY_OBJECT_VIEW(object);
        output->field_44.word = 0;
        output->field_18 = field_18;
        output->field_1A = arg4;
        output->update = (DisplayObjectCallback)BuildDeck_UpdateCardTransferIcon;
        output->flags &= 0xFFF7;
        output->attribute &= ~GsROTOFF;
        return output;
    }
}
