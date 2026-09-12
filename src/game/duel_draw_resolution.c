#define FUNC_80018004_AMBIENT_POSITION_ARGS
#include "../types.h"
#include "duel_card_staging.h"
#include "duel_draw_resolution.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "duel_side_state.h"
#include "duel_scene_state.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_card_layout.h"
#include "duel_deck_card.h"
#include "duel_rank.h"
#include "sound.h"
#include "duel_scene_state.h"
#include "../unmatched.h"
#include "func_80018004.h"
#include "func_80018C34.h"
#include "display_object_api.h"

extern DuelSideState *D_8009B1C8_hand asm("D_8009B1C8");

s32 Duel_HasAllExodiaPieces(void) {
    s16 hand[HAND_SIZE];
    s32 i;
    s32 card_id;

    for (i = 0; i < HAND_SIZE; i++) {
        hand[i] = D_8009B1C8_hand->hand[i];
    }

    for (card_id = EXODIA_FIRST_CARD_ID;
         card_id < EXODIA_CARD_ID_END;
         card_id++) {
        for (i = 0; i < HAND_SIZE; i++) {
            s16 index = hand[i];
            if (index >= 0) {
                s16 id = D_8015C424_cards.cards[index].id;
                if (id == card_id) {
                    hand[i] = -1;
                    goto found;
                }
            }
        }
        return 0;
    found:;
    }

    return 1;
}

extern u8 D_8009B1ED;

void DuelScene_UpdateDrawResolution(void) {
    u8 *p;
    u8 *c;
    DuelCardReplayRecordBlock *g;
    u8 *base;
    s32 i;
    s32 k;
    s32 v;
    s32 t;
    s32 b;
    s32 a;
    s32 n;
    s32 off;
    s32 y;

    v = gDuel_wSceneStateFlags;
    if ((v & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = v | DUEL_SCENE_FLAG_INITIALIZED;
        D_8009B1ED = 1;
    }

    if ((gDuel_wSceneStateFlags & 0x4000) == 0) {
        t = D_8009B1ED - 1;
        D_8009B1ED = t;
        if ((s8)t > 0) {
            return;
        }
        D_8009B1ED = 8;
        b = D_8009B1EC - 1;
        /* Raw offset spelling preserves the matching draw-loop schedule. */
        c = (u8 *)D_8009B1C8;
        D_8009B1EC = b;
        if (*(s8 *)(c + 0x18) >= DECK_SIZE) {
            gDuel_bWinnerSide = D_8009B1D5 ^ 1;
            D_800E9FF0[gDuel_bWinnerSide].rank.result_adjustment =
                DUEL_RANK_ADJUST_DECK_OUT_WIN;
            gDuel_wSceneStateFlags = 0xC;
            return;
        }
        i = HAND_SIZE - 1 - (s8)b;
        a = D_800907CC[i + D_8009B1D5 * HAND_SIZE];
        if ((a & 0x80) != 0) {
            k = (a & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
        } else {
            k = a;
        }
        Duel_SetupCardRecord(k, *(s8 *)((u8 *)D_8009B1C8 + 0x18));
        p = (u8 *)func_80018004(
            (DuelCardRecord *)((u8 *)D_801A7AD8 + k * DUEL_CARD_RECORD_SIZE),
            i * 60 + 0x14E,
            0x92
        );
        p[0x6C] = 1;
        *(s16 *)(p + 0x60) = 0xC;
        *(s32 *)(p + 0x24) = (s32)func_80018C34;
        D_800EA030[i].object = p;
        base = D_8015C424;
        g = (DuelCardReplayRecordBlock *)(base +
            p[0x6A] * sizeof(DuelCardRecord) +
            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
        y = *(s8 *)&((DuelDeckCardRecord *)g->record.data)->index_02;
        *(s8 *)((u8 *)D_8009B1C8 + i + 0x1A) = y;
        n = *(u8 *)((u8 *)D_8009B1C8 + 0x18);
        *(u8 *)((u8 *)D_8009B1C8 + 0x18) = n + 1;
        if (*(s8 *)&D_8009B1EC == 0) {
            gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x4000;
        }
        SD_SEPlayFull(0xA);
    } else {
        if (func_80042B40(1) == 0) {
            gDuel_wSceneStateFlags = 4;
            if (Duel_HasAllExodiaPieces() != 0) {
                gDuel_wSceneStateFlags = 0xE;
            }
        }
    }
}
