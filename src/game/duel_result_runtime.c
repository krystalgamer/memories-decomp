#define D_8009B0CC_IN_DATA
#define D_8009B362_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "duel_card_display_state.h"
#include "duel_scene_state.h"
#include "duel_check_ritual.h"
#include "../psyq/rand.h"
#include "display_object_motion.h"
#include "input.h"
#include "rand_get_interval.h"
#include "trig_constants.h"
#include "func_800179F4.h"
#include "file_transfer.h"
#include "func_80020BE4.h"
#include "sound.h"
#include "sound_output.h"
#include "fade.h"
#include "duel_side_state.h"
#include "duel_package.h"
#include "duel_screen_tables.h"
#include "duel_result_outro.h"
#include "save_data.h"
#include "card_constants.h"
#include "duel_grid.h"
#include "duel_rank.h"
#include "duel_result_display.h"
#include "duel_rewards.h"
#include "text_staging.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "display_object_config.h"
#include "../unmatched.h"

#define DUEL_RESULT_ORBIT_ANGLE_STEP 0x30

/* The complete duel-result outro, rank, drop, and award runtime.

   The duel-result outro and the two update callbacks it hangs on its
   confetti: func_80020D4C orbits a sprite around its spawn point,
   func_80020EE8 sends it flying off, and func_80020F4C is the outro
   sequence that spawns the sprites on the first and retargets them at the
   second. The three sources were recorded at gcc_2_8_1_g8_no_split,
   gcc_2_8_1_g8 and gcc_2_8_1_g8_split, and each compiles to an identical
   object at gcc_2_8_1_g8_split. Bounded below by the outro package's
   transfer callback func_80020BE4, which cannot reproduce its object under
   this unit's split profile, and above by unmatched assembly. */

/* Per-frame update for an object that orbits a fixed base position
   (field_2C/field_2E) at DUEL_RESULT_ORBIT_ANGLE_STEP per frame, with radius
   the object's field_28 (which also serves as its countdown timer -- it decays
   by 2/frame). When the timer reaches 0: clears field_6C/update, snaps the
   current position (field_30/field_32) back to the base position in one 32-bit
   copy, and returns. Otherwise, while D_8009B0CC's bit 0 is set, spawns a
   companion slot object at the object's current position (tagged via
   sub_table_lookup_set_flag using field_16-1), then advances the angle and
   recomputes the orbit position from base + (rcos,rsin)*radius/ONE. */

void func_80020D4C(DisplayObject *arg0) {
    s16 timer;
    u16 angle;
    DisplayObject *slot;
    s32 vy;

    timer = (s16)arg0->position.h.field_28 - 2;
    arg0->position.h.field_28 = timer;
    if (timer <= 0) {
        arg0->field_6C = 0;
        arg0->update = 0;
        arg0->field_30.word = arg0->field_2C.word;
        return;
    }

    if (D_8009B0CC & 1) {
        slot = func_800400AC(func_8004002C(), 2);
        if (slot != 0) {
            func_800428A8(slot, (s16)arg0->field_30.h.field_30,
                                   (s16)arg0->field_30.h.field_32, 0,
                                   arg0->field_68, arg0->field_69, 0x11, 9,
                                   D_801AF000);
            slot->field_40.h.field_40 = (u16)arg0->field_40.h.field_40 + 0x80;
            slot->flags |= 0x28;
            slot->attribute |= (GsALON | GsAONE);
            func_800428EC((u8 *)slot, (u8)arg0->field_16 - 1);
            slot->field_60 = 8;
            slot->update = (DisplayObjectCallback)func_80042BC0;
        }
    }

    angle = arg0->position.h.field_2A + DUEL_RESULT_ORBIT_ANGLE_STEP;
    arg0->position.h.field_2A = angle;
    arg0->field_30.h.field_30 =
        (s16)arg0->field_2C.h.field_2C +
        rcos((s16) angle) * (s16)arg0->position.h.field_28 / ONE;
    vy = rsin((s16) arg0->position.h.field_2A) * (s16)arg0->position.h.field_28;
    arg0->field_30.h.field_32 = arg0->field_2C.h.field_2E + vy / ONE;
}

void func_80020EE8(DuelCardDisplayObject *object)
{
    if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    object->field_21 -= 2;
    if (object->field_21 < 0xC0) {
        func_8004036C((u8 *)object);
    }
}

