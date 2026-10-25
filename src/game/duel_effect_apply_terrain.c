/* The unit compiles at -G8 like its neighbours, and retail reaches the
 * terrain byte absolutely, so it takes duel_terrain_boost.h's .data scalar
 * rather than the small-data declaration. */
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "../types.h"
#include "duel_effect_allocate_request.h"
#include "duel_init_scene.h"
#include "duel_terrain_boost.h"
#include "duel_side_state.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "sound.h"
#include "display_object_config.h"
#include "../unmatched.h"
#include "duel_effect_apply_terrain.h"

/* The first sector of a terrain's effect data. A regional build whose WA.MRG
   lays the terrain packages out differently supplies its own. */
#ifndef DUEL_TERRAIN_EFFECT_SECTOR
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((terrain) * DUEL_TERRAIN_PACKAGE_SECTOR_COUNT + \
     DUEL_TERRAIN_EFFECT_DATA_FIRST_SECTOR)
#endif

/* The last two differences of the original match were the `n = v & 0xFF`
 * that gcc sank into the jal's delay slot where retail keeps the andi before
 * the call and puts `n - 1` in the slot. The mask is not a mask: it is the
 * read-back of the byte global just stored, and the decrement belongs to the
 * same expression -- `n = gDuel_bTerrain - 1;` before the call. */
void DuelEffect_ApplyTerrain(void) {
    DuelSideState *r;
    DuelCardRecord *q;
    DuelEffectRequest *e;
    s32 i;
    s32 n;
    s32 f;
    s32 v;
    s32 b;
    DisplayObjectConfig *a;

    if (DuelEffect_MarkInitialized() == 0) {
        r = gDuel_pActiveSideState;
        r->rank.field_0A = r->rank.field_0A + 1;
        v = (u8)gDuel_wEffectCardID - 0x49;
        gDuel_bTerrain = v;
        n = gDuel_bTerrain - 1;
        e = DuelEffect_AllocateRequest(0xA);
        gDuelEffect_pTrackedRequest = e;
        e->phase = n;
        SD_SEPlayFull(0x13);
        return;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x40) == 0) {
        if (gDuelEffect_pTrackedRequest->progress != 0) {
            gDuel_wCardEffectFlags = f | 0x40;
            File_RequestAsyncTransfer(
                0, (u8 *)0,
                DUEL_TERRAIN_EFFECT_SECTOR(gDuel_bTerrain),
                DUEL_TERRAIN_EFFECT_DATA_SECTOR_COUNT,
                (FileTransferCallback)0, 0, 0x1000280);
        }
        return;
    }

    if ((f & 0x20) == 0) {
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) == 0) {
            a = (DisplayObjectConfig *)gDuel_pTerrainIcon;
            b = gDuel_bTerrain;
            gDuelEffect_pTrackedRequest->phase = -2;
            DisplayObject_SetResourceVariant(a, b);
            gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x20;
        }
        return;
    }

    if ((gDuelEffect_pTrackedRequest->flags &
         DUEL_EFFECT_REQUEST_FLAG_ACTIVE) != 0) {
        return;
    }

    q = gDuel_aCardRecords;
    i = 0;
    do {
        if ((q->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            q->terrain_modifier =
                Duel_GetTerrainBoost(((DisplayObject *)q->object)->field_68);
        }
        i++;
        q++;
    } while (i < DUEL_CARD_RECORD_COUNT);

    gDuel_wCardEffectFlags = 0;
}
