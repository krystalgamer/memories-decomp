/*
 * Duel scene-state 9: the battle. It presents the attacker and the defender,
 * resolves the exchange, shows the damage and the guardian-star bonus, and
 * hands off to the trap, fusion-result and turn-switch states.
 *
 * Matches retail under gcc_2_8_1_g8_split_comm with no inline assembly,
 * register pins or asm links. What the listing fixes about the source:
 * - One s32 (result) carries the card stats, the battle result, the level
 *   nibble, the damage and the fade step: retail keeps all of them in $s2
 *   because the same variable crosses the sound call in state 9.
 * - The two saved stat words and the two saved flag words are side pairs
 *   (u16 [2]): as struct elements their loads cannot pass the record stores,
 *   which is why retail reloads D_8009B17A after each flags store.
 * - State 4 falls through to its flag tests when there is no defender (the
 *   candidate returned there), and the do-while gives both exits one label so
 *   cse cannot thread the first test past the reload.
 * - State 3, 8, 9 and 10 store their next state inline; cross-jumping merges
 *   the identical tails and reorg lifts the li into the branch slot.
 * - Each state reads D_8009B174 into its own block-local variable; the
 *   function-wide copy conflicted with the temporaries retail keeps in $v1.
 * - The state-4 request block reads the flags before the field stores into a
 *   block-local, keeps the x coordinate and the level index in one shared
 *   variable so the second assignment waits for the first store, and the
 *   record pointer is &D_800EA0E8[D_8009B1B9] with the meter copied through
 *   the sound-id scratch (retail's $a0).
 * - State 8 copies the position words through non-struct views so the copy
 *   cannot pass the flag byte store, and the shake block's flags are an s32
 *   so the mask writes the same register.
 * - State 5 writes the model properties in member order (model id first);
 *   the defender's stat word goes through a byte view so the pair loads for
 *   func_8001EFD4 wait for it.
 * - The two replay bytes are tentative scalar definitions, D_8009B208 and
 *   D_8009B209: only a definition in this unit makes gcc treat them as small
 *   data and emit the load-delay nops retail has before their gp-relative
 *   stores, and only two symbols let the stores keep retail's order without
 *   cse deriving the second address from the first. The attacker's card id
 *   is read through a byte view so it stays behind those stores. The _comm
 *   profile keeps the definitions common, so the linker resolves them to
 *   the retail addresses.
 */
#define D_8009B369_IN_DATA
#define D_8009B374_IN_DATA
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#define D_8009B170_AS_SIDE_ARRAY
#define D_8009B178_AS_SIDE_ARRAY
#include "../types.h"
#include "../psyq/rand.h"
#include "duel_scene_state.h"
#include "file_transfer.h"
#include "duel_side_state.h"
#include "duel_action_lock.h"
#include "duel_init_scene.h"
#include "duel_card_staging.h"
#include "duel_card.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "duel_selection_layout.h"
#include "duel_card_pick_cursor.h"
#include "fade.h"
#include "main_mode_state.h"
#include "duel_effect_request.h"
#include "duel_effect_allocate_request.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_resource_setup.h"
#define D_8009B1B9_AS_SIGNED
#include "func_800291E0.h"
#include "display_object_helpers.h"
#include "display_object_core.h"
#include "func_80019BA0.h"
#include "duel_battle_stats.h"
#include "duel_trap_resolution.h"
#include "duel_card_record_lifecycle.h"
#include "duel_scene_battle.h"
#include "duel_scene_field_actions.h"
#define D_8009B208_AS_REPLAY_BYTES
#include "duel_scene_resume.h"
#include "duel_scene_card_placement.h"
#include "sound.h"
#include "main_modes.h"
#include "func_8001944C.h"
#include "duel_apply_card_object_flags.h"
#include "sound_output.h"
#include "sound_sequence_state.h"
#include "model_scene_states.h"
#include "display_object_motion.h"
#include "../unmatched.h"

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))
#define DUEL_EFFECT_REQUEST_VIEW(request) ((DuelEffectRequest *)(request))
#define DUEL_EFFECT_REQUEST_BYTES(request) ((u8 *)(request))

s8 D_8009B208;
s8 D_8009B209;