/* Duel-result outro sequence, driven from the scene state word D_8009B23A.

   The first call (bit 0x8000 clear) fades the BGM out, records the winning
   side in D_8009B362/D_8009B238, requests the outro package (0x1DAB, 0x22
   sectors) through func_80020BE4, picks the win or lose track for
   D_8009B1E0 -- 0x72F0 only when the player won against a real opponent --
   and hands the two banner objects D_8009B214/D_8009B21C to func_8001EC70
   before entering step 1 of D_8009B174.

   Afterwards the low nibble of D_8009B174 is the step and bit 0x80 marks
   "step already entered":

     1  wait for the DMA/queue flags to drain, then reload the selected
        three-sector duelist data block into 0x801781D8. The adjacent
        func_800472A8 call handles D_8009B1E0 separately; once the transfer
        has drained, SD_BGMPlay starts that track.
     2  spawn the seven confetti sprites from the D_80090928 (real opponent)
        or D_80090960 (no opponent) table row for the winning side, each on
        func_80020D4C with a random radius and orbit key, and remember them
        in the gDuel_awRitualData slot table; then wait for func_80042B40.
     3  hold for 0x258 frames or until the player presses one of the 0xE0
        buttons, then retarget every spawned sprite at func_80020EE8 so it
        flies off, and wait for func_80042B40 again.
     4  once the fade at gFade_State.flags has finished, hand the scene over to
        state 0xD.  */

extern u16 D_8009B1E0;

extern s8 gDuel_bOpponentID __attribute__((section(".data")));

