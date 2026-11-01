#define gDuel_bEffectRequestStatus_IN_DATA
#define GDIALOG_CHOICE_SIZED
#include "../types.h"
#include "duel_side_state.h"
#include "input.h"
#include "duel_check_quit_input.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "text_box_update_channels.h"
#include "duel_scene_callbacks.h"
#include "duel_scene_state.h"
#include "duel_scene_update.h"
#include "duel_effect.h"
#include "duel_effect_request.h"
#include "duel_field_display_objects.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "duel_field_effects.h"
#include "dialog_choice.h"
#include "ai_opponent_data.h"
#include "rand_get_interval.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "duel_shuffle_deck.h"
#include "duel_shuffle_both_decks.h"

/* One frame of the duel scene. It services the quit dialog when
 * gDuel_bQuitDialogState is live -- creating the box on the first frame and
 * tearing it down once its 0x2000 flag clears -- and otherwise dispatches the
 * current scene step through the scene callback table. */
/* The quit dialog's box. The Japanese and European builds draw it at another
   size, so a regional build supplies its own. */
#ifndef DUEL_QUIT_BOX_X
#define DUEL_QUIT_BOX_X 0x78
#define DUEL_QUIT_BOX_WIDTH 0x50
#define DUEL_QUIT_BOX_HEIGHT 0x24
#endif
/* Its flags: 0x40 in the European build. */
#ifndef DUEL_QUIT_BOX_FLAGS
#define DUEL_QUIT_BOX_FLAGS 0x20
#endif

void DuelScene_Update(void)
{
    u8 value;
    DuelEffectChannel *window;

    if (gDuel_wDisplayTransitionFlags != 0) {
        Duel_UpdateDisplayTransition();
    }
    DuelEffect_UpdateRequests();
    value = gDuel_bEffectRequestStatus;
    if (value & DUEL_EFFECT_REQUEST_STATUS_ACTIVE) {
        if (value & DUEL_EFFECT_REQUEST_STATUS_BLOCKING) {
            return;
        }
        gDuel_bEffectRequestStatus =
            value & ~DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
    }
    if (DuelEffect_UpdateCardEffect() != 0 || DuelEffect_UpdateState() != 0) {
        return;
    }
    value = gDuel_bQuitDialogState;
    if (value != 0) {
        if (!(value & 0x80)) {
            gDuel_bQuitDialogState = value | 0x80;
            SD_SEPlayFull(0x30);
            window =
                TextBox_CreateFlagged(3, 0x22, DUEL_QUIT_BOX_X, 0x58,
                                      DUEL_QUIT_BOX_WIDTH,
                                      DUEL_QUIT_BOX_HEIGHT, DUEL_QUIT_BOX_FLAGS);
            do {
                TextBox_UpdateChannels();
            } while (window->prompt_object == 0);
        } else {
            DuelEffectChannel *cleanup;

            TextBox_UpdateChannels();
            cleanup = (DuelEffectChannel *)gText_Channel3;
            if (cleanup->flags_34 & 0x2000) {
                TextBox_Destroy(cleanup);
                gDuel_bQuitDialogState = 0;
                if (gDialog_bChoice[0] != 0) {
                    gDuel_wOutcomeFlags |= 0x2000;
                }
            }
        }
    } else {
        void (**callbacks)(void);
        u16 index;

        callbacks = gDuel_apfnSceneStateHandler;
        index = gDuel_wSceneStateFlags;
        callbacks[index & DUEL_SCENE_PHASE_MASK]();
        if (!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
            gDuel_bSceneSubStep = 0;
        }
    }
}

void DuelScene_UpdateWithSideInput(void)
{
    int value = 0;

    if (gDuel_bOpponentID < 0) {
        value = gDuel_bActiveSide;
        if (gDuel_bOutroWinnerSide >= 0) {
            value = gDuel_bOutroWinnerSide;
        }
    }
    if (value != 0) {
        Input_BackupPad1AndUsePad2();
        DuelScene_Update();
        Input_RestorePad1FromBackup();
    } else {
        DuelScene_Update();
    }
}

void Duel_ShuffleDeck(const u16 *src, u16 *out16, u8 *out8) {
    u8 buf[CARD_COUNT];
    u16 *b16;
    u8 *b8;
    u16 *e;
    u16 *t;
    u8 *q;
    s32 i;
    s32 n;
    s32 acc;
    s32 lim;
    s32 x;
    s32 y;
    s32 t16;
    s32 t8;
    s32 u16b;
    s32 u8b;

    b16 = out16;
    b8 = out8;

    if (src == 0) {
        for (i = CARD_COUNT - 1; i >= 0; i--) {
            buf[i] = 0;
        }

        t = gDuel_awOpponentDeckPool;
        n = 0;
        while (n < DECK_SIZE) {
            lim = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
            acc = 0;
            i = 0;
            e = t;
            while (1) {
                acc += *e;
                if (acc >= lim) {
                    q = buf + i;
                    if (*q < DECK_CARD_COPY_LIMIT) {
                        *out16 = i + 1;
                        *out8 = n;
                        n++;
                        out16 += 1;
                        out8 += 1;
                        /* `t16` is the block-copy temp forty lines down; its
                         * live range resumes there, and borrowing the name
                         * here is 10 differences against 4. A fresh local is
                         * not the same thing (the permuter found this). */
                        t16 = *q + 1;
                        *q = t16;
                    }
                    break;
                }
                i++;
                e += 1;
                if (i >= STARTER_DECK_WEIGHT_SCAN_COUNT) {
                    break;
                }
            }
        }
    } else {
        for (i = 0; i < DECK_SIZE; i++) {
            *out16 = *src;
            src += 1;
            *out8 = i;
            out8++;
            out16 += 1;
        }
    }

    for (i = 0; i < DUEL_DECK_SHUFFLE_SWAP_COUNT; i++) {
        x = Rand_GetInterval(DECK_SIZE);
        y = Rand_GetInterval(DECK_SIZE);
        t16 = t8 = b16[x];
        u16b = b16[y];
        b16[x] = u16b;
        b16[y] = t16;
        t8 = b8[x];
        u8b = b8[y];
        b8[x] = u8b;
        b8[y] = t8;
    }
}

void Duel_ShuffleBothDecks(const u16 *arg0, const u16 *arg1)
{
    u16 *base = gDuel_awPlayerShuffledDeck;

    Duel_ShuffleDeck(arg0, base, (u8 *)base - 0x54);
    Duel_ShuffleDeck(arg1, &base[DECK_SIZE],
                     (u8 *)base - 0x2C);
}