void DuelScene_UpdateBattle(void)
{
    DisplayObject *o;
    DisplayObject *side;
    DuelCardRecord *rec;
    DuelCardRecord *left;
    DuelCardRecord *right;
    DuelEffectRequest *req;
    s32 result;
    s32 id;
    s32 level;
    s32 arg;
    u16 life;
    u8 flags;
    s32 ix;
    s8 *wins;
    u8 *view;
    AnimatedBattleModelProperties *models;
    DisplayObject **pair;
    DisplayObject **slots;
    DisplayObjectCallback cb;
    s32 one;
    s32 y;
    s32 h;
    s32 w;
    s32 big;
    s32 pos;
    DuelSelectionRecord *cur;
    u8 *pw;
    DuelEffectResourceRecord *effects;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        big = 0x48000;
        gDuel_wSceneStateFlags |= 0x8000;
        w = 0x10;
        one = 1;
        slots = D_800E9EF0;
        pw = D_8015C424;
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
        func_80029164(0, S(pw + D_800E9EF0[0]->field_6A * 0x1C + big, 0x36C0));
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
        cur = (DuelSelectionRecord *)(D_800E9F64 + D_8009B1D5 * 0x70);
        D_8009B1B4 = (DuelCardPickCursor *)cur;
        DisplayObject_ReleaseIfPresent(cur->cursor_object);
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
                        u8 *pw1 = D_8015C424;
                        s32 big1 = 0x48000;

                        id = S(pw1 + D_800E9EF0[1]->field_6A * 0x1C + big1, 0x36C0);
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
            if ((D_8009B174 & 0xA0) == 0xA0 && DisplayObject_FindAllocatedByTag(1) == 0) {
                D_8009B174 = 2;
                return;
            }
        }
        break;

    case 2: {
        u8 f = D_8009B174;

        if (!(f & 0x80)) {
            o = D_800E9EF0[0];
            D_8009B174 = f | 0x80;
            {
                s32 b21 = o->field_20.b.field_21;

                func_80019BA0(o, b21, b21 + 0x40, 8);
            }
            result = Duel_CalcCardStats(&D_801A7AD8[o->field_6A]);
            o = (DisplayObject *)func_800291E0(0, result & 0xFFFF, result >> 16);
            effects = D_800EA0E8;
            effects[0].field_3C |= 0x40;
            o->field_30.h.field_30 = 0xA;
            o->field_30.h.field_32 = 0x16;
            o->field_20.b.field_21 = 0xC0;
            DisplayObject_SetDepthOffset(o, -0xA);
            o->flags = (o->flags | 4) & 0xFFBF;
            D_800E9EF0[2] = o;
            o = D_800E9EF0[1];
            D_800E9EF0[3] = 0;
            if (o != 0) {
                {
                    s32 b21 = o->field_20.b.field_21;

                    func_80019BA0(o, b21, b21 + 0x40, 8);
                }
                result = Duel_CalcCardStats(&D_801A7AD8[o->field_6A]);
                o = (DisplayObject *)func_800291E0(1, result & 0xFFFF, result >> 16);
                if (D_8009B178[1] & 0x800) {
                    effects[1].field_3C |= 0x80;
                } else {
                    effects[1].field_3C |= 0x40;
                }
            } else if (D_8009B22A != 0) {
                o = (DisplayObject *)func_800291E0(1, -1, -1);
            }
            if (o != 0) {
                o->field_30.h.field_30 = 0xAA;
                o->field_30.h.field_32 = 0x16;
                o->field_20.b.field_21 = 0xC0;
                DisplayObject_SetDepthOffset(o, -0xA);
                o->flags = (o->flags | 4) & 0xFFBF;
                D_800E9EF0[3] = o;
            }
        }
        f = D_8009B174;
        if (f & 0x20) {
            if (func_8001F364() == 0) {
                D_8009B174 &= 0xDF;
                return;
            }
        } else if (!(f & 0x40)) {
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                D_800E9EF0[0]->flags &= 0xFFBF;
                D_800E9EF0[2]->flags |= 0x40;
                if (D_800E9EF0[1] != 0) {
                    D_800E9EF0[1]->flags &= 0xFFBF;
                }
                if (D_800E9EF0[3] != 0) {
                    D_800E9EF0[3]->flags |= 0x40;
                }
                {
                    u8 g = D_8009B174;

                    D_8009B174 = g | 0x40;
                    if (D_8009B22A != 0) {
                        D_8009B174 = g | 0x60;
                    D_8009B210 = 0;
                    return;
                    }
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
                u8 fade = D_800E9EF0[2]->field_20.b.field_21;

                D_800E9EF0[3]->field_20.b.field_21 = fade;
                if (fade == 0) {
                    D_800E9EF0[3]->flags &= 0xFFFB;
                    return;
                }
            }
        }
        break;
    }

    case 3:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            Fade_StartOutKeepOverlayAndHideSecondaryTables();
            return;
        }
        if (!(D_800E9ECE[0] & 0x80)) {
            D_8009B174 = 4;
            if (D_8009B22A != 0) {
                D_8009B1B9 = D_8009B22A == 0x2B2;
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
                    if (D_800E9EF0[1] != 0 && (D_8009B178[1] & 0x800)) {
                        D_8009B1C8->rank.field_0B++;
                        D_8009B1B0[1] = -1;
                        D_800E9FF0[D_8009B1D5 ^ 1].rank.field_0C++;
                        return;
                    }
                    view = (u8 *)&D_800E9FF0[D_8009B1D5 ^ 1];
                    life = H(view, 0x14) - result;
                    H(view, 0x14) = life;
                    if ((s16)life < 0) {
                        H(view, 0x14) = 0;
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
                    view = (u8 *)&D_800E9FF0[D_8009B1D5];
                    life = H(view, 0x14) + result;
                    H(view, 0x14) = life;
                    if ((s16)life < 0) {
                        H(view, 0x14) = 0;
                    }
                    D_8009B1A4[0] = result;
                    D_8009B1B0[1] = 1;
                    if (D_8009B178[1] & 0x800) {
                        D_8009B1B0[0] = 1;
                        D_800E9FF0[D_8009B1D5 ^ 1].rank.defensive_wins++;
                        return;
                    }
                }
            }
        }
        break;

    case 4:
        do {
            if (D_8009B174 & 0x80) {
                break;
            }
            D_8009B174 |= 0x80;
            if (D_800E9EF0[1] == 0) {
                break;
            }
            {
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
                    result = level & 0xF;
                    SD_SEPlayFull(0x1D);
                    req =
                        DUEL_EFFECT_REQUEST_VIEW(DuelEffect_AllocateRequest(0xE));
                    {
                        u8 t = D_8009B174;

                        D_8009B17C = DUEL_EFFECT_REQUEST_BYTES(req);
                        pos = side->field_30.h.field_30;
                        pos += 0x46;
                        req->field_00 = pos;
                        D_8009B1D0 = 0;
                        arg = side->field_30.h.field_32;
                        arg += 0x62;
                        req->field_02 = arg;
                        pos = result - 1;
                        req->field_1A = pos;
                        D_8009B174 = t | 0x60;
                    }
                }
                return;
            }
        } while (0);
        {
            u8 f = D_8009B174;

            if (f & 0x20) {
                if (!(DUEL_EFFECT_REQUEST_VIEW(D_8009B17C)->flags & 0x80)) {
                    D_8009B174 = f & 0xDF;
                }
            }
            f = D_8009B174;
            if (f & 0x40) {
                D_8009B1D0 += 0x10;
                if ((s16)D_8009B1D0 >= 0x1F4) {
                    D_8009B174 = f & 0xBF;
                    D_8009B1D0 = 0x1F4;
                }
                view = (u8 *)&D_800EA0E8[D_8009B1B9];
                arg = D_8009B1D0;
                H(view, 0x36) = arg;
                H(view, 0x38) = arg;
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
        if (!(D_800E9ECE[0] & 0x80) && SD_IsBgmFadeActive() == 0) {
            pair = D_800E9EF0;
            func_800472A8(D_8009B374);
            Model_SetPresentationBgmCommand(D_8009B374);
            left = &D_801A7AD8[pair[0]->field_6A];
            right = &D_801A7AD8[pair[1]->field_6A];
            func_80024D34(pair[0]->field_6A, pair[0]->field_6B);
            left->flags |= (D_8009B178[0] & 0xA00) | 0x4000;
            left->stat_modifier = D_8009B170[0];
            func_80024D34(pair[1]->field_6A, pair[1]->field_6B);
            right->flags |= D_8009B178[1] & 0xA00;
            H(right, 0x12) = D_8009B170[1];
            models = D_800EF658;
            D_8009B209 = -1;
            D_8009B208 = -1;
            models[0].model_id = H(left, 0xC);
            models[0].field_06 = 0;
            models[0].field_02 = 0;
            models[0].field_04 = 0;
            models[0].field_07 = (D_8009B178[0] >> 9) & 1;
            models[1].model_id = right->card_id;
            models[1].field_07 = (D_8009B178[1] >> 9) & 1;
            models[1].field_02 = 0;
            models[1].field_04 = 0;
            models[1].field_06 = (D_8009B178[1] >> 0xB) & 1;
            result = func_8001EFD4(pair[0], pair[1]);
            if (result == -1) {
                D_8009B208 = pair[0]->field_6A;
                D_8009B209 = pair[1]->field_6A;
            } else {
                if (result > 0) {
                    models[0].field_02 = 1;
                    D_8009B208 = pair[1]->field_6A;
                }
                if (result < 0) {
                    if (!(right->flags & 0x800)) {
                        models[1].field_02 = 1;
                        D_8009B208 = pair[0]->field_6A;
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
                *(u32 *)((u8 *)side + 0x28) = *(u32 *)((u8 *)side + 0x30);
                wins = D_8009B1B0;
                if (wins[ix] != 0) {
                    req =
                        DUEL_EFFECT_REQUEST_VIEW(DuelEffect_AllocateRequest(2));
                    req->field_00 = side->field_30.h.field_30 + 0x46;
                    req->field_02 = side->field_30.h.field_32 + 0x62;
                    result = D_8009B1A4[D_8009B1B9];
                    D_8009B17C = DUEL_EFFECT_REQUEST_BYTES(req);
                    req->field_12 = result;
                    result = (result < 0 ? -result : result) / 1000;
                    if (result >= 3) {
                        result = 2;
                    }
                    req->field_1A = result;
                    if (wins[D_8009B1B9] > 0) {
                        D_8009B174 |= 0x20;
                    }
                    arg = result + 0x10;
                    if (D_8009B1B9 == 0) {
                        arg = result + 0xD;
                    }
                    SD_SEPlayFull(arg);
                    goto shake;
                }
                D_8009B174 = 6;
                return;
            }
        shake:
        {
            s32 f = D_8009B174;

            if (f & 0x40) {
                if (!(DUEL_EFFECT_REQUEST_VIEW(D_8009B17C)->flags & 0x80)) {
                    f &= 0xBF;
                    D_8009B174 = f;
                    *(u32 *)((u8 *)side + 0x30) = side->position.word;
                    D_8009B1D0 = 0xA;
                    return;
                }
                if (!(f & 0x20) &&
                    DUEL_EFFECT_REQUEST_VIEW(D_8009B17C)->field_1D != 0) {
                    side->field_30.h.field_30 = side->position.h.field_28 + ((rand() & 3) - 2);
                    side->field_30.h.field_32 = side->position.h.field_2A + ((rand() & 3) - 2);
                    return;
                }
            } else {
                D_8009B1D0 = D_8009B1D0 - 1;
                if ((s16)D_8009B1D0 <= 0) {
                    if (D_8009B174 & 0x20) {
                        D_8009B174 = 6;
                        return;
                    }
                    D_8009B174 = 0xA;
                    return;
                }
            }
        }
        } else {
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
        D_8009B174 = 0xB;
        return;

    case 10:
        side = D_800E9EF0[D_8009B1B9 + 2];
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            func_8001944C(side);
            SD_SEPlayFull(0x1B);
            req = DUEL_EFFECT_REQUEST_VIEW(DuelEffect_AllocateRequest(3));
            D_8009B17C = DUEL_EFFECT_REQUEST_BYTES(req);
            req->field_00 = side->field_30.h.field_30 + 0x46;
            req->field_02 = side->field_30.h.field_32 + 0x62;
            if (D_8009B22A != 0) {
                req->field_1A = 1;
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
        } else if (!(DUEL_EFFECT_REQUEST_VIEW(D_8009B17C)->flags & 0x80)) {
            D_8009B174 = 6;
            if (D_8009B22A != 0) {
                D_8009B174 = 0xB;
                return;
            }
        }
        break;

    case 11:
        if (!(D_8009B174 & 0x80)) {
            D_8009B174 |= 0x80;
            if (D_800E9EF0[2] != 0) {
                func_80024D34(D_800E9EF0[0]->field_6A, D_800E9EF0[0]->field_6B);
                left = &D_801A7AD8[D_800E9EF0[0]->field_6A];
                left->flags |= (D_8009B178[0] & 0xA00) | 0x4000;
                left->stat_modifier = D_8009B170[0];
                Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
            }
            if (D_800E9EF0[3] != 0) {
                if (D_800E9EF0[1] != 0) {
                    func_80024D34(D_800E9EF0[1]->field_6A, D_800E9EF0[1]->field_6B);
                    left = &D_801A7AD8[D_800E9EF0[1]->field_6A];
                    left->flags |= D_8009B178[1] & 0xA00;
                    left->stat_modifier = D_8009B170[1];
                    if (D_8009B22A != 0) {
                        left->flags |= D_8009B178[1] & 0x3000;
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
                result = B(o, 0xC);
                result -= 8;
                if (result <= 0) {
                    func_80029528(0);
                    D_800E9EF0[2] = 0;
                } else {
                    B(o, 0xE) = result;
                    B(o, 0xD) = result;
                    B(o, 0xC) = result;
                }
            }
            o = D_800E9EF0[3];
            if (o != 0) {
                result = B(o, 0xC);
                result -= 8;
                if (result <= 0) {
                    func_80029528(1);
                    D_800E9EF0[3] = 0;
                } else {
                    B(o, 0xE) = result;
                    B(o, 0xD) = result;
                    B(o, 0xC) = result;
                }
            }
            if (D_800E9EF0[2] == 0 && D_800E9EF0[3] == 0) {
                D_8009B174 |= 0x40;
                Fade_StartInKeepOverlay();
                return;
            }
        } else {
            gDuel_wSceneStateFlags = 5;
        }
        break;
    }
}
