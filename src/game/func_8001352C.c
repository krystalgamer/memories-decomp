#include "../types.h"
#include "func_800134E0.h"
#include "view_state.h"
#include "../psyq/libgte.h"
#include "model.h"

void func_8001352C(void) {
    ViewState *p = &D_800F2848;
    s32 n = -D_800F2848.field_00;
    s32 x = n * rcos(p->field_04) / MODEL_FIXED_ONE;
    s32 z = n * rsin(p->field_04) / MODEL_FIXED_ONE;
    s32 y = x * rsin(p->angle) / MODEL_FIXED_ONE;
    s32 w = x * rcos(p->angle) / MODEL_FIXED_ONE;

    x = w;
    func_800134E0(p, x, z, y);
}
