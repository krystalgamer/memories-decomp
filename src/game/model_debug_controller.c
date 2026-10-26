#include "../types.h"
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#include "model_handler_state.h"
#include "model_debug_controller.h"
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD2_PRESSED_IN_DATA
#define GINPUT_PAD1_HELD_IN_DATA
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#include "input.h"
#include "model.h"
#include "model_load_step.h"
#include "model_graphics_state.h"
#include "model_background.h"
#include "camera_view.h"
#include "model_scene_controllers.h"
#include "model_state_setters.h"
#include "model_set_slot_shadow_enabled.h"
#include "model_update_coordinate_unit_matrix.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "../psyq/rand.h"

#ifdef VERSION_JAPAN
#define MODEL_DEBUG_CANCEL_MASK 0x40
/* Keep the load delay while the original data segment owns the storage. */
ModelDebugState D_8009B004;
#else
#define MODEL_DEBUG_CANCEL_MASK 0x20
ModelDebugState D_8009B004 = {0};
#endif

s32 ModelDebug_UpdateController(void)
{
    s32 reload = 0;
    char *normal = gModelDebug_szNormalColor;
    char *selected = gModelDebug_szSelectedColor;
    ModelSlot *slots;

    if (gModel_bPresentationPhase == -2)
        return 1;
    if (gModel_bPresentationPhase >= 40)
        return 0;
    if ((gInput_wPad1Pressed & MODEL_DEBUG_CANCEL_MASK) ||
        (gInput_wPad2Pressed & MODEL_DEBUG_CANCEL_MASK)) {
        gModel_bPresentationPhase = 40;
        return 0;
    }
    slots = gModel_aSlots;
    if (!slots[2].is_active) {
        Model_SetFrameStepOverride(1);
#ifndef VERSION_JAPAN
        FntPrint(gModelDebug_szLoadStageFormat, slots[2].field_DF8, slots[2].load_step);
#endif
        if (slots[2].load_step == 255) {
            if (!((gFile_dwTransferState_abs & 0x2000030) | gFile_dwCancelRequestState_abs))
                Model_LoadMonsterMerge(2, -1, -1, -1, -1, -1, 0);
        } else {
            Model_AdvanceSlotLoad(2);
        }
        if (gModel_aSlots[2].is_active) {
            ModelSlot *stage = gModel_aSlots;
            s32 height;

            stage += 2;
            /* A natural member store moves the stage-pointer load across it;
               the u16 member view preserves the original ordering. */
            *(u16 *)&D_8009B004.fields.height =
                gModel_pBackgroundRecord->stage_height;
            height = D_8009B004.fields.height;
            if (stage->root_coordinate) {
                stage->root_coordinate->rot.vx = 0;
                stage->root_coordinate->rot.vy = 0;
                stage->root_coordinate->rot.vz = 0;
                stage->root_coordinate->matrix.t[0] = 0;
                stage->root_coordinate->matrix.t[1] = height;
                stage->root_coordinate->matrix.t[2] = 0;
            }
            Model_UpdateCoordinateUnitMatrix(stage->root_coordinate, 0);
            gModel_bPresentationPhase = 0;
        }
    } else if (!slots[0].is_active) {
        Model_SetFrameStepOverride(1);
#ifndef VERSION_JAPAN
        FntPrint(gModelDebug_szLoadModel0Format, slots[0].field_DF8, slots[0].load_step);
#endif
        if (slots[0].load_step == 255) {
            if (!((gFile_dwTransferState_abs & 0x2000030) | gFile_dwCancelRequestState_abs))
                Model_LoadMonsterMerge(0, -1, -1, -1, -1, -1, 0);
        } else {
            Model_AdvanceSlotLoad(0);
        }
        if (gModel_aSlots[0].is_active)
            Model_SetSlotShadowEnabled(0, ((gModel_pBackgroundRecord->flags >> 1) ^ 1) & 1);
    } else if (!slots[1].is_active) {
        Model_SetFrameStepOverride(1);
#ifndef VERSION_JAPAN
        FntPrint(gModelDebug_szLoadModel1Format, slots[1].field_DF8, slots[1].load_step);
#endif
        if (slots[1].load_step == 255) {
            if (!((gFile_dwTransferState_abs & 0x2000030) | gFile_dwCancelRequestState_abs))
                Model_LoadMonsterMerge(1, -1, -1, -1, -1, -1, 0);
        } else {
            Model_AdvanceSlotLoad(1);
        }
        if (gModel_aSlots[1].is_active)
            Model_SetSlotShadowEnabled(1, ((gModel_pBackgroundRecord->flags >> 1) ^ 1) & 1);
    }
#ifndef VERSION_JAPAN
    if ((gInput_wPad1Held & 0x100) && (gInput_wPad2Pressed & 0x100))
        gModelDebug_Display.display_enabled ^= 1;
    if (!gModelDebug_Display.display_enabled)
        return 0;
    if (gModel_bPresentationPhase == -2) {
        s32 i = 0;
        s32 excluded;
        s32 value;

        while (1) {
            do {
                value = (rand() >> 8) % 722;
                excluded = 720;
            } while ((u32)value >= 722);
            if ((u32)(value - 300) >= 50 && (u32)(value - 650) >= 50 && value != excluded) {
                gModel_awSlotModelIds[i] = value;
                if ((u16)value != gModel_aSlots[i].field_DF8) {
                    i++;
                    if (i >= 2)
                        break;
                }
            }
        }
        reload++;
        gModel_awSlotModelIds[2] = (rand() >> 8) % 7;
    }
    if (gInput_wPad1Pressed & 0x10)
        reload++;
    if (gInput_wPad1Repeat & 0x900) {
        if (gInput_wPad1Repeat & 0x100)
            gModel_awSlotModelIds[2] = ((u16)gModel_awSlotModelIds[2] + 6) % 7;
        else
            gModel_awSlotModelIds[2] = ((u16)gModel_awSlotModelIds[2] + 8) % 7;
    }
    if (gInput_wPad1Repeat & 0xA000) {
        if (gInput_wPad1Repeat & 0x8000)
            gModelDebug_bSelectedDigit = (gModelDebug_bSelectedDigit + 5) % 6;
        else
            gModelDebug_bSelectedDigit = (gModelDebug_bSelectedDigit + 7) % 6;
    }
    if (gInput_wPad1Held & 0x80) {
        if (gInput_wPad1Repeat & 0x5000) {
            if (gInput_wPad1Repeat & 0x1000)
                D_8009B004.halfwords[1] -= 10;
            else
                D_8009B004.halfwords[1] += 10;
            if (gModel_aSlots[2].is_active) {
                ModelSlot *stage = gModel_aSlots;
                s32 height;

                stage += 2;
                height = D_8009B004.fields.height;
                if (stage->root_coordinate) {
                    stage->root_coordinate->rot.vx = 0;
                    stage->root_coordinate->rot.vy = 0;
                    stage->root_coordinate->rot.vz = 0;
                    stage->root_coordinate->matrix.t[0] = 0;
                    stage->root_coordinate->matrix.t[1] = height;
                    stage->root_coordinate->matrix.t[2] = 0;
                }
                Model_UpdateCoordinateUnitMatrix(stage->root_coordinate, 0);
            }
        }
    } else if (gInput_wPad1Repeat & 0x5000) {
        s32 side = gModelDebug_bSelectedDigit >= 3u;
        u16 *number = (u16 *)&gModel_awSlotModelIds[side];
        s32 digits[3] = {
            (u16)((u16)(*number / 100u) % 10u),
            (u16)((u16)(*number / 10u) % 10u),
            (u16)(*number % 10u)
        };

        if (gInput_wPad1Repeat & 0x1000) {
            digits[(u8)(gModelDebug_bSelectedDigit % 3u)] =
                (digits[(u8)(gModelDebug_bSelectedDigit % 3u)] + 11) % 10;
        } else {
            digits[(u8)(gModelDebug_bSelectedDigit % 3u)] =
                (digits[(u8)(gModelDebug_bSelectedDigit % 3u)] + 9) % 10;
        }
        gModel_awSlotModelIds[side] = 100 * digits[0] + 10 * digits[1] + digits[2];
    }
    if (gInput_wPad1Pressed & 4)
        gModelDebug_abSlotAfFlags[0] ^= 1;
    if (gInput_wPad1Pressed & 1)
        gModelDebug_abSlotGfFlags[0] ^= 1;
    if (gInput_wPad1Pressed & 8)
        gModelDebug_abSlotAfFlags[1] ^= 1;
    if (gInput_wPad1Pressed & 2)
        gModelDebug_abSlotGfFlags[1] ^= 1;
    FntPrint(gModelDebug_szDigitFormat, gModelDebug_bSelectedDigit == 0 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[0] / 100u) % 10u));
    FntPrint(gModelDebug_szDigitFormat, gModelDebug_bSelectedDigit == 1 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[0] / 10u) % 10u));
    FntPrint(gModelDebug_szDigitFormat, gModelDebug_bSelectedDigit == 2 ?
        selected : normal, (u16)((u16)gModel_awSlotModelIds[0] % 10u));
    FntPrint(normal);
    FntPrint(gModelDebug_szSlotFlagsFormat, gModelDebug_abSlotAfFlags[0], gModelDebug_abSlotGfFlags[0]);
    FntPrint(gModelDebug_szVersusSeparator);
    FntPrint(gModelDebug_szDigitFormat, gModelDebug_bSelectedDigit == 3 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[1] / 100u) % 10u));
    FntPrint(gModelDebug_szDigitFormat, gModelDebug_bSelectedDigit == 4 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[1] / 10u) % 10u));
    FntPrint(gModelDebug_szDigitFormat, gModelDebug_bSelectedDigit == 5 ?
        selected : normal, (u16)((u16)gModel_awSlotModelIds[1] % 10u));
    FntPrint(normal);
    FntPrint(gModelDebug_szSlotFlagsFormat, gModelDebug_abSlotAfFlags[1], gModelDebug_abSlotGfFlags[1]);
    FntPrint(gModelDebug_szLineBreak);
    FntPrint(gModelDebug_szStageFormat, (u16)gModel_awSlotModelIds[2], D_8009B004.fields.height);
    if (reload) {
        File_WaitForTransfers();
        Model_ResetSceneState();
        Model_SetSlotProperties(0, (u16)gModel_awSlotModelIds[0], -1, -1,
            gModelDebug_abSlotAfFlags[0], gModelDebug_abSlotGfFlags[0]);
        Model_SetSlotProperties(1, (u16)gModel_awSlotModelIds[1], -1, -1,
            gModelDebug_abSlotAfFlags[1], gModelDebug_abSlotGfFlags[1]);
        Model_SetSlotProperties(2, (u16)gModel_awSlotModelIds[2]);
    }
#endif
    return 0;
}
