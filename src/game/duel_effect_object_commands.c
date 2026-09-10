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
