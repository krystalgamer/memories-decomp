#include "../types.h"
#include "model.h"
#include "graphics_frame.h"
#include "model_graphics_state.h"
#include "model_slot_properties.h"
#include "model_has_insufficient_buffer_space.h"

extern s32 D_800FE240 __attribute__((section(".data")));

s32 Model_HasInsufficientBufferSpace(s32 index, s32 type) {
    s32 used;
    s32 room;
    ModelSlot *record;
    s32 size;

    used = D_800FE240 - (s32)D_800A5768;
    used -= D_8009AFA2 * 140000;
    room = 0x222E0 - used;
    /* Typed indexing changes the accepted address construction. */
    record = (ModelSlot *)((u8 *)D_800F2C40 + index * MODEL_SLOT_SIZE);
    if (type < 0) {
        type = func_80059520(index)[3];
    }
    size = *(u16 *)&record->pad_E00[0];
    if (type == 5) {
        room -= size * 2;
    } else {
        room -= size;
    }
    return room < MODEL_DATA_MIN_FREE_BYTES;
}
