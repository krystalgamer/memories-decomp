/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_run_duel_and_library.c.
 */
#define D_8009B0C0_IN_DATA
#define D_8009B362_IN_DATA
#define D_8009B369_IN_DATA
#define D_8009B368_IN_DATA
#include "../types.h"
#include "../game/func_800179F4.h"
#include "../game/graphics_frame.h"
#include "../game/func_800339D0.h"
#include "../game/save_data.h"
#include "../game/main_frame.h"
#include "../game/fade.h"
#include "../game/file_transfer.h"
#include "../game/main_reset_frontend_runtime.h"
#include "../game/main_services.h"
#include "../game/sound.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"
#include "../game/library_runtime.h"
#include "../game/duel_scene_update.h"
#include "../game/duel_side_state.h"
#include "../unmatched.h"

extern u8 D_8009B26C;
extern u8 D_8009B2F8[9];
extern s8 gDuel_bOpponentID[9];
extern u8 gCampaignSceneIndex[9];
extern u8 D_8009B370[9];
extern u16 D_8009B16C[9];
#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "../game/high_memory_addresses.h"

void Main_RunDuel(void)
{
    u8 value = D_8009B26C;
    int state;

    if (!(value & 0x40)) {
        D_8009B26C = value | 0x40;
        D_8009B26E = 1;
        if (!D_8009B369 && gDuel_bOpponentID[0] >= 0)
            D_8009B26E = 0;
        D_8009B0A3[0] = 10;
        return;
    }

    value = D_8009B26E;
    state = value & 0xF;
    switch (state) {
    case 0:
        if (!(value & 0x80)) {
            D_8009B26E = value | 0x80;
            D_8009B2F8[0] = 0x80;
            func_800323F8((u32)D_80010000[0].payload_bases[0], (u8 *)gDuel_awPlayerDeck, 0, 0x80);
            Fade_WaitIn();
        } else if (func_80033BE8() == 0) {
            SD_BGMFadeOut();
            Fade_WaitOut();
            Main_ResetFrontendRuntime();
            D_8009B26E = 1;
        }
        break;
    case 1:
        if (!(value & 0x80)) {
            D_8009B26E = value | 0x80;
            func_800179F4();
        } else {
            func_80024388();
            if (D_8009B16C[0] & 0x2000)
                D_8009B26E = 2;
        }
        break;
    case 2:
    {
        u8 *table = D_8009B370;
        u8 next;

        Fade_WaitOut();
        SD_BGMFadeOut();
        func_80047AD0(2);
        func_800134B4();
        D_8009B0A3[0] = 6;
        func_80012D84(4);
        File_WaitForTransfers();
        next = D_8009B368;
        __asm__ volatile("nop");
        D_8009B26C = next;
        if (D_8009B26C == state)
            gCampaignSceneIndex[0] = table[D_8009B362 * 2];
        break;
    }
    }
}

