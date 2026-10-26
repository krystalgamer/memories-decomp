#include "../types.h"
#include "model_queue_tint_request.h"
#include "model.h"

void Model_QueueTintRequestForAllParts(
    s32 slot, s32 mode, ModelTintColor start, ModelTintColor end, s32 duration)
{
    start.blend_mode = mode & 0x7F;
    Model_QueueTintRequest(slot, 0, start, end, duration, 0);
}
