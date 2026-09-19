/*
 * Duel scene-state 5: the field. It runs the scripted field path for a
 * computer-controlled side, moves the field cursor, opens the card viewer,
 * picks the attacker and its target, commits a fusion or sacrifice selection
 * and fades the field for the battle state.
 *
 * Matches retail under gcc_2_8_1_g8_split. What the listing fixes about the
 * source:
 * - The pad word is volatile: every test re-reads it.
 * - The side records are the DuelSelectionRecord struct, not byte views:
 *   the cursor-object loads pass the state stores only as struct members.
 *   Record 3 of the current side is D_800E9F10 + 0x54 (spimdisasm names it
 *   D_800E9F64) so cse can derive one from the other, and the pick cursor is
 *   record 2 of the same table.
 * - The slot index row * 5 + col is a separate statement into the loop
 *   counter i wherever retail computes the sum into its own register.
 * - One variable holds the scripted card id, the viewer request, the attack
 *   row, the commit flag, the fade level and the sprite tint: it is the $s4
 *   of every block. flags is the substate word and the fade target. The
 *   constants that retail lifts to a block top (1 at entry, 2/3/4 in the
 *   attack branches) go through the multiply-assigned x and y.
 * - The row == 2 test lets cse fold row * 5 to 10 in the grid expression.
 * - commit: sits before sel = 0 so the store survives cse, and case 5 keeps
 *   its copy through the branch slot.
 * - The bank reads take the deck base through a pointer so the 0x36BC
 *   member offset stays in the load displacement.
 */
