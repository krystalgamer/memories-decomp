#include "../types.h"
#include "../game/func_8004A6D8.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_event_runtime.h"
#include "../game/sound_init.h"
#include "../game/sound_transfer_lifecycle.h"

s32 func_800496C4(u8 *input, s16 expected, s32 value)
{
    volatile int pad[2];
    int zero;
    u8 *initial;
    u8 *state;
    u8 *entry;

    /* This single-iteration form keeps zero register-backed under GCC 2.8.1. */
    do {
        zero = 0;
    } while (expected < value && expected >= value);
    initial = (u8 *)D_8009B458;
    *(int *)(initial + 0x818) = zero;
    if (expected == SD_TRANSFER_STATE_INACTIVE &&
        *(short *)(initial + 0x4A4) != expected)
        return SD_TRANSFER_ERROR;
    state = (u8 *)D_8009B458;
    *(short *)(state + 0x4A4) = zero;
    entry = state + 0x4A4;
    *(u8 **)(entry + 4) = input;
    *(int *)(entry + 8) = (*(u16 *)(input + 0x12) << 9) + 0xA20;
    *(int *)(entry + 0x10) = *(int *)(input + 0x0C) -
                             *(int *)(entry + 8);
    entry[0x18] = input[0x18];
    entry[0x1B] = input[0x19];
    *(int *)(entry + 0x14) = value;
    return 0;
}
