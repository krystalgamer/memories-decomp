/*
 * Library screen state 2 handler: opens the card view, runs the model and
 * text-box slide-in, rotates the model light with the view angle, and walks
 * the close sequence back to the grid. Current best under
 * gcc_2_8_1_cc_g8_as_g0_split: 894 instructions against 895 with an opcode
 * census of addiu -1, addu +2, nop -2 (distance 5), with no hard register
 * assignments and no inline assembly.
 *
 * Levers measured on this body:
 * - the two slide states share one settle block, one Widget_SlideSine call
 *   and one TextBox_SetPos call placed after state 2;
 * - state 7 reads the target and viewport halfwords signed;
 * - the 0x80181000 halfwords at +2 and +4 go through a named unsigned read
 *   before the (s16) extension;
 * - state 5 re-reads the pad through a volatile access;
 * - state 6 tests the fade byte as a named (s8) value;
 * - the entry block clears vrx/vry/vrz through a pointer to the view;
 * - each arm has its own names for its scratch values;
 * - the compiler runs at -G8 with a -G0 assembler, so D_8009B338 and
 *   D_8009B0C0 are stored through $at as in retail, while D_800E9ECF keeps
 *   its aggregate arm and a split %hi/%lo store;
 * - the close path reads DisplayObject_SetResourceVariant's argument into a
 *   local before clearing D_8009B0C0, and the 0x10 counter is re-read after
 *   its store.
 *
 * Residual: the target reloads the slide phase into the register it loaded
 * it from (two load-delay nops) and materialises the case 5 pad address
 * twice; the addu +2 is not attributed yet.
 */
#define D_800E9ECE_AS_SCALAR
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../game/view_state.h"
#include "../game/main_services.h"
#include "../game/graphics_frame.h"
#include "../game/duel_card.h"
#include "../game/duel_effect.h"
#include "../game/input.h"
#include "../game/file_transfer.h"
#include "../game/duel_effect_resource_record.h"
#include "../game/display_object.h"
#include "../game/display_object_core.h"
#include "../game/display_object_helpers.h"
#include "../game/display_object_interpolation.h"
#include "../game/text_box_runtime.h"
#include "../game/text_box_lifecycle.h"
#include "../game/sound.h"
#include "../game/model_scene_setup.h"
#include "../game/duel_effect_resource_setup.h"
#include "../game/func_8002ABB4.h"
#include "../game/func_800291E0.h"
#include "../game/func_80029934.h"
#include "../game/model.h"
#include "../game/model_load_step.h"
#include "../game/func_80058DD8.h"
#include "../game/model_control_slot_animation.h"
#include "../game/model_copy_slot_u16_values.h"
#include "../game/func_80059AA8.h"
#include "../game/model_state_setters.h"
#include "../game/display_object_config.h"
#include "../game/func_80039794.h"
#include "../game/fade.h"
#include "../game/model_slot_properties.h"
#include "../game/model_slot_state_updates.h"
#include "../game/model_effect_coefficients.h"
#include "../unmatched.h"

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))

extern u8 *D_800EB24C;
unsigned char *Model_GetFlatLight(unsigned int index, unsigned int light);

