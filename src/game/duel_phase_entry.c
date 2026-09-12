#define FUNC_80018004_AMBIENT_POSITION_ARGS
#define D_8009B36A_IN_DATA
#include "../types.h"
#include "func_8002C604.h"
#include "func_800179F4.h"
#include "display_object.h"
#include "duel_card.h"
#include "duel_card_object_helpers.h"
#include "duel_effect_request.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_staging.h"
#include "duel_side_state.h"
#include "duel_scene_state.h"
#include "duel_phase_entry.h"
#include "func_8001825C.h"
#include "view_state.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_deck_card.h"
#include "fade.h"
#include "file_transfer.h"
#include "duel_selection_layout.h"

#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"
#include "../unmatched.h"
#include "func_80018004.h"
#include "duel_apply_card_object_flags.h"
#include "duel_deck_card_data.h"

extern s8 D_8009B1B9;
extern s8 D_8009B208[8];

/* Three contiguous entries from the D_80090998 duel-phase callback table:
   resume/replay reconstruction, initial deck and selection setup, and draw
   phase hand reconstruction. All three use D_8009B23A as their first-call
   latch and rebuild active duel-card and side state around fade or transfer
   gates. */

void func_8001825C(void)
{
    DuelCardRecord *rec;
    u8 *obj;
    u8 *q;
    u8 *b;
    DisplayObject *card;
    s32 i;
    s32 keep;
    s32 y;
    u16 flags;
    s8 n;

    if ((D_8009B23A & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        D_8009B23A |= DUEL_SCENE_FLAG_INITIALIZED;
        rec = D_801A7B64;
        for (i = HAND_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                func_80024D34(i, ((s8 *)rec->data)[2]);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            }
        }
        rec = D_801A7B64 + 15;
        for (i = 20; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                func_80024D34(i, ((s8 *)rec->data)[2]);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            }
        }
        func_8001352C();
        for (i = 0; i < DUEL_SIDE_COUNT; i++) {
            if (D_800E9FF0[i].field_19 != 0) {
                obj = func_8002C604(0x15);
                *(u16 *)(obj + 0x1A) = i + 2;
                obj[0x1C] |= 0x20;
                D_8009B1F0[i] = obj;
            }
        }
        if (D_8009B1C8->field_00 == 0x28) {
            D_8009B23A |= 0x2000;
            for (i = 0; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
                rec = &D_801A7AD8[D_800907D8[
                    i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT]];
                if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                    func_80024954(rec);
                }
            }
        } else {
            q = (u8 *)D_800E9FF0;
            if (*(s16 *)(q + 0x14) != 0 && *(s16 *)(q + 0x34) != 0) {
                SD_BGMPlay(D_8009B36A);
            }
        }
        Fade_StartIn();
        return;
    }

    if ((D_8009B23A & 0x4000) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) != 0) {
            return;
        }
        if ((gFade_State.flags & 0x80) != 0) {
            return;
        }
        D_8009B23A |= 0x4000;
        D_8009B1B9 = 2;
        if ((D_8009B23A & 0x2000) != 0) {
            D_8009B23A = 0xC;
        }
        return;
    }

    for (;;) {
        n = D_8009B1B9 - 1;
        D_8009B1B9 = n;
        if (n < 0) {
            D_8009B23A = 5;
            return;
        }
        if (D_8009B208[n] >= 0) {
            break;
        }
    }
    {
        s32 replay_offset = DUEL_CARD_STAGING_REPLAY_BASE_OFFSET;
        DuelCardReplayRecordBlock *replay;

        b = D_8015C424;
        replay = (DuelCardReplayRecordBlock *)(b +
            D_8009B208[n] * sizeof(DuelCardRecord) + replay_offset);
        card = replay->record.object;
    }
    func_8001352C();
    obj = (u8 *)func_8002C68C(0xB);
    *(u16 *)obj = card->field_30.h.field_30;
    *(u16 *)(obj + 2) = card->field_30.h.field_32;
    *(u16 *)(obj + 4) = *(u16 *)&card->field_34;
    *(u16 *)(obj + 0x1A) = func_800181EC((CardObject *)card);
    func_80024954(&D_801A7AD8[card->field_6A]);
    SD_SEPlayFull(0x1F);
}

