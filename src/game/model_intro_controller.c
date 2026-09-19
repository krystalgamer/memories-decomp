/*
 * Model scene controllers: the mode 15 controller, then the mode 19 battle
 * intro and its two neighbours.
 *
 * func_8004EB00, the mode 15 controller, copies the scene's four effect
 * handlers and its eight-byte view offset, then steps the signed D_8009AF9A
 * phase through lighting, fades, effect requests and the per-slot handler
 * calls. It matches all 4,908 text bytes and owns the 43-entry phase table
 * and the 7-entry model-kind table that precede the intro controller's
 * tables. Each phase keeps its temporaries in its own block, which is what
 * lets retail reuse $s0 across phases; the angle-window tests keep a bound
 * compare in each signed arm, and phases 13 and 17 classify the animation
 * state with nested inequality tests rather than a switch.
 *
 * The view-offset words and the diagnostics string use their unknown-bound
 * declarations, which select retail's split absolute addresses. The two
 * intro-controller views are selected here as well, because this is the
 * unit's first inclusion of those headers.
 */
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#define MODEL_HANDLER_VIEW_OFFSET_ABSOLUTE
#define MODEL_HANDLER_OFFSET_ABSOLUTE
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "func_8004EB00.h"
#include "model.h"
#include "model_graphics_state.h"
#include "model_handler_state.h"
#include "duel_card.h"
#include "model_slot_properties.h"
#include "sound_output.h"
#include "model_effect_state.h"
#include "model_effect_coefficients.h"
#include "func_8005A188.h"
#include "func_8005A878.h"
#include "model_effect_wrapped_value.h"
#include "model_transfer_state.h"
#include "model_transfer_flags.h"
#include "func_80058E1C.h"
#include "model_control_slot_animation.h"
#include "func_8005106C.h"
#include "model_effect_requests.h"
#include "model_distance_queries.h"
#include "model_update_view_metrics.h"
#include "model_scene_states.h"
#include "fade.h"
#include "sound_pending_entries.h"
#include "../unmatched.h"

