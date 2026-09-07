#include "../types.h"
#include "save_data.h"

typedef struct {
    u8 pad_000[SAVE_DATA_SEQUENCE_OFFSET];
    u32 save_sequence;
    u32 vblank_counter;
    u8 pad_40C[
        SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET - SAVE_DATA_PLAYER_NAME_OFFSET
    ];
    u8 campaign_scene_index;
    u8 field_5DD;
    u8 output_type;
} SaveDataRuntimeState;

extern u8 D_801B125A[16];
extern void Text_SjisToGlyphCodes(void *, void *, s32);
extern u32 D_8009B3B8;
extern s8 gSD_bOutputType[16];
extern void SD_SetOutputType(s16);

#define D_8009B0C4 (*(u32 *)0x8009B0C4)
#define gCampaignSceneIndex (*(u8 *)0x8009B27A)
#define D_8009B408_write (*(u8 *)0x8009B408)

void SaveData_ApplyRuntimeState(SaveDataRuntimeState *state) {
    Text_SjisToGlyphCodes(D_801B125A, (u8 *)state + SAVE_DATA_PLAYER_NAME_OFFSET, SAVE_DATA_PLAYER_NAME_LENGTH);

    /*
     * These discarded addresses use the retail assembler-temporary form.
     * Symbolic stores allocate ordinary registers and change exact codegen.
     */
    D_8009B0C4 = state->vblank_counter;
    D_8009B3B8 = state->save_sequence;
    gCampaignSceneIndex = state->campaign_scene_index;

    if (gSD_bOutputType[0] < 0) {
        u8 output_type = state->output_type;

        D_8009B408_write = output_type;
        SD_SetOutputType((s8)output_type);
    }
}
