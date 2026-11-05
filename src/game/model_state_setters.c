#include "../types.h"
#include "model_graphics_state.h"
#include "model_state_setters.h"
#include "graphics_frame_controls.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gGraphics_FrameStepControl;
u16 gModel_wScreenYOverride;

void Model_SetScreenYOverride(int value)
{
    gModel_wScreenYOverride = value;
}

void Model_SetFrameStepOverride(int value)
{
    gGraphics_FrameStepControl = value;
}
