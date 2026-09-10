#define D_8009B0C4_IN_DATA
#include "../types.h"
#include "save_data.h"
#include "graphics_frame.h"
#include "sound.h"
#include "text_sjis_to_glyph_codes.h"

extern s8 gSD_bOutputType[16];

#define gCampaignSceneIndex (*(u8 *)0x8009B27A)
#define D_8009B408_write (*(u8 *)0x8009B408)

void SaveData_ApplyRuntimeState(SaveDataState *state) {
    Text_SjisToGlyphCodes(D_801B125A, state->player_name_sjis, SAVE_DATA_PLAYER_NAME_CHAR_COUNT);

    /*
     * These discarded addresses use the retail assembler-temporary form.
     * Symbolic stores allocate ordinary registers and change exact codegen.
     */
    D_8009B0C4 = state->vblank_counter;
    gSaveDataSequence = state->save_sequence;
    gCampaignSceneIndex = state->campaign_scene_index;

    if (gSD_bOutputType[0] < 0) {
        u8 output_type = state->output_type;

        D_8009B408_write = output_type;
        SD_SetOutputType((s8)output_type);
    }
}
