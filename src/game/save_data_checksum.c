#include "../types.h"
#include "save_data.h"

extern u32 gSaveData_dwMaskStateLow;
extern u32 gSaveData_dwMaskStateHigh;

/* Advances the two-word save-data mask state and returns the next word. */
u32 SaveData_NextMaskWord(void)
{
    register u32 *state asm("$6") = &gSaveData_dwMaskStateLow;
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
u32 SaveData_CalcCrc16(u8 *data, s32 len)
{
    u16 crc = 0;
    s32 i;

    i = 0;
    if (len > 0) {
        do {
            s32 bit;

            crc ^= data[i] << 8;
            for (bit = 0; bit < 8; bit++) {
                if (crc & SAVE_DATA_CRC16_HIGH_BIT) {
                    crc = (crc << 1) ^ SAVE_DATA_CRC16_POLYNOMIAL;
                } else {
                    crc = crc << 1;
                }
            }
            i++;
        } while (i < len);
    }
    return crc;
}

void SaveData_WritePrimarySecondaryIntegrity(u8 *data)
{
    s32 value = SaveData_CalcCrc16(data, SAVE_DATA_PRIMARY_LENGTH);
    u32 seed = value & SAVE_DATA_CRC16_MASK;
    s32 *output = (s32 *)(data + SAVE_DATA_PRIMARY_MASK_LAST_OFFSET);
    s32 i = SAVE_DATA_PRIMARY_MASK_WORD_COUNT;

    *(s16 *)(data + SAVE_DATA_PRIMARY_CHECKSUM_OFFSET + sizeof(s16)) = value;
    *(s16 *)(data + SAVE_DATA_PRIMARY_CHECKSUM_OFFSET) = value;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);

    do {
        i--;
        *output = SaveData_NextMaskWord();
        output--;
    } while (i != 0);

    value = SaveData_CalcCrc16(
        data + SAVE_DATA_SECONDARY_OFFSET,
        SAVE_DATA_SECONDARY_LENGTH
    );
    seed = value & SAVE_DATA_CRC16_MASK;
    output = (s32 *)(data + SAVE_DATA_SECONDARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_SECONDARY_MASK_WORD_COUNT;

    *(s16 *)(data + SAVE_DATA_SECONDARY_CHECKSUM_OFFSET + sizeof(s16)) = value;
    *(s16 *)(data + SAVE_DATA_SECONDARY_CHECKSUM_OFFSET) = value;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);

    do {
        i--;
        *output = SaveData_NextMaskWord();
        output--;
    } while (i != 0);
}
