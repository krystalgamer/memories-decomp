#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "duel_effect_resource_record.h"
#include "duel_card.h"
#include "func_800291E0.h"
#include "card_preview_callbacks.h"

u8 *func_800291E0(s32 index, s32 x, s32 y)
{
    DuelEffectResourceRecord *entry;
    DisplayObject *object;
    s32 variant;
    s32 setup;
    s32 card_id;
    u8 byte_value;

    setup = x;
    entry = &D_800EA0E8[index];

    object = func_800400AC(func_8004002C(), 6);
    func_80040510((DisplayObjectConfigView *)object, 0, 0, 0x66, 0x60, 0, 0,
        ((s16)entry->src_x >> 6) + (((s16)entry->src_y >> 8) << 4),
        (s16)entry->field_2C, (s16)entry->field_2E);

    object->field_30.word = 0x001F0048;
    object->field_3C.word = 0x00100038;
    object->field_5E = 0x9E00;
    object->attribute |= 0x01000000;

    card_id = (s16)entry->field_30;
    variant = (gDuel_adwCardStats[card_id - 1] >> 26) & 0x1F;
    object->field_67 = index;
    object->field_68 = (u8)variant;

    if (variant == 0x15) {
        goto disp_15;
    }
    if (variant >= 0x16) {
        goto disp_ge16;
    }
    if (variant == 0x14) {
        goto disp_14;
    }
    object->field_30.h.field_30 = 0x48;
    goto path_a;

disp_ge16:
    if (variant == 0x16) {
        goto disp_16;
    }
    if (variant == 0x17) {
        goto disp_17;
    }
    object->field_30.h.field_30 = 0x48;
    goto path_a;

disp_16:
    setup = 0x103;
    variant = 1;
    goto shared_tail;

disp_17:
    byte_value = ((u8 *)&object->field_5E)[1];
    byte_value = (u8)(byte_value + 0x20);
    ((u8 *)&object->field_5E)[1] = byte_value;
    /* fallthrough */
disp_14:
    setup = 0x101;
    variant = 1;
    goto shared_tail;

disp_15:
    setup = 0x102;
    byte_value = ((u8 *)&object->field_5E)[1];
    variant = 1;
    byte_value = (u8)(byte_value + 0x10);
    ((u8 *)&object->field_5E)[1] = byte_value;
    goto shared_tail;

path_a:
    object->field_30.h.field_32 = 0x9E;
    ((u8 *)&object->field_5E)[1] = 0xCE;
    object->field_3C.h.field_3C = 0x18;
    object->field_3C.h.field_3E = 0xC;

    {
        s16 x_value = setup;
        entry->field_32 = x_value;
        if (x_value < 0) {
            entry->field_32 =
                (s16)((gDuel_adwCardStats[card_id - 1] & 0x1FF) * 10);
        }
    }
    {
        s16 y_value = y;
        entry->field_34 = y_value;
        if (y_value < 0) {
            entry->field_34 =
                (s16)(((gDuel_adwCardStats[card_id - 1] >> 9) & 0x1FF) * 10);
        }
    }
    setup = 0x100;
    variant = 0;
    entry->field_38 = 0;
    entry->field_36 = 0;

shared_tail:
    entry->field_3B = (u8)(gDuel_abCardLevelAttr[card_id] >> 4);
    entry->field_3A = (u8)(gDuel_abCardLevelAttr[card_id] & 0xF);
    entry->field_3C = 0;

    object->field_4C = (s32)func_80028B08;
    func_80042918(object);

    entry->object_04 = object;

    object = func_800400AC(func_8004002C(), 2);
    func_800404CC(object, 2, 4, 1, 0, variant, 0x1C, setup + 8);

    object->field_18 = 0x46;
    object->field_48.h.field_48 = 0x46;
    object->field_1A = 0x62;
    object->field_48.h.field_4A = 0x62;
    object->field_6A = (u8)variant;
    object->attribute |= 0x01000000;
    object->flags |= 8;

    func_80042918(object);
    func_800428EC((u8 *)object, -1);

    ((DisplayObject *)entry->object_04)->field_65 = 3;
    object->field_65 = 3;
    object->field_10 = (u32)func_80029108;

    {
        DisplayObject *secondary = entry->object_04;
        DisplayObject *primary;

        primary = object;

        entry->object_00 = primary;
        secondary->field_54 = primary;
    }
    return (u8 *)object;
}
