/*
 * Duel scene-state 9: sequences the attacker and defender presentation,
 * damage, and the post-battle state transitions. Current best under
 * gcc_2_8_1_g8_split_comm: 1244 instructions against 1246, opcode distance
 * 10 (4 surplus, 6 missing), with no hard register assignments and no inline
 * assembly.
 *
 * Levers measured on this body, in the order they were found:
 * - the entry block holds its repeated constants (1, 0x10, 0x18, 0x5C,
 *   func_8001ED20, 0x48000) in locals and reuses one object pointer for
 *   D_8009B214, D_8009B21C and both slots; retail keeps them in s0-s5;
 * - the damage record is named once per block (`lp`), and the sign tests
 *   are `(s16)life < 0` rather than a 0x8000 mask;
 * - case 1 reads D_8009B0F4/D_8009B134 through their _abs names and case 5
 *   reads D_8009B374 from .data, which is the %hi/%lo form retail uses;
 * - case 1 names the 0x48000 split offset, case 2 re-reads D_8009B174
 *   after its entry block, case 8 names the flags byte, the signed
 *   slot index and the D_8009B1B0 base, and case 11 uses one record name
 *   with halfword cast stores so D_8009B17A is reloaded;
 * - D_8009B208 is a tentative definition, which supplies retail's
 *   load-delay nops before its gp-relative stores; the _comm profile
 *   keeps it common, where plain g8_split would allocate it in .sbss;
 * - the two fade blocks name the side object before storing through it,
 *   case 4's else branch reads D_8009B174 into flags, and the first
 *   func_80019BA0 call reads byte 0x21 once.
 *
 * Residual: census addiu +1, andi -2, beqz -1, bnez +1, lhu -2, lui -1,
 * sll +2. Case 11 still loads D_8009B178 and D_8009B170 together and keeps
 * one D_8009B17A read, and case 4 reads the D_800EA0E8 base once where
 * retail materialises it on both arms.
 */
#define D_8009B369_IN_DATA
#define D_8009B374_IN_DATA
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../types.h"
#include "../psyq/rand.h"
#include "../game/duel_scene_state.h"
#include "../game/file_transfer.h"
#include "../game/duel_side_state.h"
#include "../game/duel_action_lock.h"
#include "../game/func_800179F4.h"
#include "../game/duel_card_staging.h"
#include "../game/duel_card.h"
#include "../game/display_object.h"
#include "../game/display_object_work_slots.h"
#include "../game/duel_selection_layout.h"
#include "../game/duel_card_pick_cursor.h"
#include "../game/fade.h"
#include "../game/main_mode_state.h"
#include "../game/duel_effect_request.h"
#include "../game/duel_effect_allocate_request.h"
#include "../game/duel_effect_resource_record.h"
#include "../game/duel_effect_resource_setup.h"
#define D_8009B1B9_AS_SIGNED
#include "../game/func_800291E0.h"
#include "../game/display_object_helpers.h"
#include "../game/display_object_core.h"
#include "../game/func_80019BA0.h"
#include "../game/duel_battle_stats.h"
#include "../game/duel_trap_resolution.h"
#include "../game/duel_card_record_lifecycle.h"
#include "../game/duel_scene_card_placement.h"
#include "../game/sound.h"
#include "../game/main_modes.h"
#include "../game/func_8001944C.h"
#include "../game/duel_apply_card_object_flags.h"
#include "../game/sound_output.h"
#include "../game/sound_sequence_state.h"
#include "../game/model_scene_states.h"
#include "../game/display_object_motion.h"
#include "../unmatched.h"

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))

extern u16 D_8009B170;
extern u16 D_8009B172;
extern u16 D_8009B178;
extern u16 D_8009B17A;
extern s16 D_8009B1A4[2];
extern s8 D_8009B1B0[2];
s8 D_8009B208[8];
extern u8 D_8009B229;
extern u8 D_800E9F64[];

