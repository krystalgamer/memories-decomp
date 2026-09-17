/*
 * Model scene mode 15 controller: copies the scene's four effect handlers
 * and view offset, then steps the signed D_8009AF9A phase through lighting,
 * fades, effect requests and the per-slot handler calls. Current best under
 * gcc_2_8_1_g8_split: 1227 instructions against 1227 with opcode distance 36
 * (18 surplus, 18 missing), with no hard register assignments and no inline
 * assembly.
 *
 * Levers measured on this body, following the matched sibling func_8004FE2C:
 * - every arm advances the phase in place (D_8009AF9A++ then break), which
 *   keeps retail's per-arm load/add in front of the shared store;
 * - the dispatch is (s8)((u8)D_8009AF9A + 1);
 * - the diagnostics string and D_800E9ECE use their unsized declaration arms,
 *   which is retail's %hi/%lo form;
 * - the slot key comparisons read the halfword at +0xCF8;
 * - state 10 picks the mode with a comparison chain and halves the offset
 *   at run time;
 * - state 27 falls through to the fade-out test once the handler reports 2;
 * - the fade-out stores live in state 34 and state 27 reaches them by goto.
 *
 * Residual: census addiu -4, addu +1, and +1, beqz -1, bgez +1, bltz -2,
 * j -1, lb +1, lbu -3, lh +1, lui +5, lw +1, nop -5, sll +4, slti +2,
 * sltiu -1, sra -1, subu +1. Retail tests the two mode bytes of each slot
 * separately where gcc merges them into one masked word compare, lays state
 * 10's mode arms out of line, and keeps states 13 and 17 as comparison chains.
 */
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../game/func_8004EB00.h"
#include "../game/model.h"
#include "../game/model_graphics_state.h"
#include "../game/model_handler_state.h"
#include "../game/duel_card.h"
#include "../game/model_slot_properties.h"
#include "../game/sound_output.h"
#include "../game/model_effect_state.h"
#include "../game/model_effect_coefficients.h"
#include "../game/func_8005A188.h"
#include "../game/func_8005A878.h"
#include "../game/model_effect_wrapped_value.h"
#include "../game/model_transfer_state.h"
#include "../game/model_transfer_flags.h"
#include "../game/func_80058E1C.h"
#include "../game/model_control_slot_animation.h"
#include "../game/func_8005106C.h"
#include "../game/model_effect_requests.h"
#include "../game/model_distance_queries.h"
#include "../game/model_update_view_metrics.h"
#include "../game/fade.h"
#include "../game/sound_pending_entries.h"
#include "../unmatched.h"

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))

