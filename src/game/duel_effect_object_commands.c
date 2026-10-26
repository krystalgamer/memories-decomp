#include "../types.h"
#include "text_stream_read_le.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_object_commands.h"
#ifdef VERSION_EUROPE
#include "save_data.h"
#include "player_name_glyphs.h"
#endif

/* Regional value: the channel flag Text_DisableCompletionWait clears (8 -> 0x10). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_CLEARED_CHANNEL_FLAG
#define DUEL_EFFECT_CLEARED_CHANNEL_FLAG 8
#endif

void Text_DisableCompletionWait(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~DUEL_EFFECT_CLEARED_CHANNEL_FLAG;
}

#ifndef DUEL_EFFECT_STREAM_HIGH_MASK
#define DUEL_EFFECT_STREAM_HIGH_MASK 0xFFFF0000
#endif

void Text_JumpBySelectorA(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_58] += gText_BranchSelectorA * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_58] =
        (u8 *)(((u32)owner->streams[object->stream_58] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}

void Text_JumpBySelectorB(DuelEffectChannel *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    s32 value;

    owner->streams[object->stream_58] += gText_BranchSelectorB * 2;
    value = TextStream_ReadU16LE(object);
    owner->streams[object->stream_58] =
        (u8 *)(((u32)owner->streams[object->stream_58] &
                DUEL_EFFECT_STREAM_HIGH_MASK) |
               (value & 0xFFFF));
}

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

void Text_CloseChoice(DuelEffectChannel *object)
{
    DisplayObject_ReleaseIfPresent(object->prompt_object);
    object->prompt_object = 0;
    object->playback_state = 2;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    object->field_62 = 0;
#endif
    gText_CommandDisposition = 1;
}

#ifdef VERSION_EUROPE
/* European only: pushes the player name's glyph codes, gText_aCurrentPlayerNameGlyphs, as the
   next text stream. */
void Text_PushPlayerNameStream(DuelEffectChannel *object)
{
    ((TextStreamOwner *)object)->streams[object->stream_58 + 1] = gText_aCurrentPlayerNameGlyphs;
    object->stream_58++;
}
#endif