void func_8002ACA4(u8 *state)
{
    DuelEffectResourceRecord *rec;
    u8 *o;
    u8 *light;
    s32 flags;
    s32 sel;
    s32 id;
    s32 i;
    s32 x;
    s32 phase;
    s32 a;
    s32 b;
    s32 c;
    s32 t;
    s32 v;
    s32 t_b1;
    s32 t_b4;
    s32 t_b7;
    s32 v_a1;
    s32 v_a2;
    s32 v_b0;
    s32 v_b1;
    s32 v_b4;
    s32 v_b6;
    s32 v_b7;
    GsRVIEW2 *rv;
    u8 *p;
    ViewState *vs;

    rec = D_800EA0E8;
    flags = state[0];
    if (!(flags & 0x80)) {
        state[0] = flags | 0x80;
        state[1] = 0;
        state[3] = 0;
        state[4] = 1;
        func_800530C4();
        func_800533D8();
        D_800F2848.field_00 = 0x334;
        D_800F2848.angle = 0x400;
        D_800F2848.field_04 = 0xC4;
        D_800F2848.field_0C = 0;
        D_800F2848.view.rz = 0;
        D_800F2848.view.super = 0;
        D_800F2848.projection = 0x12C;
        GsSetProjection(0x12C);
        rv = &D_800F2848.view;
        rv->vrx = 0;
        D_800F2848.field_06 = 0;
        rv->vry = 0;
        D_800F2848.field_08 = 0;
        rv->vrz = 0;
        D_800F2848.field_0A = 0;
        func_8001352C();
        func_80029164(0, H(state, 6));
        func_80015C84();
        D_800E9ECF[0] = 6;
        W(state, 0x50) = 0;
        return;
    }
    func_80039794();
    sel = state[4];
    if ((sel & 0xF) && ((D_8009B0F4 & 0x02000030) | D_8009B134) == 0) {
        if (sel == 1) {
            state[4] = 0;
            id = H(state, 6) - 1;
            if (((gDuel_adwCardStats[id] >> 0x1A) & 0x1F) < 0x14) {
                Model_LoadMonsterMerge(0, id, 0, 0, 0, 0, 4);
                W(state, 0x20) = 0x12C;
                state[4] = 2;
            }
        } else {
            func_80056828(0);
            if (func_80058DD8(0) == 1) {
                state[4] = 0;
                o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(o, 0x130, 0xCD, 3, 0, 2, 0xB, 0x20C);
                H(o, 8) |= 0x28;
                DisplayObject_SelectOrderingTable1((DisplayObject *)o);
                DisplayObject_SetDepthOffset((DisplayObject *)o, 0xA);
                W(state, 0x50) = (s32)o;
            }
        }
    }

    flags = state[3];
    switch (flags & 0xF) {
    case 1:
        o = (u8 *)W(state, 0x4C);
        if (!(flags & 0x80)) {
            state[3] = flags | 0x80;
            S(o, 0x30) = 0x148;
            S(o, 0x32) = 0xE;
            DisplayObject_SavePosition((void *)o);
            S(o, 0x60) = -0x400;
        }
        v_a1 = H(o, 0x60) + 0x33;
        H(o, 0x60) = v_a1;
        phase = (s16)v_a1;
        x = 0x94;
        if (phase >= 0) {
            goto settle;
        }
        goto slide;
    case 2:
        o = (u8 *)W(state, 0x4C);
        if (!(flags & 0x80)) {
            state[3] = flags | 0x80;
            DisplayObject_SavePosition((void *)o);
            H(o, 0x60) = 0x400;
        }
        v_a2 = H(o, 0x60) - 0x33;
        H(o, 0x60) = v_a2;
        phase = (s16)v_a2;
        x = 0x148;
        if (phase > 0) {
            goto slide;
        }
    settle:
        S(o, 0x30) = x;
        S(o, 0x32) = 0xE;
        state[3] = 0;
        goto show;
    slide:
        Widget_SlideSine((void *)o, x, 0xE, phase);
    show:
        TextBox_SetPos(&D_800EB0F8[0], S(o, 0x30), S(o, 0x32));
        break;
    }

    switch (state[1] & 0x1F) {
    case 0:
        B(state + H(state, 6) * 4, 0x56) &= 0x7F;
        o = func_800291E0(0, -1, -1);
        H(o, 8) |= 4;
        W(o, 4) &= 0xF7FFFFFF;
        S(o, 0x30) = H(state, 0x12) - (u16)gGraphics_sViewportX - H(o, 0x48);
        S(o, 0x46) = 0;
        S(o, 0x44) = 0;
        S(o, 0x32) = H(state, 0x14) - (u16)gGraphics_sViewportY - H(o, 0x4A);
        DisplayObject_SavePosition((void *)o);
        H(o, 0x60) = 0;
        SD_SEPlayFull(0x32);
        W(rec->object_04, 4) |= 0x80000000;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(o, 0x148, 0xE, 0, 2, 0, 0x1B, 0x107);
        B(o, 0x5F) = 0x80;
        S(o, 0x60) = -0x400;
        H(o, 8) |= 8;
        DisplayObject_SelectOrderingTable1((DisplayObject *)o);
        DisplayObject_SetDepthOffset((DisplayObject *)o, 4);
        W(state, 0x4C) = (s32)o;
        id = H(state, 6);
        state[3] = 1;
        gDuel_wSelectedCardID = id;
        v_b0 = 3;
        if (((gDuel_adwCardStats[(s16)id - 1] >> 0x1A) & 0x1F) >= 0x14) {
            v_b0 = 4;
        }
        o = TextBox_Create(0, v_b0, 0x94, 0xE, 0xA8, 0xC0);
        B(o, 0x54) = 0;
        B(o, 0x53) = 1;
        B(o, 0x59) = 4;
        state[1] = 1;
        return;
    case 1:
        o = rec->object_00;
        func_8002ABB4((DisplayObject *)o, -1);
        if (!(W(o, 4) & 0x08000000)) {
            t_b1 = H(o, 0x60) + 0x66;
            B(o, 0x22) += 0xC;
            v_b1 = H(o, 0x44) + 0xCC;
            H(o, 0x44) = v_b1;
            H(o, 0x46) = v_b1;
            H(o, 0x60) = t_b1;
            B(o, 0x21) += 6;
            DisplayObject_InterpolatePositionCosine((void *)o, 2, 4, (s16)t_b1);
            if (S(o, 0x60) >= 0x800) {
                W(o, 0x20) = 0x8000;
                W(o, 0x30) = 0x40002;
                W(o, 0x44) = 0x10001000;
                W(rec->object_04, 4) &= 0x7FFFFFFF;
                W(o, 4) |= 0x08000000;
            }
            if (!(W(o, 4) & 0x08000000)) {
                break;
            }
        }
        if (state[3] == 0) {
            state[1] = 2;
        }
        return;
    case 2:
        if (state[4] != 1 && !(D_800E9ECE & 0x80)) {
            D_800E9DB0[3] = 0;
            i = 0;
            do {
                H(W(state + i * 4, 0x24), 8) &= 0xFFBF;
                i++;
            } while (i < 9);
            H(D_800EB24C, 8) &= 0xFFBF;
            state[1] = 3;
            SD_SEPlayFull(0x31);
    case 3:
            o = rec->object_00;
            B(o, 0x21) += 8;
            if (!(B(o, 0x21) & 0xFF)) {
                H(o, 8) &= 0xFFFB;
            next:
                state[1] = 5;
                return;
            }
        }
        break;
    case 4:
        light = Model_GetFlatLight(0, 2);
        vs = &D_800F2848;
        a = -vs->field_00;
        b = a * rcos(vs->field_04) / 4096;
        c = a * rsin(vs->field_04) / 4096;
        a = b * rsin(vs->angle + 0x800) / 4096;
        b = b * rcos(vs->angle + 0x800) / 4096;
        W(light, 0) = b;
        W(light, 4) = c;
        W(light, 8) = a;
        flags = state[1];
        if (!(flags & 0x80)) {
            state[1] = flags | 0x80;
            state[2] = 0;
            D_8009B0C0 = 1;
            D_800E9DB0[2] = (void (*)(void))func_80029934;
            flags = state[0];
            H(state, 0x10) = 0;
            if (!(flags & 0x40)) {
                state[0] = flags | 0x40;
                light = Model_GetFlatLight(0, 0);
                W(light, 4) = -0x1000;
                W(light, 0) = 0;
                W(light, 8) = 0;
                W(light, 0xC) = 0xFFFFFF;
                W(light, 0x10) = 0;
                W(light, 0x14) = 0x1000;
                W(light, 0x18) = 0;
                W(light, 0x1C) = 0x404040;
                W(light, 0x2C) = 0xC0C0C0;
                func_800595C8(0, 0x400, 0x400, 0x400);
                Model_CopySlotU16Values(0, (u16 *)0x80181000);
                p = (u8 *)0x80181000;
                D_800F2848.view.vrx = S(p, 0);
                t_b4 = H(p, 2);
                D_800F2848.view.vry = (s16)t_b4;
                t_b4 = H(p, 4);
                D_800F2848.view.vrz = (s16)t_b4;
                D_800F2848.field_00 = func_8005F1B8(0, 0x334);
                func_80059AA8(0, 1);
                func_80059AE0(0);
            }
            Model_ControlSlotAnimation(0, 0, 1);
            func_800597C8(0, 1, 0);
            W(state, 0x20) = 0x12C;
            state[3] = 2;
            o = rec->object_00;
            H(o, 8) |= 4;
            DisplayObject_SavePosition((void *)o);
            H(o, 0x60) = 0;
            SD_SEPlayFull(0x34);
            func_80015C0C();
            return;
        }
        sel = state[2];
        switch (sel) {
        case 0:
            o = rec->object_00;
            if (S(o, 0x60) < 0x800) {
                B(o, 0x21) -= 4;
                if (B(o, 0x21) < 0xD8) {
                    B(o, 0x21) = 0xD8;
                }
                v_b4 = H(o, 0x60) + 0xCC;
                H(o, 0x60) = v_b4;
                DisplayObject_InterpolatePositionCosine((void *)o, -0x16, 4, (s16)v_b4);
                if (S(o, 0x60) >= 0x800) {
                    B(o, 0x21) = 0xD8;
                    S(o, 0x30) = -0x16;
                    S(o, 0x32) = 4;
                }
            }
            H(state, 0x10) = H(state, 0x10) + 0x2A;
            if (S(state, 0x10) >= 0x400) {
                H(state, 0x10) = 0x400;
                if (!(D_800E9ECE & 0x80) && state[3] == 0) {
                    state[2] = 1;
                    DisplayObject_SetResourceVariant((void *)W(state, 0x50), 1);
                    return;
                }
            }
            break;
        case 1:
            if (gInput_wPad1Pressed & 0x80E0) {
                func_80015C84();
                state[3] = sel;
                state[2] = 2;
                SD_SEPlayFull(0x34);
                o = rec->object_00;
                DisplayObject_SavePosition((void *)o);
                H(o, 0x60) = 0;
                return;
            }
            break;
        case 2:
            o = rec->object_00;
            if (H(o, 8) & 4) {
                v_b4 = H(o, 0x60) + 0xCC;
                B(o, 0x21) += 4;
                H(o, 0x60) = v_b4;
                DisplayObject_InterpolatePositionCosine((void *)o, 2, 4, (s16)v_b4);
                if (S(o, 0x60) >= 0x800) {
                    B(o, 0x21) = 0;
                    S(o, 0x30) = sel;
                    S(o, 0x32) = 4;
                    H(o, 8) &= 0xFFFB;
                }
                if (H(o, 8) & 4) {
                    break;
                }
            }
            if (!(D_800E9ECE & 0x80) && state[3] == 0) {
                SD_KeyOffVoiceSlots();
                D_800E9DB0[2] = 0;
                {
                    void *p = (void *)W(state, 0x50);

                    D_8009B0C0 = 0;
                    DisplayObject_SetResourceVariant(p, 2);
                }
                goto next;
            }
            break;
        }
        break;
    case 5:
        if (gInput_wPad1Pressed & 0x20) {
            H(rec->object_00, 8) |= 4;
            state[1] = 6;
            state[4] = 0;
            DisplayObject_ReleaseIfPresent((void *)W(state, 0x50));
            func_80014FA4();
            SD_SEPlayFull(0x31);
            return;
        }
        if (state[4] == 0 && (*(volatile u16 *)&gInput_wPad1Pressed & 0x20C0)
            && ((gDuel_adwCardStats[H(state, 6) - 1] >> 0x1A) & 0x1F) < 0x14
            && state[3] == 0) {
            state[1] = 4;
            return;
        }
        break;
    case 6:
        o = rec->object_00;
        v_b6 = B(o, 0x21) + 8;
        B(o, 0x21) = v_b6;
        if ((s8)v_b6 < 0) {
            W(rec->object_04, 4) |= 0x80000000;
            W(o, 4) &= 0xF7FFFFFF;
            DisplayObject_SavePosition((void *)o);
            i = 0;
            H(o, 0x60) = 0;
            do {
                H(W(state + i * 4, 0x24), 8) |= 0x40;
                i++;
            } while (i < 9);
            H(D_800EB24C, 8) |= 0x40;
            D_800E9DB0[3] = func_80029EC4;
            func_80015C0C();
            state[3] = 2;
            state[1] = 7;
            SD_SEPlayFull(0x33);
            return;
        }
        break;
    case 7:
        o = rec->object_00;
        if (o != 0) {
            func_8002ABB4((DisplayObject *)o, 1);
            t_b7 = H(o, 0x60) + 0x66;
            B(o, 0x22) += 0xC;
            B(o, 0x21) += 6;
            H(o, 0x60) = t_b7;
            DisplayObject_InterpolatePositionCosine((void *)o,
                S(state, 8) - S(o, 0x48) - gGraphics_sViewportX,
                S(state, 0xA) - S(o, 0x4A) - gGraphics_sViewportY, (s16)t_b7);
            v_b7 = H(o, 0x44) - 0xCC;
            H(o, 0x44) = v_b7;
            H(o, 0x46) = v_b7;
            if (S(o, 0x60) >= 0x800) {
                func_80029528(0);
                B(state + H(state, 6) * 4, 0x56) |= 0x80;
                return;
            }
        } else if (state[3] == 0) {
            DisplayObject_ReleaseIfPresent((void *)W(state, 0x4C));
            W(state, 0x4C) = 0;
            TextBox_Destroy(&D_800EB0F8[0]);
            state[1] = 8;
            return;
        }
        break;
    case 8:
        state[0] = 1;
        break;
    }
}