void func_8004EB00(void)
{
    s32 (*handlers[4])(s32, s32);
    SVECTOR offset;
    ModelSlot *slot;
    u8 *r;
    ModelEffectCoefficient *coef;
    s32 n;
    s32 a;
    s32 b;
    s32 d;
    s32 mode;
    s32 off;
    s32 kind;
    s32 f;
    s32 c0;
    s32 c1;

    *(ModelSlotS32Quad *)handlers = *(ModelSlotS32Quad *)D_800114E8;
    n = 1;
    if (((gDuel_adwCardStats[D_800F2C40[1].field_DF8] >> 0x1A) & 0x1F) == 1) {
        n = 2;
    }
    switch ((s8)((u8)D_8009AF9A + 1)) {
    case 1:
        func_800595C8(2, -0x1000, -0x1000, -0x1000);
        func_80059590(0, 5, 0, 0, 0);
        func_80059590(1, 5, 0, 0, 0);
        D_8009AF9A++;
        break;
    case 2:
        func_80047314(D_8009AF96);
        D_8009AF9A++;
        break;
    case 3:
        *(ModelBytes8 *)&offset = *(ModelBytes8 *)D_8009AFEC;
        a = 0x78;
        switch (D_8009AF88[0xA0]) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            func_8005F27C(2, 0xA, 0);
            break;
        case 2:
            coef = func_8005F1A4(0xA);
            func_8005F3B8(2, 0x64, coef->angle + 0x800, -0x400, 0);
            offset.vy = -0x1000;
            offset.vx = coef->field_00;
            a = 0x96;
            offset.vz = coef->field_04 + 0x400;
            break;
        }
        func_8005F91C(0, (void *)&offset, (void *)0, a);
        D_8009AF9A++;
        break;
    case 4:
        if (D_800F2C40[2].field_DA0[0] < 0x800) {
            func_80059284(2, 3);
            D_800F2C40[2].field_DA0[0] += 0x40;
            D_800F2C40[2].field_DA0[2] += 0x40;
            D_800F2C40[2].field_DA0[1] += 0x40;
        } else if (func_8005FB08() != 0) {
            func_80059EBC(0);
            func_8005F588(1);
            func_8005F5C8(0, 0, 0, -0xC);
            func_8005F714(0, 0, 0x4000);
            D_8009AF9A++;
            break;
        }
        break;
    case 5:
        if (D_800F2C40[0].field_E1F != 0) {
            r = (u8 *)func_8005FB14();
            if (r != 0 && S(r, 6) == 2) {
                func_8005F588(1);
                func_8005F5C8(0, 0, 0, -0xC);
                func_8005F714(0, 0, 0x4000);
                D_8009AFA0 = 0;
                handlers[0](D_800F2C40[0].field_DEC, 0);
                D_800F2C40[0].field_E15 = 0;
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 6:
        slot = D_800F2C40;
        if (D_800F2C40[0].field_E15 == 0) {
            D_8009AFA0 = 0;
            if (handlers[0](D_800F2C40[0].field_DEC, -1) == 2) {
                D_800F2C40[0].field_E15++;
                goto fade_in0;
            }
        } else {
        fade_in0:
            if ((s8)D_800F2C40[0].field_DC0[0] >= 0) {
                D_800F2C40[0].field_DC0[0] += 2;
                c0 = D_800F2C40[0].field_DC0[1] + 2;
                c1 = D_800F2C40[0].field_DC0[2] + 2;
            colour:
                slot->field_DC0[1] = c0;
                slot->field_DC0[2] = c1;
            } else {
                if (D_800F2C40[0].field_E11 != 4) {
                    func_80059284(0, 3);
                }
                func_80059590(0, 0, 0x80, 0x80, 0x80);
                func_80059590(1, 0, 0x80, 0x80, 0x80);
                Model_ControlSlotAnimation(0, 2, 1);
                D_8009AFE9 = 0;
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 7:
        if (D_800F2C40[0].field_E11 != 4) {
            func_80059284(0, 3);
        }
        if (D_800F2C40[0].field_E0F == 0) {
            D_8009AFE9 += func_80058E1C();
            if (D_8009AFE9 >= 0x3C && D_800F2C40[1].field_E1F != 0) {
                func_8005F588(1);
                func_8005F5C8(1, 0, 0, -0xA);
                func_8005F714(1, 1, 0x4000);
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 8:
        if (D_800F2C40[0].field_E11 != 4) {
            func_80059284(0, 3);
        }
        r = (u8 *)func_8005FB14();
        if (r != 0 && S(r, 6) == 2) {
            func_8005F27C(1, 0, 0);
            D_8009AF9A++;
            break;
        }
        break;
    case 9:
        if ((s8)D_800F2C40[1].field_DC0[0] >= 0) {
            D_800F2C40[1].field_DC0[0] += 2;
            c0 = D_800F2C40[1].field_DC0[1] + 2;
            c1 = D_800F2C40[1].field_DC0[2] + 2;
        colour1:
            D_800F2C40[1].field_DC0[1] = c0;
            D_800F2C40[1].field_DC0[2] = c1;
        } else {
            Model_ControlSlotAnimation(1, 2, 1);
            D_8009AFE9 = 0;
            D_8009AF9A++;
            break;
        }
        break;
    case 10:
        if (D_800F2C40[1].field_E0F == 0) {
            D_8009AFE9 += func_80058E1C();
            if (D_8009AFE9 >= 0x3C) {
                kind = D_800F2C40[D_800F2C40[0].field_DFE]
                           .field_CF8.prefix.bytes.field_0A[0] & 0x1F;
                off = -0x14;
                if (kind == 1) {
                    mode = 0;
                } else if (kind < 2) {
                    mode = 3;
                } else if (kind == 2) {
                    mode = 1;
                } else if (kind == 3) {
                    mode = 2;
                } else {
                    mode = 3;
                }
                if ((u32)mode < 2) {
                    off = off / 2;
                }
                func_8005F5C8(0, mode, 0, off);
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 11:
        if (func_8005FB08() != 0) {
            func_80059F18(1, -1, 0, -0x1E);
            D_8009AF9A++;
            break;
        }
        break;
    case 12:
        if (func_8005A878(1) != 0) {
            a = func_8005A618(0);
            b = (func_80058E1C() + 2) * 8;
            d = (s16)D_8009B47A - a;
            if (d < 0) {
                d = a - (s16)D_8009B47A;
            }
            if (b >= d) {
                func_80050F24(0);
                D_8009AFA0 = 1;
                handlers[n](D_800F2C40[1].field_DEC, 0x18E);
                D_800F2C40[1].field_E15 = 0xFF;
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 13:
        switch (D_800F2C40[1].field_E0F) {
        case 5:
            a = 1;
            break;
        case 6:
            a = 2;
            break;
        case 8:
            a = 3;
            break;
        default:
            a = 0;
            break;
        }
        if (D_800F2C40[1].field_E15 == 0xFF && D_800F2C40[1].field_E0F == 8) {
            D_800F2C40[1].field_E15 = 0;
        }
        if (D_800F2C40[1].field_E15 == 0) {
            D_8009AFA0 = 1;
            handlers[n](D_800F2C40[1].field_DEC, -1);
        }
        if (D_800F2C40[0].field_E0E == 2) {
            if (D_800F2C40[0].field_E0F == 0) {
                a = 4;
            }
        } else if (func_8005F174() == 0) {
            a = 0;
        }
        if (a != 0) {
            b = func_8005A2E0(1) * 0x1E / 1000;
            if (func_8005F174() == 2) {
                func_8005F180(1);
            }
            func_8005F91C(0, (void *)0, (void *)0, 0);
            if (b < 0xA) {
                b = 0xA;
            }
            func_80059F18(1, -1, 1, b);
            D_8009AF9A++;
            break;
        }
        break;
    case 14:
        if (D_800F2C40[1].field_E0F != 6) {
            if (D_800F2C40[1].field_E15 == 0) {
                D_8009AFA0 = 1;
                handlers[n](D_800F2C40[1].field_DEC, -1);
            }
            if (func_8005A878(1) != 0 && func_8005FB08() != 0) {
                a = W(&D_800F2C40[0].field_E0C, 0) & 0xFFFF0000;
                if (a == 0x20000 && (W(&D_800F2C40[1].field_E0C, 0) & 0xFFFF0000) == a) {
                    if (D_800F2C40[1].field_DFF != 0) {
                        func_8005A0DC(0x78);
                        D_8009AF9A = 0x1E;
                    } else {
                        D_8009AF9A++;
                    }
                    break;
                }
            }
            break;
        }
        D_8009AF9A = 0x14;
        break;
    case 15:
        if (func_8005FB08() != 0) {
            func_80059F18(1, -1, 1, -0x1E);
            D_8009AF9A++;
            break;
        }
        break;
    case 16:
        if (func_8005A878(1) != 0) {
            a = func_8005A618(1);
            b = (func_80058E1C() + 2) * 8;
            d = (s16)D_8009B47A - a;
            if (d < 0) {
                d = a - (s16)D_8009B47A;
            }
            if (b >= d) {
                func_80050F24(1);
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 17:
        switch (D_800F2C40[0].field_E0F) {
        case 5:
            a = 1;
            break;
        case 6:
            a = 2;
            break;
        case 8:
            a = 3;
            break;
        default:
            a = 0;
            break;
        }
        if (D_800F2C40[1].field_E0E == 2) {
            if (D_800F2C40[1].field_E0F == 0) {
                a = 4;
            }
        } else if (func_8005F174() == 0) {
            a = 0;
        }
        if (a != 0) {
            b = func_8005A2E0(0) * 0x1E / 1000;
            if (func_8005F174() == 2) {
                func_8005F180(1);
            }
            func_8005F91C(0, (void *)0, (void *)0, 0);
            if (b < 0xA) {
                b = 0xA;
            }
            func_80059F18(1, -1, 0, b);
            D_8009AF9A++;
            break;
        }
        break;
    case 18:
        if (D_800F2C40[0].field_E0F != 6) {
            if (func_8005A878(1) != 0 && func_8005FB08() != 0) {
                a = W(&D_800F2C40[0].field_E0C, 0) & 0xFFFF0000;
                if (a == 0x20000 && (W(&D_800F2C40[1].field_E0C, 0) & 0xFFFF0000) == a) {
                    FntPrint(D_8009AFF4);
                }
            }
            break;
        }
        D_8009AF9A = 0x14;
        break;
    case 21:
        f = D_800F2C40[0].field_E0F != 6;
        slot = &D_800F2C40[f ^ 1];
        if (slot->field_E0E == 2 && slot->field_E0F == 0 && D_800F2C40[f].field_E16 == 0x23) {
            if (D_800F2C40[1].field_DFF == 0
                && H(&D_800F2C40[0].field_CF8, 0) == H(&D_800F2C40[1].field_CF8, 0)) {
                if (D_800F2C40[1].field_E13 == 0xFF) {
                    D_800F2C40[1].field_E13 = 0x80;
                }
                Model_ControlSlotAnimation(1, 6, 1);
            }
            func_8005F91C(0, (void *)0, (void *)0, 0);
            if (func_8005A878(1) != 0) {
                b = func_8005A2E0(f) * 0x1E / 1000;
                if (b < 0xA) {
                    b = 0xA;
                }
                func_80059F18(1, -1, f, -b);
            }
            D_8009AF9A++;
            break;
        }
        break;
    case 22:
        f = D_800F2C40[0].field_E0F != 6;
        slot = &D_800F2C40[f];
        if (slot->field_E13 == 0 && func_8005A878(1) != 0) {
            func_80059590(f, 5, 0x80, 0x80, 0x80);
            D_8009AFA0 = f;
            handlers[3](slot->field_DEC, 0);
            slot->field_E15 = 0;
            D_8009AF9A++;
            break;
        }
        break;
    case 23:
        f = D_800F2C40[0].field_E0F != 6;
        slot = &D_800F2C40[f];
        if (slot->field_E15 == 0) {
            D_8009AFA0 = f;
            if (handlers[3](slot->field_DEC, -1) == 2) {
                slot->field_E15++;
                goto fade_out;
            }
        } else {
        fade_out:
            if (slot->field_DC0[0] != 0) {
                slot->field_DC0[0] -= 2;
                c0 = slot->field_DC0[1] - 2;
                c1 = slot->field_DC0[2] - 2;
                goto colour;
            }
            if (f > 0) {
                a = (s16)D_8009B47A + 0xC00;
            } else {
                a = (s16)D_8009B47A + 0x400;
            }
            if ((u32)(a % 0x1000 - 0x201) < 0xBFF) {
                b = func_8005A2E0(f ^ 1) * 0x1E / 1000;
                if (b < 0x1E) {
                    b = 0x1E;
                }
                func_80059F18(1, -1, f ^ 1, -b);
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 24:
        f = D_800F2C40[0].field_E0F == 6;
        if (func_8005FB08() != 0) {
            a = (s16)D_8009B478;
            if (func_8005F1B8(f, 0x2BC) + 0xA >= a && func_8005A878(1) != 0) {
                if (H(&D_800F2C40[0].field_CF8, 0) == H(&D_800F2C40[1].field_CF8, 0)
                    && D_800F2C40[1].field_DFF == 0) {
                    D_8009AFA0 = 1;
                    handlers[3](D_800F2C40[1].field_DEC, 0);
                    D_800F2C40[1].field_E15 = 0;
                    D_8009AF9A = 0x19;
                    D_8009AFE9 = 0;
                } else {
                    func_8005F714(-1, f, 0x4000);
                    func_8005A010(-1, f);
                    Model_ControlSlotAnimation(f, 7, 1);
                    D_8009AF9A++;
                    break;
                }
            }
        }
        break;
    case 25:
        f = D_800F2C40[0].field_E0F == 6;
        if (D_800F2C40[f].field_E0F != 7) {
            func_8005F91C(0, (void *)0, (void *)0, 0);
            func_80059F18(1, -1, f, -0x1E);
            D_8009AFE9 = 0x3C;
            D_8009AF9A++;
            break;
        }
        break;
    case 26:
        f = D_800F2C40[0].field_E0F == 6;
        if (D_800F2C40[f].field_E13 == 0 && func_8005A878(1) != 0) {
            D_8009AFE9 += func_80058E1C();
            if (D_8009AFE9 >= 0x3C) {
                func_80059590(f, 5, 0x80, 0x80, 0x80);
                D_8009AF9A++;
                break;
            }
        }
        break;
    case 27:
        f = D_800F2C40[0].field_E0F == 6;
        if (H(&D_800F2C40[0].field_CF8, 0) == H(&D_800F2C40[1].field_CF8, 0)
            && D_800F2C40[1].field_DFF == 0 && D_800F2C40[1].field_E15 == 0) {
            D_8009AFA0 = 1;
            if (handlers[3](D_800F2C40[1].field_DEC, -1) != 2) {
                break;
            }
            D_800F2C40[1].field_E15++;
        }
        slot = &D_800F2C40[f];
        c0 = slot->field_DC0[0];
        if (c0 == 0) {
            D_8009AF9A++;
            break;
        }
        goto dim;
    case 28:
        if (func_8005A878(1) != 0) {
            D_8009AF9A = 0x28;
        }
        break;
    case 31:
        if (D_800F2C40[1].field_E15 == 0) {
            D_8009AFA0 = 1;
            if (handlers[n](D_800F2C40[1].field_DEC, -2) == 2) {
                D_800F2C40[1].field_E15++;
            }
        } else if (func_8005A878(1) != 0) {
            func_80059590(1, 5, 0x80, 0x80, 0x80);
            D_8009AF9A++;
            break;
        }
        break;
    case 32:
        if (D_800F2C40[1].field_DC0[0] != 0) {
            D_800F2C40[1].field_DC0[0] -= 2;
            c0 = D_800F2C40[1].field_DC0[1] - 2;
            c1 = D_800F2C40[1].field_DC0[2] - 2;
            goto colour1;
        }
        a = (s16)D_8009B47A + 0xC00;
        if ((u32)(a % 0x1000 - 0x201) < 0xBFF) {
            b = func_8005A2E0(0) * 0x1E / 1000;
            if (b < 0x1E) {
                b = 0x1E;
            }
            func_80059F18(1, -1, 0, -b);
            D_8009AF9A++;
            break;
        }
        break;
    case 33:
        if (func_8005FB08() != 0 && func_8005A878(1) != 0) {
            func_80059590(0, 5, 0x80, 0x80, 0x80);
            D_8009AF9A++;
            break;
        }
        break;
    case 34:
        slot = D_800F2C40;
        c0 = D_800F2C40[0].field_DC0[0];
        if (c0 == 0) {
            D_8009AF9A = 0x28;
            break;
        }
    dim:
        slot->field_DC0[0] = c0 - 2;
        slot->field_DC0[1] -= 2;
        slot->field_DC0[2] -= 2;
        break;
    case 41:
        func_80059284(2, 3);
        Fade_StartOut();
        D_8009AF9A++;
        break;
    case 42:
        func_80059284(2, 3);
        if (!(D_800E9ECE[0] & 0x80)) {
            func_8004763C();
            D_8009AF9A = -2;
            break;
        }
        break;
    }
    func_8005A188(1);
}
