#include "../types.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#define D_8009B360_AS_SIDE_ARRAY
#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define GINPUT_PAD1_REPEAT_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#define TEXT_STRING_ID_IN_DATA
#define DUEL_SAVE_WINDOWS_AS_PAIR
#define DUEL_RITUAL_DATA_RESULT_VIEW
#include "duel_scene_state.h"
#include "func_800179F4.h"
#include "text_constants.h"
#include "text_staging.h"
#include "display_asset_banks.h"
#include "view_state.h"
#include "graphics_frame.h"
#include "duel_result_display.h"
#include "duel_rewards.h"
#include "duel_check_ritual.h"
#include "duel_check_quit_input.h"
#include "duel_side_state.h"
#include "ai_opponent_data.h"
#include "duel_effect.h"
#include "fade.h"
#include "input.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "display_object_api.h"

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))
#define W(p, o) (*(u32 *)((u8 *)(p) + (o)))

void DuelScene_UpdateResultRewards(void)
{
    DisplayObject *object;
    s32 score;
    s32 sound;
    s32 count;
    s32 offset;
    s32 x;
    s16 dropped_card;
    u16 value;
    s8 page;
    u8 opponent;

    D_800F2848.angle = (u16)D_800F2848.angle + 2;
    func_8001352C();
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        func_80015C84();
        Fade_SetTargetLevel(128, 2);
        D_8009B1E8 = &gDuel_awRitualData;
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        D_8009B34E = 0;
        D_8009B355 = 0;
        gText_abColorSlots[0] = 4;
        gText_abColorSlots[1] = 4;
        gText_abColorSlots[gDuel_bWinnerSide] = 0;
        sound = 0x72E1;
        if (gDuel_bWinnerSide) {
            sound = 0x72F1;
            opponent = *(u8 *)&gDuel_bOpponentID;
            D_8009B355 = 1;
            D_8009B32E = (s8)opponent - 31960;
        }
        SD_BGMPlay(sound);
        if (D_8009B360[0] < 0 && gDuel_bOpponentID < 0) {
            D_8009B34E = 1;
            D_8009B355 += 2;
        }
        D_8009B1E8->is_tec_rank = 0;
        Duel_CalcRankScore();
        score = D_8009B1E8->side_scores[gDuel_bWinnerSide];
        if (score < 50) {
            D_8009B1E8->is_tec_rank = 1;
            if (score < 0)
                score = 0;
            score = 99 - score;
        }
        if (score >= 100)
            score = 99;
        score -= 50;
        D_8009B1E8->rank_tier = score / 10;
        D_8009B1E8->page_index = 0;
        object = func_800400AC(func_8004002C(), 2);
        func_800404CC(object, 32, 16, 3, 1, 2, 11, 524);
        func_80042918(object);
        object->flags |= 0x28;
        object = func_800400AC(func_8004002C(), 2);
        func_800404CC(object, 288, 16, 3, 1, 0, 11, 524);
        func_80042918(object);
        count = 9;
        object->flags |= 0x28;
        object = func_800400AC(func_8004002C(), 2);
        func_800428A8(object, 0, 8, 0, 4, 0, 16, 8, D_801AF000);
        func_80042918(object);
        func_800428EC((u8 *)object, -1);
        object->flags |= 0x20;
        D_8009B1E8->root = object;
        do {
            D_8009B1E8->children[count] = 0;
            count--;
        } while (count >= 0);
        gDuel_wSelectedCardID = 0;
        if (D_8009B360[0] < 0 && gDuel_bOpponentID >= 0) {
            if (gDuel_bWinnerSide)
                goto side_result;
            D_8009B1E8->starchip_prize = D_8009B1E8->rank_tier + 1;
            score = 2 * (D_8009B1E8->is_tec_rank != 0);
            if (D_8009B1E8->rank_tier < 3)
                score = 1;
            dropped_card = Duel_SelectCardDrop(score);
            count = 0;
            gDuel_wSelectedCardID = dropped_card;
            D_8009B1E8->dropped_card_id = dropped_card;
            D_801D56A8[0] = dropped_card;
            offset = 8;
            if (D_8009B1E8->starchip_prize) {
                x = 160;
                do {
                    count++;
                    object = func_800400AC(func_8004002C(), 2);
                    func_800404CC(object, x, 192, 3, 4, 0, 11, 524);
                    func_80042918(object);
                    x += 20;
                    object->flags |= 0x20;
                    *(DisplayObject **)((u8 *)D_8009B1E8 + offset + 4) = object;
                    offset += 4;
                } while (count < D_8009B1E8->starchip_prize);
            }
        }
side_result:
        if (D_8009B360[gDuel_bWinnerSide] < 0) {
            object = func_800400AC(func_8004002C(), 2);
            func_800428A8(object, 0, 16, 0, 5, D_8009B1E8->is_tec_rank,
                16, 8, D_801AF000);
            func_80042918(object);
            func_800428EC((u8 *)object, -2);
            object->flags |= 0x20;
            D_8009B1E8->children[0] = object;
            object = func_800400AC(func_8004002C(), 2);
            func_800428A8(object, 0, 16, 0, 6, D_8009B1E8->rank_tier,
                16, 8, D_801AF000);
            func_80042918(object);
            func_800428EC((u8 *)object, -1);
            object->flags |= 0x20;
            D_8009B1E8->children[1] = object;
        }
        goto show_page;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        if (!(D_800E9EC8_arr[6] & 0x80)) {
            if (!(gDuel_wSceneStateFlags & 0x2000)) {
                gDuel_wSceneStateFlags |= 0x2000;
                Fade_StartOut();
                D_800E9EC8_arr[4] = 255;
                func_800156B8(255);
            } else {
                u8 *save = D_8009B1D8[gDuel_bWinnerSide];
                D_8009B16C |= 0x2000;
                if (save) {
                    if (D_8009B360[0] < 0 && gDuel_bOpponentID >= 0) {
                        W(D_8009B1D8[0], 0x5E0) += D_8009B1E8->starchip_prize;
                        if (W(D_8009B1D8[0], 0x5E0) > 999999)
                            W(D_8009B1D8[0], 0x5E0) = 999999;
                        Duel_AwardCard(D_8009B1E8->dropped_card_id);
                    } else {
                        value = H(D_8009B1D8[gDuel_bWinnerSide], 0x518) + 1;
                        H(D_8009B1D8[gDuel_bWinnerSide], 0x518) = value;
                        if (value >= 10000)
                            H(D_8009B1D8[gDuel_bWinnerSide], 0x518) = 9999;
                        value = H(D_8009B1D8[gDuel_bWinnerSide ^ 1], 0x51A) + 1;
                        H(D_8009B1D8[gDuel_bWinnerSide ^ 1], 0x51A) = value;
                        if (value >= 10000)
                            H(D_8009B1D8[gDuel_bWinnerSide ^ 1], 0x51A) = 9999;
                    }
                }
            }
        }
    } else if (gInput_wPad1Repeat & 0xA000) {
        D_8009B1E8->page_index++;
        if (gInput_wPad1Repeat & 0x8000) {
            page = D_8009B1E8->page_index - 2;
            D_8009B1E8->page_index = page;
            if (page < 0)
                D_8009B1E8->page_index = 2;
        }
        if ((s8)D_8009B1E8->page_index >= 3)
            D_8009B1E8->page_index = 0;
        SD_SEPlayFull(6);
show_page:
        Duel_ShowResultPage((s8)D_8009B1E8->page_index);
    } else if (gInput_wPad1Pressed & 0x40) {
        gDuel_wSceneStateFlags |= 0x4000;
        Fade_SetTargetLevel(0, 6);
        SD_SEPlayFull(0x30);
    }
}
