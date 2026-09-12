#include "../types.h"
#define MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE
#include "model_graphics_state.h"
#include "func_80058E1C.h"

int func_80058E1C(void)
{
    unsigned int result = 6;

    if (D_8009AFA3 < 6) {
        result = D_8009AFA3;
    }
    return result;
}
