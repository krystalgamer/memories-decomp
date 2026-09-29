#include "../../types.h"
#include "primary.h"

s32 func_8013A004(u8 *context, s32 command)
{
    ModelPrimaryEffectState *work = (ModelPrimaryEffectState *)context;
    s32 slot;

    if (command >= 0) {
        work->config = &D_8013A0DC[command % 100];
        work->animation = command / 100 + 3;
    } else {
        slot = Model_GetActiveSlotIndex();
        if (Model_GetSlotAnimationIndex(slot) == work->animation) {
            func_8005D994(slot, work->config->arg1, work->config->arg2,
                         work->config->arg3, &work->config->offset, work->config->arg5);
        }
    }
    return 0;
}