void func_80020F4C(void)
{
    DuelResultSpriteSlot *slots;
    DisplayObject *obj;
    DuelResultSpriteSpec *spec;
    s32 i;
    s32 flags;
    s32 state;
    s32 id;
    s32 timer;
    s32 mode;
    s32 v;

    slots = (DuelResultSpriteSlot *)gDuel_awRitualData;

    v = D_8009B23A;
    if ((v & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        D_8009B23A = v | DUEL_SCENE_FLAG_INITIALIZED;
        SD_BGMFadeOut();
        id = gDuel_bWinnerSide;
        D_8009B362 = 0;
        if (id != 0) {
            D_8009B362 = 1;
        }
        D_8009B238 = id;
        File_RequestAsyncTransfer(
            0, 0,
            FILE_WA_DUEL_RESULTS_START_SECTOR,
            FILE_WA_DUEL_RESULTS_SECTOR_COUNT,
            func_80020BE4, 0, 0
        );
        mode = 0x72E0;
        if (gDuel_bWinnerSide != 0) {
            if (gDuel_bOpponentID >= 0) {
                mode = 0x72F0;
            }
        }
        obj = D_8009B214;
        D_8009B1E0 = mode;
        /* The record spells 0x28 u16 and this is the one site that puts a
           negative value there. Through the plain member GCC materialises
           -116 as `ori 0xff8c` where retail has `addiu -116`; the store at
           0x198 below is unaffected, so the divergence is the constant's
           sign, not the member. */
        *(s16 *)&obj->position.h.field_28 = -116;
        obj->field_2C.h.field_2C = 0x30;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)func_8001EC70;
        obj->position.h.field_2A = (s16)obj->field_30.h.field_32;
        obj = D_8009B21C;
        obj->position.h.field_28 = 0x198;
        obj->field_2C.h.field_2C = 0x30;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)func_8001EC70;
        D_8009B174 = 1;
        obj->position.h.field_2A = (s16)obj->field_30.h.field_32;
        return;
    }

    if (D_8009B162 != 0) {
        return;
    }

    flags = D_8009B174;
    state = flags & 0xF;
    switch (state) {
    case 1:
        if ((flags & 0x80) == 0) {
            if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) {
                return;
            }
            D_8009B174 = flags | 0x80;
            D_8009B162 = 8;
            id = gDuel_bOpponentID;
            if (id < 0) {
                id = 1;
            }
            File_RequestAsyncTransfer(
                0, 0,
                id * DUELIST_DATA_SECTOR_COUNT + DUELIST_DATA_FIRST_SECTOR,
                DUELIST_DATA_SECTOR_COUNT, 0, 0,
                (s32)((u8 *)slots - 0x1800));
            func_800472A8(D_8009B1E0);
        } else {
            if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) {
                return;
            }
            D_8009B174 = 2;
            SD_BGMPlay(D_8009B1E0);
        }
        break;
    case 2:
        if ((flags & 0x80) == 0) {
            D_8009B174 = flags | 0x80;
            for (i = 0; i < DUEL_RESULT_SPRITE_COUNT; i++) {
                if (gDuel_bOpponentID >= 0) {
                    spec = &D_80090928[gDuel_bWinnerSide][i];
                } else {
                    spec = &D_80090960[gDuel_bWinnerSide][i];
                }
                slots[i].object = 0;
                if (spec->kind != 0) {
                    obj = func_800400AC(func_8004002C(), 2);
                    func_800428A8(obj, spec->x, spec->y, 0,
                                  gDuel_bWinnerSide, spec->kind, 0x11, 9,
                                  D_801AF000);
                    if (obj->field_69 >= 0x1A) {
                        obj->field_40.h.field_40 =
                            (u16)obj->field_40.h.field_40 + 0x10;
                    }
                    obj->flags |= 0x28;
                    obj->attribute |= (GsALON | GsAONE);
                    obj->field_48.h.field_48 = spec->tag;
                    obj->field_48.h.field_4A = 0x18;
                    func_80042918(obj);
                    obj->field_2C.word = obj->field_30.word;
                    obj->position.h.field_28 = (rand() & 0x3F) + 0x140;
                    obj->position.h.field_2A =
                        Rand_GetInterval(TRIG_ANGLE_FULL_TURN);
                    obj->field_6C = 1;
                    obj->update = (DisplayObjectCallback)func_80020D4C;
                    slots[i].object = obj;
                }
            }
        } else {
            if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) {
                return;
            }
            if (func_80042B40(1) == 0) {
                D_8009B174 = 3;
            }
        }
        break;
    case 3:
        if ((flags & 0x80) == 0) {
            D_8009B174 = flags | 0x80;
            D_8009B1D0 = 0x258;
        }
        if ((D_8009B174 & 0x40) == 0) {
            timer = D_8009B1D0 - 1;
            D_8009B1D0 = timer;
            if ((s16)timer > 0) {
                if ((gInput_wPad1Pressed &
                     (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
                    if ((func_8004703C() & 0x80) != 0) {
                        return;
                    }
                }
            }
            D_8009B174 = D_8009B174 | 0x40;
            for (i = 0; i < DUEL_RESULT_SPRITE_COUNT; i++) {
                obj = slots[i].object;
                if (obj != 0) {
                    obj->field_6C = 1;
                    obj->update = (DisplayObjectCallback)func_80020EE8;
                }
            }
        } else {
            if (func_80042B40(1) == 0) {
                D_8009B174 = 4;
            }
        }
        break;
    case 4:
        if ((flags & 0x80) == 0) {
            D_8009B174 = flags | 0x80;
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            D_8009B23A = 0xD;
        }
        break;
    }
}

/* The duel's reward step: Duel_ShowResultPage shows one of the three result
   pages and opens its text box, Duel_CalcRankScoreChange is the per-rule
   lookup Duel_CalcRankScore sums into that display's side scores, and the
   card drop and award follow. The first two were recorded at gcc_2_8_1_g8
   and gcc_2_8_1_g0_split and compile to identical objects at this unit's
   gcc_2_8_1_g8_split. */

void Duel_ShowResultPage(s32 page)
{
    s32 i;
    DisplayObject *child;
    void *object;

    func_80040410((DisplayObjectConfig *)D_8009B1E8->root, page);
    if (page == 0) {
        DuelResultDisplayState *state = D_8009B1E8;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    } else {
        DuelResultDisplayState *state = D_8009B1E8;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    }
    object = TextBox_Create(
        0, D_8009B1E8->page_text_ids[page], 0x1A, 0x28, 0x120, 0x120
    );
    func_80039A14(object);
}

s32 Duel_CalcRankScoreChange(s32 arg0, s32 arg1)
{
    DuelRankScoreChangeEntry *p = &gDuel_awRankScoreChange[arg0][0];

    while (1) {
        if (arg1 < p->threshold) {
            return p->score_change;
        }
        p++;
    }
}

