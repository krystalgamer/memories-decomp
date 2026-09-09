#include "../types.h"
#include "save_data.h"

s32 SaveData_ValidateIntegrity(u8 *data)
{
    s32 seed;
    s32 i;
    s32 *word;

    seed = SaveData_CalcCrc16(data, SAVE_DATA_PRIMARY_LENGTH) &
           SAVE_DATA_CRC16_MASK;
    word = (s32 *)(data + SAVE_DATA_PRIMARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_PRIMARY_MASK_WORD_COUNT;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    seed = SaveData_CalcCrc16(
        data + SAVE_DATA_SECONDARY_OFFSET,
        SAVE_DATA_SECONDARY_LENGTH
    ) & SAVE_DATA_CRC16_MASK;
    word = (s32 *)(data + SAVE_DATA_SECONDARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_SECONDARY_MASK_WORD_COUNT;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    seed = SaveData_CalcCrc16(
        data + SAVE_DATA_TERTIARY_OFFSET,
        SAVE_DATA_TERTIARY_LENGTH
    ) & SAVE_DATA_CRC16_MASK;
    word = (s32 *)(data + SAVE_DATA_TERTIARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_TERTIARY_MASK_WORD_COUNT;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    return 1;
}
