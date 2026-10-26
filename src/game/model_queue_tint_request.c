#include "../types.h"
#include "model_queue_tint_request.h"
#include "model.h"

void Model_QueueTintRequestForParts(
    s32 a0, s32 a1, ModelTintColor a2, ModelTintColor a3, s32 a4, ...)
{
    u8 bits[8];
    u8 *arguments;
    s32 value;
    s32 byte_index;
    s32 i;
    u8 *cursor;

    i = 7;
    cursor = bits + 7;
    for (; i >= 0; i--)
        *cursor-- = 0;
    arguments = (u8 *)&a4 + 4;
    while (1) {
        arguments += 4;
        value = *(s32 *)(arguments - 4);
        if (value < 0)
            break;
        byte_index = value >> 3;
        bits[byte_index] |= 1 << (value - byte_index * 8);
    }
    a2.blend_mode = a1 & 127;
    Model_QueueTintRequest(a0, a1 & 128, a2, a3, a4, bits);
}

void Model_QueueTintRequest(
    s32 a0, s32 a1, ModelTintColor a2, ModelTintColor a3, s32 a4,
    const u8 *bits)
{
    ModelSlot *slot;
    ModelTintRequest *r;
    ModelTintRequest *end;
    register u8 *base = (u8 *)gModel_aTintRequests;
    s32 off;
    s32 fill;
    s32 f;
    s32 mode;
    s32 f1, f2, f3, f4, f5, f6;
    s32 sel;

    slot = &gModel_aSlots[a0];
    mode = (a0 & 1) << 1;
    sel = (a1 & 1) << 2;
    fill = 0xFF;
    off = 0;
    r = gModel_aTintRequests;
    end = r + MODEL_TINT_REQUEST_COUNT;
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
            f = f6 | ((slot->current_animation_row & 0x1F) << 3);
            r->flags = f;
            r->animation_position_snapshot = slot->animation_position;
            r->elapsed = 0;
            r->duration = (a4 *= 2);
            r->start = a2;
            r->end = a3;
            for (; mode < 8; mode++) {
                base[mode + sel + 1] = bits ? bits[mode] : fill;
            }
            break;
        }
        off += sizeof(ModelTintRequest);
        r++;
    } while ((s32)r < (s32)end);
}
