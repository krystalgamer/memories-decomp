#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "sound.h"
#include "sound_output_state.h"

s32 func_80045054(void)
{
    s32 select = SpuReadDecodedData(
        (SpuDecodedData *)g_SDValue->buffer_053C, SPU_CDONLY
    );
    SDValue *choice_state = g_SDValue;
    s16 *values;
    s32 i;
    SDValue *loaded;
    SDValue *state;

    *(s32 *)((u8 *)choice_state + 0x538) = select;
    if (select == SPU_DECODED_FIRSTHALF) {
        values = (s16 *)choice_state->buffer_ptrs_153C[0];
    } else {
        values = (s16 *)choice_state->buffer_ptrs_153C[1];
    }
    loaded = g_SDValue;
    i = 0;
    state = loaded;
    *(s32 *)((u8 *)state + 0x154C) = 0;
    *(s32 *)((u8 *)state + 0x1550) = 0;
    do {
        s32 value = *values;
        u32 square = value * value;
        *(u32 *)((u8 *)state + 0x154C) += square >> 8;
        i++;
        values++;
    } while (i < SD_MIX_SAMPLE_COUNT);
    {
        s32 result;
        s32 flags;
        s32 other;
        state = g_SDValue;
        do {
            result = *(s16 *)((u8 *)state + 0x154E);
        } while (0);
        flags = state->flags_0040;
        other = *(s16 *)((u8 *)state + 0x1552);
        flags &= 3;
        *(s32 *)((u8 *)state + 0x154C) = result;
        *(s32 *)((u8 *)state + 0x1550) = other;
        if (!flags) {
            return result;
        }
        return 0;
    }
}
