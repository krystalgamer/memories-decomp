#include "../types.h"
#define MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE
#include "model_graphics_state.h"
#include "model.h"
#include "model_get_slot_readiness.h"
#include "model_get_frame_step.h"

int func_80058DC0(void)
{
    return D_8009AFA6;
}

int Model_GetActiveSlotIndex(void)
{
    return gModel_bActiveSlotIndex;
}

int Model_GetSlotReadiness(int index)
{
    ModelSlot *entry = &gModel_aSlots[index];
    if (entry->load_step != 0xFF) return 2;
    return entry->is_active != 0;
}

int Model_GetFrameStep(void)
{
    unsigned int result = 6;

    if (gGraphics_bFrameStep < 6) {
        result = gGraphics_bFrameStep;
    }
    return result;
}