#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define D_8009B300_IN_DATA
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE88_VISIBLE
#define D_800EAE88_AS_BYTES
#define D_800EAE91_AS_ARRAY
#define D_800EAE92_AS_ARRAY
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GDUEL_BCARDVIEWERYOFFSET_IN_DATA
#define GDUEL_BEFFECTSTATE_IN_DATA
#define GDUEL_WVIEWERCARDID_IN_DATA
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "../types.h"
#include "duel_scene_field_actions.h"
#include "duel_scene_state.h"
#include "duel_side_state.h"
#include "duel_selection_layout.h"
#include "duel_card.h"
#include "duel_card_can_act_this_turn.h"
#include "duel_card_staging.h"
#include "duel_card_display_state.h"
#include "duel_card_record_lifecycle.h"
#include "duel_cursor_status.h"
#include "duel_field_display_objects.h"
#include "duel_grid.h"
#include "func_8001D240.h"
#include "display_object_property_transitions.h"
#include "func_8001D518.h"
#include "func_8001D5B4.h"
#include "func_80020988.h"
#include "duel_action_lock.h"
#include "duel_card_viewer.h"
#include "duel_effect.h"
#include "duel_check_quit_input.h"
#include "duel_side_view_angles.h"
#include "ai.h"
#include "duel_load_package_stage.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object_work_slots.h"
#include "duel_get_card_viewer_request_id.h"
#include "func_80022D94.h"
#include "input.h"
#include "sorted_entry.h"
#include "sound.h"
#include "duel_card_state_helpers.h"
#include "../unmatched.h"

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define SB(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define SIDE_SIZE 4
/* Record 3 of the side's selection records: the same symbol as record 0. */
#define SEL_REC3 ((DuelSelectionRecord *)(D_800E9F10 + 0x54))
#define FIELD_CURSOR (((DuelSelectionSide *)D_800E9F10)[D_8009B1D5].records[2])
/* Grid card, two spellings: the [side][slot] form and the flat form. */
#define GRID_CARD(s) (&D_801A7AD8[D_800907D8[(s)->row * 5 + (s)->col + D_8009B1D5 * 20]])
#define GRID_CARD_FLAT(s) \
    (&D_801A7AD8[D_800907D8[(s)->row * 5 + (s)->col + D_8009B1D5 * 20]])
#define BANK(i, o) (big = 0x48000, H(pw + (i) * 0x1C + big, (o)))
#define CARD_KIND(id) ((gDuel_adwCardStats[(id) - 1] >> 0x1A) & 0x1F)

/* Tentative definitions: the _comm profile keeps them common, which is what
 * puts retail's load-delay nop in front of each gp-relative store. */
u16 D_8009B170;
u16 D_8009B172;
u16 D_8009B178;
u16 D_8009B17A;
u8 D_8009B19C;

void DuelScene_UpdateFieldActions(void)
{
    DuelSelectionRecord *side;
    DuelSelectionRecord *rec;
    u8 *o;
    u8 *other;
    DuelCardRecord *card;
    DuelCardRecord *pick;
    DisplayObject *co;
    s32 a;
    s32 k;
    s32 n;
    s32 i;
    s32 x;
    s32 y;
    s32 t;
    s32 lvl;
    s32 r;
    s32 d;
    s32 big;
    u8 *pw;
    s32 idx;
    s8 row;
    u32 f;
    s32 flags;

    side = (DuelSelectionRecord *)D_800E9F48 + D_8009B1D5 * SIDE_SIZE;
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        if (D_800E9FF0[0].life_points.signed_value == 0 ||
            D_800E9FF0[1].life_points.signed_value == 0) {
            gDuel_bWinnerSide = D_8009B1D5;
            if (D_800E9FF0[D_8009B1D5].life_points.signed_value == 0) {
                gDuel_bWinnerSide = D_8009B1D5 ^ 1;
            }
            D_800E9FF0[gDuel_bWinnerSide].rank.result_adjustment = 2;
            gDuel_wSceneStateFlags = 0xC;
            return;
        }
        D_8009B1F8 = 0;
        D_8009B21A = 0;
        D_8009B1B4 = (DuelCardPickCursor *)side;
        side->field_12 = 4;
        side->field_0C = 0xAE;
        y = 1;
        side->field_18 = y;
        side->field_13 = 0;
        side->field_11 = 0;
        D_8009B162 = 3;
        D_8009B174 = y;
    }

    switch (D_8009B174 & 0xF) {
    case 1:
        if (D_8009B162 == 0) {
            D_8009B174 = 3;
        }
        return;
    case 2:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            func_80028220();
            AiScript_Init(D_801A9800);
            D_8009B20C[0] = 0;
            return;
        }
        flags = (u16)D_8009B20C[0];
        k = flags & 0xF;
        switch (k) {
        case 0:
            a = AiScript_Run();
            if (a == 0) {
                return;
            }
            if (a == 3) {
                D_8009B16C &= 0xEFFF;
                DisplayObject_ReleaseIfPresent(D_8009B1F8);
                D_8009B1F8 = 0;
                D_8009B174 = 0xA;
                SD_SEPlayFull(0x30);
                return;
            }
            D_8009B20C[0] = 1;
            return;
        case 1:
            if (!(flags & 0x8000)) {
                a = D_800EAE91[0];
                D_8009B20C[0] = flags | 0x8000;
                D_8009B160 = -1;
                D_8009B1D7 = 2;
                D_8009B1D6 = (a - 1) % 5;
                if (a >= 6) {
                    D_8009B1D7 = 3;
                }
            }
            if (func_8001D5B4((DuelFieldCursor *)side) != 0) {
                return;
            }
            if (D_800EAE88[0xA] == 0 && D_800EAE88[0xB] == 0) {
                card = GRID_CARD(side);
                f = card->flags;
                if (f & 0x8000) {
                    card->flags = f | 0x4000;
                    func_80017E3C(card->object);
                }
                D_8009B174 = 2;
                return;
            }
            D_8009B20C[0] = 3;
            if (D_800EAE88[0xB] != 0 && D_800EAE88[9] < 6) {
                D_8009B20C[0] = 2;
            }
            return;
        case 2:
            card = GRID_CARD(side);
            if (!(flags & 0x8000)) {
                f = card->flags;
                D_8009B20C[0] = flags | 0x8000;
                if (f & 0x8000) {
                    if (f & 0x800) {
                        card->flags = f | 0x4000;
                        func_80017E3C(card->object);
                        goto back_to_menu;
                    }
                    o = card->object;
                    W(o, 0x24) = (s32)func_8001D240;
                    B(o, 0x6C) = 0xF;
                    return;
                }
            back_to_menu:
                D_8009B174 = 2;
                return;
            }
            if (DisplayObject_FindAllocatedByTag(0xF) == 0) {
                card->flags |= 0x4000;
                func_80017E3C(card->object);
                D_8009B174 = k;
            }
            return;
        case 3:
            if (!(flags & 0x8000)) {
                D_8009B20C[0] = flags | 0x8000;
                card = GRID_CARD(side);
                if (card->flags & 0x800) {
                    D_8009B20C[0] = flags | 0xC000;
                    o = card->object;
                    W(o, 0x24) = (s32)func_8001D240;
                    B(o, 0x6C) = 0xF;
                }
                return;
            }
            if (!(flags & 0x4000) || DisplayObject_FindAllocatedByTag(0xF) == 0) {
                D_8009B21A = 1;
                goto attack;
            }
            return;
        }
        return;
    case 3:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0xC0;
            D_8009B21A = 0;
            if (D_8009B360[D_8009B1D5] >= 0) {
                D_8009B174 = 2;
                return;
            }
            side->status = 0;
        }
        if (DisplayObject_FindAllocatedByTag(0xF) != 0) {
            return;
        }
        if (func_80024060((DuelCursorStatus *)side) != 0) {
            D_8009B174 |= 0x40;
            return;
        }
        if (D_8009B174 & 0x40) {
            D_8009B174 &= 0xBF;
            if (side->row == 2
                && DuelCard_CanActThisTurn(GRID_CARD(side)) != 0) {
                D_8009B1F8 = func_8001D518(side->cursor_object);
            } else {
                DisplayObject_ReleaseIfPresent(D_8009B1F8);
                D_8009B1F8 = 0;
            }
        }
        if (D_8009B360[D_8009B1D5] < 0 && (gInput_wPad1Pressed & 0x800)) {
            D_8009B174 = 0xB;
            return;
        }
        if (Duel_CheckQuitInput() != 0) {
            return;
        }
        if (gInput_wPad1Pressed & 0xC) {
            r = side->row;
            card = &D_801A7AD8[D_800907D8[r * 5 + side->col + D_8009B1D5 * 20]];
            if (r == 2 && DuelCard_CanActThisTurn(card) != 0) {
                o = card->object;
                W(o, 0x24) = (s32)func_8001D240;
                B(o, 0x6C) = 0xF;
                SD_SEPlayFull(0xB);
            }
            return;
        }
        a = Duel_GetCardViewerRequestId(GRID_CARD(side));
        if (a != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = a;
            gDuel_bEffectState = 2;
            return;
        }
        if (gInput_wPad1Pressed & 0xC0) {
            if (side->row == 2) {
                if (D_8009B1C8->swords_turns_remaining != 0) {
                    SD_SEPlayFull(9);
                    return;
                }
                if (D_8009B16C & 0x1000) {
                    SD_SEPlayFull(9);
                    return;
                }
            }
            if (side->row < 2) {
                SD_SEPlayFull(9);
                return;
            }
        attack:
            i = side->row * 5 + side->col;
            card = &D_801A7AD8[D_800907D8[i + D_8009B1D5 * 20]];
            if ((W(card, 0x14) & 0xC8000000) == 0x80000000) {
                SD_SEPlayFull(7);
                DisplayObject_ReleaseIfPresent(D_8009B1F8);
                D_8009B1F8 = 0;
                a = side->row;
                if (a >= 2) {
                    side = SEL_REC3 + D_8009B1D5 * SIDE_SIZE;
                    if (a >= 3) {
                        D_8009B174 = 7;
                        if (CARD_KIND(card->card_id) == 0x17) {
                            x = 2;
                            y = 3;
                            side->field_11 = x;
                            side->row = x;
                            side->field_12 = y;
                            x = 4;
                            D_8009B174 = x;
                        }
                    } else {
                        x = 4;
                        y = 1;
                        D_8009B174 = x;
                        side->field_11 = y;
                        x = 2;
                        side->field_12 = x;
                        side->row = y;
                    }
                }
            } else {
                SD_SEPlayFull(9);
            }
        }
        return;
    case 4:
        side = SEL_REC3 + D_8009B1D5 * SIDE_SIZE;
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)side;
            side->field_18 = 0;
            side->field_0C = 0x74;
            func_800234E4((DuelFieldDisplaySource *)side);
            func_80022D94(0x10, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                D_800907AC[D_8009B1D5][side->field_18][side->row]);
            o = (u8 *)side->cursor_object;
            other = *(u8 **)((u8 *)SEL_REC3 + D_8009B1D5 * 0x70 - 0x18);
            DisplayObject_ResetVelocity((void *)o);
            S(o, 0x60) = 0x10;
            B(o, 0x6C) = 0;
            W(o, 0x2C) = W(o, 0x28);
            S(o, 0x36) = ((S(o, 0x28) - S(other, 0x28)) << 8) / 16;
            S(o, 0x3A) = ((S(o, 0x2A) - S(other, 0x2A)) << 8) / 16;
            D_8009B162 = 0x52;
            ((DisplayObject *)o)->position.word = ((DisplayObject *)other)->position.word;
            return;
        }
        if (D_8009B162 == 0) {
            func_8002348C((DuelFieldDisplaySource *)side);
            D_8009B174 = 6;
            if (D_8009B21A != 0) {
                D_8009B174 = 5;
            }
        }
        return;
    case 5:
        side = SEL_REC3 + D_8009B1D5 * SIDE_SIZE;
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B229 = 0;
            a = D_800EAE92[0];
            if (a < 0x38) {
                if (a >= 6) {
                    D_8009B1D6 = a - 6;
                    x = 3;
                } else {
                    D_8009B1D6 = a - 1;
                    x = 2;
                }
            } else {
                D_8009B1D6 = 0x3C - a;
                x = 1;
            }
            D_8009B1D7 = x;
        }
        if (func_8001D5B4((DuelFieldCursor *)side) != 0) {
            return;
        }
        goto commit;
    case 6:
        side = SEL_REC3 + D_8009B1D5 * SIDE_SIZE;
        if (D_8009B174 & 0x80) {
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)side;
        }
        if (D_8009B174 & 0x40) {
            if (D_8009B162 == 0) {
                DisplayObject_ReleaseIfPresent(side->cursor_object);
                side->cursor_object = 0;
                D_8009B174 = 3;
                D_8009B1B4 = (DuelCardPickCursor *)&((DuelSelectionSide *)D_800E9F10)[D_8009B1D5].records[2];
            }
            return;
        }
        if (func_80024060((DuelCursorStatus *)side) != 0) {
            return;
        }
        if (gInput_wPad1Pressed & 0x20) {
            func_80022D94(0x10, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                D_800907AC[D_8009B1D5][FIELD_CURSOR.field_18][FIELD_CURSOR.row]);
            o = (u8 *)side->cursor_object;
            other = (u8 *)FIELD_CURSOR.cursor_object;
            DisplayObject_ResetVelocity((void *)o);
            S(o, 0x60) = 0x10;
            B(o, 0x6C) = 0;
            S(o, 0x36) = ((S(other, 0x28) - S(o, 0x28)) << 8) / 16;
            S(o, 0x3A) = ((S(other, 0x2A) - S(o, 0x2A)) << 8) / 16;
            W(o, 0x2C) = W(other, 0x28);
            D_8009B162 = 0x58;
            D_8009B174 |= 0x40;
            return;
        }
        a = Duel_GetCardViewerRequestId(GRID_CARD(side));
        if (a != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = a;
            gDuel_bEffectState = 2;
            return;
        }
        if (!(gInput_wPad1Pressed & 0xC0)) {
            return;
        }
        D_8009B229 = 0;
        if (gInput_wPad1Pressed & 0x80) {
            D_8009B229 = 1;
        }
    commit:
        a = 0;
        if (side->row == 1) {
            i = 5;
            a = 1;
            for (n = 0; n < 5; n++, i++) {
                pick = &D_801A7AD8[D_800907D8[i + D_8009B1D5 * 20]];
                if (pick->flags & 0x8000) {
                    a = 0;
                    break;
                }
            }
        }
        i = side->row * 5 + side->col;
        pick = &D_801A7AD8[D_800907D8[i + D_8009B1D5 * 20]];
        if (a == 0 && !(pick->flags & 0x8000)) {
            SD_SEPlayFull(9);
            return;
        }
        SD_SEPlayFull(7);
        for (n = 6; n >= 0; n--) {
            D_800E9EF0[n] = 0;
        }
        i = FIELD_CURSOR.row * 5 + FIELD_CURSOR.col;
        pick = &D_801A7AD8[D_800907D8[i + D_8009B1D5 * 20]];
        D_8009B178 = H(pick, 0x16);
        D_8009B170 = H(pick, 0x12);
        o = func_80017F04(pick, S(pick, 8), S(pick, 0xA));
        DisplayObject_SetDepthOffset((DisplayObject *)o, -0xA);
        D_800E9EF0[0] = (DisplayObject *)o;
        d = pick->card_id;
        DuelCard_DeactivateRecord(pick);
        i = side->row * 5 + side->col;
        pick = &D_801A7AD8[D_800907D8[i + D_8009B1D5 * 20]];
        if (a == 0) {
            D_8009B17A = H(pick, 0x16);
            D_8009B172 = H(pick, 0x12);
            o = func_80017F04(pick, S(pick, 8), S(pick, 0xA));
            D_800E9EF0[1] = (DisplayObject *)o;
            D_8009B19C = B(o, 0x6A);
            DuelCard_DeactivateRecord(pick);
            if (CARD_KIND(d) == 0x17) {
                D_8009B174 = 8;
                return;
            }
        }
        gDuel_wSceneStateFlags = 9;
        return;
    case 7:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            i = side->row * 5 + side->col;
            card = &D_801A7AD8[D_800907D8[i + D_8009B1D5 * 20]];
            o = func_80017F04(card, S(card, 8), S(card, 0xA));
            card->flags &= 0x7FFF;
            if (card->flags & 0x1000) {
                B(o, 0x21) = 0x80;
                H(o, 8) |= 4;
            }
            S(o, 0x60) = 0xC;
            W(o, 0x24) = (s32)func_8001D3C4;
            S(o, 0x28) = 0x86;
            S(o, 0x2A) = 0x5A;
            S(o, 0x2C) = 0;
            B(o, 0x6C) = 1;
            D_8009B1CC = (DisplayObject *)o;
            co = side->cursor_object;
            co->flags &= 0xFFBF;
            if (D_8009B21A == 0) {
                D_8009B1BC = 0x202020;
                card = D_801A7AD8;
                for (n = 0; n < 0x1E; card++, n++) {
                    if (card->flags & 0x8000) {
                        o = card->object;
                        B(o, 0x6C) = 1;
                        S(o, 0x60) = 8;
                        W(o, 0x24) = (s32)func_8001D344;
                        a = 0x20;
                        if (card->flags & 0x4000) {
                            a = 0x10;
                        }
                        S(o, 0x2C) = a;
                        S(o, 0x2A) = a;
                        S(o, 0x28) = a;
                    }
                }
                D_8009B152 = 0;
                D_8009B174 |= 0x20;
            }
            return;
        }
        if (D_8009B21A != 0) {
            if (DisplayObject_FindAllocatedByTag(1) != 0) {
                return;
            }
            DuelCard_RemoveFromField(&D_801A7AD8[B(D_8009B1CC, 0x6A)]);
            D_8009B174 = 9;
            D_800E9EF0[0] = D_8009B1CC;
            return;
        }
        if (D_8009B174 & 0x20) {
            a = *(u8 *)&D_8009B300;
            flags = (u8)D_8009B1BC;
            if (a >= flags) {
                a -= 8;
                t = a < flags;
            } else {
                a += 8;
                t = flags < a;
            }
            if (t != 0) {
                a = flags;
            }
            t = (a << 16) | (a << 8) | a;
            D_8009B300 = t;
            if (t == D_8009B1BC && DisplayObject_FindAllocatedByTag(1) == 0) {
                D_8009B174 &= 0xDF;
            }
            return;
        }
        if (D_8009B174 & 0x40) {
            if (D_8009B174 & 0x10) {
                D_801A7AD8[B(D_8009B1CC, 0x6A)].flags |= 0x8000;
                DisplayObject_ReleaseIfPresent(D_8009B1CC);
                D_8009B174 = 3;
                co = side->cursor_object;
                co->flags |= 0x40;
                return;
            }
            DuelCard_RemoveFromField(&D_801A7AD8[B(D_8009B1CC, 0x6A)]);
            D_8009B174 = 9;
            D_800E9EF0[0] = D_8009B1CC;
            return;
        }
        a = func_80020988();
        if (a == 0) {
            return;
        }
        if (a != 1) {
            D_8009B174 |= 0x10;
            SD_SEPlayFull(8);
            o = (u8 *)D_8009B1CC;
            S(o, 0x60) = 0xC;
            pw = D_8015C424;
            W(o, 0x24) = (s32)func_8001D3C4;
            H(o, 0x28) = BANK(B(o, 0x6A), 0x36BC);
            H(o, 0x2A) = BANK(B(o, 0x6A), 0x36BE);
            S(o, 0x2C) = 0;
            if (D_801A7AD8[B(o, 0x6A)].flags & 0x1000) {
                S(o, 0x2C) = 0x80;
            }
            B(o, 0x6C) = 1;
        } else {
            SD_SEPlayFull(7);
        }
        D_8009B1BC = 0x808080;
        card = D_801A7AD8;
        D_8009B174 |= 0x60;
        for (n = 0; n < 0x1E; card++, n++) {
            if (card->flags & 0x8000) {
                o = card->object;
                B(o, 0x6C) = 1;
                S(o, 0x60) = 8;
                W(o, 0x24) = (s32)func_8001D344;
                a = 0x80;
                if (card->flags & 0x4000) {
                    a = 0x40;
                }
                S(o, 0x2C) = a;
                S(o, 0x2A) = a;
                S(o, 0x28) = a;
            }
        }
        return;
    case 8:
        if (!(D_8009B174 & 0x80)) {
            rec = SEL_REC3 + D_8009B1D5 * SIDE_SIZE;
            co = rec->cursor_object;
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)rec;
            DisplayObject_ReleaseIfPresent(co);
            D_8009B162 = 8;
            ((DuelSelectionRecord *)D_8009B1B4)->cursor_object = 0;
        }
        if (D_8009B162 != 0) {
            return;
        }
        if (!(D_8009B174 & 0x40)) {
            D_8009B174 |= 0x40;
            D_8009B162 = 0xC;
            D_8009B1B4 = (DuelCardPickCursor *)&((DuelSelectionSide *)D_800E9F10)[D_8009B1D5].records[2];
            return;
        }
        gDuel_wSceneStateFlags = 7;
        return;
    case 9:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B162 = 0xC;
        }
        if (D_8009B162 == 0) {
            gDuel_wSceneStateFlags = 6;
        }
        return;
    case 11:
        D_8009B16C &= 0xEFFF;
        DisplayObject_ReleaseIfPresent(D_8009B1F8);
        D_8009B1F8 = 0;
        D_8009B174 = 0xA;
        SD_SEPlayFull(0x30);
        return;
    case 10:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B162 = 0xC;
        }
        if (D_8009B162 == 0) {
            gDuel_wSceneStateFlags = 0xA;
        }
        return;
    }
}
