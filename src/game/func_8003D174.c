#include "../types.h"
#include "save_data.h"

extern u32 D_8009AF64;
extern u32 D_8009AF68;

s32 func_8003D174(u8 *data)
{
    s32 seed;
    s32 i;
    s32 *word;

    seed = SaveData_CalcCrc16(data, SAVE_DATA_PRIMARY_LENGTH) & 0xFFFF;
    word = (s32 *)(data + SAVE_DATA_PRIMARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_PRIMARY_MASK_WORD_COUNT;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);
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
    ) & 0xFFFF;
    word = (s32 *)(data + SAVE_DATA_SECONDARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_SECONDARY_MASK_WORD_COUNT;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);
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
    ) & 0xFFFF;
    word = (s32 *)(data + SAVE_DATA_TERTIARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_TERTIARY_MASK_WORD_COUNT;
    D_8009AF68 = seed | (seed << 16);
    D_8009AF64 = seed | (seed << 16);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    return 1;
}
