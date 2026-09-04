#include "../types.h"

extern u32 D_8009AF64;
extern u32 D_8009AF68;

/* Advances the two-word save-data mask state and returns the next word. */
u32 func_8003CE74(void)
{
    register u32 *state asm("$6") = &D_8009AF64;
    register u32 low asm("$3");
    register u32 next asm("$2");
    u32 high;
    u32 sum;

    low = state[0];
    high = state[1];
    next = (high << 31) | (low >> 1);
    next ^= low << 12;
    sum = high + (low & 1);
    high += sum;
    state[1] = high;
    return state[0] = next ^ (next >> 20);
}

/* CRC-16/XMODEM (poly 0x1021, zero-initialized) over data[0..len). */
u32 func_8003CEB8(u8 *data, s32 len)
{
    u16 crc = 0;
    s32 i;

    i = 0;
    if (len > 0) {
        do {
            s32 bit;

            crc ^= data[i] << 8;
            for (bit = 0; bit < 8; bit++) {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc = crc << 1;
                }
            }
            i++;
        } while (i < len);
    }
    return crc;
}

void func_8003CF14(u8 *data)
{
    s32 value = func_8003CEB8(data, 0x340);
    u32 seed = value & 0xFFFF;
    s32 *output = (s32 *)(data + 0x378);
    s32 i = 0xF;

    *(s16 *)(data + 0x37E) = value;
    *(s16 *)(data + 0x37C) = value;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);

    do {
        i--;
        *output = func_8003CE74();
        output--;
    } while (i != 0);

    value = func_8003CEB8(data + 0x380, 0x6C);
    seed = value & 0xFFFF;
    output = (s32 *)(data + 0x3F8);
    i = 4;

    *(s16 *)(data + 0x3FE) = value;
    *(s16 *)(data + 0x3FC) = value;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);

    do {
        i--;
        *output = func_8003CE74();
        output--;
    } while (i != 0);
}
