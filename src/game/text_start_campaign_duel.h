#ifndef MEMORIES_DECOMP_TEXT_START_CAMPAIGN_DUEL_H
#define MEMORIES_DECOMP_TEXT_START_CAMPAIGN_DUEL_H

#include "../types.h"

/* D_80090EAC entry: arms a campaign duel from the object's script stream. It
 * takes four bytes in order -- opponent id, D_8009B370, D_8009B372, terrain --
 * then a word through func_80036D3C for the BGM, picks the duel's music bank
 * from the opponent id (0x7280 for ids 9 through 16, 0x7290 for 0x11, 0x72B0 for
 * 0x26, 0x72A0 otherwise), and hands the scene to mode 3.
 *
 * It also resets D_8009B360 to -1 before any of that, which is what makes the
 * duel's own state read as unset. Register-pinned throughout. */
void Text_StartCampaignDuel(u8 *object);

#endif
