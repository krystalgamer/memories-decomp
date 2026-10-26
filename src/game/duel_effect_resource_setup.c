#define DISPLAY_OBJECT_FIELD_5E_BYTES(object) ((u8 *)&(object)->field_5E)
#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_resource_setup.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "duel_card.h"
#include "card_preview_objects.h"
#include "card_preview_callbacks.h"

#ifndef DUEL_RESOURCE_TRANSFER_POSITION
#define DUEL_RESOURCE_TRANSFER_POSITION(value) \
    (((value) - 1) * 7 + CARD_COUNT)
#endif
#ifndef DUEL_RESOURCE_TRANSFER_SECTORS
#define DUEL_RESOURCE_TRANSFER_SECTORS 7
#endif
#ifndef DUEL_RESOURCE_DEFAULT_X
#define DUEL_RESOURCE_DEFAULT_X 0x48
#endif
#ifndef DUEL_RESOURCE_VARIANT_BYTE
#define DUEL_RESOURCE_VARIANT_BYTE 1
#endif
#ifndef DUEL_RESOURCE_VARIANT_17_DELTA
#define DUEL_RESOURCE_VARIANT_17_DELTA 0x20
#endif
#ifndef DUEL_RESOURCE_VARIANT_15_DELTA
#define DUEL_RESOURCE_VARIANT_15_DELTA 0x10
#endif
#ifndef DUEL_RESOURCE_PATH_Y
#define DUEL_RESOURCE_PATH_Y 0x9E
#endif
#ifndef DUEL_RESOURCE_PATH_FIELD_5E
#define DUEL_RESOURCE_PATH_FIELD_5E 0xCE
#endif
#ifndef DUEL_RESOURCE_PATH_FIELD_3C
#define DUEL_RESOURCE_PATH_FIELD_3C 0x18
#endif
#ifndef DUEL_RESOURCE_PATH_FIELD_3E
#define DUEL_RESOURCE_PATH_FIELD_3E 0xC
#endif
#ifndef DUEL_RESOURCE_SECONDARY_X
#define DUEL_RESOURCE_SECONDARY_X 4
#endif
#ifndef DUEL_RESOURCE_SECONDARY_Y
#define DUEL_RESOURCE_SECONDARY_Y 0x62
#endif

FileTransferDescriptor *CardPreview_RequestResourceTransfer(s32 slot, s32 card_id)
{
    FileTransferDescriptor *object;

    gCardPreview_Resources[slot].card_id = card_id;
    object = File_TryRequestAsyncTransfer(
        0, 0, DUEL_RESOURCE_TRANSFER_POSITION(card_id),
        DUEL_RESOURCE_TRANSFER_SECTORS, CardPreview_HandleResourceTransfer, 0, 0);
    object->callback_data = (void *)slot;
    gFile_dwTransferState_abs =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return object;
}

/*
 * Retail leaves the primary preview object in $v0 and callers consume that
 * pointer. The stat overrides feed the default path before setup is reused;
 * a negative signed low halfword selects the packed base-stat fallback.
 * The primary object is published at record offset zero
 * and linked from the secondary object at offset four.
 */
