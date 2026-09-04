#include "../types.h"

#include "sound.h"

extern int func_80076D20(short *, int);
extern s32 func_80045208(u16, s32);
extern void func_80045BE8(u8 *);

void func_8004503C(short value, unsigned char flag, int unused)
{
    g_SDValue->field_0512 = value;
    g_SDValue->field_0049 = flag;
}

int func_80045054(void)
{
    int select = func_80076D20(
        (short *)((u8 *)g_SDValue + 0x53C),
        5
    );
    register u8 *choice_state asm("$3") = (u8 *)g_SDValue;
    register short *values asm("$4");
    register int i asm("$6");
    register u8 *loaded asm("$2");
    register u8 *state asm("$5");

    *(int *)(choice_state + 0x538) = select;
    if (select == 0)
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
    } while (i < 256);
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

void func_80045114(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->flags_004A & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands[count - 2].command == 0x11)
                return;
        }
    }
    func_8004503C(-0x20, 0, 1);
    {
        u8 local[SD_COMMAND_RECORD_SIZE];
        local[0] = 0x11;
        func_80045BE8(local);
    }
}

s16 func_800451E0(u16 value)
{
    return func_80045208(value, 0x80);
}
