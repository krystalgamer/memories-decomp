#include "../../types.h"
#include "primary.h"

s32 func_8013A004(u8 *context, s32 command)
{
    ModelPrimaryCopyState *work = (ModelPrimaryCopyState *)context;
    s32 frame;
    s32 slot;

    if (command >= 0) {
        work->config = &D_8013A118[command];
        work->tick = 0;
    } else {
        frame = (work->tick / work->config->delay) % work->config->frames;
        slot = Model_GetActiveSlotIndex() << 8;
        work->rectangle.x = work->config->source_x + work->config->width * frame + slot;
        work->rectangle.y = work->config->source_y + 256;
        work->rectangle.w = work->config->width;
        work->rectangle.h = work->config->height;
        MoveImage(&work->rectangle, slot + work->config->destination_x,
                  work->config->destination_y + 256);
        work->tick++;
    }
    return 0;
}
