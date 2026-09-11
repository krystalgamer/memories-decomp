/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/text_start_campaign_duel.c.
 */
#include "../types.h"
#include "../game/text_start_campaign_duel.h"
#include "../game/duel_effect.h"
#include "../game/func_80036D3C.h"
#include "../game/duel_side_state.h"
#define D_8009B26C_AS_SCALAR
#include "../unmatched.h"

extern u8 gDuel_bOpponentID,gDuel_bTerrain;
void Text_StartCampaignDuel(DuelEffectChannel*o){register u8**p asm("$3");register u8*q asm("$2");register unsigned v asm("$5");int x;D_8009B360=-1;p=&((u8**)o)[o->stream_58];q=*p;v=*q++;*p=q;gDuel_bOpponentID=v;p=&((u8**)o)[o->stream_58];q=*p;v=*q++;*p=q;D_8009B370=v;p=&((u8**)o)[o->stream_58];q=*p;v=*q++;*p=q;D_8009B372=v;p=&((u8**)o)[o->stream_58];q=*p;v=*q++;*p=q;gDuel_bTerrain=v;D_8009B36A=func_80036D3C(o);D_8009B374=0x72a0;x=gDuel_bOpponentID;if((unsigned)(x-9)<8)D_8009B374=0x7280;if((s8)x==0x11)D_8009B374=0x7290;if((s8)x==0x26)D_8009B374=0x72b0;D_8009B368=2;D_8009B369=0;D_8009B26C=3;}
