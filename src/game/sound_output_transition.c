#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_transition.h"

typedef struct {
    int field0;
    short field4;
    short field6;
    char pad8[8];
    short field10;
    short field12;
    int field14;
    int field18;
    int pad1C[3];
} Entry;

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
    Entry entry;
    entry.field0 = 707;
    entry.field4 = 16383;
    entry.field6 = 16383;
    entry.field10 = 32767;
    entry.field12 = 32767;
    entry.field14 = 0;
    entry.field18 = 1;
    SpuSetCommonAttr((SpuCommonAttr *)&entry);
}
