#ifndef MEMORIES_DECOMP_FUNC_80058E1C_H
#define MEMORIES_DECOMP_FUNC_80058E1C_H

#include "../types.h"

/* Returns the frame step: the volatile counter at D_8009AFA3 clamped to at
 * most 6. Callers scale an animation rate by it (m->field_E0D * step) or test
 * it directly against 2, so the result is consumed as a signed value even
 * though the definition computes it in an unsigned register. */
s32 func_80058E1C(void);

#endif
