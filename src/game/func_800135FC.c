#include "../types.h"
#include "func_800134E0.h"
#include "view_state.h"
#include "../psyq/libgte.h"
#include "model.h"

void func_800135FC(void)
{
    ViewState *record = &D_800F2848;
    s32 distance = -record->field_00;
    s32 x = distance * rcos(record->angle) / MODEL_FIXED_ONE;
    s32 z = distance * rsin(record->angle) / MODEL_FIXED_ONE;
    s32 y = x * rsin(
        record->field_04 + MODEL_ANGLE_QUARTER_TURN
    ) / MODEL_FIXED_ONE;
    s32 w = x * rcos(
        record->field_04 + MODEL_ANGLE_QUARTER_TURN
    ) / MODEL_FIXED_ONE;

    x = w;
    func_800134E0(record, x, y, z);
}
