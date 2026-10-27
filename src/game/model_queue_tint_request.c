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
    ModelTintRequest *request_end;
    register u8 *raw_requests = (u8 *)gModel_aTintRequests;
    s32 request_offset;
    s32 full_mask_byte;
    s32 flag_work;
    s32 mode;
    s32 active_flags, slot_cleared_flags, slot_flags, selection_cleared_flags, selection_flags, row_cleared_flags;
    s32 sel;

    slot_state = &gModel_aSlots[slot];
    mode = (slot & 1) << 1;
    sel = (selection & 1) << 2;
    full_mask_byte = 0xFF;
    request_offset = 0;
    request = gModel_aTintRequests;
    request_end = request + MODEL_TINT_REQUEST_COUNT;
    do {
        if ((*(u8 *)request & 1) == 0) {
            flag_work = request->flags;
            active_flags = flag_work | 1;
            slot_cleared_flags = active_flags & 0xFFFD;
            slot_flags = slot_cleared_flags | mode;
            selection_cleared_flags = slot_flags & 0xFFFB;
            selection_flags = selection_cleared_flags | sel;
            do {
                mode = 0;
            } while (0);
            request->flags = selection_flags;
            row_cleared_flags = selection_flags & 0xFF07;
            sel = request_offset;
            flag_work = row_cleared_flags | ((slot_state->current_animation_row & 0x1F) << 3);
            request->flags = flag_work;
            request->animation_position_snapshot = slot_state->animation_position;
            request->elapsed = 0;
            request->duration = (duration *= 2);
            request->start = start;
            request->end = end;
            for (; mode < 8; mode++) {
                raw_requests[mode + sel + 1] = part_mask ? part_mask[mode] : full_mask_byte;
            }
            break;
        }
        request_offset += sizeof(ModelTintRequest);
        request++;
    } while ((s32)request < (s32)request_end);
}