void DuelScene_UpdateBattle(void)
{
    DisplayObject *o;
    DisplayObject *side;
    u8 *made;
    DuelCardRecord *rec;
    DuelCardRecord *left;
    DuelCardRecord *right;
    DuelEffectRequest *req;
    s32 stats;
    s32 result;
    s32 id;
    s32 damage;
    s32 level;
    s32 se;
    u16 life;
    u8 flags;
    s8 fade;
    s32 ix;
    s8 *bp;
    s32 ad;
    u8 *lp;
    AnimatedBattleModelProperties *rp;
    DisplayObject **sl;
    s32 lv;
    DisplayObject **slots;
    DisplayObjectCallback cb;
    s32 one;
    s32 y;
    s32 h;
    s32 w;
    s32 big;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        big = 0x48000;
        gDuel_wSceneStateFlags |= 0x8000;
        w = 0x10;
        one = 1;
        slots = D_800E9EF0;
        o = D_8009B214;
        y = 0x5C;
        S(o, 0x28) = -0x74;
        cb = (DisplayObjectCallback)func_8001ED20;
        o->field_2C.h.field_2C = w;
        o->field_6C = one;
        o->update = cb;
        o->position.h.field_2A = o->field_30.h.field_32;
        o = D_8009B21C;
        h = 0x18;
        o->position.h.field_28 = 0x198;
        o->field_2C.h.field_2C = w;
        o->field_6C = one;
        o->update = cb;
        o->position.h.field_2A = o->field_30.h.field_32;
        o = slots[0];
        o->position.h.field_28 = 0x38;
        o->position.h.field_2A = y;
        o->field_2C.h.field_2C = h;
        o->field_6C = one;
        o->update = cb;
        func_80029164(0, S(D_8015C424 + slots[0]->field_6A * 0x1C + big, 0x36C0));
        o = slots[1];
        D_8009B22A = 0;
        D_8009B1B8 = 0;
        if (o != 0) {
            o->position.h.field_28 = 0xD8;
            o->position.h.field_2A = y;
            o->field_2C.h.field_2C = h;
            o->field_6C = one;
            o->update = cb;
        } else {
            D_8009B229 = 0;
        }
        if (func_8001F0D0((u8 *)D_800E9EF0[0]) != 0) {
            D_8009B229 = 0;
        }
        D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F64 + D_8009B1D5 * 0x70);
        DisplayObject_ReleaseIfPresent(*(void **)((u8 *)D_8009B1B4 + 4));
        D_8009B162 = 8;
        D_8009B174 = 1;
        *(void **)((u8 *)D_8009B1B4 + 4) = 0;
    }

    switch (D_8009B174 & 0xF) {
    case 1:
        if (!(D_8009B174 & 0x80) && ((D_8009B0F4_abs & 0x02000030) | D_8009B134_abs) == 0) {
            if (!(D_8009B174 & 0x40)) {
                id = D_8009B22A;
                D_8009B174 |= 0x40;
                if (id == 0) {
                    if (D_800E9EF0[1] != 0) {
                        big = 0x48000;
                        id = S(D_8015C424 + D_800E9EF0[1]->field_6A * 0x1C + big, 0x36C0);
                        goto request;
                    }
                } else {
                request:
                    func_80029164(1, id);
                    return;
                }
            } else {
                D_8009B174 |= 0x80;
                goto wait;
            }
        } else {
        wait:
            if (!(D_8009B174 & 0x20) && D_8009B162 == 0) {
                if (!(D_8009B174 & 0x10)) {
                    D_8009B174 |= 0x10;
                    D_8009B162 = 0xC;
                    D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F48 + D_8009B1D5 * 0x70);
                } else {
                    D_8009B174 |= 0x20;
                }
            }
            if ((D_8009B174 & 0xA0) == 0xA0 && func_80042B40(1) == 0) {
                D_8009B174 = 2;
                return;
            }
        }
        break;

    case 2:
        flags = D_8009B174;
        if (!(flags & 0x80)) {
            o = D_800E9EF0[0];
            D_8009B174 = flags | 0x80;
            {
                s32 b21 = o->field_20.b.field_21;

                func_80019BA0(o, b21, b21 + 0x40, 8);
            }
            stats = Duel_CalcCardStats(&D_801A7AD8[o->field_6A]);
            made = func_800291E0(0, stats & 0xFFFF, stats >> 16);
            B(D_800EA0E8, 0x3C) |= 0x40;
            H(made, 0x30) = 0xA;
            H(made, 0x32) = 0x16;
            B(made, 0x21) = 0xC0;
            DisplayObject_SetDepthOffset((DisplayObject *)made, -0xA);
            H(made, 8) = (H(made, 8) | 4) & 0xFFBF;
            D_800E9EF0[2] = (DisplayObject *)made;
            side = D_800E9EF0[1];
            D_800E9EF0[3] = 0;
            made = 0;
            if (side != 0) {
                func_80019BA0(side, side->field_20.b.field_21, side->field_20.b.field_21 + 0x40, 8);
                stats = Duel_CalcCardStats(&D_801A7AD8[side->field_6A]);
                made = func_800291E0(1, stats & 0xFFFF, stats >> 16);
                if (D_8009B17A & 0x800) {
                    B(D_800EA0E8, 0x7C) |= 0x80;
                } else {
                    B(D_800EA0E8, 0x7C) |= 0x40;
                }
            } else if (D_8009B22A != 0) {
                made = func_800291E0(1, -1, -1);
            }
            if (made != 0) {
                H(made, 0x30) = 0xAA;
                H(made, 0x32) = 0x16;
                B(made, 0x21) = 0xC0;
                DisplayObject_SetDepthOffset((DisplayObject *)made, -0xA);
                H(made, 8) = (H(made, 8) | 4) & 0xFFBF;
                D_800E9EF0[3] = (DisplayObject *)made;
            }
        }
        if (D_8009B174 & 0x20) {
            if (func_8001F364() == 0) {
                D_8009B174 &= 0xDF;
                return;
            }
        } else if (!(D_8009B174 & 0x40)) {
            if (func_80042B40(1) == 0) {
                D_800E9EF0[0]->flags &= 0xFFBF;
                D_800E9EF0[2]->flags |= 0x40;
                if (D_800E9EF0[1] != 0) {
                    D_800E9EF0[1]->flags &= 0xFFBF;
                }
                if (D_800E9EF0[3] != 0) {
                    D_800E9EF0[3]->flags |= 0x40;
                }
                flags = D_8009B174;
                D_8009B174 = flags | 0x40;
                if (D_8009B22A != 0) {
                    D_8009B174 = flags | 0x60;
                    D_8009B210 = 0;
                    return;
                }
            }
        } else {
            D_800E9EF0[2]->field_20.b.field_21 += 8;
            if ((s8)D_800E9EF0[2]->field_20.b.field_21 >= 0) {
                D_800E9EF0[2]->field_20.b.field_21 = 0;
                D_8009B174 = 3;
                D_800E9EF0[2]->flags &= 0xFFFB;
            }
            if (D_800E9EF0[3] != 0) {
                fade = D_800E9EF0[2]->field_20.b.field_21;
                D_800E9EF0[3]->field_20.b.field_21 = fade;
                if (!(fade & 0xFF)) {
                    D_800E9EF0[3]->flags &= 0xFFFB;
                    return;
                }
            }
        }
        break;

    case 3:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            func_80015CC0();
            return;
        }
        if (!(D_800E9ECE[0] & 0x80)) {
            D_8009B174 = 4;
            if (D_8009B22A != 0) {
                D_8009B1B9 = D_8009B22A == 0x2B2;
            set_state_a:
                D_8009B174 = 0xA;
                return;
            }
            result = func_8001EFD4(D_800E9EF0[0], D_800E9EF0[1]);
            if (result >= 0) {
                D_8009B1B0[0] = 0;
                D_8009B1A4[0] = 0;
                D_8009B1B0[1] = 1;
                D_8009B1A4[1] = 0;
                if (result != 0) {
                    if (D_800E9EF0[1] != 0 && (D_8009B17A & 0x800)) {
                        D_8009B1C8->rank.field_0B++;
                        D_8009B1B0[1] = -1;
                        D_800E9FF0[D_8009B1D5 ^ 1].rank.field_0C++;
                        return;
                    }
                    lp = (u8 *)&D_800E9FF0[D_8009B1D5 ^ 1];
                    life = H(lp, 0x14) - result;
                    H(lp, 0x14) = life;
                    if ((s16)life < 0) {
                        H(lp, 0x14) = 0;
                    }
                    if (D_800E9EF0[1] != 0) {
                        D_8009B1C8->rank.field_0B++;
                        D_8009B1C8->rank.effective_attacks++;
                    }
                    D_8009B1B0[1] = -1;
                    D_8009B1A4[1] = result;
                    return;
                }
            } else {
                D_8009B1B0[0] = -1;
                D_8009B1B0[1] = -1;
                D_8009B1A4[0] = 0;
                D_8009B1A4[1] = 0;
                if (result < -1) {
                    lp = (u8 *)&D_800E9FF0[D_8009B1D5];
                    life = H(lp, 0x14) + result;
                    H(lp, 0x14) = life;
                    if ((s16)life < 0) {
                        H(lp, 0x14) = 0;
                    }
                    D_8009B1A4[0] = result;
                    D_8009B1B0[1] = 1;
                    if (D_8009B17A & 0x800) {
                        D_8009B1B0[0] = 1;
                        D_800E9FF0[D_8009B1D5 ^ 1].rank.defensive_wins++;
                        return;
                    }
                }
            }
        }
        break;

    case 4:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            if (D_800E9EF0[1] != 0) {
                result = Duel_CalcGuardianStarBonus(&D_801A7AD8[D_800E9EF0[0]->field_6A],
                                                    &D_801A7AD8[D_800E9EF0[1]->field_6A]);
                if (result != 0) {
                    side = D_800E9EF0[2];
                    D_8009B1B9 = 0;
                    if (result < 0) {
                        side = D_800E9EF0[3];
                        D_8009B1B9 = 1;
                    }
                    rec = &D_801A7AD8[D_800E9EF0[D_8009B1B9]->field_6A];
                    if (rec->flags & 0x200) {
                        level = gDuel_adwCardStats[rec->card_id - 1] >> 0x12;
                    } else {
                        level = gDuel_adwCardStats[rec->card_id - 1] >> 0x16;
                    }
                    lv = level & 0xF;
                    SD_SEPlayFull(0x1D);
                    made = DuelEffect_AllocateRequest(0xE);
                    D_8009B17C = made;
                    D_8009B1D0 = 0;
                    S(made, 0) = side->field_30.h.field_30 + 0x46;
                    S(made, 0x1A) = lv - 1;
                    D_8009B174 |= 0x60;
                    S(made, 2) = side->field_30.h.field_32 + 0x62;
                    return;
                }
            }
        } else {
            flags = D_8009B174;
            if (flags & 0x20) {
                if (!(((DuelEffectRequest *)D_8009B17C)->flags & 0x80)) {
                    D_8009B174 &= 0xDF;
                }
            }
            if (D_8009B174 & 0x40) {
                D_8009B1D0 += 0x10;
                if ((s16)D_8009B1D0 >= 0x1F4) {
                    D_8009B174 &= 0xBF;
                    D_8009B1D0 = 0x1F4;
                }
                lp = (u8 *)D_800EA0E8 + (D_8009B1B9 << 6);
                H(lp, 0x36) = D_8009B1D0;
                H(lp, 0x38) = D_8009B1D0;
            }
            if (!(D_8009B174 & 0x60)) {
                D_8009B174 = 7;
                if (D_8009B229 != 0) {
                    D_8009B174 = 5;
                    return;
                }
            }
        }
        break;

    case 5:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            SD_BGMFadeOut();
            Fade_StartOut();
            D_800E9ECC[0] = 0xFF;
            Fade_FillBandLevels(0xFF);
            return;
        }
        if (!(D_800E9ECE[0] & 0x80) && func_80049120() == 0) {
            sl = D_800E9EF0;
            func_800472A8(D_8009B374);
            func_80059C18(D_8009B374);
            left = &D_801A7AD8[sl[0]->field_6A];
            right = &D_801A7AD8[sl[1]->field_6A];
            func_80024D34(sl[0]->field_6A, sl[0]->field_6B);
            left->flags |= (D_8009B178 & 0xA00) | 0x4000;
            left->stat_modifier = D_8009B170;
            func_80024D34(sl[1]->field_6A, sl[1]->field_6B);
            right->flags |= D_8009B17A & 0xA00;
            right->stat_modifier = D_8009B172;
            rp = D_800EF658;
            D_8009B208[1] = -1;
            D_8009B208[0] = -1;
            rp[0].field_06 = 0;
            rp[0].field_02 = 0;
            rp[0].field_04 = 0;
            rp[0].model_id = left->card_id;
            rp[0].field_07 = (D_8009B178 >> 9) & 1;
            rp[1].field_02 = 0;
            rp[1].field_04 = 0;
            rp[1].field_07 = (D_8009B17A >> 9) & 1;
            rp[1].field_06 = (D_8009B17A >> 0xB) & 1;
            rp[1].model_id = right->card_id;
            result = func_8001EFD4(sl[0], sl[1]);
            if (result == -1) {
                D_8009B208[0] = sl[0]->field_6A;
                D_8009B208[1] = sl[1]->field_6A;
            } else {
                if (result > 0) {
                    rp[0].field_02 = 1;
                    D_8009B208[0] = sl[1]->field_6A;
                }
                if (result < 0) {
                    if (!(right->flags & 0x800)) {
                        rp[1].field_02 = 1;
                        D_8009B208[0] = sl[0]->field_6A;
                    }
                }
            }
            D_8009B369 = 1;
            D_8009B269 = 3;
            D_8009B26C = 1;
            return;
        }
        break;

    case 7:
        if (D_800E9EF0[1] == 0) {
            D_8009B174 = 9;
            return;
        }
        D_8009B1B9 = 2;
        D_8009B174 = 6;
    case 6:
        D_8009B1B9 = D_8009B1B9 - 1;
        if ((s8)D_8009B1B9 >= 0) {
            D_8009B174 = 8;
        case 8:
            flags = D_8009B174;
            ix = D_8009B1B9;
            side = D_800E9EF0[ix + 2];
            if (!(flags & 0x80)) {
                D_8009B174 = flags | 0xC0;
                side->position.word = side->field_30.word;
                bp = D_8009B1B0;
                if (bp[ix] != 0) {
                    req = (DuelEffectRequest *)DuelEffect_AllocateRequest(2);
                    req->field_00 = side->field_30.h.field_30 + 0x46;
                    req->field_02 = side->field_30.h.field_32 + 0x62;
                    damage = D_8009B1A4[D_8009B1B9];
                    ad = (damage < 0 ? -damage : damage) / 1000;
                    D_8009B17C = (u8 *)req;
                    req->field_12 = damage;
                    if (ad >= 3) {
                        ad = 2;
                    }
                    req->field_1A = ad;
                    if (bp[D_8009B1B9] > 0) {
                        D_8009B174 |= 0x20;
                    }
                    se = ad + 0x10;
                    if (D_8009B1B9 == 0) {
                        se = ad + 0xD;
                    }
                    SD_SEPlayFull(se);
                    goto shake;
                }
                goto next_side;
            }
        shake:
            if (D_8009B174 & 0x40) {
                if (!(((DuelEffectRequest *)D_8009B17C)->flags & 0x80)) {
                    D_8009B174 &= 0xBF;
                    side->field_30.word = side->position.word;
                    D_8009B1D0 = 0xA;
                    return;
                }
                if (!(D_8009B174 & 0x20) && ((DuelEffectRequest *)D_8009B17C)->field_1D != 0) {
                    side->field_30.h.field_30 = side->position.h.field_28 + ((rand() & 3) - 2);
                    side->field_30.h.field_32 = side->position.h.field_2A + ((rand() & 3) - 2);
                    return;
                }
            } else {
                D_8009B1D0 = D_8009B1D0 - 1;
                if ((s16)D_8009B1D0 <= 0) {
                    if (D_8009B174 & 0x20) {
                    next_side:
                        D_8009B174 = 6;
                        return;
                    }
                    goto set_state_a;
                }
            }
        } else {
        set_state_b:
            D_8009B174 = 0xB;
            return;
        }
        break;

    case 9:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            req = DuelEffect_CreateRequest(2);
            req->field_00 = 0xF0;
            req->field_02 = 0x78;
            result = func_8001EFD4(D_800E9EF0[0], 0);
            req->field_1A = result / 1000;
            if (req->field_1A >= 3) {
                req->field_1A = 2;
            }
            SD_SEPlayFull(req->field_1A + 0x10);
            req->field_12 = result;
            req->field_1A = req->field_1A + 3;
            return;
        }
        goto set_state_b;

    case 10:
        side = D_800E9EF0[D_8009B1B9 + 2];
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            func_8001944C(side);
            SD_SEPlayFull(0x1B);
            made = DuelEffect_AllocateRequest(3);
            D_8009B17C = made;
            S(made, 0) = side->field_30.h.field_30 + 0x46;
            S(made, 2) = side->field_30.h.field_32 + 0x62;
            if (D_8009B22A != 0) {
                S(made, 0x1A) = 1;
            }
            D_8009B1D0 = 2;
            return;
        }
        if (!(D_8009B174 & 0x40)) {
            D_8009B1D0 = D_8009B1D0 - 1;
            if ((s16)D_8009B1D0 <= 0) {
                D_8009B174 |= 0x40;
                func_80029528(D_8009B1B9);
                if (D_8009B1B9 != 1 || D_8009B22A == 0) {
                    D_800E9EF0[D_8009B1B9 + 2] = 0;
                    return;
                }
            }
        } else if (!(((DuelEffectRequest *)D_8009B17C)->flags & 0x80)) {
            D_8009B174 = 6;
            if (D_8009B22A != 0) {
                goto set_state_b;
            }
        }
        break;

    case 11:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            if (D_800E9EF0[2] != 0) {
                func_80024D34(D_800E9EF0[0]->field_6A, D_800E9EF0[0]->field_6B);
                left = &D_801A7AD8[D_800E9EF0[0]->field_6A];
                left->flags |= (D_8009B178 & 0xA00) | 0x4000;
                left->stat_modifier = D_8009B170;
                Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
            }
            if (D_800E9EF0[3] != 0) {
                if (D_800E9EF0[1] != 0) {
                    func_80024D34(D_800E9EF0[1]->field_6A, D_800E9EF0[1]->field_6B);
                    left = &D_801A7AD8[D_800E9EF0[1]->field_6A];
                    H(left, 0x16) |= D_8009B17A & 0xA00;
                    H(left, 0x12) = D_8009B172;
                    if (D_8009B22A != 0) {
                        H(left, 0x16) |= D_8009B17A & 0x3000;
                    }
                    Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
                }
            }
            DisplayObject_ReleaseIfPresent(D_800E9EF0[0]);
            DisplayObject_ReleaseIfPresent(D_800E9EF0[1]);
            o = D_8009B214;
            o->position.h.field_28 = 0xC;
            o->field_2C.h.field_2C = 0x10;
            o->field_6C = 1;
            o->update = (DisplayObjectCallback)func_8001ED20;
            o->position.h.field_2A = o->field_30.h.field_32;
            o = D_8009B21C;
            o->position.h.field_28 = 0x118;
            o->field_2C.h.field_2C = 0x10;
            o->field_6C = 1;
            o->update = (DisplayObjectCallback)func_8001ED20;
            o->position.h.field_2A = o->field_30.h.field_32;
        }
        if (!(D_8009B174 & 0x40)) {
            o = D_800E9EF0[2];
            if (o != 0) {
                fade = o->field_0C - 8;
                if (fade <= 0) {
                    func_80029528(0);
                    D_800E9EF0[2] = 0;
                } else {
                    B(o, 0xE) = fade;
                    B(o, 0xD) = fade;
                    B(o, 0xC) = fade;
                }
            }
            o = D_800E9EF0[3];
            if (o != 0) {
                fade = o->field_0C - 8;
                if (fade <= 0) {
                    func_80029528(1);
                    D_800E9EF0[3] = 0;
                } else {
                    B(o, 0xE) = fade;
                    B(o, 0xD) = fade;
                    B(o, 0xC) = fade;
                }
            }
            if (D_800E9EF0[2] == 0 && D_800E9EF0[3] == 0) {
                D_8009B174 |= 0x40;
                func_80015C0C();
                return;
            }
        } else {
            gDuel_wSceneStateFlags = 5;
        }
        break;
    }
}
