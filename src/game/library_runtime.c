#define D_8009B118_IN_DATA
#define D_8009B0C0_IN_DATA_VOLATILE
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "func_80058DD8.h"
#include "camera_view.h"
#include "file_transfer.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "library_runtime.h"
#include "model_copy_slot_u16_values.h"
#include "model_update_view_metrics.h"
#include "sound_voice_data.h"
#include "view_state.h"
#include "../psyq/rand.h"
#include "model_slot_queries.h"
#include "../unmatched.h"
#include "model_control_slot_animation.h"
#include "library_grid_cursor.h"
#include "func_80029EC4.h"
#include "func_8002A2F4.h"
#include "save_data.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "card_grid.h"
#include "display_object_layout.h"
#include "func_8003B6AC.h"
#include "main_services.h"
#include "text_box_lifecycle.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "display_object_helpers.h"
#include "duel_deck_lookup.h"
#include "func_80029574.h"
#include "func_80029590.h"
#include "sound.h"
#include "text_render_state.h"
#include "text_staging.h"
#include "func_8002A660.h"
#include "text_box_runtime.h"
#include "display_object_config.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "duel_effect.h"
#include "duel_effect_resource_setup.h"
#include "fade.h"
#include "func_800291E0.h"
#include "func_80029934.h"
#include "func_8002ABB4.h"
#include "func_80039794.h"
#include "func_80059AA8.h"
#include "input.h"
#include "model_effect_coefficients.h"
#include "model.h"
#include "model_load_step.h"
#include "model_scene_setup.h"
#include "model_slot_properties.h"
#include "model_slot_state_updates.h"
#include "model_slot_support.h"
#include "model_state_setters.h"
#include "../psyq/libgs.h"

/* The seven contiguous functions of the Library screen: the card-view state
   handler func_8002ACA4, the trivial state handlers for states 0 and 3, the
   per-frame dispatcher func_8002BAB4, the package-transfer callback, the
   owned-card pass and the screen entry func_8002BFCC.

   The state handlers were recorded at gcc_2_8_1_g8. They compile to
   identical objects at gcc_2_8_1_g8_split, which the dispatcher and entry
   need, so they build there with the rest of the screen. */

/* Library screen state 2, the card view. It opens the view, slides the model
   and text box in, rotates the model light with the view angle and walks the
   close sequence back to the grid. The state block and the display objects
   are still reached by offset through the four views below.

   What the retail object depends on:
   - the two slide states are separate arms; cross-jumping merges their
     settle and Widget_SlideSine tails;
   - state 0 updates the object's flag words through `bits` and `attr`, each
     assigned twice, so sched1 does not glue their loads to their uses, and
     reads the object's y origin into the reused `id` ahead of the position
     stores;
   - the selected card ID is stored and read straight back as the stats index;
   - D_8009B0C0 is volatile in this unit, which keeps the delay slot of the
     close path's DisplayObject_SetResourceVariant call empty, and the pad
     word is volatile because state 5 loads it twice;
   - `state + (index << 2)` rather than `* 4` puts the base first in the
     address sum. */
#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define LIBRARY_MOTION_STATE_VIEW(state) ((LibraryMotionState *)(state))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

