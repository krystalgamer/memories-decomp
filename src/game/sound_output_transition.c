#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_transition.h"

void func_8004666C(void)
{
    SDValue *first = g_SDValue;
    SDValue *state;

    first->field_0049 = 255;
    state = g_SDValue;
    first->field_0512 = 64;
    state->flags_0040 = (state->flags_0040 & 0xFFFC) | 4;
    if ((state->flags_0040 & 0x80) &&
        state->field_1588 >= 0) {
        state->field_1588 = -16;
        state->field_1584 = 220;
    }
}

void func_800466C8(void)
{
    register SDValue *state asm("$3") = g_SDValue;
    register SDValue *flags asm("$4");

    if (state->flags_0040 & 0x80) {
        state->field_1588 = 8;
        state->field_1584 = 255;
        asm volatile("" : : : "memory");
        state = g_SDValue;
    }
    state->field_0049 = 0;
    asm volatile("" : : : "memory");
    flags = g_SDValue;
    state->field_0512 = -64;
    flags->flags_0040 &= 0xFFFB;
}

void func_8004671C(void)
{
    SpuCommonAttr entry;

    /* 707 is exactly these five bits, and the two fields it does not select
       are still written because retail writes them. */
    entry.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL |
                 SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX;
    entry.mvol.left = 16383;
    entry.mvol.right = 16383;
    entry.cd.volume.left = 32767;
    entry.cd.volume.right = 32767;
    entry.cd.reverb = SPU_OFF;
    entry.cd.mix = SPU_ON;
    SpuSetCommonAttr(&entry);
}
