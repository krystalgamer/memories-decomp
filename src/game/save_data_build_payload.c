#include "../types.h"
#include "save_data.h"

extern u8 D_801D4000[];
extern s32 D_8009B0C4[];
extern s8 gSD_bOutputType __attribute__((section(".data")));
extern s32 gSaveDataSequence;

extern void Util_CopyWords(void *, void *, u32);
void SaveData_BuildPayload(u8 *data)
{
    u8 *copy;
    register s32 output_type asm("$3");
    s32 value;
    register s32 saved_value asm("$2");

    Util_CopyWords(data, D_801D4000, SAVE_DATA_HEADER_SIZE);

    saved_value = D_8009B0C4[0];
    output_type = gSD_bOutputType;
    *(s32 *)(data + SAVE_DATA_HEADER_SIZE + SAVE_DATA_TERTIARY_OFFSET) = 0;
    *(s32 *)(data + SAVE_DATA_HEADER_SIZE + SAVE_DATA_VBLANK_COUNTER_OFFSET) =
        saved_value;

    if (output_type < 0) {
        gSD_bOutputType = 0;
    }

    data[SAVE_DATA_HEADER_SIZE + SAVE_DATA_OUTPUT_TYPE_OFFSET] = gSD_bOutputType;
    copy = data + SAVE_DATA_HEADER_SIZE;
    value = gSaveDataSequence + 1;
    *(s32 *)(data + SAVE_DATA_HEADER_SIZE + SAVE_DATA_SEQUENCE_OFFSET) = value;
    *(s32 *)(data + SAVE_DATA_DUPLICATE_STATE_OFFSET +
             SAVE_DATA_SEQUENCE_OFFSET) = value;

    SaveData_WritePrimarySecondaryIntegrity(copy);
    SaveData_WriteTertiaryIntegrity(copy);

    {
        u32 i;

        i = 0;
        do {
            *((u8 *)((u32)data + i) + 0x828) = 0;
            i++;
        } while (i < 0x58);
    }

    Util_CopyWords(
        data + SAVE_DATA_DUPLICATE_STATE_OFFSET,
        data + SAVE_DATA_HEADER_SIZE,
        SAVE_DATA_STATE_SIZE
    );
}
