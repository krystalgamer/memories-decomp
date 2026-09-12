/* Reclassified from matching_c (#3859). This was src/game/func_80024E58.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g4_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G4). Under
 * gcc_2_8_1_g8, a single threshold, it is 113 instructions against the
 * target's 116, opcode distance 3. The source below is the match, unchanged
 * apart from its include paths. */
#include "../types.h"
#include "../game/func_8002C604.h"
#include "../game/func_800179F4.h"
#include "../game/duel_terrain_boost.h"
#include "../game/duel_side_state.h"
#include "../game/duel_effect_request.h"
#include "../game/duel_action_lock.h"
#include "../game/duel_card.h"
#include "../game/duel_card_layout.h"
#include "../game/duel_package.h"
#include "../game/file_transfer.h"
#include "../game/sound.h"
#include "../game/display_object_config.h"
#include "../unmatched.h"

/* One byte at 0x8009B364; the 8 is a threshold, not a length. This TU's
 * profile compiles at -G8 but assembles at -G4, so the array needs a size
 * the assembler can see to be above 4. Measured: an incomplete [] here
 * costs 4 bytes of text, though it is exact in func_8001798C.c, which
 * assembles at -G8. See duel_terrain_boost.h for all five spellings. */
extern u8 gDuel_bTerrain[8];

/* MATCH (2026-09-05), from a park at 2 differences. The last two were the
 * `n = v & 0xFF` that gcc sank into the jal's delay slot where retail keeps
 * the andi before the call and puts `n - 1` in the slot. The mask is not a
 * mask: it is the READ-BACK of the byte global just stored, and the
 * decrement belongs to the same expression -- `n = gDuel_bTerrain[0] - 1;`
 * before the call. Written as the read-back and the decrement in two
 * statements it is 4, as `v & 0xFF` with the decrement before the call 3.
 * Flags: default compiler, as -G4 (gDuel_bTerrain sized out of small data, the
 * D_8009B0F4_abs / D_8009B134_abs sized arms).
 */

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
    u8 *a;

    if (DuelEffect_MarkInitialized() == 0) {
        r = (u8 *)D_8009B1C8;
        r[0xA] = r[0xA] + 1;
        v = *(u8 *)&gDuel_wEffectCardID - 0x49;
        gDuel_bTerrain[0] = v;
        n = gDuel_bTerrain[0] - 1;
        e = func_8002C604(0xA);
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
            gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x20;
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

    gDuel_wCardEffectFlags = 0;
}
