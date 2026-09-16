#include "../types.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "duel_card.h"
#include "card_constants.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

#include "duel_card_stat_display.h"

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
DisplayObject *func_80031574(s32 index, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
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
    DisplayObject_ConfigureScreenSprite(object, x, y, 0x10, 0x10, 0, 0xC8, 0xB, 0x260, 0xFC);
    do { stats = gDuel_adwCardStats; } while (0);
    table_index--;
    table_index <<= 2;
    table_index += (s32)stats;
    bits = *(s32 *)table_index;
    kind = (bits >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    switch (kind) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        *(u16 *)&object->field_40 += 0x10;
        break;
    case CARD_TYPE_TRAP:
        *(u16 *)&object->field_40 += 0x20;
        break;
    case CARD_TYPE_RITUAL:
        *(u16 *)&object->field_40 += 0x30;
        break;
    }
    func_80042918((DisplayObject *)object);
    func_800428EC((u8 *)object, 10);
    {
        DisplayObject *output = (DisplayObject *)object;
        output->field_44.word = 0;
        output->field_18 = field_18;
        output->field_1A = arg4;
        output->update = (DisplayObjectCallback)func_800313E8;
        output->flags &= 0xFFF7;
        output->attribute &= ~GsROTOFF;
        return output;
    }
}
