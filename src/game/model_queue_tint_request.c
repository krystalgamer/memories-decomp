#include "../types.h"
#include "model_queue_tint_request.h"
#include "model.h"

void Model_QueueTintRequestForParts(
    s32 slot, s32 mode, ModelTintColor start, ModelTintColor end, s32 duration, ...)
{
    u8 part_mask[8];
    u8 *argument_cursor;
    s32 part_index;
    s32 mask_byte_index;
    s32 clear_index;
    u8 *clear_cursor;

    clear_index = 7;
    clear_cursor = part_mask + 7;
    for (; clear_index >= 0; clear_index--)
        *clear_cursor-- = 0;
    argument_cursor = (u8 *)&duration + 4;
    while (1) {
        argument_cursor += 4;
        part_index = *(s32 *)(argument_cursor - 4);
        if (part_index < 0)
            break;
        mask_byte_index = part_index >> 3;
        part_mask[mask_byte_index] |= 1 << (part_index - mask_byte_index * 8);
    }
    start.blend_mode = mode & 127;
    Model_QueueTintRequest(slot, mode & 128, start, end, duration, part_mask);
}

void Model_QueueTintRequest(
    s32 slot, s32 selection, ModelTintColor start, ModelTintColor end, s32 duration,
    const u8 *part_mask)
{
    ModelSlot *slot_state;
    ModelTintRequest *request;
    s32 i;
    s32 j;

    slot_state = &gModel_aSlots[slot];
    for (i = 0; i < MODEL_TINT_REQUEST_COUNT; i++) {
        request = &gModel_aTintRequests[i];
        /* The live bit is tested through the record's first byte, as
           Model_UpdateTintRequests also does. */
        if ((*(u8 *)request & 1) == 0) {
            request->active = 1;
            request->slot = slot;
            request->masked_parts_only = selection;
            request->animation_row = slot_state->current_animation_row;
            request->animation_position_snapshot = slot_state->animation_position;
            request->elapsed = 0;
            request->duration = duration * 2;
            request->start = start;
            request->end = end;
            for (j = 0; j < 8; j++) {
                gModel_aTintRequests[i].part_mask[j] = part_mask ? part_mask[j] : 0xFF;
            }
            break;
        }
    }
}