void func_8002ACA4(u8 *state)
{
    DuelEffectResourceRecord *rec;
    u8 *o;
    u8 *light;
    u8 *box;
    s32 flags;
    s32 sel;
    s32 mode;
    s32 id;
    s32 i;
    s32 b;
    s32 a;
    s32 c;
    s32 layout;
    GsRVIEW2 *rv;
    u8 *p;
    ViewState *vs;
    s32 bits;
    s32 attr;

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
        Fade_StartOutKeepOverlay();
        D_800E9ECF[0] = 6;
        W(state, 0x50) = 0;
        return;
    }
    func_80039794();
    mode = state[4];
    if ((mode & 0xF) && ((D_8009B0F4_abs & 0x02000030) | D_8009B134_abs) == 0) {
        if (mode == 1) {
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
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), 0xA);
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
        H(o, 0x60) += 0x33;
        if (S(o, 0x60) >= 0) {
            S(o, 0x30) = 0x94;
            S(o, 0x32) = 0xE;
            state[3] = 0;
        } else {
            Widget_SlideSine((void *)o, 0x94, 0xE, S(o, 0x60));
        }
        TextBox_SetPos(&D_800EB0F8[0], S(o, 0x30), S(o, 0x32));
        break;
    case 2:
        o = (u8 *)W(state, 0x4C);
        if (!(flags & 0x80)) {
            state[3] = flags | 0x80;
            DisplayObject_SavePosition((void *)o);
            H(o, 0x60) = 0x400;
        }
        H(o, 0x60) -= 0x33;
        if (S(o, 0x60) <= 0) {
            S(o, 0x30) = 0x148;
            S(o, 0x32) = 0xE;
            state[3] = 0;
        } else {
            Widget_SlideSine((void *)o, 0x148, 0xE, S(o, 0x60));
        }
        TextBox_SetPos(&D_800EB0F8[0], S(o, 0x30), S(o, 0x32));
        break;
    }

    switch (state[1] & 0x1F) {
    case 0:
        B(state + (H(state, 6) << 2), 0x56) &= 0x7F;
        o = func_800291E0(0, -1, -1);
        bits = H(o, 8);
        bits |= 4;
        H(o, 8) = bits;
        attr = W(o, 4);
        attr &= 0xF7FFFFFF;
        W(o, 4) = attr;
        id = H(o, 0x4A);
        S(o, 0x30) = H(state, 0x12) - gGraphics_sViewportX - H(o, 0x48);
        S(o, 0x32) = H(state, 0x14) - gGraphics_sViewportY - id;
        S(o, 0x44) = S(o, 0x46) = 0;
        DisplayObject_SavePosition((void *)o);
        H(o, 0x60) = 0;
        SD_SEPlayFull(0x32);
        W(rec->object_04, 4) |= 0x80000000;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(o, 0x148, 0xE, 0, 2, 0, 0x1B, 0x107);
        B(o, 0x5F) = 0x80;
        S(o, 0x60) = -0x400;
        H(o, 8) |= 8;
        DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
        DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), 4);
        W(state, 0x4C) = (s32)o;
        state[3] = 1;
        gDuel_wSelectedCardID = H(state, 6);
        layout = 3;
        if (((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >> 0x1A) & 0x1F) >= 0x14) {
            layout = 4;
        }
        box = TextBox_Create(0, layout, 0x94, 0xE, 0xA8, 0xC0);
        B(box, 0x54) = 0;
        B(box, 0x53) = 1;
        B(box, 0x59) = 4;
        state[1] = 1;
        return;
    case 1:
        o = rec->object_00;
        func_8002ABB4(DISPLAY_OBJECT_VIEW(o), -1);
        if (!(W(o, 4) & 0x08000000)) {
            B(o, 0x22) += 0xC;
            B(o, 0x21) += 6;
            H(o, 0x46) = H(o, 0x44) += 0xCC;
            H(o, 0x60) += 0x66;
            DisplayObject_InterpolatePositionCosine((void *)o, 2, 4, S(o, 0x60));
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
        if (state[4] != 1 && !(D_800E9ECE[0] & 0x80)) {
            D_800E9DB0[3] = 0;
            i = 0;
            do {
                H(W(state + i * 4, 0x24), 8) &= 0xFFBF;
                i++;
            } while (i < 9);
            H(D_800EB0F8[3].field_28, 8) &= 0xFFBF;
            state[1] = 3;
            SD_SEPlayFull(0x31);
    case 3:
            o = rec->object_00;
            B(o, 0x21) += 8;
            if (!(B(o, 0x21) & 0xFF)) {
                H(o, 8) &= 0xFFFB;
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
                D_800F2848.view.vry = (s16)H(p, 2);
                D_800F2848.view.vrz = (s16)H(p, 4);
                D_800F2848.field_00 = func_8005F1B8(0, 0x334);
                func_80059AA8(0, 1);
                Model_SetScreenYOverride(0);
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
            Fade_StartInKeepOverlay();
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
                H(o, 0x60) += 0xCC;
                DisplayObject_InterpolatePositionCosine((void *)o, -0x16, 4, S(o, 0x60));
                if (S(o, 0x60) >= 0x800) {
                    B(o, 0x21) = 0xD8;
                    S(o, 0x30) = -0x16;
                    S(o, 0x32) = 4;
                }
            }
            H(state, 0x10) = H(state, 0x10) + 0x2A;
            if (S(state, 0x10) >= 0x400) {
                H(state, 0x10) = 0x400;
                if (!(D_800E9ECE[0] & 0x80) && state[3] == 0) {
                    state[2] = 1;
                    DisplayObject_SetResourceVariant((void *)W(state, 0x50), 1);
                    return;
                }
            }
            break;
        case 1:
            if (gInput_wPad1Pressed & 0x80E0) {
                Fade_StartOutKeepOverlay();
                state[3] = 1;
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
                B(o, 0x21) += 4;
                H(o, 0x60) += 0xCC;
                DisplayObject_InterpolatePositionCosine((void *)o, 2, 4, S(o, 0x60));
                if (S(o, 0x60) >= 0x800) {
                    B(o, 0x21) = 0;
                    S(o, 0x30) = 2;
                    S(o, 0x32) = 4;
                    H(o, 8) &= 0xFFFB;
                }
                if (H(o, 8) & 4) {
                    break;
                }
            }
            if (!(D_800E9ECE[0] & 0x80) && state[3] == 0) {
                SD_KeyOffVoiceSlots();
                D_800E9DB0[2] = 0;
                {
                    void *p = (void *)W(state, 0x50);

                    D_8009B0C0 = 0;
                    DisplayObject_SetResourceVariant(p, 2);
                }
                state[1] = 5;
                return;
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
        if (state[4] == 0 && (gInput_wPad1Pressed & 0x20C0)
            && ((gDuel_adwCardStats[H(state, 6) - 1] >> 0x1A) & 0x1F) < 0x14
            && state[3] == 0) {
            state[1] = 4;
            return;
        }
        break;
    case 6:
        o = rec->object_00;
        B(o, 0x21) += 8;
        if ((s8)B(o, 0x21) < 0) {
            W(rec->object_04, 4) |= 0x80000000;
            W(o, 4) &= 0xF7FFFFFF;
            DisplayObject_SavePosition((void *)o);
            i = 0;
            H(o, 0x60) = 0;
            do {
                H(W(state + i * 4, 0x24), 8) |= 0x40;
                i++;
            } while (i < 9);
            H(D_800EB0F8[3].field_28, 8) |= 0x40;
            D_800E9DB0[3] = Library_DrawCardGrid;
            Fade_StartInKeepOverlay();
            state[3] = 2;
            state[1] = 7;
            SD_SEPlayFull(0x33);
            return;
        }
        break;
    case 7:
        o = rec->object_00;
        if (o != 0) {
            func_8002ABB4(DISPLAY_OBJECT_VIEW(o), 1);
            B(o, 0x22) += 0xC;
            B(o, 0x21) += 6;
            H(o, 0x60) += 0x66;
            DisplayObject_InterpolatePositionCosine((void *)o,
                S(state, 8) - S(o, 0x48) - gGraphics_sViewportX,
                S(state, 0xA) - S(o, 0x4A) - gGraphics_sViewportY, S(o, 0x60));
            H(o, 0x46) = H(o, 0x44) -= 0xCC;
            if (S(o, 0x60) >= 0x800) {
                func_80029528(0);
                B(state + (H(state, 6) << 2), 0x56) |= 0x80;
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

#undef B
#undef H
#undef S
#undef W

void func_8002BAA0(u8 *value)
{
    *value = 1;
}

void func_8002BAAC(u8 *value)
{
}

void func_8002BAB4(void)
{
    u8 *state;
    ViewState *model;
    s32 one;
    s32 r;
    s32 a;
    s32 step;
    s32 cur;
    s32 target;
    s32 count;
    s32 mode;
    u8 *dst;
    ViewState *p;

    state = D_800EA1E8;
    one = 1;
    mode = D_800EA1E8[0];
    mode &= 0xF;
    switch (mode) {
    case 0:
        func_8002BAA0(state);
        break;
    case 1:
        Library_UpdateGridCursor(state);
        break;
    case 2:
        func_8002ACA4(state);
        model = &D_800F2848;
        r = func_80058DD8(0);
        if (r == one) {
            if (Model_GetSlotAnimationIndex(0) != r) {
                Model_CopySlotU16Values(0, (u16 *)0x80181010);
                r = D_80181012 - D_80181002;
                a = r >= 0 ? r : -r;
                step = a / 24;
                step += 1;
                cur = model->view.vry;
                if (D_80181012 < cur) {
                    model->view.vry = cur - step;
                    cur = *(volatile long *)&model->view.vry;
                }
                if (cur < D_80181012) {
                    model->view.vry = cur + step;
                }
            } else {
                r = model->view.vry;
                target = D_80181002;
                if (r != target) {
                    if (target < r) {
                        r -= 8;
                        if (r < target) {
                            r = target;
                        }
                    } else {
                        r += 8;
                        if (target < r) {
                            r = target;
                        }
                    }
                    model->view.vry = r;
                }
                count = *(s32 *)(state + 0x20) - 1;
                *(s32 *)(state + 0x20) = count;
                if (count <= 0) {
                    *(s32 *)(state + 0x20) = 0x12C;
                    r = 2;
                    if (rand() & 1) {
                        r = 7;
                    }
                    Model_ControlSlotAnimation(0, r, 1);
                }
            }
        }
        p = &D_800F2848;
        p->angle += 0xC;
        ViewState_ApplyOrbit();
        dst = Model_GetCameraViewBuffer();
        ((LibraryViewQuad *)dst)[0] = *(LibraryViewQuad *)&model->view.vpx;
        ((LibraryViewQuad *)dst)[1] = *(LibraryViewQuad *)&model->view.vry;
        Model_UpdateViewMetrics(0);
        break;
    case 3:
        func_8002BAAC(state);
        break;
    }
}

/* The latter three Library functions in address order: the package-transfer
   callback, the pass that marks every owned card in the screen's state, and
   func_8002BFCC, which sets the screen up, installs that callback and hands
   control to the dispatcher above.

   The owned-card pass was recorded at gcc_2_8_1_g0_split. It compiles to an
   identical object at gcc_2_8_1_g8_split, which the entry needs, so the
   unit builds there. */

#define gStageRect (D_800E9D70[0])

void func_8002BD0C(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 4:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF6;
        gStageRect.w = 0x100;
        gStageRect.h = 2;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->done = 3;
        object->field_30.word = 0x26810;
        object->phase_size = 20 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 3:
    case 5:
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 6:
        SD_LoadSequenceBankPair(1, (u32 *)D_8009B118);
        break;
    }
}

void Library_MarkOwnedCards(void)
{
    u8 *p = gLibrary_abCardChest;
    s32 i = 0;
    u16 *q;
    do {
        if (*p != 0)
            Library_UpdateCardUsedFlag(i + (CAMPAIGN_FLAG_LIBRARY_CARD_BASE + CARD_ID_FIRST));
        i++;
        p++;
    } while (i < CARD_COUNT);
    q = gDuel_awPlayerDeck;
    i = 0;
    do {
        if (*q != 0)
            Library_UpdateCardUsedFlag(*q + CAMPAIGN_FLAG_LIBRARY_CARD_BASE);
        i++;
        q++;
    } while (i < DECK_SIZE);
}

void func_8002BFCC(void) {
    s16 *q;
    DuelEffectResourceRecord *b;
    u8 *r;
    u8 *o;
    u8 *e;
    u8 *m;
    s32 n;
    s32 x;
    s32 y;
    s32 v;
    s32 c;
    s32 d;
    s32 k;
    s32 off;
    s32 rb;

    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    func_80035668(0);
    DuelEffect_ClearResourceObjectPointers(0);
    n = CARD_COUNT - 1;
    q = (s16 *)0x801805A2;
    b = &D_800EA0E8[0];
    b->src_x = 0x100;
    b->src_y = 0x100;
    b->field_2C = 0x200;
    b->field_2E = 0xF0;
    do {
        *q = n + 1;
        n--;
        q--;
    } while (n >= 0);
    File_RequestAsyncTransfer(0, (u8 *)0, 0x1DCD, 0x8A,
                              (FileTransferCallback)func_8002BD0C, 0, 0);
    File_WaitForTransfers();
    Library_MarkOwnedCards();
    D_800E9DB0[3] = Library_DrawCardGrid;
    func_80029590();
    r = D_800EA1E8;
    D_800EA1E8[0] = 0;
    c = gCardGrid_bCursorColumn;
    d = c;
    if (c >= CARD_GRID_SECTION_SIDE_LENGTH) {
        x = (c % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xAE;
    } else {
        x = (d % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xE;
    }
    LIBRARY_MOTION_STATE_VIEW(r)->rest_x = x;
    LIBRARY_MOTION_STATE_VIEW(r)->x = x;
    k = gCardGrid_bCursorRow;
    y = (k / CARD_GRID_SECTION_SIDE_LENGTH) * 0xB2
        + (k % CARD_GRID_SECTION_SIDE_LENGTH) * 0x10 + 0xE;
    LIBRARY_MOTION_STATE_VIEW(r)->rest_y = y;
    LIBRARY_MOTION_STATE_VIEW(r)->y = y;
    func_8002A660(r);
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(o, 0x10, 0xD8, 0, 2, 1, 0x1B, 0x127);
    o[0x5F] = 0x80;
    DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
    n = CARD_ID_FIRST;
    *(u16 *)(o + 8) =
        *(u16 *)(o + 8) | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    *(u8 **)(r + 0x48) = o;
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        o, LIBRARY_MOTION_STATE_VIEW(r)->x, LIBRARY_MOTION_STATE_VIEW(r)->y,
        0, 2, 2, 0x1B, 0x147);
    rb = (s32)r;
    o[0x5F] = 0x80;
    LIBRARY_MOTION_STATE_VIEW(r)->render = DISPLAY_OBJECT_VIEW(o);
    r[0x56] = 0;
    *(s16 *)(r + 0x54) = 0;
    do {
        *(u8 *)(rb + n * 4 + 0x56) = 0;
        *(s16 *)(r + 0x54) = 0;
        off = n * 4 - 4;
        v = (*(s32 *)((u8 *)gDuel_adwCardStats + off) >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
        switch (v) {
        case CARD_TYPE_MAGIC:
        case CARD_TYPE_EQUIP:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_MAGIC_EQUIP;
            break;
        case CARD_TYPE_TRAP:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_TRAP;
            break;
        case CARD_TYPE_RITUAL:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_RITUAL;
            break;
        default:
            *(s16 *)(rb + n * 4 + 0x54) = LIBRARY_CARD_SELECTOR_DEFAULT;
            break;
        }
        n++;
    } while (n < CARD_ID_END);
    D_801D5608[0].library_count = 0;
    n = CARD_ID_FIRST;
    do {
        if (Campaign_TestStoryFlag(n + CAMPAIGN_FLAG_LIBRARY_CARD_BASE) != 0) {
            D_801D5608[0].library_count += 1;
            *(u8 *)(r + n * 4 + 0x56) = 0x80;
            if (Library_CheckCardOwned(n) < 0) {
                *(u8 *)(r + n * 4 + 0x56) |= 1;
            }
        }
        n++;
    } while (n < CARD_ID_END);
    func_8003B6AC(3, 1);
    m = TextBox_Create(3, 0xF8, 0x58, -0x18, 0x90, 0x10);
    m[0x5A] = 0x10;
    m[0x5B] = 0x10;
    func_80039A14((struct DuelEffectChannel *)m);
    DisplayObject_SelectOrderingTable3(*(DisplayObject **)(m + 0x28));
    *(u16 *)(*(u8 **)(m + 0x28) + 8) &=
        ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_8002A2F4(r);
    SD_BGMPlay(0x72D0);
}