DisplayObject *CardPreview_CreateResourceObjects(s32 index, s32 attack, s32 defense)
{
    DuelEffectResourceRecord *entry;
    DisplayObject *object;
    s32 variant;
    s32 setup;
    s32 card_id;
    u8 byte_value;

    setup = attack;
    entry = &gCardPreview_Resources[index];

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_ConfigureScreenSprite(object, 0, 0, 0x66, 0x60, 0, 0,
        ((s16)entry->src_x >> 6) + (((s16)entry->src_y >> 8) << 4),
        (s16)entry->field_2C, (s16)entry->field_2E);

    object->field_30.word = 0x001F0048;
    object->field_3C.word = 0x00100038;
#ifdef VERSION_EUROPE
    object->attribute |= 0x01000000;
    object->field_5E = ((gText_bLanguageIndex << 4) + 0x9E) << 8;
#else
    object->field_5E = 0x9E00;
    object->attribute |= 0x01000000;
#endif

    card_id = (s16)entry->card_id;
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
    object->field_30.h.field_30 = DUEL_RESOURCE_DEFAULT_X;
    goto path_a;

disp_ge16:
    if (variant == 0x16) {
        goto disp_16;
    }
    if (variant == 0x17) {
        goto disp_17;
    }
    object->field_30.h.field_30 = DUEL_RESOURCE_DEFAULT_X;
    goto path_a;

disp_16:
    setup = 0x103;
    variant = 1;
    goto shared_tail;

disp_17:
    byte_value = DISPLAY_OBJECT_FIELD_5E_BYTES(object)[
        DUEL_RESOURCE_VARIANT_BYTE];
    byte_value = (u8)(byte_value + DUEL_RESOURCE_VARIANT_17_DELTA);
    DISPLAY_OBJECT_FIELD_5E_BYTES(object)[DUEL_RESOURCE_VARIANT_BYTE] =
        byte_value;
    /* fallthrough */
disp_14:
    setup = 0x101;
    variant = 1;
    goto shared_tail;

disp_15:
    setup = 0x102;
    byte_value = DISPLAY_OBJECT_FIELD_5E_BYTES(object)[
        DUEL_RESOURCE_VARIANT_BYTE];
    variant = 1;
    byte_value = (u8)(byte_value + DUEL_RESOURCE_VARIANT_15_DELTA);
    DISPLAY_OBJECT_FIELD_5E_BYTES(object)[DUEL_RESOURCE_VARIANT_BYTE] =
        byte_value;
    goto shared_tail;

path_a:
    object->field_30.h.field_32 = DUEL_RESOURCE_PATH_Y;
    DISPLAY_OBJECT_FIELD_5E_BYTES(object)[DUEL_RESOURCE_VARIANT_BYTE] =
        DUEL_RESOURCE_PATH_FIELD_5E;
    object->field_3C.h.field_3C = DUEL_RESOURCE_PATH_FIELD_3C;
    object->field_3C.h.field_3E = DUEL_RESOURCE_PATH_FIELD_3E;
#ifdef VERSION_EUROPE
    DISPLAY_OBJECT_FIELD_5E_BYTES(object)[1] =
        (gText_bLanguageIndex << 4) - 0x60;
#endif

    {
        s16 attack_value = setup;
        entry->attack = attack_value;
        if (attack_value < 0) {
            entry->attack =
                (s16)((gDuel_adwCardStats[card_id - 1] & 0x1FF) * 10);
        }
    }
    {
        s16 defense_value = defense;
        entry->defense = defense_value;
        if (defense_value < 0) {
            entry->defense =
                (s16)(((gDuel_adwCardStats[card_id - 1] >> 9) & 0x1FF) * 10);
        }
    }
    setup = 0x100;
    variant = 0;
    entry->defense_adjustment = 0;
    entry->attack_adjustment = 0;

shared_tail:
    entry->attribute_icon = (u8)(gDuel_abCardLevelAttr[card_id] >> 4);
    entry->level = (u8)(gDuel_abCardLevelAttr[card_id] & 0xF);
    entry->stat_palette_flags = 0;

    object->field_4C = (s32)CardPreview_DrawDetailPanel;
    DisplayObject_SelectOrderingTable1(object);

    entry->secondary_object = object;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 2, DUEL_RESOURCE_SECONDARY_X, 1, 0, variant, 0x1C,
        setup + 8);

    object->field_18 = 0x46;
    object->field_48.h.field_48 = 0x46;
    object->field_1A = DUEL_RESOURCE_SECONDARY_Y;
    object->field_48.h.field_4A = DUEL_RESOURCE_SECONDARY_Y;
    object->field_6A = (u8)variant;
    object->attribute |= 0x01000000;
    object->flags |= 8;

    DisplayObject_SelectOrderingTable1(object);
    DisplayObject_SetDepthOffset(object, -1);

    ((DisplayObject *)entry->secondary_object)->field_65 = 3;
    object->field_65 = 3;
    object->field_10 = (u32)CardPreview_UpdateVariant;

    {
        DisplayObject *secondary = entry->secondary_object;
        DisplayObject *primary;

        primary = object;

        entry->primary_object = primary;
        secondary->field_54 = primary;
    }
    return object;
}

void DuelEffect_ReleaseResourceObjects(s32 index)
{
    DuelEffectResourceRecord *entry = &gCardPreview_Resources[index];

    DisplayObject_ReleaseIfPresent(entry->primary_object);
    DisplayObject_ReleaseIfPresent(entry->secondary_object);
    entry->secondary_object = 0;
    entry->primary_object = 0;
}

void DuelEffect_ClearResourceObjectPointers(int index)
{
    DuelEffectResourceRecord *entry = &gCardPreview_Resources[index];

    entry->secondary_object = 0;
    entry->primary_object = 0;
}
