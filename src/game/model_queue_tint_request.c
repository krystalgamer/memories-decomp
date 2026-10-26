#include "../types.h"
#include "model_queue_tint_request.h"
#include "model.h"

void Model_QueueTintRequestForParts(
    s32 slot, s32 mode, ModelTintColor start, ModelTintColor end, s32 duration, ...)
{
    u8 part_mask[8];
    u8 *arguments;
    s32 value;
    s32 byte_index;
    s32 i;
    u8 *cursor;

    i = 7;
    cursor = part_mask + 7;
    for (; i >= 0; i--)
        *cursor-- = 0;
    arguments = (u8 *)&duration + 4;
    while (1) {
        arguments += 4;
        value = *(s32 *)(arguments - 4);
        if (value < 0)
            break;
        byte_index = value >> 3;
        part_mask[byte_index] |= 1 << (value - byte_index * 8);
    }
    start.blend_mode = mode & 127;
    Model_QueueTintRequest(slot, mode & 128, start, end, duration, part_mask);
}

void Model_QueueTintRequest(
    s32 slot, s32 selection, ModelTintColor start, ModelTintColor end, s32 duration,
    const u8 *part_mask)
{
    ModelSlot *slot_state;
    ModelTintRequest *r;
    ModelTintRequest *request_end;
    register u8 *base = (u8 *)gModel_aTintRequests;
    s32 off;
    s32 fill;
    s32 f;
    s32 mode;
    s32 f1, f2, f3, f4, f5, f6;
    s32 sel;

    slot_state = &gModel_aSlots[slot];
    mode = (slot & 1) << 1;
    sel = (selection & 1) << 2;
    fill = 0xFF;
    off = 0;
    r = gModel_aTintRequests;
    request_end = r + MODEL_TINT_REQUEST_COUNT;
    do {
        if ((*(u8 *)r & 1) == 0) {
            f = r->flags;
            f1 = f | 1;
            f2 = f1 & 0xFFFD;
            f3 = f2 | mode;
            f4 = f3 & 0xFFFB;
            f5 = f4 | sel;
            do {
                mode = 0;
            } while (0);
            r->flags = f5;
            f6 = f5 & 0xFF07;
            sel = off;
            f = f6 | ((slot_state->current_animation_row & 0x1F) << 3);
            r->flags = f;
            r->animation_position_snapshot = slot_state->animation_position;
            r->elapsed = 0;
            r->duration = (duration *= 2);
            r->start = start;
            r->end = end;
            for (; mode < 8; mode++) {
                base[mode + sel + 1] = part_mask ? part_mask[mode] : fill;
            }
            break;
        }
        off += sizeof(ModelTintRequest);
        r++;
    } while ((s32)r < (s32)request_end);
}
