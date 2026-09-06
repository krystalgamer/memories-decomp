#include "../types.h"
#include "../psyq/libgte.h"
#include "trig_constants.h"

typedef struct {
    u8 pad_00[0x30];
    s16 out_x;
    s16 out_y;
    s16 unused;
    s16 x;
    s16 y;
} DisplayObjectPosition;

void func_8004318C(u8 *arg0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 x = (*(s16 *)(arg0 + 0x36) + arg1) >> 1;
    s32 y = (*(s16 *)(arg0 + 0x38) + arg2) >> 1;
    s32 scale = rcos(arg3);
    s32 scale2 = scale;

    *(u16 *)(arg0 + 0x30) =
        x + (*(s16 *)(arg0 + 0x36) - x) * scale / ONE;
    *(u16 *)(arg0 + 0x32) =
        y + (*(s16 *)(arg0 + 0x38) - y) * scale2 / ONE;
}

void func_80043230(
    DisplayObjectPosition *object,
    int target_x,
    int target_y,
    int phase
)
{
    int dx = object->x - target_x;
    int dy = object->y - target_y;
    int factor;

    if (phase < 0) {
        factor = rsin(phase + TRIG_ANGLE_QUARTER_TURN);
        object->out_x = object->x - (dx * factor) / ONE;
        object->out_y = object->y - (dy * factor) / ONE;
    } else {
        factor = -rsin(phase);
        object->out_x = target_x - (dx * factor) / ONE;
        object->out_y = target_y - (dy * factor) / ONE;
    }
}
