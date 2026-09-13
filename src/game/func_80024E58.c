/* The unit compiles at -G8 like its neighbours, and retail reaches the
 * terrain byte absolutely, so it takes duel_terrain_boost.h's .data scalar
 * rather than the small-data declaration. */
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "../types.h"
#include "duel_effect_allocate_request.h"
#include "func_800179F4.h"
#include "duel_terrain_boost.h"
#include "duel_side_state.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "sound.h"
#include "display_object_config.h"
#include "../unmatched.h"
#include "func_80024E58.h"

/* The last two differences of the original match were the `n = v & 0xFF`
 * that gcc sank into the jal's delay slot where retail keeps the andi before
 * the call and puts `n - 1` in the slot. The mask is not a mask: it is the
 * read-back of the byte global just stored, and the decrement belongs to the
 * same expression -- `n = gDuel_bTerrain - 1;` before the call. */
void DuelEffect_ApplyTerrain(void) {
    u8 *p;
    u8 *r;
    u8 *q;
    u8 *e;
    s32 i;
    s32 n;
    s32 f;
    s32 v;
    s32 b;
    DisplayObjectConfig *a;

    if (DuelEffect_MarkInitialized() == 0) {
        r = (u8 *)D_8009B1C8;
        r[0xA] = r[0xA] + 1;
        v = *(u8 *)&gDuel_wEffectCardID - 0x49;
        gDuel_bTerrain = v;
        n = gDuel_bTerrain - 1;
        e = DuelEffect_AllocateRequest(0xA);
        D_8009B17C = e;
        *(s16 *)(e + 0x1A) = n;
        SD_SEPlayFull(0x13);
        return;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x40) == 0) {
        if (D_8009B17C[0x1D] != 0) {
            gDuel_wCardEffectFlags = f | 0x40;
            File_RequestAsyncTransfer(
                0, (u8 *)0,
                gDuel_bTerrain * DUEL_TERRAIN_PACKAGE_SECTOR_COUNT +
                    DUEL_TERRAIN_EFFECT_DATA_FIRST_SECTOR,
                DUEL_TERRAIN_EFFECT_DATA_SECTOR_COUNT,
                (FileTransferCallback)0, 0, 0x1000280);
        }
        return;
    }

    if ((f & 0x20) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) == 0) {
            a = (DisplayObjectConfig *)D_8009B214;
            b = gDuel_bTerrain;
            *(s16 *)(D_8009B17C + 0x1A) = -2;
            func_80040410(a, b);
            gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x20;
        }
        return;
    }

    if ((D_8009B17C[0x1C] & DUEL_EFFECT_REQUEST_FLAG_ACTIVE) != 0) {
        return;
    }

    q = (u8 *)D_801A7AD8;
    i = 0;
    p = q + 0x14;
    do {
        if ((*(u16 *)(p + 2) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            *(s16 *)(p + 0) = Duel_GetTerrainBoost((*(u8 **)q)[0x68]);
        }
        i++;
        p += DUEL_CARD_RECORD_SIZE;
        q += DUEL_CARD_RECORD_SIZE;
    } while (i < DUEL_CARD_RECORD_COUNT);

    gDuel_wCardEffectFlags = 0;
}
