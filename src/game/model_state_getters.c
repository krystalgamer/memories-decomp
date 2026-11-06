#include "../types.h"
#include "model_graphics_state.h"
#include "graphics_frame_controls.h"
#include "model.h"
#include "model_get_slot_readiness.h"
#include "model_get_frame_step.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
volatile u8 gGraphics_bFrameStep;
u8 gModel_bActiveSlotIndex;
u8 D_8009AFA6;

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

    if (*(u8 *)&gGraphics_bFrameStep < 6) {
        result = *(u8 *)&gGraphics_bFrameStep;
    }
    return result;
}
