/*
 * Duel scene-state 4: hand navigation, inspection and card play. It builds
 * the hand selection display, runs the scripted selection path, lets the
 * player move through the hand, flip and pick cards, and hands the chosen
 * cards to the placement state. Current best under gcc_2_8_1_g8_split:
 * 1324 instructions against 1326 with opcode distance 16 (7 surplus,
 * 9 missing), with no hard register assignments and no inline assembly.
 *
 * Levers measured on this body:
 * - the pad words, the card viewer bytes, the effect state and the AI
 *   selection flag use their .data views, which is retail's %hi/%lo form;
 * - the repeat word is volatile, and the second pad test in each of states
 *   1, 3 and 4 re-reads the pressed word through a volatile access;
 * - each card id read materialises its 0x48000 bank offset separately;
 * - the AI selection byte is compared as an int, the slot index and the
 *   AI target column by their own signedness, and the fade and target
 *   tests are signed byte tests;
 * - the card confirmation block sits in state 4 and the AI path reaches it
 *   by goto;
 * - the shared restore, cursor-reset and ritual-pick bodies sit at their
 *   first use in retail's layout, and the other paths reach them by goto;
 * - func_80017F04 takes its three-argument prototype;
 * - when no hand slot carries the searched order, the collect loop sets its
 *   counter past the end and stops, as retail does;
 * - the scripted step compares the slot index through a signed byte view,
 *   and the downward step reads the index into dir before subtracting;
 * - both ritual-pick paths read D_8009B1C8 into base ahead of the two
 *   display-object clears and store the marker through it;
 * - the selection-reset loop starts its cursor at D_800E9EF0 and moves it
 *   to slot 6 in a separate statement;
 * - the first func_8001EC70 arm reads the object's 0x16 byte into f ahead
 *   of its stores;
 * - state 3 names its second display object's 0x20C argument at the top
 *   of the state.
 *
 * Residual: census addiu -3, andi -1, j -1, lb -1, lhu +1, lw -2, nop +3,
 * sb -1, sll +2, sra +1.
 * Retail saves one more callee-saved register and keeps the selection
 * table base in $s0, and several slot and column reads are signed where
 * this source reads them unsigned.
 */
