/*
 * Duel scene-state 5: human field navigation, inspection, attack selection,
 * position changes, sacrifices and battle transitions. It runs the scripted
 * field path, moves the field cursor, opens the card viewer, picks the
 * attacker and its target, commits the fusion or sacrifice selection and
 * fades the field for the battle state. Current best under
 * gcc_2_8_1_g8_split_comm: 1405 instructions against 1408 with opcode distance
 * 15 (6 surplus, 9 missing), with no hard register assignments and no
 * inline assembly.
 *
 * Levers measured on this body:
 * - the switch has an eleventh case, laid out between cases 9 and 10, which
 *   the scripted path reaches by goto; the scripted path's own four-way
 *   dispatch is then a comparison tree, as in retail;
 * - the card viewer block and the commit block sit in case 6, the attack
 *   block in case 3, the return-to-menu block in case 3 and the scene flag
 *   store in case 10, each reached from the other cases by goto;
 * - func_80017F04 takes its three-argument prototype;
 * - the grid row and column bytes are read signed, the two package-stage
 *   bytes and the func_80017034 result are held in an int;
 * - state 8 reads the cursor object before its stores;
 * - the fade step reads the low byte of D_8009B300 through its .data view;
 * - D_8009B170/172/178/17A and D_8009B19C are tentative definitions, as in
 *   fade_update.c, which supplies retail's load-delay nops in front of their
 *   gp-relative stores, and each second store of a pair sits in its own
 *   do { } while (0) so its load is not scheduled ahead of the first store;
 *   the _comm profile keeps those definitions common, where plain g8_split
 *   would allocate them in .sbss;
 * - the D_800907AC index bytes at +0x10 and +0x48 are read signed;
 * - the two BANK reads that set the object's 0x28 and 0x2A fields re-read
 *   the card index from the object, and the second store sits in its own
 *   do { } while (0);
 * - the 0x14 test on the current side's D_800E9FF0 record goes through a
 *   named pointer;
 * - one of the two identical flag-and-refresh tails sets the flag with |=,
 *   so the two tails are no longer identical.
 *
 * Residual: census addiu -1, addu +1, beqz +2, bnez -2, j +1, jal -1,
 * lb -1, lui +1, sh -1, sll -2, slti +1, subu -1. Retail keeps separate func_80017E3C calls for the
 * field-cursor and card-pick paths where this source shares one.
 */
#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE88_VISIBLE
#define D_800EAE88_AS_BYTES
#define D_800EAE92_AS_ARRAY
#define DUEL_FIELD_GRID_2D
#define GINPUT_PAD1_PRESSED_IN_DATA
#define GDUEL_BEFFECTSTATE_IN_DATA
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "../types.h"
#include "../game/duel_scene_state.h"
#include "../game/duel_side_state.h"
#include "../game/duel_selection_layout.h"
#include "../game/duel_card.h"
#include "../game/duel_card_staging.h"
#include "../game/duel_card_display_state.h"
#include "../game/duel_card_record_lifecycle.h"
#include "../game/duel_cursor_status.h"
#include "../game/duel_field_display_objects.h"
#include "../game/duel_grid.h"
#include "../game/duel_action_lock.h"
#include "../game/duel_card_viewer.h"
#include "../game/duel_effect.h"
#include "../game/duel_check_quit_input.h"
#include "../game/duel_side_view_angles.h"
#include "../game/ai.h"
#include "../game/duel_load_package_stage.h"
#include "../game/display_object.h"
#include "../game/display_object_core.h"
#include "../game/display_object_helpers.h"
#include "../game/display_object_work_slots.h"
#include "../game/func_80017034.h"
#include "../game/func_80022D94.h"
#include "../game/input.h"
#include "../game/sorted_entry.h"
#include "../game/sound.h"
#include "../unmatched.h"

