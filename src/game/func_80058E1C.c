#include "../types.h"
#include "model_graphics_state.h"
#include "func_80058E1C.h"

int func_80058E1C(void)
{
    register unsigned int result asm("$3") = 6;

    if (D_8009AFA3 < result) {
        result = D_8009AFA3;
    }
    return result;
}
