#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "camera_view.h"
#include "model_scene_controllers.h"
#include "func_800528AC.h"
#include "model_slot_control.h"
#include "func_8005A53C.h"
#include "model.h"
#include "model_background.h"
#include "model_cleanup.h"
#include "model_graphics_state.h"
#include "model_handler_registry.h"
#include "model_scene_states.h"
#include "model_slot_properties.h"
#include "model_effect_state.h"
#include "model_transfer_state.h"
#include "../unmatched.h"
#include "../game/model_draw_slot.h"

void Model_SetPresentationBgmCommand(s32 value)
{
    D_8009AF96 = value;
}

void Model_StartExodiaPresentation(void)
{
    Model_ResetSceneState();
    Model_ResetSceneView();
    D_8009AF94 = 0x13;
    func_8005F3B8(0, 10000, 0xE00, 0, 0);
    Model_SetSlotTintTarget(0, 5, 0, 0, 0);
    D_8009AF9A = -1;
}

s32 Model_IsExodiaPresentationComplete(void)
{
    return D_8009AF9A == -2;
}

void Model_StartCreditsPresentation(void)
{
    Model_ResetSceneState();
    Model_ResetSceneView();
    D_8009AF94 = 20;
    D_8009AF9A = -1;
}

s32 Model_IsCreditsPresentationComplete(void)
{
    return D_8009AF9A == -2;
}

void Model_UpdateScene(void)
{
    u8 *state = (u8 *)&gModel_RView;

    GsSetRefView2((GsRVIEW2 *)state);
    if (D_8009AF94 == 15) {
        Model_UpdateAnimatedBattle();
    }
    if (D_8009AF94 == 19) {
        Model_UpdateExodiaPresentation();
    }
    if (D_8009AF94 == 20) {
        Model_UpdateCreditsPresentation();
    }
    GsSetRefView2((GsRVIEW2 *)state);
    Model_DrawSlot(0);
    Model_DrawSlot(1);
    Model_DrawSlot(2);
    func_800559D4(0);
    func_800559D4(1);
    Model_DrawBackground();
    Model_StepBackgroundVramAnimation();
    func_800528AC();
    func_800556E8(0);
    func_800556E8(1);
    func_800556E8(2);
    if (!Model_IsEffectKeyQueueEmpty()) {
        Model_UpdateEffectKeyframes();
    }
    func_80051A48();
}

void Model_RunSlotHandlers(s32 index)
{
    ModelSlot *base = &gModel_aSlots[index];
    ModelSlotHeadEntry *slot = base->field_000;
    s32 (*callback)(s32) = func_8005FC1C;
    u32 *node;
    u32 *next;
    s32 i;

    if (base->field_E1F == 0) {
        return;
    }
    for (i = 0; i < base->field_E1A; i++, slot++) {
        node = slot->field_04;
        if (node != 0) {
            while (1) {
                if (node[2] != 0) {
                    func_8005A53C(
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