extern u8 gDuel_bCardViewerYOffset __attribute__((section(".data")));
extern u16 gDuel_wViewerCardID __attribute__((section(".data")));
extern u32 D_8009B300 __attribute__((section(".data")));

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define SB(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define GRID_CARD(s) (&D_801A7AD8[D_800907D8[D_8009B1D5][SB(s, 0x10) * 5 + SB(s, 0xF)]])
#define BANK(i, o) (big = 0x48000, H(D_8015C424 + (i) * 0x1C + big, (o)))

u16 D_8009B170;
u16 D_8009B172;
u16 D_8009B178;
u16 D_8009B17A;
u8 D_8009B19C;
extern s32 D_8009B1BC;
extern u8 D_8009B21A;
extern s8 D_8009B229;
extern u8 D_800EAE91[];
int func_8001700C(DuelCardRecord *object);
void func_8001D240(DisplayObject *o);
void func_8001D344(DisplayObject *object);
void func_8001D3C4(DisplayObject *o);
DisplayObject *func_8001D518(DisplayObject *source);
s32 func_8001D5B4(DuelFieldCursor *cursor);
s32 func_80020988(void);
void func_800234E4(DuelFieldDisplaySource *source);
void func_80028220(void);

void DuelScene_UpdateFieldActions(void)
{
    u8 *side;
    u8 *s;
    u8 *o;
    u8 *other;
    DuelCardRecord *card;
    DuelCardRecord *cards;
    s32 a;
    s32 k;
    s32 n;
    s32 i;
    s32 v;
    u8 *pw;
    s32 d;
    s32 big;
    s8 row;
    u16 f;
    u16 flags;

    side = D_800E9F48 + D_8009B1D5 * 0x70;
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        if (S(D_800E9FF0, 0x14) == 0 || S(D_800E9FF0, 0x34) == 0) {
            gDuel_bWinnerSide = D_8009B1D5;
            pw = (u8 *)D_800E9FF0 + (D_8009B1D5 << 5);
            if (S(pw, 0x14) == 0) {
                gDuel_bWinnerSide = D_8009B1D5 ^ 1;
            }
            B(&D_800E9FF0[gDuel_bWinnerSide], 0) = 2;
            f = 0xC;
            goto store_flags;
        }
        D_8009B1F8 = 0;
        D_8009B21A = 0;
        D_8009B1B4 = (DuelCardPickCursor *)side;
        B(side, 0x12) = 4;
        S(side, 0xC) = 0xAE;
        B(side, 0x18) = 1;
        B(side, 0x13) = 0;
        B(side, 0x11) = 0;
        D_8009B162 = 3;
        D_8009B174 = 1;
    }

    switch (D_8009B174 & 0xF) {
    case 1:
        if (D_8009B162 == 0) {
            D_8009B174 = 3;
            return;
        }
    default:
        return;
    case 2:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            func_80028220();
            AiScript_Init(D_801A9800);
            D_8009B20C[0] = 0;
            return;
        }
        flags = D_8009B20C[0];
        k = flags & 0xF;
        switch (k) {
        case 0:
            v = AiScript_Run();
            if (v != 0) {
                if (v == 3) {
                    goto quit_ai;
                }
                D_8009B20C[0] = 1;
                return;
            }
            break;
        case 1:
            if (!(flags & 0x8000)) {
                D_8009B20C[0] = flags | 0x8000;
                D_8009B160 = -1;
                D_8009B1D7 = 2;
                a = D_800EAE91[0];
                D_8009B1D6 = (a - 1) % 5;
                if (a >= 6) {
                    D_8009B1D7 = 3;
                }
            }
            if (func_8001D5B4((DuelFieldCursor *)side) == 0) {
                if (D_800EAE88[0xA] == 0 && D_800EAE88[0xB] == 0) {
                    card = GRID_CARD(side);
                    f = card->flags;
                    if (f & 0x8000) {
                        card->flags = f | 0x4000;
                        func_80017E3C(card->object);
                    }
                    goto back_to_menu;
                }
                D_8009B20C[0] = 3;
                if (D_800EAE88[0xB] != 0 && D_800EAE88[9] < 6) {
                    D_8009B20C[0] = 2;
                    return;
                }
            }
            break;
        case 2:
            card = GRID_CARD(side);
            if (!(flags & 0x8000)) {
                f = card->flags;
                D_8009B20C[0] = flags | 0x8000;
                if (f & 0x8000) {
                    if (f & 0x800) {
                        card->flags |= 0x4000;
                        func_80017E3C(card->object);
                        goto back_to_menu;
                    }
                pick_object:
                    o = card->object;
                    W(o, 0x24) = (s32)func_8001D240;
                    B(o, 0x6C) = 0xF;
                    return;
                }
                goto back_to_menu;
            }
            if (DisplayObject_FindAllocatedByTag(0xF) == 0) {
                card->flags |= 0x4000;
                func_80017E3C(card->object);
                D_8009B174 = k;
                return;
            }
            break;
        case 3:
            if (!(flags & 0x8000)) {
                D_8009B20C[0] = flags | 0x8000;
                card = GRID_CARD(side);
                if (card->flags & 0x800) {
                    D_8009B20C[0] = flags | 0xC000;
                    goto pick_object;
                }
            } else if (!(flags & 0x4000) || DisplayObject_FindAllocatedByTag(0xF) == 0) {
                D_8009B21A = 1;
                goto attack;
            }
            break;
        }
        break;
    case 3:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0xC0;
            D_8009B21A = 0;
            if (D_8009B360[D_8009B1D5] >= 0) {
            back_to_menu:
                D_8009B174 = 2;
                return;
            }
            B(side, 0x19) = 0;
        }
        if (DisplayObject_FindAllocatedByTag(0xF) != 0) {
            break;
        }
        if (func_80024060((DuelCursorStatus *)side) != 0) {
            v = D_8009B174 | 0x40;
        set_state:
            D_8009B174 = v;
            return;
        }
        if (D_8009B174 & 0x40) {
            D_8009B174 &= 0xBF;
            if (SB(side, 0x10) == 2
                && func_8001700C(&D_801A7AD8[D_800907D8[D_8009B1D5][SB(side, 0xF) + 10]]) != 0) {
                D_8009B1F8 = func_8001D518((DisplayObject *)W(side, 4));
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
            break;
        }
        if (gInput_wPad1Pressed & 0xC) {
            row = SB(side, 0x10);
            card = &D_801A7AD8[D_800907D8[D_8009B1D5][row * 5 + (s8)B(side, 0xF)]];
            if (row == 2 && func_8001700C(card) != 0) {
                o = card->object;
                W(o, 0x24) = (s32)func_8001D240;
                B(o, 0x6C) = 0xF;
                SD_SEPlayFull(0xB);
                return;
            }
            break;
        }
        a = func_80017034(GRID_CARD(side));
        if (a == 0) {
            if (gInput_wPad1Pressed & 0xC0) {
                if (SB(side, 0x10) == 2) {
                    if (SB(D_8009B1C8, 0x19) != 0) {
                        goto buzz;
                    }
                    if (D_8009B16C & 0x1000) {
                        goto buzz;
                    }
                }
                if (SB(side, 0x10) < 2) {
                    goto buzz;
                }
        attack:
            card = GRID_CARD(side);
            if ((W(card, 0x14) & 0xC8000000) == 0x80000000) {
                SD_SEPlayFull(7);
                DisplayObject_ReleaseIfPresent(D_8009B1F8);
                D_8009B1F8 = 0;
                row = SB(side, 0x10);
                if (row >= 2) {
                    s = D_800E9F64 + D_8009B1D5 * 0x70;
                    if (row >= 3) {
                        D_8009B174 = 7;
                        if (((gDuel_adwCardStats[card->card_id - 1] >> 0x1A) & 0x1F) == 0x17) {
                            B(s, 0x11) = 2;
                            B(s, 0x10) = 2;
                            B(s, 0x12) = 3;
                            D_8009B174 = 4;
                            return;
                        }
                    } else {
                        D_8009B174 = 4;
                        B(s, 0x11) = 1;
                        B(s, 0x12) = 2;
                        B(s, 0x10) = 1;
                        return;
                    }
                }
            } else {
                goto buzz;
            }
            }
            break;
        }
        goto view_card;
    case 4:
        s = D_800E9F64 + D_8009B1D5 * 0x70;
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)s;
            B(s, 0x18) = 0;
            S(s, 0xC) = 0x74;
            func_800234E4((DuelFieldDisplaySource *)s);
            func_80022D94(0x10, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                D_800907AC[D_8009B1D5][B(s, 0x18)][SB(s, 0x10)]);
            o = (u8 *)W(s, 4);
            other = (u8 *)W(D_800E9F64 + D_8009B1D5 * 0x70 - 0x18, 0);
            DisplayObject_ResetVelocity((void *)o);
            S(o, 0x60) = 0x10;
            B(o, 0x6C) = 0;
            W(o, 0x2C) = W(o, 0x28);
            d = S(o, 0x28) - S(other, 0x28);
            S(o, 0x36) = (d << 8) / 16;
            d = S(o, 0x2A) - S(other, 0x2A);
            S(o, 0x3A) = (d << 8) / 16;
            D_8009B162 = 0x52;
            W(o, 0x28) = W(other, 0x28);
            return;
        }
        if (D_8009B162 == 0) {
            func_8002348C((DuelFieldDisplaySource *)s);
            D_8009B174 = 6;
            if (D_8009B21A != 0) {
                D_8009B174 = 5;
                return;
            }
        }
        break;
    case 5:
        s = D_800E9F64 + D_8009B1D5 * 0x70;
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B229 = 0;
            a = D_800EAE92[0];
            if (a < 0x38) {
                if (a >= 6) {
                    D_8009B1D6 = a - 6;
                    row = 3;
                } else {
                    D_8009B1D6 = a - 1;
                    row = 2;
                }
            } else {
                D_8009B1D6 = 0x3C - a;
                row = 1;
            }
            D_8009B1D7 = row;
        }
        a = 0;
        if (func_8001D5B4((DuelFieldCursor *)s) != 0) {
            break;
        }
        goto commit;
    case 6:
        s = D_800E9F64 + D_8009B1D5 * 0x70;
        if (D_8009B174 & 0x80) {
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)s;
        }
        if (D_8009B174 & 0x40) {
            if (D_8009B162 == 0) {
                DisplayObject_ReleaseIfPresent((void *)W(s, 4));
                W(s, 4) = 0;
                D_8009B174 = 3;
                v = D_8009B1D5 * 0x70;
                goto set_cursor;
            }
            break;
        }
        if (func_80024060((DuelCursorStatus *)s) != 0) {
            break;
        }
        if (gInput_wPad1Pressed & 0x20) {
            other = D_800E9F10 + D_8009B1D5 * 0x70;
            func_80022D94(0x10, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                D_800907AC[D_8009B1D5][B(other, 0x50)][SB(other, 0x48)]);
            o = (u8 *)W(s, 4);
            other = (u8 *)W(D_800E9F10 + D_8009B1D5 * 0x70, 0x3C);
            DisplayObject_ResetVelocity((void *)o);
            S(o, 0x60) = 0x10;
            B(o, 0x6C) = 0;
            d = S(other, 0x28) - S(o, 0x28);
            S(o, 0x36) = (d << 8) / 16;
            d = S(other, 0x2A) - S(o, 0x2A);
            S(o, 0x3A) = (d << 8) / 16;
            W(o, 0x2C) = W(other, 0x28);
            D_8009B162 = 0x58;
            v = D_8009B174 | 0x40;
            goto set_state;
        }
        a = func_80017034(GRID_CARD(s));
        if (a != 0) {
        view_card:
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = a;
            gDuel_bEffectState = 2;
            return;
        }
        if (!(gInput_wPad1Pressed & 0xC0)) {
            break;
        }
        D_8009B229 = 0;
        if (gInput_wPad1Pressed & 0x80) {
            D_8009B229 = 1;
        }
        a = 0;
    commit:
        if (SB(s, 0x10) == 1) {
            a = 1;
            for (i = 5, n = 0; n < 5; n++, i++) {
                if (D_801A7AD8[D_800907D8[D_8009B1D5][i]].flags & 0x8000) {
                    a = 0;
                    break;
                }
            }
        }
        if (a == 0 && !(GRID_CARD(s)->flags & 0x8000)) {
        buzz:
            SD_SEPlayFull(9);
            return;
        }
        SD_SEPlayFull(7);
        n = 6;
        {
            DisplayObject **slot = &D_800E9EF0[6];
            do {
                *slot = 0;
                n--;
                slot--;
            } while (n >= 0);
        }
        other = D_800E9F10 + D_8009B1D5 * 0x70;
        card = &D_801A7AD8[D_800907D8[D_8009B1D5][SB(other, 0x48) * 5 + SB(other, 0x47)]];
        D_8009B178 = card->flags;
        do {
            D_8009B170 = card->stat_modifier;
        } while (0);
        o = func_80017F04(card, S(card, 8), S(card, 0xA));
        DisplayObject_SetDepthOffset((DisplayObject *)o, -0xA);
        D_800E9EF0[0] = (DisplayObject *)o;
        d = S(card, 0xC);
        func_80024914(card);
        card = GRID_CARD(s);
        if (a == 0) {
            D_8009B17A = card->flags;
            do {
                D_8009B172 = card->stat_modifier;
            } while (0);
            o = func_80017F04(card, S(card, 8), S(card, 0xA));
            D_800E9EF0[1] = (DisplayObject *)o;
            D_8009B19C = B(o, 0x6A);
            func_80024914(card);
            f = 9;
            if (((gDuel_adwCardStats[d - 1] >> 0x1A) & 0x1F) == 0x17) {
                D_8009B174 = 8;
                return;
            }
            goto store_flags;
        }
        f = 9;
        goto store_flags;
    case 7:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            card = GRID_CARD(side);
            f = card->flags;
            o = func_80017F04(card, S(card, 8), S(card, 0xA));
            card->flags = f & 0x7FFF;
            if (f & 0x1000) {
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
            H(W(side, 4), 8) &= 0xFFBF;
            if (D_8009B21A == 0) {
                D_8009B1BC = 0x202020;
                cards = D_801A7AD8;
                for (n = 0; n < 0x1E; n++, cards++) {
                    if (cards->flags & 0x8000) {
                        o = cards->object;
                        B(o, 0x6C) = 1;
                        S(o, 0x60) = 8;
                        W(o, 0x24) = (s32)func_8001D344;
                        v = 0x20;
                        if (cards->flags & 0x4000) {
                            v = 0x10;
                        }
                        S(o, 0x2C) = v;
                        S(o, 0x2A) = v;
                        S(o, 0x28) = v;
                    }
                }
                D_8009B152 = 0;
                D_8009B174 |= 0x20;
                return;
            }
        } else {
            if (D_8009B21A != 0) {
                if (DisplayObject_FindAllocatedByTag(1) != 0) {
                    return;
                }
                goto finish;
            }
            if (D_8009B174 & 0x20) {
                a = *(u8 *)&D_8009B300;
                k = (u8)D_8009B1BC;
                if (a >= k) {
                    a -= 8;
                    v = a < k;
                } else {
                    a += 8;
                    v = k < a;
                }
                if (v != 0) {
                    a = k;
                }
                v = (a << 16) | (a << 8) | a;
                D_8009B300 = v;
                if (v == D_8009B1BC && DisplayObject_FindAllocatedByTag(1) == 0) {
                    D_8009B174 &= 0xDF;
                    return;
                }
            } else if (D_8009B174 & 0x40) {
                if (D_8009B174 & 0x10) {
                    D_801A7AD8[B(D_8009B1CC, 0x6A)].flags |= 0x8000;
                    DisplayObject_ReleaseIfPresent(D_8009B1CC);
                    D_8009B174 = 3;
                    H(W(side, 4), 8) |= 0x40;
                    return;
                }
            finish:
                func_80024954(&D_801A7AD8[B(D_8009B1CC, 0x6A)]);
                D_8009B174 = 9;
                D_800E9EF0[0] = D_8009B1CC;
                return;
            } else {
                v = func_80020988();
                if (v != 0) {
                    if (v != 1) {
                        D_8009B174 |= 0x10;
                        SD_SEPlayFull(8);
                        o = (u8 *)D_8009B1CC;
                        S(o, 0x60) = 0xC;
                        W(o, 0x24) = (s32)func_8001D3C4;
                        H(o, 0x28) = BANK(B(o, 0x6A), 0x36BC);
                        S(o, 0x2C) = 0;
                        do {
                            H(o, 0x2A) = BANK(B(o, 0x6A), 0x36BE);
                        } while (0);
                        if (D_801A7AD8[B(o, 0x6A)].flags & 0x1000) {
                            S(o, 0x2C) = 0x80;
                        }
                        B(o, 0x6C) = 1;
                    } else {
                        SD_SEPlayFull(7);
                    }
                    cards = D_801A7AD8;
                    D_8009B1BC = 0x808080;
                    D_8009B174 |= 0x60;
                    for (n = 0; n < 0x1E; n++, cards++) {
                        if (cards->flags & 0x8000) {
                            o = cards->object;
                            B(o, 0x6C) = 1;
                            S(o, 0x60) = 8;
                            W(o, 0x24) = (s32)func_8001D344;
                            v = 0x80;
                            if (cards->flags & 0x4000) {
                                v = 0x40;
                            }
                            S(o, 0x2C) = v;
                            S(o, 0x2A) = v;
                            S(o, 0x28) = v;
                        }
                    }
                    return;
                }
            }
        }
        break;
    case 8:
        if (!(D_8009B174 & 0x80)) {
            s = D_800E9F64 + D_8009B1D5 * 0x70;
            o = (u8 *)W(s, 4);
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)s;
            DisplayObject_ReleaseIfPresent(o);
            D_8009B162 = 8;
            W(D_8009B1B4, 4) = 0;
        }
        if (D_8009B162 == 0) {
            f = 7;
            if (!(D_8009B174 & 0x40)) {
                D_8009B174 |= 0x40;
                D_8009B162 = 0xC;
                v = D_8009B1D5 * 0x70;
            set_cursor:
                D_8009B1B4 = (DuelCardPickCursor *)&D_800E9F48[v];
                return;
            }
            goto store_flags;
        }
        break;
    case 9:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B162 = 0xC;
        }
        f = 6;
        if (D_8009B162 == 0) {
            goto store_flags;
        }
        break;
    case 11:
    quit_ai:
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
        f = 0xA;
        if (D_8009B162 != 0) {
            break;
        }
    store_flags:
        gDuel_wSceneStateFlags = f;
        break;
    }
}
