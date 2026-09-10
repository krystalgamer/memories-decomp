#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_control.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "dialog_choice_state.h"

void func_800374A8(DuelEffectChannel *object)
{
    u8 flags = object->state_51;

    if ((flags & 0x80) == 0) {
        object->state_51 = flags | 0x80;
        func_800373C8(object, 3, 0);
        object->state_51 = 0x82;
    }
}

u8 *Dialog_OpenChoice(DuelEffectChannel *record)
{
    u8 *cursor = func_800400AC((s32)func_8004006C(), 2);

    func_800404CC(
        cursor,
        record->field_3C + record->field_3E - 0x10,
        record->field_40 + record->field_42 - 0x10,
        3,
        0,
        0,
        11,
        0x20C
    );
    *(u16 *)(cursor + 8) |= 0x28;
    func_80042918((DisplayObject *)cursor);
    func_800428EC(cursor, (s8)(record->field_59 + 1));
    return cursor;
}