void func_8004EB00(void)
{
    s32 (*handlers[4])(s32, s32);
    SVECTOR offset;
    s32 n;

    *(ModelSlotS32Quad *)handlers = *(ModelSlotS32Quad *)D_800114E8;
    n = 1;
    if (((gDuel_adwCardStats[D_800F2C40[1].field_DF8] >> 0x1A) & 0x1F) == 1) {
        n = 2;
    }
    switch ((s8)(D_8009AF9A + 1)) {
    case 0:
        break;
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
    case 3: {
        ModelEffectCoefficient *coef;
        s32 a;

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
            offset.vx = coef->field_00;
            offset.vy = -0x1000;
            offset.vz = coef->field_04 + 0x400;
            a = 0x96;
            break;
        }
        func_8005F91C(0, (void *)&offset, (void *)0, a);
        D_8009AF9A++;
        break;
    }
    case 4:
        if (D_800F2C40[2].field_DA0[0] < 0x800) {
            func_80059284(2, 3);
            D_800F2C40[2].field_DA0[0] += 0x40;
            D_800F2C40[2].field_DA0[1] += 0x40;
            D_800F2C40[2].field_DA0[2] += 0x40;
        } else if (func_8005FB08() != 0) {
            func_80059EBC(0);
            func_8005F588(1);
            func_8005F5C8(0, 0, 0, -0xC);
            func_8005F714(0, 0, 0x4000);
            D_8009AF9A++;
        }
        break;
    case 5:
        if (D_800F2C40[0].field_E1F != 0) {
            s16 *r = (s16 *)func_8005FB14();
            if (r != 0 && r[3] == 2) {
                func_8005F588(1);
                func_8005F5C8(0, 0, 0, -0xC);
                func_8005F714(0, 0, 0x4000);
                D_8009AFA0 = 0;
                handlers[0](D_800F2C40[0].field_DEC, 0);
                D_800F2C40[0].field_E15 = 0;
                D_8009AF9A++;
            }
        }
        break;
    case 6:
        if (D_800F2C40[0].field_E15 == 0) {
            D_8009AFA0 = 0;
            if (handlers[0](D_800F2C40[0].field_DEC, -1) != 2) {
                break;
            }
            D_800F2C40[0].field_E15++;
        }
        if ((s8)D_800F2C40[0].field_DC0[0] >= 0) {
            D_800F2C40[0].field_DC0[0] += 2;
            D_800F2C40[0].field_DC0[1] += 2;
            D_800F2C40[0].field_DC0[2] += 2;
        } else {
            if (D_800F2C40[0].field_E11 != 4) {
                func_80059284(0, 3);
            }
            func_80059590(0, 0, 0x80, 0x80, 0x80);
            func_80059590(1, 0, 0x80, 0x80, 0x80);
            Model_ControlSlotAnimation(0, 2, 1);
            D_8009AFE9 = 0;
            D_8009AF9A++;
        }
        break;
    case 7:
        if (D_800F2C40[0].field_E11 != 4) {
            func_80059284(0, 3);
        }
        if (D_800F2C40[0].field_E0F == 0) {
            D_8009AFE9 += Model_GetFrameStep();
            if (D_8009AFE9 >= 0x3C && D_800F2C40[1].field_E1F != 0) {
                func_8005F588(1);
                func_8005F5C8(1, 0, 0, -0xA);
                func_8005F714(1, 1, 0x4000);
                D_8009AF9A++;
            }
        }
        break;
    case 8: {
        s16 *r;

        if (D_800F2C40[0].field_E11 != 4) {
            func_80059284(0, 3);
        }
        r = (s16 *)func_8005FB14();
        if (r != 0 && r[3] == 2) {
            func_8005F27C(1, 0, 0);
            D_8009AF9A++;
        }
        break;
    }
    case 9:
        if ((s8)D_800F2C40[1].field_DC0[0] >= 0) {
            D_800F2C40[1].field_DC0[0] += 2;
            D_800F2C40[1].field_DC0[1] += 2;
            D_800F2C40[1].field_DC0[2] += 2;
        } else {
            Model_ControlSlotAnimation(1, 2, 1);
            D_8009AFE9 = 0;
            D_8009AF9A++;
        }
        break;
    case 10:
        if (D_800F2C40[1].field_E0F == 0) {
            D_8009AFE9 += Model_GetFrameStep();
            if (D_8009AFE9 >= 0x3C) {
                s32 kind;
                s32 off;

                kind = D_800F2C40[0].field_CF8.prefix.bytes.field_0A[D_800F2C40[0].field_DFE] & 0x1F;
                off = -0x14;
                if (kind == 1) {
                    goto s10_m0;
                }
                if (kind < 2) {
                    goto s10_m3;
                }
                if (kind == 2) {
                    goto s10_m1;
                }
                if (kind == 3) {
                    goto s10_m2;
                }
                kind = 3;
                goto s10_teste;
            s10_m0:
                kind = 0;
                goto s10_teste;
            s10_m1:
                kind = 1;
                goto s10_teste;
            s10_m2:
                kind = 2;
                goto s10_teste;
            s10_m3:
                kind = 3;
            s10_teste:
                if ((u32)kind < 2) {
                    off = off / 2;
                }
                func_8005F5C8(0, kind, 0, off);
                D_8009AF9A++;
            }
        }
        break;
    case 11:
        if (func_8005FB08() != 0) {
            func_80059F18(1, -1, 0, -0x1E);
            D_8009AF9A++;
        }
        break;
    case 12:
        if (func_8005A878(1) != 0) {
            s32 a;
            s32 b;
            s32 d;

            a = func_8005A618(0);
            b = (Model_GetFrameStep() + 2) * 8;
            d = (s16)D_8009B47A - a;
            if (d >= 0 ? b >= d : b >= a - (s16)D_8009B47A) {
                func_80050F24(0);
                D_8009AFA0 = 1;
                handlers[n](D_800F2C40[1].field_DEC, 0x18E);
                D_800F2C40[1].field_E15 = 0xFF;
                D_8009AF9A++;
            }
        }
        break;
    case 13: {
        s32 a;
        s32 cl;

        if (D_800F2C40[1].field_E0F != 5) {
            if (D_800F2C40[1].field_E0F != 6) {
                if (D_800F2C40[1].field_E0F == 8) {
                    a = 3;
                } else {
                    a = 0;
                }
            } else {
                a = 2;
            }
        } else {
            a = 1;
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
            a = func_8005A2E0(1) * 0x1E / 1000;
            if (func_8005F174() == 2) {
                func_8005F180(1);
            }
            func_8005F91C(0, (void *)0, (void *)0, 0);
            cl = a;
            if (cl < 0xA) {
                cl = 0xA;
            }
            func_80059F18(1, -1, 1, cl);
            D_8009AF9A++;
        }
        break;
    }
    case 14:
        if (D_800F2C40[1].field_E0F == 6) {
            D_8009AF9A = 0x14;
            break;
        }
        if (D_800F2C40[1].field_E15 == 0) {
            D_8009AFA0 = 1;
            handlers[n](D_800F2C40[1].field_DEC, -1);
        }
        if (func_8005A878(1) != 0 && func_8005FB08() != 0
            && D_800F2C40[0].field_E0E == 2 && D_800F2C40[0].field_E0F == 0
            && D_800F2C40[1].field_E0E == 2 && D_800F2C40[1].field_E0F == 0) {
            if (D_800F2C40[1].field_DFF != 0) {
                func_8005A0DC(0x78);
                D_8009AF9A = 0x1E;
            } else {
                D_8009AF9A++;
            }
        }
        break;
    case 15:
        if (func_8005FB08() != 0) {
            func_80059F18(1, -1, 1, -0x1E);
            D_8009AF9A++;
        }
        break;
    case 16:
        if (func_8005A878(1) != 0) {
            s32 a;
            s32 b;
            s32 d;

            a = func_8005A618(1);
            b = (Model_GetFrameStep() + 2) * 8;
            d = (s16)D_8009B47A - a;
            if (d >= 0 ? b >= d : b >= a - (s16)D_8009B47A) {
                func_80050F24(1);
                D_8009AF9A++;
            }
        }
        break;
    case 17: {
        s32 a;
        s32 cl;

        if (D_800F2C40[0].field_E0F != 5) {
            if (D_800F2C40[0].field_E0F != 6) {
                if (D_800F2C40[0].field_E0F == 8) {
                    a = 3;
                } else {
                    a = 0;
                }
            } else {
                a = 2;
            }
        } else {
            a = 1;
        }
        if (D_800F2C40[1].field_E0E == 2) {
            if (D_800F2C40[1].field_E0F == 0) {
                a = 4;
            }
        } else if (func_8005F174() == 0) {
            a = 0;
        }
        if (a != 0) {
            a = func_8005A2E0(0) * 0x1E / 1000;
            if (func_8005F174() == 2) {
                func_8005F180(1);
            }
            func_8005F91C(0, (void *)0, (void *)0, 0);
            cl = a;
            if (cl < 0xA) {
                cl = 0xA;
            }
            func_80059F18(1, -1, 0, cl);
            D_8009AF9A++;
        }
        break;
    }
    case 18:
        if (D_800F2C40[0].field_E0F == 6) {
            D_8009AF9A = 0x14;
            break;
        }
        if (func_8005A878(1) != 0 && func_8005FB08() != 0
            && D_800F2C40[0].field_E0E == 2 && D_800F2C40[0].field_E0F == 0
            && D_800F2C40[1].field_E0E == 2 && D_800F2C40[1].field_E0F == 0) {
            FntPrint(D_8009AFF4);
        }
        break;
    case 21: {
        s32 f = D_800F2C40[0].field_E0F != 6;
        ModelSlot *slot = &D_800F2C40[f ^ 1];

        if (slot->field_E0E == 2) {
            if (slot->field_E0F == 0 && D_800F2C40[f].field_E16 == 0x23) {
                if (D_800F2C40[1].field_DFF == 0
                    && D_800F2C40[0].field_CF8.prefix.values.field_00 == D_800F2C40[1].field_CF8.prefix.values.field_00) {
                    if (D_800F2C40[1].field_E13 == 0xFF) {
                        D_800F2C40[1].field_E13 = 0x80;
                    }
                    Model_ControlSlotAnimation(1, 6, 1);
                }
                func_8005F91C(0, (void *)0, (void *)0, 0);
                if (func_8005A878(1) != 0) {
                    s32 b = func_8005A2E0(f) * 0x1E / 1000;
                    if (b < 0xA) {
                        b = 0xA;
                    }
                    func_80059F18(1, -1, f, -b);
                }
                D_8009AF9A++;
            }
        }
        break;
    }
    case 22: {
        s32 f = D_800F2C40[0].field_E0F != 6;
        ModelSlot *slot = &D_800F2C40[f];

        if (slot->field_E13 == 0 && func_8005A878(1) != 0) {
            func_80059590(f, 5, 0x80, 0x80, 0x80);
            D_8009AFA0 = f;
            handlers[3](slot->field_DEC, 0);
            slot->field_E15 = 0;
            D_8009AF9A++;
        }
        break;
    }
    case 23: {
        s32 f = D_800F2C40[0].field_E0F != 6;
        ModelSlot *slot = &D_800F2C40[f];
        s32 a;
        s32 v;

        if (slot->field_E15 == 0) {
            D_8009AFA0 = f;
            if (handlers[3](slot->field_DEC, -1) != 2) {
                break;
            }
            slot->field_E15++;
        }
        if (slot->field_DC0[0] != 0) {
            slot->field_DC0[0] -= 2;
            slot->field_DC0[1] -= 2;
            slot->field_DC0[2] -= 2;
            break;
        }
        v = (s16)D_8009B47A;
        if (f > 0) {
            a = (v + 0xC00) % 0x1000;
        } else {
            a = (v + 0x400) % 0x1000;
        }
        if ((u32)(a - 0x201) < 0xBFF) {
            s32 g = f ^ 1;
            s32 b = func_8005A2E0(g) * 0x1E / 1000;

            if (b < 0x1E) {
                b = 0x1E;
            }
            func_80059F18(1, -1, g, -b);
            D_8009AF9A++;
        }
        break;
    }
    case 24: {
        s32 f = D_800F2C40[0].field_E0F == 6;

        if (func_8005FB08() != 0) {
            s32 a = (s16)D_8009B478;
            if (func_8005F1B8(f, 0x2BC) + 0xA >= a && func_8005A878(1) != 0) {
                if (D_800F2C40[0].field_CF8.prefix.values.field_00 == D_800F2C40[1].field_CF8.prefix.values.field_00
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
                }
            }
        }
        break;
    }
    case 25: {
        s32 f = D_800F2C40[0].field_E0F == 6;

        if (D_800F2C40[f].field_E0F != 7) {
            func_8005F91C(0, (void *)0, (void *)0, 0);
            func_80059F18(1, -1, f, -0x1E);
            D_8009AFE9 = 0x3C;
            D_8009AF9A++;
        }
        break;
    }
    case 26: {
        s32 f = D_800F2C40[0].field_E0F == 6;

        if (D_800F2C40[f].field_E13 == 0 && func_8005A878(1) != 0) {
            D_8009AFE9 += Model_GetFrameStep();
            if (D_8009AFE9 >= 0x3C) {
                func_80059590(f, 5, 0x80, 0x80, 0x80);
                D_8009AF9A++;
            }
        }
        break;
    }
    case 27: {
        s32 f = D_800F2C40[0].field_E0F == 6;
        ModelSlot *slot;
        ModelSlot *base;

        if (D_800F2C40[0].field_CF8.prefix.values.field_00 == D_800F2C40[1].field_CF8.prefix.values.field_00
            && D_800F2C40[1].field_DFF == 0 && D_800F2C40[1].field_E15 == 0) {
            D_8009AFA0 = 1;
            if (handlers[3](D_800F2C40[1].field_DEC, -1) != 2) {
                break;
            }
            D_800F2C40[1].field_E15++;
        }
        base = D_800F2C40;
        slot = &base[f];
        if (slot->field_DC0[0] != 0) {
            slot->field_DC0[0] -= 2;
            slot->field_DC0[1] -= 2;
            slot->field_DC0[2] -= 2;
        } else {
            D_8009AF9A++;
        }
        break;
    }
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
        }
        break;
    case 32:
        if (D_800F2C40[1].field_DC0[0] != 0) {
            D_800F2C40[1].field_DC0[0] -= 2;
            D_800F2C40[1].field_DC0[1] -= 2;
            D_800F2C40[1].field_DC0[2] -= 2;
        } else {
            s32 a = (s16)D_8009B47A + 0xC00;

            if ((u32)(a % 0x1000 - 0x201) < 0xBFF) {
                s32 b = func_8005A2E0(0) * 0x1E / 1000;
                if (b < 0x1E) {
                    b = 0x1E;
                }
                func_80059F18(1, -1, 0, -b);
                D_8009AF9A++;
            }
        }
        break;
    case 33:
        if (func_8005FB08() != 0 && func_8005A878(1) != 0) {
            func_80059590(0, 5, 0x80, 0x80, 0x80);
            D_8009AF9A++;
        }
        break;
    case 34:
        if (D_800F2C40[0].field_DC0[0] != 0) {
            D_800F2C40[0].field_DC0[0] -= 2;
            D_800F2C40[0].field_DC0[1] -= 2;
            D_800F2C40[0].field_DC0[2] -= 2;
        } else {
            D_8009AF9A = 0x28;
        }
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
        }
        break;
    }
    func_8005A188(1);
}

