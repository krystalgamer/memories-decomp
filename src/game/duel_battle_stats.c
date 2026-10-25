#include "../types.h"
#include "card_constants.h"
#include "display_object.h"
#include "duel_battle_stats.h"
#include "duel_card_stat_rules.h"
#include "duel_card.h"
#include "duel_side_state.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_staging.h"
#include "duel_action_lock.h"
#include "duel_card_layout.h"
#include "duel_effect.h"
#include "duel_effect_request.h"
#include "duel_grid.h"
#include "sound.h"
#include "view_state.h"
#include "duel_view_tween.h"
#include "../unmatched.h"
#include "duel_trap_resolution.h"
#include "view_state_current.h"

/* Battle arithmetic between two duel cards, built up in address order.

   Duel_CalcGuardianStarBonus reads each card's active guardian star from
   gDuel_adwCardStats and asks the wheel for the signed bonus. The attack and
   defence calculators add that bonus to the card's own stat and clamp it.
   Duel_CompareBattleStrength then picks attack or defence by each card's position and
   compares the result against the opponent's stat, for the AI's field scans.

   These were four sources at two profiles, gcc_2_8_1_g8 for the calculators
   and gcc_2_8_1_g8_split for the bonus and the comparison. The calculators
   compile to identical objects at gcc_2_8_1_g8_split, so the unit builds
   there. It is bounded below by the display-object motion pair and above by
   the attack-trap selector, which is a different mechanism. */

