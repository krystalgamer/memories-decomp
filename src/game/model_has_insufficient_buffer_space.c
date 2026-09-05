#include "../types.h"

#define MODEL_SLOT_CUSTOM_EXTERN
#include "model.h"

extern s32 D_800FE240 __attribute__((section(".data")));
extern u8 D_800A5768[];
extern u8 D_8009AFA2;
extern u8 D_800F2C40[];
extern u8 *func_80059520(s32);

s32 Model_HasInsufficientBufferSpace(s32 index, s32 type) {
    s32 used;
    s32 room;
    u8 *record;
    s32 size;

    used = D_800FE240 - (s32)D_800A5768;
    used -= D_8009AFA2 * 140000;
    room = 0x222E0 - used;
    record = &D_800F2C40[index * MODEL_SLOT_SIZE];
    if (type < 0) {
        type = func_80059520(index)[3];
    }
    size = *(u16 *)(record + 0xE00);
    if (type == 5) {
        room -= size * 2;
    } else {
        room -= size;
    }
    return room < MODEL_DATA_MIN_FREE_BYTES;
}
