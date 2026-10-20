#include "../types.h"
#include "model_graphics_state.h"
#include "model_state_setters.h"

void Model_SetScreenYOverride(int value)
{
    gModel_wScreenYOverride = value;
}

void Model_SetFrameStepOverride(int value)
{
    D_8009AFA4 = value;
}
