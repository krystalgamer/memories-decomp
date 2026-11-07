/* Model state and slot getters: the active slot, slot readiness, frame step
 * and per-slot animation speed, row, frame and length. In every executable
 * these are one contiguous run of text (0x80058DC0-0x80058F10 in the US
 * build), ending where the embedded Graphics_GetDefaultOrderingTable getter
 * begins. */
#include "../types.h"
#include "model_graphics_state.h"
#include "graphics_frame_controls.h"
#include "model.h"
#include "model_get_slot_readiness.h"
#include "model_get_frame_step.h"
#include "model_slot_queries.h"

/* Small globals src/game/model.c owns, declared here as common symbols the
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

s32 Model_GetSlotAnimationSpeed(s32 index)
{
    return gModel_aSlots[index].playback_speed;
}

s32 Model_GetSlotAnimationIndex(s32 index)
{
    return gModel_aSlots[index].current_animation_row;
}

s32 Model_GetSlotAnimationFrame(s32 index)
{
    return gModel_aSlots[index].animation_position >> 4;
}

u16 Model_GetSlotAnimationLength(s32 idx)
{
    u8 sub = gModel_aSlots[idx].current_animation_row;
    return gModel_aSlots[idx].animation_duration_rows[sub].max;
}
