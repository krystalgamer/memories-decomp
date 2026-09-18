#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "text_start_campaign_duel.h"
#include "duel_effect.h"
#include "text_stream_read_u16_le.h"
#include "duel_side_state.h"
#include "../unmatched.h"

#include "ai_opponent_data.h"
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "duel_terrain_boost.h"
#include "main_mode_state.h"

#define TEXT_STREAM_OWNER_VIEW(object) ((TextStreamOwner *)(object))

/*
 * Each byte read owns its slot pointer, advancing cursor and unsigned value.
 * Those separate lifetimes recover all 352 bytes under uniform G0 without
 * register bindings. Sharing the three locals instead changes the index
 * register in all four reads (twelve words).
 *
 * The opponent is reloaded unsigned after the opaque word reader, then
 * explicitly narrowed for the two signed-byte equality tests.
 */
void Text_StartCampaignDuel(DuelEffectChannel *o)
{
    s32 x;
    D_8009B360 = -1;
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_58];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_bOpponentID = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_58];
        q = *p;
        v = *q++;
        *p = q;
        D_8009B370 = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_58];
        q = *p;
        v = *q++;
        *p = q;
        D_8009B372 = v;
    }
    {
        u8 **p;
        u8 *q;
        u32 v;
        p = &TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_58];
        q = *p;
        v = *q++;
        *p = q;
        gDuel_bTerrain = v;
    }
    gDuel_wBgmId = TextStream_ReadU16LE(o);
    D_8009B374 = 0x72A0;
    x = (u8)gDuel_bOpponentID;
    if ((u32)(x - 9) < 8) D_8009B374 = 0x7280;
    if ((s8)x == 0x11) D_8009B374 = 0x7290;
    if ((s8)x == 0x26) D_8009B374 = 0x72B0;
    D_8009B368 = 2;
    D_8009B369 = 0;
    D_8009B26C = 3;
}
