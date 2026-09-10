#include "../types.h"
#include "func_80036D3C.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_api.h"
#include "duel_effect.h"
#include "duel_effect_object_commands.h"

void func_800389C4(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~8;
}

void func_800389D8(DuelEffectChannel *object)
{
    s32 offset = object->stream_58 * 4;
    s32 value;

    *(s32 *)((u8 *)object + offset) += D_8009B34E * 2;
    value = func_80036D3C(object);
    offset = object->stream_58 * 4;
    *(s32 *)((u8 *)object + offset) =
        (*(s32 *)((u8 *)object + offset) & 0xFFFF0000) |
        (value & 0xFFFF);
}

void func_80038A44(DuelEffectChannel *object)
{
    s32 offset = object->stream_58 * 4;
    s32 value;

    *(s32 *)((u8 *)object + offset) += D_8009B355 * 2;
    value = func_80036D3C(object);
    offset = object->stream_58 * 4;
    *(s32 *)((u8 *)object + offset) =
        (*(s32 *)((u8 *)object + offset) & 0xFFFF0000) |
        (value & 0xFFFF);
}

void func_80038AB0(DuelEffectChannel *object)
{
    register u32 value asm("$3");
    register s32 duelist_id asm("$16");
    u8 **stream;
    u8 *cursor;

    stream = &((u8 **)object)[object->stream_58];
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

void func_80038B08(DuelEffectChannel *object)
{
    func_8004036C(object->field_30);
    object->field_30 = 0;
    object->state_51 = 2;
    object->field_62 = 0;
    D_8009B350 = 1;
}
