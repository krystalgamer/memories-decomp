#include "../types.h"
#include "text_stream_read_u16_le.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "duel_effect.h"
#include "duel_effect_object_commands.h"

void func_800389C4(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~8;
}

void func_800389D8(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_58] += D_8009B34E * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_58] =
        (u8 *)(((u32)owner->streams[object->stream_58] & 0xFFFF0000) |
               (value & 0xFFFF));
}

void func_80038A44(DuelEffectChannel *object)
{
    s32 offset = object->stream_58 * 4;
    s32 value;

    *(s32 *)((u8 *)object + offset) += D_8009B355 * 2;
    value = TextStream_ReadU16LE(object);
    offset = object->stream_58 * 4;
    *(s32 *)((u8 *)object + offset) =
        (*(s32 *)((u8 *)object + offset) & 0xFFFF0000) |
        (value & 0xFFFF);
}

void Text_UnlockDuelist(DuelEffectChannel *object)
{
    s16 duelist_id;
    u32 value;
    u8 **stream;
    u8 *cursor;

    stream = &((TextStreamOwner *)object)->streams[object->stream_58];
    cursor = *stream;
    value = *cursor++;
    duelist_id = value;
    *stream = cursor;
    if (duelist_id > 0) {
        Library_UpdateCardUsedFlag(
            duelist_id + CAMPAIGN_FLAG_DUELIST_DEFEATED_BASE);
        Library_UpdateCardUsedFlag(duelist_id + FREE_DUEL_UNLOCK_FLAG_BASE);
    }
}

void Text_CloseChoice(DuelEffectChannel *object)
{
    DisplayObject_ReleaseIfPresent(object->field_30);
    object->field_30 = 0;
    object->state_51 = 2;
    object->field_62 = 0;
    D_8009B350 = 1;
}
