#include "../types.h"
#define TEXT_START_CAMPAIGN_DUEL_DECLARE_STATE
#include "text_start_campaign_duel.h"
#include "duel_effect.h"
#include "func_80036D3C.h"
#include "duel_side_state.h"

void Text_StartCampaignDuel(DuelEffectChannel *object)
{
    unsigned value;
    int offset;

    D_8009B360 = -1;

    offset = object->stream_58;
    offset *= 4;
    {
        u8 **stream = (u8 **)((u8 *)object + offset);
        u8 *current = *stream;
        value = *current++;
        *stream = current;
    }
    gDuel_bOpponentID = value;

    offset = object->stream_58;
    offset *= 4;
    {
        u8 **stream = (u8 **)((u8 *)object + offset);
        u8 *current = *stream;
        value = *current++;
        *stream = current;
    }
    D_8009B370 = value;

    offset = object->stream_58;
    offset *= 4;
    {
        u8 **stream = (u8 **)((u8 *)object + offset);
        u8 *current = *stream;
        value = *current++;
        *stream = current;
    }
    D_8009B372 = value;

    offset = object->stream_58;
    offset *= 4;
    {
        u8 **stream = (u8 **)((u8 *)object + offset);
        u8 *current = *stream;
        value = *current++;
        *stream = current;
    }
    gDuel_bTerrain = value;

    D_8009B36A = func_80036D3C(object);
    D_8009B374 = 0x72A0;
    offset = gDuel_bOpponentID;
    if ((unsigned)(offset - 9) < 8) {
        D_8009B374 = 0x7280;
    }
    if ((s8)offset == 0x11) {
        D_8009B374 = 0x7290;
    }
    if ((s8)offset == 0x26) {
        D_8009B374 = 0x72B0;
    }
    D_8009B368 = 2;
    D_8009B369 = 0;
    D_8009B26C = 3;
}