/*
 * Scene mode 19's intro controller. The control-module entry points and
 * eight-byte view offset are captured before the frame-step setter; the
 * signed phase, module data arguments and model fields remain live.
 *
 * The full reconstruction matches all 1,880 text bytes and the 17-entry
 * signed-phase table with the uniform GCC 2.8.1 G8 split-address profile.
 * The byte block-move view preserves the original unaligned eight-byte copy.
 * The timing callback is sequenced before the final live scale-byte read.
 */
#include "../psyq/libgte.h"
#include "model.h"
#include "model_load_step.h"
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "model_graphics_state.h"
#include "model_slot_properties.h"
#include "model_slot_state_updates.h"
#include "model_state_setters.h"
#include "model_transfer_state.h"
#include "model_effect_state.h"
#include "func_80058E1C.h"
#include "model_control_slot_animation.h"
#include "func_8005D994.h"
#include "file_transfer.h"
#include "high_memory_addresses.h"
#include "fade.h"
#include "sound.h"
#include "sound_output.h"
#include "../unmatched.h"

#define MODEL_HANDLER_OFFSET_ABSOLUTE
#include "model_handler_state.h"
#include "model_scene_states.h"

void func_8004FE2C(void)
{
    SVECTOR offset;
    ModelSlot *slot;
    s32 (*first)(s32, s32);
    s32 (*second)(s32, s32);

    *(ModelBytes8 *)&offset = *(ModelBytes8 *)D_8009AFFC;
    slot = D_800F2C40;
    first = (s32 (*)(s32, s32))(D_80010014 + 4);
    second = (s32 (*)(s32, s32))(D_80010018 + 4);
    Model_SetFrameStepOverride(1);
    if ((u32)((u8)D_8009AF9A - 8) < 2 && slot->field_E15 == 0) {
        D_8009AFA0 = 0;
        if (first(D_80010024, -1) == 2) {
            slot->field_E15++;
        }
    }
    if (D_8009AF9A >= 10 && D_800F2C40[1].field_E15 == 0) {
        D_8009AFA0 = 0;
        if (second(D_80010028, -1) == 2) {
            func_80059700(0, 0);
            func_800156DC();
            D_8009AF9A = 15;
        }
    }
    switch ((s8)(D_8009AF9A + 1)) {
    case 0: {
        ModelSlot *current;
        if (D_800F2C40[0].field_E14 == 0xFF) {
            if ((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs) {
                goto check_ready;
            }
            Model_LoadMonsterMerge(
                0, MODEL_SPECIAL_BATTLE_ID, 0, 0, 0, 0, 4);
        } else {
            func_80056828(0);
        }
check_ready:
        current = D_800F2C40;
        if (!current->field_E1F) {
            break;
        }
        func_8005F3B8(0, 10000, 0xE00, 0, 0);
        func_800597C8(0, 1, 0);
        if (slot->field_E0F) {
            Model_ControlSlotAnimation(0, 0, 0);
        }
        func_80059700(0, 1);
        func_80059590(0, 5, 0, 0, 0);
        slot->field_BF6 = 1;
        slot->field_BF4 = 2;
        func_80047314(0x7310);
        current->field_CF8.field_10 = -1;
        current->field_CF8.field_14 = -1;
        current->field_CF8.field_18 = -1;
        D_8009AFA0 = 0;
        first(D_80010024, 0);
        second(D_80010028, 0);
        current[0].field_E15 = 0;
        current[1].field_E15 = 0;
        D_8009AF9A++;
        break;
    }
    case 1:
        if (func_8004703C() & 0x80) {
            offset.vy = -2000;
            func_800597C8(0, 0, 10);
            func_8005D994(0, 2000, 0x100, 0x100, &offset, 110);
            D_8009AF9A++;
        }
        break;
    case 2: {
        ModelSlot *current = D_800F2C40;
        if ((s8)current->field_DC0[0] >= 0) {
            current->field_DC0[0] += 2;
            current->field_DC0[1] += 2;
            current->field_DC0[2] += 2;
        } else {
            func_80059590(0, 0, 0x80, 0x80, 0x80);
            D_8009AF9A++;
        }
        break;
    }
    case 3:
        if (func_8005FB08() && slot->field_E06 >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A++;
        }
        break;
    case 4:
        if (slot->field_E06 >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A++;
        }
        break;
    case 5:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = -2500;
            offset.vy = -1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, -0x100, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 6:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = 2500;
            offset.vy = -1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, 0x100, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 7:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = -500;
            offset.vy = 1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, -0x200, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 8:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = 500;
            offset.vy = 1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, 0x200, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 9:
        if (slot->field_E06 >= 0x1E0) {
            offset.vy = -2000;
            func_8005F3B8(0, 3500, 0x40, 0x200, &offset);
            D_8009AF9A++;
        }
        break;
    case 10:
        if (slot->field_BF5 == 2) {
            D_8009AF9A++;
        }
        break;
    case 11:
        if (slot->field_E06 >= 0x8C0) {
            offset.vy = -1500;
            func_8005D994(0, 3500, 0, -0x80, &offset, 40);
            D_8009AF9A++;
        }
        break;
    case 12:
        if (slot->field_E06 >= 0xE60) {
            offset.vz = -1000;
            func_8005F3B8(0, 5000, 0x200, -0x100, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 13:
        if (slot->field_E06 >= 0xE60) {
            offset.vz = -1000;
            func_8005F3B8(0, 5000, -0x200, -0x100, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 14:
        if (slot->field_E06 >= 0xE60) {
            offset.vy = -1000;
            func_8005F3B8(0, 3000, 0, -0x80, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 15: {
        s32 remaining = slot->field_750[slot->field_BF5].max << 4;
        s32 timing = Model_GetFrameStep();
        remaining -= slot->field_E0D * timing;
        if (slot->field_E06 >= remaining) {
            func_80059700(0, 0);
        }
        break;
    }
    case 16:
        D_8009AF9A = -2;
        break;
    }
}

#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "model.h"
#include "model_load_step.h"
#include "func_8005922C.h"
#include "model_cleanup.h"
#include "model_scene_states.h"

void func_80050584(s32 arg0) {
    ModelSlot *p;
    ModelSlot *b;
    ModelSlot *b0;
    ModelSlot *q;
    GsCOORDUNIT *r;
    ModelSlot *s;
    s32 v;
    s32 t;
    s32 a;
    s32 m1;

    b0 = D_800F2C40;
    p = b0 + arg0;
    if (p->field_E1F == 0) {
        if (p->field_E14 == 0xFF) {
            if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 D_8009B134_abs) == 0) {
                do {
                    t = rand() >> 8;
                    v = t % CARD_COUNT;
                } while (v < 0 || v >= CARD_COUNT ||
                         (v >= MODEL_MRG_FIRST_GAP_START
                          && v < MODEL_MRG_FIRST_GAP_END) ||
                         (v >= MODEL_MRG_SECOND_GAP_START
                          && v < MODEL_MRG_SECOND_GAP_END) ||
                         v == MODEL_MRG_SINGLE_GAP_ID);
                m1 = -1;
                Model_LoadMonsterMerge(arg0 | 0x80, v, m1, m1, m1, m1, 0);
            }
        } else {
            func_80056828(arg0);
        }

        b = D_800F2C40;
        if ((b + arg0)->field_E1F != 0) {
            q = b + (arg0 ^ 1);
            a = MODEL_ANGLE_QUARTER_TURN;
            if (q->field_E1F != 0) {
                r = q->field_D18;
                if (r != (GsCOORDUNIT *)0) {
                    t = r->rot.vy + MODEL_ANGLE_QUARTER_TURN;
                    a = t / MODEL_ANGLE_FULL_TURN;
                    a = t - a * MODEL_ANGLE_FULL_TURN;
                }
            }
            s = D_800F2C40 + arg0;
            if (s->field_D18 != (GsCOORDUNIT *)0) {
                s->field_D18->rot.vx = 0;
                s->field_D18->rot.vy = a;
                s->field_D18->rot.vz = 0;
                s->field_D18->matrix.t[0] = 0;
                s->field_D18->matrix.t[1] = 0;
                s->field_D18->matrix.t[2] = 0;
            }
            func_8005922C(s->field_D18, 0);
            Model_RunSlotHandlers(arg0);
            s->field_E15 = 0;
        }
    }
}

#include "../psyq/rand.h"
#include "model.h"
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "model_graphics_state.h"
#include "model_scene_states.h"
#include "model_scene_imports.h"
#include "high_memory_addresses.h"
#include "model_handler_state.h"
#include "model_state_setters.h"
#include "model_slot_properties.h"
#include "model_effect_requests.h"
#include "model_control_slot_animation.h"
#include "file_transfer.h"
#include "file_names.h"
#include "sound_init.h"
#include "sound_output.h"
#include "sound_pending_entries.h"

#define ACTIVE_SLOT D_8009AFA4[3]
#define MODEL_SLOT_VIEW(slot) ((ModelSlot *)(slot))

void func_800507D0(void)
{
  s8 phase_value;
  ModelSlot *first_base;
  ModelSlot *cross_base;
  ModelSlot *lookup_base;
  ModelSlot *fade_models;
  u8 *fade_a;
  u8 *fade_b;
  s32 active_offset;
  u8 *active_color;
  s32 red_sample;
  s32 red_target;
  s32 green_sample;
  s32 green_target;
  s32 red;
  s32 blue_target;
  s8 next_red;
  s32 green;
  s8 next_green;
  s32 blue;
  s8 next_blue;
  s32 *active_model;
  u32 frame;
  s32 animation;
  s8 next_phase;
  s32 peer;
  s32 crossfade_offset;
  u8 *outgoing_color;
  u8 *incoming_color;
  u32 peer_red;
  u32 peer_green;
  u32 peer_blue;
  s32 result;

  Model_SetFrameStepOverride(1);
  switch ( D_8009AF9A )
  {
    case -1:
      File_RequestAsyncTransfer(1, D_800114F8, 1223, 16, 0, 0, D_80010030);
      File_WaitForTransfers();
      D_8009B004.fields.field_01 = 0;
      D_8009B004.fields.field_00 = 0;
      ACTIVE_SLOT = 0;
      D_800F2C40[0].field_E1F = 0;
      D_800F2C40[1].field_E1F = 0;
      func_80059590(0, 2, 0, 0, 0);
      func_80059590(1, 2, 0, 0, 0);
      func_801807B0();
      func_80181C4C(0);
      func_80049394((u16 *)D_80010034);
      func_80047314(29488);
      phase_value = D_8009AF9A + 1;
      goto store_phase;
    case 0:
      first_base = D_800F2C40;
      active_offset = (u32)&((ModelSlot *)0)[(u8)ACTIVE_SLOT];
      if ( MODEL_SLOT_VIEW((u8 *)first_base + active_offset)->field_E1F )
      {
        if ( (MODEL_SLOT_VIEW((u8 *)D_800F2C40 + active_offset)->field_E15 & 3) == 0 )
        {
          active_color = MODEL_SLOT_VIEW((u8 *)D_800F2C40 + active_offset)->field_DC0;
          red_sample = rand() >> 8;
          red_sample %= 24;
          red_target = red_sample + 8;
          green_sample = rand() >> 8;
          green_sample %= 24;
          green_target = green_sample + 8;
          blue_target = rand() >> 8;
          blue_target %= 24;
          blue_target += 8;
          red = active_color[0];
          if ( red != red_target )
          {
            if ( red >= red_target )
              next_red = red - 1;
            else
              next_red = red + 1;
            active_color[0] = next_red;
          }
          green = active_color[1];
          if ( green != green_target )
          {
            if ( green >= green_target )
              next_green = green - 1;
            else
              next_green = green + 1;
            active_color[1] = next_green;
          }
          blue = active_color[2];
          if ( blue != blue_target )
          {
            if ( blue >= blue_target )
              next_blue = blue - 1;
            else
              next_blue = blue + 1;
            active_color[2] = next_blue;
          }
        }
        if ( !D_8009B004.fields.field_01 )
          func_80050584((u8)ACTIVE_SLOT ^ 1);
        lookup_base = D_800F2C40;
        active_model = (s32 *)&lookup_base[(u8)ACTIVE_SLOT];
        if ( !MODEL_SLOT_VIEW(active_model)->field_E0F )
        {
          frame = MODEL_SLOT_VIEW(active_model)->field_E15;
          animation = -1;
          if ( !(u8)(frame % 0x1E) )
          {
            switch ( (u8)(frame / 0x1E) )
            {
              case 1u:
                animation = 2;
                break;
              case 3u:
                animation = 7;
                break;
              case 0u:
              case 2u:
              case 4u:
                animation = 1;
                break;
              default:
                break;
            }
            if ( animation < 0 )
            {
              if ( D_8009B004.fields.field_01 )
              {
                next_phase = 2;
                goto set_phase_and_tick;
              }
              peer = (u8)ACTIVE_SLOT ^ 1;
              if ( MODEL_SLOT_VIEW((u8 *)D_800F2C40 + (904 * peer) * 4)->field_E1F )
              {
                func_80059F18(1, -1, peer, 90);
                next_phase = D_8009AF9A + 1;
                goto set_phase_and_tick;
              }
            }
            else
            {
              Model_ControlSlotAnimation((u8)ACTIVE_SLOT, animation, 1);
            }
          }
          goto tick_active_slot;
        }
      }
      else
      {
        func_80050584((u8)ACTIVE_SLOT);
        if ( MODEL_SLOT_VIEW(
                 (u8 *)D_800F2C40 + (904 * (u8)ACTIVE_SLOT) * 4)->field_E1F )
          func_80059F18(1, -1, (u8)ACTIVE_SLOT, 30);
      }
      goto poll_module;
    case 1:
      cross_base = D_800F2C40;
      crossfade_offset = 904 * (u8)ACTIVE_SLOT;
      if ( (MODEL_SLOT_VIEW(
                (u8 *)cross_base + (crossfade_offset) * 4)->field_E15 & 3) == 0 )
      {
        outgoing_color = MODEL_SLOT_VIEW(
            (u8 *)D_800F2C40 + (crossfade_offset) * 4)->field_DC0;
        incoming_color = MODEL_SLOT_VIEW(
            (u8 *)D_800F2C40 + (904 * ((u8)ACTIVE_SLOT ^ 1)) * 4)->field_DC0;
        if ( outgoing_color[0] )
          --outgoing_color[0];
        if (outgoing_color[1])
          --outgoing_color[1];
        if (outgoing_color[2])
          --outgoing_color[2];
        peer_red = incoming_color[0];
        if ( peer_red < 8 )
          incoming_color[0] = peer_red + 1;
        peer_green = incoming_color[1];
        if ( peer_green < 8 )
          incoming_color[1] = peer_green + 1;
        peer_blue = incoming_color[2];
        if ( peer_blue < 8 )
          incoming_color[2] = peer_blue + 1;
        if ( !outgoing_color[0] && !outgoing_color[1] && !outgoing_color[2] )
        {
          MODEL_SLOT_VIEW(
              (u8 *)cross_base + (904 * (u8)ACTIVE_SLOT) * 4)->field_E1F = 0;
          D_8009AF9A = 0;
          ACTIVE_SLOT ^= 1u;
        }
      }
      goto tick_active_slot;
    case 2:
      fade_models = D_800F2C40;
      if ( (MODEL_SLOT_VIEW(
                (u8 *)fade_models + (904 * (u8)ACTIVE_SLOT) * 4)->field_E15 & 3) != 0 )
        goto tick_active_slot;
      fade_a = fade_models[0].field_DC0;
      fade_b = fade_models[1].field_DC0;
      if ( fade_models[0].field_DC0[0] )
        --fade_models[0].field_DC0[0];
      if ( fade_a[1] )
        --fade_a[1];
      if ( fade_a[2] )
        --fade_a[2];
      if ( fade_models[1].field_DC0[0] )
        fade_models[1].field_DC0[0] = fade_models[1].field_DC0[0] - 1;
      if ( fade_b[1] )
        --fade_b[1];
      if ( fade_b[2] )
        --fade_b[2];
      if ( fade_models[0].field_DC0[0]
        || fade_a[1] || fade_a[2]
        || fade_models[1].field_DC0[0]
        || fade_b[1] || fade_b[2] )
      {
        goto tick_active_slot;
      }
      fade_models[1].field_E1F = 0;
      fade_models[0].field_E1F = 0;
      next_phase = D_8009AF9A + 1;
      goto set_phase_and_tick;
set_phase_and_tick:
      D_8009AF9A = next_phase;
tick_active_slot:
      ++MODEL_SLOT_VIEW((u8 *)D_800F2C40 + (904 * (u8)ACTIVE_SLOT) * 4)->field_E15;
      goto poll_module;
    case 3:
      func_800493F8();
      func_8004763C();
      phase_value = -2;
store_phase:
      D_8009AF9A = phase_value;
      goto poll_module;
    default:
      goto poll_module;
  }
poll_module:
      if ( D_8009B004.fields.field_00 )
      {
        result = func_80180A24();
        if ( result )
        {
          result = D_8009B004.fields.field_01;
          if ( !D_8009B004.fields.field_01 )
          {
            result = (s16)func_8004703C();
            if ( (s16)result != 128 )
            {
              ++D_8009B004.fields.field_01;
              D_8009AF9A = 2;
            }
          }
        }
      }
      else
      {
        if (func_8004703C() & 0x80)
        {
          result = (u8)D_800F2C40[0].field_E1F;
          if ( D_800F2C40[0].field_E1F )
          {
            result = D_8009B004.fields.field_00 + 1;
            ++D_8009B004.fields.field_00;
          }
        }
      }
      return;
}