void func_80018608(void)
{
    u16 *w;
    u8 *r1;
    u8 *r2;
    DuelDeckCardRecord *rec;
    s32 i;
    s32 atk;
    s32 def;
    s32 stat;
    s32 stat2;

    w = (u16 *)&D_800F2848;
    if ((D_8009B23A & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        D_8009B23A |= DUEL_SCENE_FLAG_INITIALIZED;
        Duel_RequestCombinedDeckData();
        D_800F2848.field_00 = 0x4B0;
        w[2] = 0x358;
        w[1] = 0x16C0;
        func_8001352C();
        D_8009B174 = 2;
        Fade_StartIn();
        return;
    }

    switch (D_8009B174 & 0x1F) {
    case 2:
        D_800F2848.field_00 -= 2;
        w[2] -= 2;
        w[1] -= 0x10;
        if ((s16)w[1] <= 0x400) {
            D_800F2848.field_00 = 0x258;
            w[2] = 0x100;
            w[1] = 0x400;
            D_8009B174 = 3;
        }
        func_8001352C();
        break;
    case 3:
        if ((D_8009B174 & 0x80) == 0) {
            D_8009B174 |= 0x80;
            D_8009B162 = 2;
            D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F10 +
                D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE);
            *(u16 *)&D_8009B1B4->field_0C = 0xAE;
        }
        if (D_8009B162 == 0) {
            D_8009B174 = 4;
        }
        break;
    case 4: {
        u32 mask = FILE_TRANSFER_REQUEST_BLOCKED_MASK;

        if (((D_8009B0F4_abs & mask) |
             D_8009B134_abs) != 0) {
            break;
        }
        SD_BGMPlay(D_8009B36A);
        Duel_PopulateCombinedDeckData();
        atk = 0;
        def = 0;
        rec = gDuel_aDeckCardRecords;
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat = gDuel_adwCardStats[rec->id - 1];
            atk += (stat & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        r1 = (u8 *)D_800E9FF0;
        *(s16 *)(r1 + 0xE) = atk / DECK_SIZE;
        *(s16 *)(r1 + 0x10) = def / DECK_SIZE;
        atk = 0;
        def = 0;
        rec = &gDuel_aDeckCardRecords[DECK_SIZE];
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat2 = gDuel_adwCardStats[rec->id - 1];
            atk += (stat2 & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat2 >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        r2 = (u8 *)D_800E9FF0;
        *(s16 *)(r2 + 0x2E) = atk / DECK_SIZE;
        *(s16 *)(r2 + 0x30) = def / DECK_SIZE;
        D_8009B174 = 5;
        break;
    }
    case 5:
        Duel_ClearHandSlots();
        D_8009B1EC = HAND_SIZE;
        D_8009B23A = 3;
        ((DuelSelectionSideView *)(D_800E9F10 +
            D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE))->hand = D_800EA030;
        break;
    }
}

/* Draw phase entry. On its first call it sets the phase flag, points the
   side state and selection records at the current side, ticks down the
   side's pending counter (releasing the pending object at D_8009B1F0 when
   it reaches zero), resets the used flag on every card record, then
   rebuilds the hand: the five hand slot bytes are copied out and cleared,
   each valid one gets its card record set up and a hand object spawned at
   a fixed x and a 0x3C-spaced y, and the number of empty slots is stored
   for the draw. Later calls only clear the phase flag once the message
   state has returned to zero. */
void func_8001898C(void) {
    u8 hand[HAND_SIZE];
    DuelCardRecord *rec;
    u8 *fl;
    DuelCardRecord *placed;
    DuelHandSlot *base;
    DuelHandSlot *slot;
    u8 *p;
    s32 side;
    s32 i;
    s32 n;
    s32 idx;
    s32 y;
    u16 flags;
    s8 c;

    if ((D_8009B23A & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        D_8009B23A |= DUEL_SCENE_FLAG_INITIALIZED;
        *(u16 *)&D_8009B21C->field_40.h.field_40 =
            (D_8009B1D5 << 4) | 0x2E0;
        Duel_ClearHandSlots();
        side = D_8009B1D5;
        D_8009B1C8 = (DuelSideState *)((u8 *)D_800E9FF0 + side * sizeof(DuelSideState));
        D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F10 +
                side * DUEL_SELECTION_SIDE_SIZE);
        base = D_800EA030;
        *(DuelHandSlot **)((u8 *)D_8009B1B4 + 8) = base;
        if (*(s8 *)((u8 *)D_8009B1C8 + 0x19) != 0) {
            c = ((u8 *)D_8009B1C8)[0x19] - 1;
            ((u8 *)D_8009B1C8)[0x19] = c;
            if (c <= 0) {
                ((u8 *)D_8009B1C8)[0x19] = 0;
                *(u16 *)(D_8009B1F0[D_8009B1D5] + 0x1A) = 0xFFFD - D_8009B1D5;
                D_8009B1F0[D_8009B1D5] = 0;
            }
        }
        ((u8 *)D_8009B1C8)[1]++;
        rec = D_801A7AD8;
        for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                rec->flags = flags & ~DUEL_CARD_FLAG_USED_THIS_TURN;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            } else {
                rec->flags = 0;
            }
        }
        for (i = 0; i < HAND_SIZE; i++) {
            hand[i] = ((u8 *)D_8009B1C8 + i)[0x1A];
            *(s8 *)((u8 *)D_8009B1C8 + i + 0x1A) = -1;
        }
        n = 0;
        i = n;
        slot = base;
        y = 0xE;
        idx = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT;
        rec = &D_801A7AD8[idx];
        placed = rec;
        for (; i < HAND_SIZE; i++, rec++) {
            p = &hand[i];
            rec->flags = 0;
            rec->object = 0;
            if (*(s8 *)p >= 0) {
                ((u8 *)D_8009B1C8 + n)[0x1A] = *p;
                Duel_SetupCardRecord(idx, *(s8 *)p);
                idx++;
                n++;
                slot->object = (u8 *)func_80018004(placed, y, 0x292);
                slot++;
                y += 0x3C;
                placed++;
            }
        }
        D_8009B1EC = HAND_SIZE - n;
        D_8009B162 = 2;
        D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F10 +
                D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE);
        *(u16 *)&D_8009B1B4->field_0C = 0xAE;
    } else if (D_8009B162 == 0) {
        D_8009B23A = 3;
    }
}
