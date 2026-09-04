#include "../types.h"

#include "sound.h"

extern void func_800771B0(void *);
extern int func_80077150(int, int);

int func_800496C4(u8 *input, short expected, int value)
{
    volatile int pad[2];
    register int zero asm("$8") = 0;
    register u8 *initial asm("$3") = (u8 *)D_8009B458;
    u8 *state;
    u8 *entry;

    *(int *)(initial + 0x818) = zero;
    if (expected == -1 && *(short *)(initial + 0x4A4) != expected)
        return -1;
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

int func_8004975C(int value, short expected)
{
    register int saved;
    SDSecondaryState *state = D_8009B458;
    short current = state->transfer.field_0000;

    if (current != expected)
        return -1;
    saved = value;
    {
        SDSecondaryTransfer *entry = &state->transfer;
        func_800771B0(entry->field_0014);
        if (func_80077150(saved, entry->field_0010) != entry->field_0010)
            return -1;
        entry->field_000C = saved;
    }
    return current;
}
