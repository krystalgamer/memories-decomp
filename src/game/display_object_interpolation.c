#include "../types.h"

typedef struct {
    u8 pad_00[0x30];
    s16 out_x;
    s16 out_y;
    s16 unused;
    s16 x;
    s16 y;
} DisplayObjectPosition;

extern s32 func_80086770();
extern int func_800866A0(int);

void func_8004318C(u8 *arg0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 x = (*(s16 *)(arg0 + 0x36) + arg1) >> 1;
    s32 y = (*(s16 *)(arg0 + 0x38) + arg2) >> 1;
    s32 scale = func_80086770(arg3);
    s32 scale2 = scale;

    *(u16 *)(arg0 + 0x30) =
        x + (*(s16 *)(arg0 + 0x36) - x) * scale / 4096;
    *(u16 *)(arg0 + 0x32) =
        y + (*(s16 *)(arg0 + 0x38) - y) * scale2 / 4096;
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
        factor = func_800866A0(phase + 0x400);
        object->out_x = object->x - (dx * factor) / 4096;
        object->out_y = object->y - (dy * factor) / 4096;
    } else {
        factor = -func_800866A0(phase);
        object->out_x = target_x - (dx * factor) / 4096;
        object->out_y = target_y - (dy * factor) / 4096;
    }
}
