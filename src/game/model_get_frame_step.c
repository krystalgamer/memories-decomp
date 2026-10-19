#include "../types.h"
#define MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE
#include "model_graphics_state.h"
#include "model_get_frame_step.h"

int Model_GetFrameStep(void)
{
    unsigned int result = 6;

    if (D_8009AFA3 < 6) {
        result = D_8009AFA3;
    }
    return result;
}
