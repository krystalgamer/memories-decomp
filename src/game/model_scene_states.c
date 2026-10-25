#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "camera_view.h"
#include "model_scene_controllers.h"
#include "model_update_tint_requests.h"
#include "model_slot_control.h"
#include "model_remap_primitive_handlers.h"
#include "model.h"
#include "model_background.h"
#include "model_cleanup.h"
#include "model_graphics_state.h"
#include "model_handler_registry.h"
#include "primitive_handler_modes.h"
#include "model_scene_states.h"
#include "model_slot_properties.h"
#include "model_effect_state.h"
#include "model_transfer_state.h"
#include "../unmatched.h"
#include "../game/model_draw_slot.h"

void Model_SetPresentationBgmCommand(s32 value)
{
    gModel_wPresentationBgmCommand = value;
}

void Model_StartExodiaPresentation(void)
{
    Model_ResetSceneState();
    Model_ResetSceneView();
    gModel_bSceneMode = 0x13;
    Model_SetCameraOrbit(0, 10000, 0xE00, 0, 0);
    Model_SetSlotTintTarget(0, 5, 0, 0, 0);
    gModel_bPresentationPhase = -1;
}

s32 Model_IsExodiaPresentationComplete(void)
{
    return gModel_bPresentationPhase == -2;
}

void Model_StartCreditsPresentation(void)
{
    Model_ResetSceneState();
    Model_ResetSceneView();
    gModel_bSceneMode = 20;
    gModel_bPresentationPhase = -1;
}

s32 Model_IsCreditsPresentationComplete(void)
{
    return gModel_bPresentationPhase == -2;
}

void Model_UpdateScene(void)
{
    u8 *state = (u8 *)&gModel_RView;

    GsSetRefView2((GsRVIEW2 *)state);
    if (gModel_bSceneMode == 15) {
        Model_UpdateAnimatedBattle();
    }
    if (gModel_bSceneMode == 19) {
        Model_UpdateExodiaPresentation();
    }
    if (gModel_bSceneMode == 20) {
        Model_UpdateCreditsPresentation();
    }
    GsSetRefView2((GsRVIEW2 *)state);
    Model_DrawSlot(0);
    Model_DrawSlot(1);
    Model_DrawSlot(2);
    Model_UpdateSlotControl(0);
    Model_UpdateSlotControl(1);
    Model_DrawBackground();
    Model_StepBackgroundVramAnimation();
    Model_UpdateTintRequests();
    Model_UpdateSlotAnimation(0);
    Model_UpdateSlotAnimation(1);
    Model_UpdateSlotAnimation(2);
    if (!Model_IsEffectKeyQueueEmpty()) {
        Model_UpdateEffectKeyframes();
    }
    Model_UpdateCamera();
}

void Model_RunSlotHandlers(s32 index)
{
    ModelSlot *base = &gModel_aSlots[index];
    ModelSlotHeadEntry *slot = base->hmd_units;
    s32 (*callback)(s32) = Model_MapToWireframePrimitiveHandler;
    u32 *node;
    u32 *next;
    s32 i;

    if (base->is_active == 0) {
        return;
    }
    for (i = 0; i < base->hmd_unit_count; i++, slot++) {
        node = slot->primitive_data;
        if (node != 0) {
            while (1) {
                if (node[2] != 0) {
                    Model_RemapPrimitiveHandlers(
                        callback,
                        (ModelHandlerRunEntry *)&node[3],
                        node[1],
                        node[2]
                    );
                }
                next = (u32 *)node[0];
                if (next == (u32 *)-1) {
                    break;
                }
                node = next;
            }
        }
    }
}
