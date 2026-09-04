#include "../types.h"

#define SAVE_DATA_PRIMARY_LENGTH 0x340
#define SAVE_DATA_PRIMARY_MASK_LAST_OFFSET 0x378
#define SAVE_DATA_PRIMARY_MASK_WORD_COUNT 15
#define SAVE_DATA_PRIMARY_CHECKSUM_OFFSET \
    (SAVE_DATA_PRIMARY_MASK_LAST_OFFSET + sizeof(s32))
#define SAVE_DATA_SECONDARY_OFFSET 0x380
#define SAVE_DATA_SECONDARY_LENGTH 0x6C
#define SAVE_DATA_SECONDARY_MASK_LAST_OFFSET 0x3F8
#define SAVE_DATA_SECONDARY_MASK_WORD_COUNT 4
#define SAVE_DATA_SECONDARY_CHECKSUM_OFFSET \
    (SAVE_DATA_SECONDARY_MASK_LAST_OFFSET + sizeof(s32))

extern u32 D_8009AF64;
extern u32 D_8009AF68;

/* Advances the two-word save-data mask state and returns the next word. */
u32 SaveData_NextMaskWord(void)
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
    s32 value = func_8003CEB8(data, SAVE_DATA_PRIMARY_LENGTH);
    u32 seed = value & 0xFFFF;
    s32 *output = (s32 *)(data + SAVE_DATA_PRIMARY_MASK_LAST_OFFSET);
    s32 i = SAVE_DATA_PRIMARY_MASK_WORD_COUNT;

    *(s16 *)(data + SAVE_DATA_PRIMARY_CHECKSUM_OFFSET + sizeof(s16)) = value;
    *(s16 *)(data + SAVE_DATA_PRIMARY_CHECKSUM_OFFSET) = value;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);

    do {
        i--;
        *output = SaveData_NextMaskWord();
        output--;
    } while (i != 0);

    value = func_8003CEB8(
        data + SAVE_DATA_SECONDARY_OFFSET,
        SAVE_DATA_SECONDARY_LENGTH
    );
    seed = value & 0xFFFF;
    output = (s32 *)(data + SAVE_DATA_SECONDARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_SECONDARY_MASK_WORD_COUNT;

    *(s16 *)(data + SAVE_DATA_SECONDARY_CHECKSUM_OFFSET + sizeof(s16)) = value;
    *(s16 *)(data + SAVE_DATA_SECONDARY_CHECKSUM_OFFSET) = value;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);

    do {
        i--;
        *output = SaveData_NextMaskWord();
        output--;
    } while (i != 0);
}
