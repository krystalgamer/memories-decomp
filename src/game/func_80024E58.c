#include "../types.h"
#include "func_8002C604.h"
#include "func_800179F4.h"
#define GDUEL_TERRAIN_IN_DATA_ARRAY8
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
#include "duel_effect_tables.h"
#include "../unmatched.h"

/* One byte at 0x8009B364; the 8 is a code-generation threshold, not a length.
 * Historically, an incomplete array view increased this function's text by
 * 4 bytes. See duel_terrain_boost.h for the uniform -G8 declaration contract. */
/* The last two residual differences were the
 * `n = v & 0xFF` that gcc sank into the jal's delay slot where retail keeps
 * the andi before the call and puts `n - 1` in the slot. The mask is not a
 * mask: it is the READ-BACK of the byte global just stored, and the
 * decrement belongs to the same expression -- `n = gDuel_bTerrain[0] - 1;`
 * before the call. Written as the read-back and the decrement in two
 * statements it is 4, as `v & 0xFF` with the decrement before the call 3.
 * The terrain declaration and D_8009B0F4_abs / D_8009B134_abs keep those
 * globals out of small data.
 */

void func_80024E58(void) {
    u8 *p;
    u8 *r;
    u8 *q;
    u8 *e;
    s32 i;
    s32 n;
    s32 f;
    s32 v;
    s32 b;
    u8 *a;

    if (DuelEffect_MarkInitialized() == 0) {
        r = (u8 *)D_8009B1C8;
        r[0xA] = r[0xA] + 1;
        v = *(u8 *)&D_8009B1D2 - 0x49;
        gDuel_bTerrain[0] = v;
        n = gDuel_bTerrain[0] - 1;
        e = func_8002C604(0xA);
        D_8009B17C = e;
        *(s16 *)(e + 0x1A) = n;
        SD_SEPlayFull(0x13);
        return;
    }

    f = D_8009B220;

    if ((f & 0x40) == 0) {
        if (D_8009B17C[0x1D] != 0) {
            D_8009B220 = f | 0x40;
            File_RequestAsyncTransfer(
                0, (u8 *)0,
                gDuel_bTerrain[0] * DUEL_TERRAIN_PACKAGE_SECTOR_COUNT +
                    DUEL_TERRAIN_EFFECT_DATA_FIRST_SECTOR,
                DUEL_TERRAIN_EFFECT_DATA_SECTOR_COUNT,
                (FileTransferCallback)0, 0, 0x1000280);
        }
        return;
    }

    if ((f & 0x20) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) == 0) {
            a = (u8 *)D_8009B214;
            b = gDuel_bTerrain[0];
            *(s16 *)(D_8009B17C + 0x1A) = -2;
            func_80040410(a, b);
            D_8009B220 = D_8009B220 | 0x20;
        }
        return;
    }

    if ((D_8009B17C[0x1C] & 0x80) != 0) {
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

    D_8009B220 = 0;
}
