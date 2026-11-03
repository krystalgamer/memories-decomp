#include "../../types.h"
#include "primary.h"

s32 ModelPrimaryEffect_HandleRequest(u8 *context, s32 command)
{
    ModelPrimaryEffectState *work = (ModelPrimaryEffectState *)context;
    s32 slot;

    if (command >= 0) {
        work->config = &gModelPrimaryEffect_Configs[command % 100];
        work->animation = command / 100 + 3;
    } else {
        slot = Model_GetActiveSlotIndex();
        if (Model_GetSlotAnimationIndex(slot) == work->animation) {
            Model_QueueRelativeCameraOrbit(slot, work->config->arg1, work->config->arg2,
                         work->config->arg3, &work->config->offset, work->config->arg5);
        }
    }
    return 0;
}
