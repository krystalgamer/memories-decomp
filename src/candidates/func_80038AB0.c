/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_effect_object_commands.c.
 */
#include "../types.h"
#include "../game/func_80036D3C.h"
#include "../game/card_constants.h"
#include "../game/campaign_flags.h"
#include "../game/display_object_api.h"
#include "../game/duel_effect.h"
#include "../game/duel_effect_object_commands.h"

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