#define GINPUT_PAD1_PRESSED_IN_DATA
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_HELD_IN_DATA
#define GDUEL_BEFFECTSTATE_IN_DATA
#define GDUEL_BCARDVIEWERYOFFSET_IN_DATA
#define GDUEL_WVIEWERCARDID_IN_DATA
#define D_800EAE90_IN_DATA
#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE88_VISIBLE
#define D_800EAE88_AS_BYTES
#define FUNC_80024088_WIDE_DIRECTION
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "../psyq/stdio.h"
#include "../game/duel_scene_state.h"
#include "../game/duel_side_state.h"
#include "../game/duel_selection_layout.h"
#include "../game/duel_hand.h"
#include "../game/duel_card.h"
#include "../game/duel_card_staging.h"
#include "../game/duel_card_display_state.h"
#include "../game/duel_card_record_lifecycle.h"
#include "../game/duel_card_pick_cursor.h"
#include "../game/duel_cursor_status.h"
#include "../game/duel_field_display_objects.h"
#include "../game/duel_grid.h"
#include "../game/duel_action_lock.h"
#include "../game/duel_card_viewer.h"
#include "../game/duel_effect.h"
#include "../game/display_object.h"
#include "../game/display_object_core.h"
#include "../game/display_object_config.h"
#include "../game/display_object_helpers.h"
#include "../game/display_object_interpolation.h"
#include "../game/display_object_motion.h"
#include "../game/display_object_work_slots.h"
#include "../game/func_8001B780.h"
#include "../game/func_8001B7AC.h"
#include "../game/func_80017034.h"
#include "../game/duel_check_quit_input.h"
#include "../game/input.h"
#include "../game/ai.h"
#include "../game/script_state.h"
#include "../game/sound.h"
#include "../unmatched.h"

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define SB(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define CARD_ID(i) (big = 0x48000, S(D_8015C424 + (i) * 0x1C + big, 0x36C0))

extern s16 D_8009B19E;
extern u8 D_8009B1E2;
extern u16 D_8009B1E4;
extern s8 D_8009B20A;
extern u8 D_800E9F2C[];
extern u8 D_800EAE89 __attribute__((section(".data")));

void func_8001B8B8(u8 *arg0);
void func_8001B938(u8 *p);
void func_8001BAF0(void);
void func_80028220(void);
void AiScript_Init(void *arg0);
s32 AiScript_Run(void);

void DuelScene_UpdateHandActions(void)
{
    u8 *base;
    u8 *side;
    DuelHandSlot *hand;
    DuelHandSlot *h;
    DisplayObject **slots;
    u8 *o;
    u8 *obj;
    DuelCardRecord *card;
    s32 n;
    s32 i;
    s32 j;
    s32 v;
    s32 kind;
    s32 flags;
    s32 found;
    s8 dir;
    u16 f;
    s32 id;
    s32 big;
    s32 k20c;

    base = D_800E9F10;
    side = (u8 *)(D_8009B1D5 * 0x70) + (s32)base;
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        n = 6;
        slots = D_800E9EF0;
        slots += 6;
        do {
            *slots = 0;
            n--;
            slots--;
        } while (n >= 0);
        D_8009B18C = 0;
        D_8009B188 = 0;
        D_8009B1B4 = (DuelCardPickCursor *)side;
        D_8009B162 = 0;
        B(side, 0xE) = 0;
        B(side, 0x15) = 0;
        o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
        func_80040468(o, 3, 0, 2, 0xB, 0x20C);
        H(o, 8) |= 0x28;
        func_80042918((DisplayObject *)o);
        func_800428EC(o, 0xA);
        W(side, 4) = (s32)o;
        func_8001B780((DuelHandStackState *)side);
        func_80023144((DuelFieldDisplaySource *)side, D_800907CC[(s8)B(side, 0xE) + D_8009B1D5 * 5]);
        D_8009B174 = 1;
        if (D_8009B360[D_8009B1D5] >= 0) {
            D_8009B174 = 2;
        }
        return;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        Duel_UpdateCardPickCursor(D_8009B1B4);
        if (D_8009B1D4 != 0) {
            return;
        }
        D_8009B1B4 = (DuelCardPickCursor *)((u8 *)(D_8009B1D5 * 0x70) + (s32)base);
        f = gDuel_wSceneStateFlags & 0xBFFF;
        goto store_flags;
    }

    hand = &D_800EA030[(s8)B(side, 0xE)];
    switch (D_8009B174 & 0xF) {
    case 2:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            D_8009B20C[0] = 0;
        }
        switch ((u16)D_8009B20C[0] & 0xF) {
        case 0:
            if (!((u16)D_8009B20C[0] & 0x8000)) {
                D_8009B20C[0] = (u16)D_8009B20C[0] | 0x8000;
                func_80028220();
                AiScript_Init(D_801A8000);
                return;
            }
            if (AiScript_Run() != 0) {
                *(s8 *)&D_8009B1E2 = -1;
                D_8009B19E = 0;
                if (D_800EAE89 != 0) {
                    D_8009B19E = 1;
                }
                D_8009B20C[0] = 1;
                func_8001BAF0();
                return;
            }
            return;
        case 1:
            if (!((u16)D_8009B20C[0] & 0x8000)) {
                D_8009B20C[0] = (u16)D_8009B20C[0] | 0x8000;
                D_8009B1E2++;
                id = D_800EAE88[(s8)D_8009B1E2];
                if (id == 0) {
                    func_8001B8B8(side);
                    D_8009B20C[0] = 2;
                    D_8009B19E = (u16)D_8009B19E & 1;
                    if (D_8009B19E != 0) {
                        D_8009B20C[0] = 3;
                    }
                    SD_SEPlayFull(7);
                    return;
                }
                if (id < 0xB) {
                    printf("Com Error SelCard\n");
                    return;
                }
                D_8009B20A = id - 0xB;
                D_8009B1E4 = 6;
            }
            D_8009B1E4--;
            if ((s16)D_8009B1E4 <= 0) {
                D_8009B1E4 = 6;
                if (SB(side, 0xE) != D_8009B20A) {
                    SD_SEPlayFull(6);
                    dir = B(side, 0xE);
                    dir = dir - 1;
                    if (D_8009B20A >= (s8)B(side, 0xE)) {
                        dir = B(side, 0xE) + 1;
                    }
                    B(side, 0xE) = dir;
                    func_8001B780((DuelHandStackState *)side);
                    func_80023144((DuelFieldDisplaySource *)side, D_800907CC[(s8)B(side, 0xE) + D_8009B1D5 * 5]);
                    return;
                }
                if ((u16)D_8009B19E & 1) {
                    func_8001B7AC((DuelHandStackState *)side);
                }
                D_8009B20C[0] = (u16)D_8009B20C[0] & 0x7FFF;
                return;
            }
            return;
        case 2:
            obj = hand->object;
            if (!((u16)D_8009B20C[0] & 0x8000)) {
                D_8009B20C[0] = (u16)D_8009B20C[0] | 0xC000;
                if (((gDuel_adwCardStats[CARD_ID(B(obj, 0x6A)) - 1] >> 0x1A) & 0x1F) < 0x14) {
                    if (D_800EAE90 == 0) {
                        D_8009B20C[0] = 3;
                        return;
                    }
                }
                f = B(obj, 0x16);
                S(obj, 0x28) = 0x86;
                S(obj, 0x2A) = 0x5A;
                S(obj, 0x2C) = 0x10;
                B(obj, 0x6C) = 1;
                W(obj, 0x24) = (s32)func_8001EC70;
                func_800428EC(obj, (s8)(f + 4));
                H(W(side, 4), 8) &= 0xFFBF;
                return;
            }
            if ((u16)D_8009B20C[0] & 0x4000) {
                if (func_80042B40(1) == 0) {
                    v = (u16)D_8009B20C[0] & 0x1000;
                    D_8009B20C[0] = (u16)D_8009B20C[0] & 0xBFFF;
                    if (v != 0) {
                        func_800428EC(obj, (s8)(B(obj, 0x16) - 4));
                        o = (u8 *)W(side, 4);
                        D_8009B20C[0] = 3;
                        H(o, 8) |= 0x40;
                        return;
                    }
                    H(obj, 0x2C) = H(obj, 0x36);
                    H(obj, 0x2E) = H(obj, 0x38);
                    if (D_800EAE90 != 0) {
                        SD_SEPlayFull(0xB);
                        D_8009B20C[0] = (u16)D_8009B20C[0] | 0x2000;
                        H(obj, 8) |= 4;
                        return;
                    }
                }
            } else if ((u16)D_8009B20C[0] & 0x2000) {
                v = B(obj, 0x21) + 8;
                B(obj, 0x21) = v;
                if ((s8)v < 0) {
                    B(obj, 0x21) = 0x80;
                    card = &D_801A7AD8[B(obj, 0x6A)];
                    D_8009B20C[0] = (u16)D_8009B20C[0] & 0xDFFF;
                    card->flags |= 0x1000;
                    return;
                }
            } else {
                kind = (gDuel_adwCardStats[CARD_ID(B(obj, 0x6A)) - 1] >> 0x1A) & 0x1F;
                if (kind >= 0x14 && B(obj, 0x21) == 0 && kind != 0x17 && kind != 0x15) {
                    D_800E9EF0[0] = (DisplayObject *)obj;
                    base = (u8 *)D_8009B1C8;
                    goto ritual_pick;
                }
                W(obj, 0x24) = (s32)func_8001EC70;
                D_8009B20C[0] = (u16)D_8009B20C[0] | 0x5000;
                H(obj, 0x2C) = 0x10;
                B(obj, 0x6C) = 1;
                W(obj, 0x28) = S(obj, 0x2C);
                return;
            }
            return;
        case 3:
            if (D_8009B162 == 0) {
                if (!((u16)D_8009B20C[0] & 0x8000)) {
                    D_8009B20C[0] = (u16)D_8009B20C[0] | 0xC000;
                    func_8001B938(side);
                    D_8009B20A = D_800EAE8E[0] - 6;
                    if (D_8009B20A < 0) {
                        D_8009B20A = D_800EAE8E[0] - 1;
                    }
                cursor_reset:
                    D_8009B160 = -1;
                    return;
                }
                if (func_80024088((void *)D_8009B1B4, D_8009B160) == 0) {
                    if (D_8009B1B4->col != D_8009B20A) {
                        D_8009B160 = 0;
                        if (D_8009B20A < D_8009B1B4->col) {
                            D_8009B160 = 2;
                        }
                        return;
                    }
                    goto confirm;
                }
                return;
            }
        }
        break;
    case 1:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            h = D_800EA030;
            for (n = 0; n < 5; n++, h++) {
                W(h->object, 0xC) = 0x808080;
            }
        }
        if (Duel_CheckQuitInput() != 0) {
            break;
        }
        if (gInput_wPad1Pressed & 0x10) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_bEffectState = 2;
            gDuel_wViewerCardID = CARD_ID(D_800907CC[(s8)B(side, 0xE) + D_8009B1D5 * 5]);
            return;
        }
        if (gInput_wPad1Repeat & 0xA000) {
            dir = (s8)B(side, 0xE);
            if (gInput_wPad1Repeat & 0x2000) {
                dir = dir + 1;
                if (dir < 5) {
                    goto move;
                }
            } else {
                dir = dir - 1;
                if (dir >= 0) {
                move:
                    B(side, 0xE) = dir;
                    func_8001B780((DuelHandStackState *)side);
                    func_80023144((DuelFieldDisplaySource *)side, D_800907CC[(s8)B(side, 0xE) + D_8009B1D5 * 5]);
                    SD_SEPlayFull(6);
                    return;
                }
            }
            break;
        }
        v = gInput_wPad1Pressed & 0xFFFF;
        if (v == 0x1000 && gInput_wPad1Held == v) {
            if (hand->active_09 == 0) {
                func_8001B7AC((DuelHandStackState *)side);
                return;
            }
        } else if ((gInput_wPad1Pressed & 0x20) || ((*(volatile u16 *)&gInput_wPad1Pressed & 0xFFFF) == 0x4000 && gInput_wPad1Held == 0x4000)) {
            id = hand->active_09;
            if (id != 0) {
                SD_SEPlayFull(0x2F);
                obj = hand->object;
                hand->active_09 = 0;
                H(obj, 0x32) += 4;
                func_8004036C(hand->child);
                hand->child = 0;
                h = D_800EA030;
                B(side, 0x15)--;
                for (n = 0; n < 5; n++, h++) {
                    v = h->pad_08;
                    if (v >= id) {
                        h->pad_08 = v - 1;
                        B(h->child, 0x5C) = (v - 2) * 0x10;
                    }
                }
                return;
            }
        } else {
            if (gInput_wPad1Held & 3) {
                D_8009B1D4 = 0;
                D_8009B1B4 = (DuelCardPickCursor *)((u8 *)(D_8009B1D5 * 0x70) + (s32)D_800E9F2C);
                gDuel_wSceneStateFlags |= 0x4000;
                return;
            }
            if ((gInput_wPad1Pressed & 0xC0) && B(side, 0x15) != 1) {
                SD_SEPlayFull(7);
                D_8009B174 = 4;
                func_8001B8B8(side);
                if (B(side, 0x15) == 0) {
                    D_8009B174 = 3;
                    return;
                }
            }
        }
        break;
    case 3:
        flags = D_8009B174;
        k20c = 0x20C;
        obj = hand->object;
        v = flags & 0x40;
        if (!(flags & 0x80)) {
            D_8009B174 = flags | 0xC0;
            S(obj, 0x28) = 0x86;
            S(obj, 0x2A) = 0x5A;
            H(obj, 0x2C) = H(obj, 0x30);
            H(obj, 0x2E) = H(obj, 0x32);
            DisplayObject_SavePosition((void *)obj);
            S(obj, 0x60) = 0;
            func_800428EC(obj, (s8)(B(obj, 0x16) + 4));
            H(W(side, 4), 8) &= 0xFFBF;
            v = D_8009B174 & 0x40;
        }
        if (v != 0) {
            DisplayObject_InterpolatePositionCosine((void *)obj, S(obj, 0x28), S(obj, 0x2A), S(obj, 0x60));
            S(obj, 0x60) = H(obj, 0x60) + 0xAA;
            if (S(obj, 0x60) >= 0x800) {
                id = D_8009B174 & 0xBF;
                v = D_8009B174 & 0x10;
                D_8009B174 = id;
                W(obj, 0x30) = W(obj, 0x28);
                if (v != 0) {
                    H(W(side, 4), 8) |= 0x40;
                    func_800428EC(obj, (s8)(B(obj, 0x16) - 4));
                    if (D_8009B174 & 0x20) {
                        D_8009B174 = 4;
                        return;
                    }
                    D_8009B174 = 1;
                    return;
                }
                kind = (gDuel_adwCardStats[(big = 0x48000, S((u8 *)W(D_8015C424 + B(obj, 0x6A) * 0x1C + big, 0x36B8), 0)) - 1] >> 0x1A) & 0x1F;
                if ((kind < 0x14 || kind == 0x15) && B(obj, 0x21) == 0) {
                    S(obj, 0x60) = 0x10;
                    D_8009B174 = id | 0x20;
                    SD_SEPlayFull(0xB);
                }
                o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
                func_800404CC(o, (s16)S(obj, 0x30) - 8, S(obj, 0x32) + 0x1E, 3, 1, 2, 0xB, 0x20C);
                func_80042918((DisplayObject *)o);
                func_800428EC(o, 0xA);
                H(o, 8) |= 0x28;
                D_8009B188 = (DisplayObject *)o;
                o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
                func_800404CC(o, (s16)S(obj, 0x30) + 0x3C, S(obj, 0x32) + 0x1E, 3, 1, 0, 0xB, k20c);
                func_80042918((DisplayObject *)o);
                func_800428EC(o, 0xA);
                H(o, 8) |= 0x28;
                D_8009B18C = (DisplayObject *)o;
                return;
            }
        } else if (flags & 0x20) {
            v = B(obj, 0x21) + B(obj, 0x60);
            H(obj, 8) |= 4;
            B(obj, 0x21) = v;
            if (!(v & 0x7F)) {
                if (!(v & 0xFF)) {
                    H(obj, 8) &= 0xFFFB;
                }
                v = D_8009B174 & 0x10;
                D_8009B174 &= 0xDF;
                if (v != 0) {
                restore:
                    func_8004036C(D_8009B188);
                    func_8004036C(D_8009B18C);
                    D_8009B18C = 0;
                    D_8009B188 = 0;
                    W(obj, 0x28) = W(obj, 0x2C);
                    DisplayObject_SavePosition((void *)obj);
                    S(obj, 0x60) = 0;
                    D_8009B174 |= 0x50;
                    return;
                }
            }
        } else {
            if (gInput_wPad1Repeat & 0xA000) {
                S(obj, 0x60) = 0x10;
                D_8009B174 = flags | 0x20;
                if (gInput_wPad1Repeat & 0x2000) {
                    S(obj, 0x60) = -0x10;
                }
                SD_SEPlayFull(0xB);
                return;
            }
            if (gInput_wPad1Pressed & 0x20) {
                SD_SEPlayFull(8);
                if (B(obj, 0x21) != 0) {
                    S(obj, 0x60) = 0x10;
                    D_8009B174 |= 0x30;
                    return;
                }
                goto restore;
            }
            if (*(volatile u16 *)&gInput_wPad1Pressed & 0xC0) {
                SD_SEPlayFull(7);
                D_801A7AD8[B(obj, 0x6A)].flags &= 0xEFFF;
                if (B(obj, 0x21) != 0) {
                    D_801A7AD8[B(obj, 0x6A)].flags |= 0x1000;
                }
                kind = (gDuel_adwCardStats[CARD_ID(B(obj, 0x6A)) - 1] >> 0x1A) & 0x1F;
                if (kind >= 0x14 && B(obj, 0x21) == 0 && kind != 0x17 && kind != 0x15) {
                    func_8004036C(D_8009B188);
                    func_8004036C(D_8009B18C);
                    D_800E9EF0[0] = (DisplayObject *)obj;
                    base = (u8 *)D_8009B1C8;
                    D_8009B18C = 0;
                    D_8009B188 = 0;
                ritual_pick:
                    *(s8 *)(base + 0x1A + (s8)B(side, 0xE)) = -1;
                    D_8009B174 = 5;
                    return;
                }
                D_8009B174 |= 0x70;
                goto restore;
            }
        }
        break;
    case 4:
        flags = D_8009B174;
        v = flags & 0x40;
        if (!(flags & 0x80)) {
            D_8009B174 = flags | 0x80;
            D_8009B210 = 0;
            func_8001B938(side);
            D_8009B174 |= 0x40;
            v = D_8009B174 & 0x40;
        }
        if (v != 0) {
            if (D_8009B162 == 0) {
                D_8009B174 = flags & 0xBF;
                if (flags & 0x10) {
                    D_8009B174 = 1;
                    D_8009B1B4 = (DuelCardPickCursor *)((u8 *)(D_8009B1D5 * 0x70) + (s32)D_800E9F10);
                    if (B(side, 0x15) == 0) {
                        D_8009B174 = 3;
                        return;
                    }
                }
            }
        } else if (func_80024060((void *)D_8009B1B4) == 0) {
            o = (u8 *)(D_8009B1D5 * 0x70) + (s32)D_800E9F10;
            dir = B(o, 0x48);
            v = func_80017034(&D_801A7AD8[D_800907D8[D_8009B1D5][dir * 5 + (s8)B(o, 0x47)]]);
            if (v != 0) {
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_wViewerCardID = v;
                gDuel_bEffectState = 2;
                return;
            }
            if (gInput_wPad1Pressed & 0x20) {
                D_8009B162 = 0xC;
                D_8009B174 |= 0x50;
                SD_SEPlayFull(8);
                return;
            }
            if (*(volatile u16 *)&gInput_wPad1Pressed & 0xC0) {
                confirm:
                    SD_SEPlayFull(7);
                    n = 6;
                    slots = &D_800E9EF0[6];
                    do {
                        *slots = 0;
                        n--;
                        slots--;
                    } while (n >= 0);
                    id = D_800907D8[D_8009B1D5][D_8009B1B4->row * 5 + D_8009B1B4->col];
                    card = &D_801A7AD8[id];
                    D_8009B19C = id;
                    i = 0;
                    if (card->flags & 0x8000) {
                        obj = card->object;
                        i = 1;
                        D_800E9EF0[0] = (DisplayObject *)func_80017F04(card, S(card, 8), S(card, 0xA));
                        func_80024914(&D_801A7AD8[B(obj, 0x6A)]);
                        goto tally;
                    }
                    j = 1;
                    if (B(side, 0x15) == 0) {
                        if (B(hand->object, 0x68) != 0x17 || D_8009B1B4->row != 2) {
                        tally:
                            j = 1;
                            if (B(side, 0x15) == 0) {
                                hand->active_09 = 1;
                                if (B(hand->object, 0x21) != 0) {
                                    D_8009B1C8->rank.face_down_plays++;
                                    j = 1;
                                }
                            }
                            goto collect;
                        }
                    } else {
                    collect:
                        do {
                            h = D_800EA030;
                            slots = &D_800E9EF0[i];
                            for (n = 0; n < 5; n++, h++) {
                                if (h->active_09 == j) {
                                    *(s8 *)((u8 *)D_8009B1C8 + 0x1A + n) = -1;
                                    i++;
                                    *slots = (DisplayObject *)h->object;
                                    break;
                                }
                            }
                            if (n == 5) {
                                j = 8;
                            }
                            j++;
                        } while (j < 6);
                        D_8009B174 = 6;
                        return;
                    }
            }
        }
        break;
    case 6:
        if (!(D_8009B174 & 0x80)) {
            D_8009B162 = 0xC;
            D_8009B174 |= 0x80;
            D_8009B1B4 = (DuelCardPickCursor *)((u8 *)(D_8009B1D5 * 0x70) + (s32)D_800E9F48);
        }
        f = 7;
        if (D_8009B162 == 0) {
        store_flags:
            gDuel_wSceneStateFlags = f;
        }
        break;
    case 5:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            hand->object = 0;
            D_8009B162 = 8;
            D_8009B1B4 = (DuelCardPickCursor *)((u8 *)(D_8009B1D5 * 0x70) + (s32)D_800E9F10);
        }
        f = 6;
        if (D_8009B162 == 0) {
            goto store_flags;
        }
        break;
    }
}
