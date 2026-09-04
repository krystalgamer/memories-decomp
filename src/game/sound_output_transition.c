#include "../types.h"

#include "sound.h"

void func_8004666C(void)
{
    SDValue *first = g_SDValue;
    SDValue *state;

    first->field_0049 = 255;
    asm volatile("" : : : "memory");
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
    register u8 *state asm("$3") = (u8 *)g_SDValue;
    register u8 *flags asm("$4");

    if (*(u16 *)(state + 0x40) & 0x80) {
        *(s16 *)(state + 0x1588) = 8;
        state[0x1584] = 255;
        asm volatile("" : : : "memory");
        state = (u8 *)g_SDValue;
    }
    state[0x49] = 0;
    asm volatile("" : : : "memory");
    flags = (u8 *)g_SDValue;
    *(s16 *)(state + 0x512) = -64;
    *(u16 *)(flags + 0x40) &= 0xFFFB;
}
