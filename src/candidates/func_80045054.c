/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 8 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_output_state.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"

#include "../game/sound.h"
#include "../game/sound_output_state.h"

s32 func_80045054(void)
{
    int select = SpuReadDecodedData(
        (SpuDecodedData *)((u8 *)g_SDValue + 0x53C),
        SPU_CDONLY
    );
    register u8 *choice_state asm("$3") = (u8 *)g_SDValue;
    register short *values asm("$4");
    register int i asm("$6");
    register u8 *loaded asm("$2");
    register u8 *state asm("$5");

    *(int *)(choice_state + 0x538) = select;
    if (select == SPU_DECODED_FIRSTHALF)
        values = *(short **)(choice_state + 0x153C);
    else
        values = *(short **)(choice_state + 0x1540);
    loaded = (u8 *)g_SDValue;
    asm volatile("" : "+r"(loaded));
    i = 0;
    state = loaded;
    *(int *)(state + 0x154C) = 0;
    *(int *)(state + 0x1550) = 0;
    do {
        int value = *values;
        unsigned int square = value * value;
        *(unsigned int *)(state + 0x154C) += square >> 8;
        i++;
        values++;
    } while (i < SD_MIX_SAMPLE_COUNT);
    {
        register int result asm("$2");
        register int flags asm("$3");
        register int other asm("$4");
        state = (u8 *)g_SDValue;
        result = *(short *)(state + 0x154E);
        flags = *(u16 *)(state + 0x40);
        other = *(short *)(state + 0x1552);
        flags &= 3;
        *(int *)(state + 0x154C) = result;
        *(int *)(state + 0x1550) = other;
        if (flags)
            result = 0;
        return result;
    }
}

#include "../game/sound_buffer_init.h"

