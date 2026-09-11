#include "../types.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "duel_effect_object_commands.h"

void func_80038AB0(DuelEffectChannel *object)
{
    s16 duelist_id;
    u32 value;
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
