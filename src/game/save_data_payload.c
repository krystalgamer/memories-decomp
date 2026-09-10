#define D_8009B0C4_IN_DATA
#define GSD_BOUTPUTTYPE_IN_DATA
#include "../types.h"
#define SAVE_DATA_DECLARE_MASK_STATE_LOCALLY
#include "save_data.h"
#include "graphics_frame.h"
#include "sound.h"
#include "text_sjis_to_glyph_codes.h"
#include "util_memory.h"

/* The save payload: sealing it, building it, applying it, and checking it.

   Each of the three payload regions is sealed the same way - a CRC-16 over
   the region, stored twice, then seeding the two-word mask generator from
   that CRC and writing a run of mask words backwards from the region's last
   mask slot. SaveData_WritePrimarySecondaryIntegrity and
   SaveData_WriteTertiaryIntegrity write those seals, SaveData_BuildPayload
   assembles the payload and calls both, and SaveData_ValidateIntegrity
   replays the same three seeds and compares the stored words against the
   generator instead of writing them. SaveData_ApplyRuntimeState is
   BuildPayload's inverse, loading the fields BuildPayload stores back into
   the runtime, and the last pair compares two payload states by duelist
   code and save sequence.

   The manifest used to spread this over six sources, recorded at
   gcc_2_8_1_g8, gcc_2_8_1_g8_split and gcc_2_8_1_g8_no_split. None of those
   differences was a boundary: every member compiles to an identical object
   at gcc_2_8_1_g8, so the unit builds there. */

/* Keeping these declarations at their original source position preserves the
 * exact GCC 2.8.1 allocation in SaveData_NextMaskWord. */
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

void SaveData_WriteTertiaryIntegrity(u8 *p)
{
    register s32 i;
    register u32 *dst;
    u16 v = SaveData_CalcCrc16(p + SAVE_DATA_TERTIARY_OFFSET,
                               SAVE_DATA_TERTIARY_LENGTH);

    dst = (u32 *)(p + SAVE_DATA_TERTIARY_MASK_LAST_OFFSET);
    i = SAVE_DATA_TERTIARY_MASK_WORD_COUNT;
    *(u16 *)(p + SAVE_DATA_TERTIARY_CHECKSUM_OFFSET + sizeof(u16)) = v;
    *(u16 *)(p + SAVE_DATA_TERTIARY_CHECKSUM_OFFSET) = v;
    {
        u32 seed = v | (v << SAVE_DATA_CRC16_BITS);

        gSaveData_dwMaskStateHigh = seed;
        gSaveData_dwMaskStateLow = seed;
    }
    do {
        --i;
        *dst = SaveData_NextMaskWord();
        dst--;
    } while (i);
}

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

#define gCampaignSceneIndex (*(u8 *)0x8009B27A)

void SaveData_ApplyRuntimeState(SaveDataState *state) {
    Text_SjisToGlyphCodes(D_801B125A, state->player_name_sjis, SAVE_DATA_PLAYER_NAME_CHAR_COUNT);

    /*
     * These discarded addresses use the retail assembler-temporary form.
     * Symbolic stores allocate ordinary registers and change exact codegen.
     */
    D_8009B0C4 = state->vblank_counter;
    gSaveDataSequence = state->save_sequence;
    gCampaignSceneIndex = state->campaign_scene_index;

    if (gSD_bOutputType < 0) {
        u8 output_type = state->output_type;

        gSD_bOutputType = output_type;
        SD_SetOutputType((s8)output_type);
    }
}

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

s32 SaveData_HasSameDuelistCode(SaveDataState *left, SaveDataState *right)
{
    s32 i;

    if (left->duelist_code == right->duelist_code) {
        i = 6;
        while (--i >= 0) {
        }
        return 1;
    }
    return 0;
}

s32 SaveData_MatchesDuelistAndCurrentSequence(
    SaveDataState *left,
    SaveDataState *right)
{
    s32 result;

    if (SaveData_HasSameDuelistCode(left, right)) {
        result = gSaveDataSequence == right->save_sequence;
    } else {
        result = 0;
    }
    return result;
}
