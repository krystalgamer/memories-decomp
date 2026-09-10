#define D_8009B0C4_IN_DATA
#define GSD_BOUTPUTTYPE_IN_DATA
#include "../types.h"
#include "save_data.h"
#include "graphics_frame.h"
#include "sound.h"
#include "util_memory.h"

void SaveData_BuildPayload(u8 *data)
{
    u8 *copy;
    register s32 output_type asm("$3");
    s32 value;
    register s32 saved_value asm("$2");

    Util_CopyWords(data, gSaveData_aHeaderTemplate, SAVE_DATA_HEADER_SIZE);

    saved_value = D_8009B0C4;
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
            *((u8 *)((u32)data + i) +
              SAVE_DATA_RESERVED_TAIL_PAYLOAD_OFFSET) = 0;
            i++;
        } while (i < SAVE_DATA_RESERVED_TAIL_SIZE);
    }

    Util_CopyWords(
        data + SAVE_DATA_DUPLICATE_STATE_OFFSET,
        data + SAVE_DATA_HEADER_SIZE,
        SAVE_DATA_STATE_SIZE
    );
}
