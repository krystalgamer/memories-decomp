#include "../types.h"
#include "text_stream_read_u16_le.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "duel_effect.h"
#include "duel_effect_object_commands.h"
#ifdef VERSION_EUROPE
#include "save_data.h"
#endif

/* Regional value: the channel flag func_800389C4 clears (8 -> 0x10). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_CLEARED_CHANNEL_FLAG
#define DUEL_EFFECT_CLEARED_CHANNEL_FLAG 8
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DUEL_EFFECT_CLEAR_FLAG_8)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800389C4))
void func_800389C4(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~DUEL_EFFECT_CLEARED_CHANNEL_FLAG;
}
#endif

#ifndef DUEL_EFFECT_STREAM_HIGH_MASK
#define DUEL_EFFECT_STREAM_HIGH_MASK 0xFFFF0000
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_800389D8)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800389D8))
void func_800389D8(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_58] += D_8009B34E * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_58] =
        (u8 *)(((u32)owner->streams[object->stream_58] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_80038A44)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80038A44))
void func_80038A44(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_58] += D_8009B355 * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_58] =
        (u8 *)(((u32)owner->streams[object->stream_58] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_TEXT_UNLOCK_DUELIST)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_TEXT_UNLOCK_DUELIST))
void Text_UnlockDuelist(DuelEffectChannel *object)
{
    s16 duelist_id;
    u32 value;
    u8 *G32 *stream;
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
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_TEXT_CLOSE_CHOICE)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_TEXT_CLOSE_CHOICE))
void Text_CloseChoice(DuelEffectChannel *object)
{
    DisplayObject_ReleaseIfPresent(object->field_30);
    object->field_30 = 0;
    object->state_51 = 2;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    object->field_62 = 0;
#endif
    D_8009B350 = 1;
}
#endif

#if defined(VERSION_EUROPE) && defined(VERSION_EUROPE_TEXT_PUSH_PLAYER_NAME_STREAM)
/* European only: pushes the player name's glyph codes, D_801B125A, as the
   next text stream. */
void Text_PushPlayerNameStream(DuelEffectChannel *object)
{
    ((TextStreamOwner *)object)->streams[object->stream_58 + 1] = D_801B125A;
    object->stream_58++;
}
#endif
