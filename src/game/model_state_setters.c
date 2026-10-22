#include "../types.h"
#include "model_graphics_state.h"
#include "model_state_setters.h"
#include "graphics_frame_controls.h"

void Model_SetScreenYOverride(int value)
{
    gModel_wScreenYOverride = value;
}

void Model_SetFrameStepOverride(int value)
{
    gGraphics_FrameStepControl = value;
}