s32 Duel_CalcGuardianStarBonus(DuelCardRecord *left, DuelCardRecord *right)
{
    int left_value, right_value;
    if (right == 0) return 0;
    if (left->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)
        left_value =
            (gDuel_adwCardStats[left->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        left_value =
            (gDuel_adwCardStats[left->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    if (right->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)
        right_value =
            (gDuel_adwCardStats[right->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        right_value =
            (gDuel_adwCardStats[right->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    return Duel_CalcGuardianStarMatchup(left_value, right_value);
}

s32 Duel_CalcBattleAttack(DuelCardRecord *card, DuelCardRecord *opponent)
{
    s32 value = (u16)Duel_CalcCardStats(card)
        + Duel_CalcGuardianStarBonus(card, opponent);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}

s32 Duel_CalcBattleDefense(DuelCardRecord *card, DuelCardRecord *opponent)
{
    s32 value = (Duel_CalcCardStats(card) >> 16)
        + Duel_CalcGuardianStarBonus(card, opponent);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}

s32 Duel_CompareBattleStrength(DisplayObject *left, DisplayObject *right)
{
    DuelCardRecord *a = &gDuel_aCardRecords[left->field_6A];
    DuelCardRecord *b;
    s32 packed;
    s32 wanted;
    s32 actual;

    if (right == 0)
        return Duel_CalcCardStats(a) & 65535;
    b = &gDuel_aCardRecords[right->field_6A];
    packed = Duel_CalcCardStats(b);
    wanted = packed & 65535;
    if (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        wanted = (u32)packed >> 16;
    if (a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        actual = Duel_CalcBattleDefense(a, b);
    else
        actual = Duel_CalcBattleAttack(a, b);
    if (actual != wanted)
        return actual - wanted;
    if ((a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) ||
        (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION))
        return 0;
    return -1;
}

#define DUEL_ATTACK_TRAP_SCRATCH_VIEW(value) \
    ((DuelAttackTrapScratchBlock *)(value))

/* Small data at 0x8009AF24, owned here: the attack threshold of each trap
   from House of Adhesive Tape through Widespread Ruin, which
   Duel_SelectAttackTrap
   scales by 100 before comparing. The last entry is the 255 that keeps
   Widespread Ruin unconditional. */
u8 gDuel_abTrapAttackThresholds[DUEL_ATTACK_TRAP_COUNT] = {
    DUEL_HOUSE_OF_ADHESIVE_TAPE_ATTACK_THRESHOLD /
        DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_EATGABOON_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_BEAR_TRAP_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_INVISIBLE_WIRE_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_ACID_TRAP_HOLE_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_WIDESPREAD_RUIN_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
};

s32 Duel_SelectAttackTrap(const DisplayObject *p) {
    s32 i;
    s32 off1;
    u8 *b1;
    s32 n;
    u8 *tbl2;
    u8 *rec2;
    u8 *b2;
    s32 off2;
    s32 h2;
    DuelCardRecord *e;
    s32 id;
    s32 sx;
    s32 sx2;
    s32 th;
    s32 sel;
    s32 off3;
    u8 *b3;
    u8 *q;
    u8 *tb;
    s32 v;
    DisplayObject *w;
    s32 off4;
    u8 *b4;
    s32 j2;
    u8 *tbl3;
    u8 *rec3;
    s32 h3;
    s32 k;

    i = 0;
    /* Keep the unrebased byte cursors and the record-index word-slot bias. */
    off1 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
    b1 = gDuel_abCardStagingBuffer;
    for (; i < DUEL_ATTACK_TRAP_COUNT; i++) {
        DUEL_ATTACK_TRAP_SCRATCH_VIEW(
            b1 + i * sizeof(u16) + off1)->table.entries.card_ids[0] = 0;
    }
    n = 0;
    i = n;
    tbl2 = gDuel_abFieldGridCardRecordIndices;
    rec2 = (u8 *)gDuel_aCardRecords;
    b2 = gDuel_abCardStagingBuffer;
    off2 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
    h2 = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    for (; i < DUEL_FIELD_ROW_SIZE; i++) {
        e = (DuelCardRecord *)(*(u8 *)(i + h2 + (s32)tbl2) *
            DUEL_CARD_RECORD_SIZE + (s32)rec2);
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            id = (u16)e->card_id;
            if ((u32)(id - DUEL_ATTACK_TRAP_FIRST_CARD_ID) <
                DUEL_ATTACK_TRAP_COUNT) {
                sx = (s16)id;
                th = sx - DUEL_ATTACK_TRAP_FIRST_CARD_ID;
                n++;
                DUEL_ATTACK_TRAP_SCRATCH_VIEW(
                    b2 + th * sizeof(u16) + off2)->table.entries.card_ids[0] = id;
                sx2 = sx - (DUEL_ATTACK_TRAP_FIRST_CARD_ID -
                    DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS);
                DUEL_ATTACK_TRAP_SCRATCH_VIEW(
                    b2 + sx2 * sizeof(u16) + off2)->table.words[0] =
                    ((DisplayObject *)e->object)->field_6A;
            }
        }
    }
    if (n != 0) {
        do {
            th = Duel_CalcCardStats(
                &gDuel_aCardRecords[p->field_6A]
            ) & 0xFFFF;
        } while (0);
        sel = -1;
        off3 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
        b3 = gDuel_abCardStagingBuffer;
        q = b3 + 0xA;
        i = DUEL_ATTACK_TRAP_COUNT - 1;
        tb = gDuel_abTrapAttackThresholds;
        do {
            if (DUEL_ATTACK_TRAP_SCRATCH_VIEW(q + off3)->table.entries.card_ids[0] != 0) {
                v = *(u8 *)(i + (s32)tb);
                if (v * DUEL_ATTACK_TRAP_THRESHOLD_SCALE < th) {
                    break;
                }
                sel = i;
            }
            i--;
            q -= 2;
        } while (i >= 0);
        if (sel >= 0) {
            off4 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
            gDuel_wSelectedTrapCardId = sel + DUEL_ATTACK_TRAP_FIRST_CARD_ID;
            b4 = gDuel_abCardStagingBuffer;
            j2 = sel + DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS;
            gDuel_bSelectedTrapSlot =
                *(u8 *)&DUEL_ATTACK_TRAP_SCRATCH_VIEW(
                    b4 + j2 * sizeof(u16) + off4)->table.words[0];
            return 1;
        }
        if (0) {
        hit:
            w = e->object;
            gDuel_wSelectedTrapCardId = v;
            gDuel_bSelectedTrapSlot = w->field_6A;
            return 1;
        }
    }
    i = 0;
    tbl3 = gDuel_abFieldGridCardRecordIndices;
    rec3 = (u8 *)gDuel_aCardRecords;
    h3 = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    k = DUEL_FAKE_TRAP_CARD_ID;
    for (; i < DUEL_FIELD_ROW_SIZE; i++) {
        e = (DuelCardRecord *)(*(u8 *)(i + h3 + (s32)tbl3) *
            DUEL_CARD_RECORD_SIZE + (s32)rec3);
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            v = e->card_id;
            if (v == k) {
                goto hit;
            }
        }
    }
    return 0;
}

/* Four-state presentation sequencer on the gDuel_wTrapPresentationMode mode byte: mode 0
 * starts the first screen effect and arms the 0x14-frame counter; mode 1
 * copies the selected card's position into a type-8 request, updates
 * the card record and plays the SE when that counter expires; mode 2 starts
 * the second screen effect; mode 3 waits once more, advances the opposing
 * side's state byte at +6 and completes. Returns 1 while busy. */
s32 Duel_UpdateTrapPresentation(void) {
    DuelEffectRequest *e;
    DuelCardReplayRecordBlock *g;
    DisplayObject *p;
    DuelSideState *q;
    u8 *r;
    s32 one;
    s32 v;
    u16 t;
    u16 *q34;
    u16 *d;

    if (gDuel_wDisplayTransitionFlags != 0) {
        return 1;
    }

    one = 1;
    v = gDuel_wTrapPresentationMode & 0xF;

    if (v == one) {
        goto m1;
    }
    if (v < 2) {
        if (v == 0) {
            goto m0;
        }
        return 1;
    }
    if (v == 2) {
        goto m2;
    }
    if (v == 3) {
        goto m3;
    }

    return 1;

m0:
    Duel_StartViewTween(0x10, 0x208, 0x200, gViewState_Current.angle,
                  0xB2 - gDuel_bActiveSide * 0x164);
    gDuel_wDisplayTransitionFlags = 0x10;
    gDuel_wTrapPresentationMode = one;
    gDuel_wStepCounter = 0x14;
    do {
    return 1;

m1:
    t = gDuel_wStepCounter - 1;
    gDuel_wStepCounter = t;
    if ((s16)t <= 0) {
    r = gDuel_abCardStagingBuffer;
    g = (DuelCardReplayRecordBlock *)(
        (u8 *)&((DuelCardRecord *)r)[gDuel_bSelectedTrapSlot] +
        DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
    p = (DisplayObject *)g->record.object;
    e = DuelEffect_CreateRequest(8);
    e->field_00 = (s16)p->field_30.h.field_30;
    e->field_02 = (s16)p->field_30.h.field_32;
    q34 = (u16 *)&p->field_34.h.field_34;
    *(d = (u16 *)&e->field_04) = *q34;
    DuelCard_RemoveFromField(&gDuel_aCardRecords[p->field_6A]);
    SD_SEPlayFull(0x17);
    gDuel_wTrapPresentationMode = 2;
    }
    return 1;

m2:
    Duel_StartViewTween(0x10, 0x258, 0x100, gViewState_Current.angle, 0);
    gDuel_wDisplayTransitionFlags = 0x10;
    gDuel_wTrapPresentationMode = 3;
    gDuel_wStepCounter = 0x14;
    } while (0);
    return 1;

m3:
    t = gDuel_wStepCounter - 1;
    gDuel_wStepCounter = t;
    if ((s16)t > 0) {
        return 1;
    }
    q = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
    q->rank.traps_triggered = q->rank.traps_triggered + 1;
    return 0;
}