/* Initializes result-message selectors at +0x34; the winner's signed
   end-reason adjustment selects the middle variant, not a rank letter.
   Both side scores at +0x2C start at DUEL_RANK_SCORE_INITIAL, then receive
   the end-reason and threshold-rule adjustments. Raw statistics are also
   copied into the separate D_801D5608[0].rank_rows[stat][side] display
   table. */
void Duel_CalcRankScore(void) {
    DuelResultDisplayState *p;
    DuelSideState *e;
    s32 i;
    s32 v;
    s32 *q;

    p = D_8009B1E8;
    e = D_800E9FF0;
    q = &D_801D5608[0].rank_rows[0][0];
    p->page_text_ids[0] = 0x44;
    p->page_text_ids[1] = DUEL_RESULT_TEXT_SELECTOR_DEFAULT;
    p->page_text_ids[2] = 0x45;
    if (D_800E9FF0[gDuel_bWinnerSide].rank.result_adjustment ==
        DUEL_RANK_ADJUST_EXODIA_WIN) {
        p->page_text_ids[1] = DUEL_RESULT_TEXT_SELECTOR_EXODIA;
    }
    if (D_800E9FF0[gDuel_bWinnerSide].rank.result_adjustment ==
        DUEL_RANK_ADJUST_DECK_OUT_WIN) {
        p->page_text_ids[1] = DUEL_RESULT_TEXT_SELECTOR_DECK_OUT;
    }

    p->side_scores[1] = DUEL_RANK_SCORE_INITIAL;
    p->side_scores[0] = DUEL_RANK_SCORE_INITIAL;
    for (i = 0; i < DUEL_SIDE_COUNT; i++, e++, q++) {
        p->side_scores[i] += e->rank.result_adjustment;
        v = e->deck_draw_cursor; q[0 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_CARDS_USED, v);
        v = e->life_points.signed_value; q[1 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_REMAINING_LP, v);
        q[2 * DUEL_SIDE_COUNT] = e->field_0E;
        q[3 * DUEL_SIDE_COUNT] = e->rank.field_0B;
        v = e->rank.effective_attacks; q[4 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EFFECTIVE_ATTACKS, v);
        q[5 * DUEL_SIDE_COUNT] = e->field_10;
        q[6 * DUEL_SIDE_COUNT] = e->rank.field_0C;
        v = e->rank.defensive_wins; q[7 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_DEFENSIVE_WINS, v);
        q[8 * DUEL_SIDE_COUNT] = e->rank.field_07;
        v = e->rank.face_down_plays; q[9 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_FACE_DOWN_PLAYS, v);
        v = e->rank.fusions_initiated; q[10 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_INITIATE_FUSION, v);
        v = e->rank.equips_used; q[11 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EQUIP_MAGIC, v);
        q[12 * DUEL_SIDE_COUNT] = e->rank.field_0A;
        v = e->rank.pure_magic_used; q[13 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_PURE_MAGIC, v);
        v = e->rank.traps_triggered; q[14 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TRAPS_TRIGGERED, v);
        v = e->rank.turns_taken; q[15 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TURNS, v);
    }
}

s32 Duel_SelectCardDrop(s32 pool_index)
{
    DuelDropTable *table = &gDuel_awSaPowCardDrops[pool_index];
    s32 threshold = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
    s32 sum = 0;
    s32 i;

    for (i = 0; i < CARD_COUNT; i++) {
        sum += table->weights[i];
        if (sum >= threshold)
            return i + 1;
    }
    return 0;
}

void Duel_AwardCard(s32 card_id)
{
    s32 i;
    u8 *base = (u8 *)gDuel_awPlayerDeck;
    u8 *quantity = base + (card_id + 0x4F);
    u16 *destination = (u16 *)(base + 0x5BC);
    u16 *entry;

    (*quantity)++;
    if (*quantity >= CARD_CHEST_QUANTITY_MAX + 1)
        *quantity = CARD_CHEST_QUANTITY_MAX;
    i = DUEL_RECENT_CARD_DROP_COUNT - 2;
    entry = destination + (DUEL_RECENT_CARD_DROP_COUNT - 2);
    do {
        entry[1] = entry[0];
        entry--;
        i--;
    } while (i >= 0);
    *destination = card_id;
}
