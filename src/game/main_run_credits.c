/*
 * Keep the staging pair absolute: `Main_RunCredits`
 *
 * The 540-byte runner at `0x8002DA1C` matches under the existing uniform
 * `gcc_2_8_1_g8_split` profile. Its six terminal refinement attempts failed
 * to link because an eight-byte scalar view of `D_801D5608` selected
 * out-of-range GP-relative relocations. The existing incomplete
 * `TextStagingValues` array and its `pair` member keep both staging stores
 * absolute without a new alias, profile, or inflated declaration.
 *
 * The source uses the shared save-state layout and reads `duelist_code` as
 * `u32` for the target's unsigned modulo-five sequence. The secret-number
 * table, memory-card operations, text-box lifecycle and model-scene calls use
 * their existing canonical declarations. The frame-delay byte selects the
 * existing DATA view; the active-mode byte remains scalar and GP-relative
 * through the guarded mode-state header pattern proposed in #3855.
 *
 * The shared completion-initialization label, save retry, secret-number
 * display and credits-scene phases retain their original branch order and
 * one-time flags. No register pins, inline assembly or local externs remain.
 * The canonical history and six-row terminal refinement history are preserved,
 * followed by one post-terminal resolution with this source evidence.
 */
#define D_8009B0C0_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "main_modes.h"
#include "main_mode_state.h"
#include "save_data.h"
#include "campaign_flags.h"
#include "credits_secret_numbers.h"
#include "text_staging.h"
#include "text_box_lifecycle.h"
#include "duel_effect.h"
#include "fade.h"
#include "data_transfer_request.h"
#include "func_80039794.h"
#include "graphics_frame.h"
#include "model_scene_setup.h"
#include "model_scene_states.h"
#include "model_cleanup.h"
#include "../unmatched.h"

void Main_RunCredits(void)
{
    u8 *save_state;
    u8 *textbox;
    u16 *number;
    u16 *table;
    s32 one;
    s32 flag;
    s32 mode;
    s32 state;
    s32 phase;

    SetGeomOffset(0xA0, 0x78);
    SetGeomScreen(0x12C);

    mode = D_8009B26C;
    if ((mode & 0x40) == 0) {
        D_8009B26C = mode | 0x40;
        Fade_InitIn();
    initialize_completion:
        save_state = (u8 *)gDuel_awPlayerDeck;
        *(s16 *)(save_state + SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET) = 0x30;
        for (flag = 0x20; flag < 0x120; flag++) {
            Library_UpdateCardUsedFlag(flag | 0x8000);
        }
        D_8009B26E = 0;
        save_state[0x3DE] = save_state[0x3DE] | 3;
    }

    state = D_8009B26E;
    one = 1;
    phase = state & 0xF;

    if (phase == one) {
        goto show_secret_number;
    }
    if (phase < 2) {
        if (phase == 0) {
            goto save_completion;
        }
        return;
    }
    if (phase == 2) {
        goto run_credits_scene;
    }
    return;

save_completion:
    if ((state & 0x80) == 0) {
        D_8009B26E = state | 0x80;
        SaveData_RequestWrite();
    }
    state = MemCardDialog_Poll();
    if (state == 0) {
        return;
    }
    if (state == 2) {
        goto initialize_completion;
    }
    D_8009B26E = one;
    return;

show_secret_number:
    if ((state & 0x80) == 0) {
        D_8009B26E = state | 0x80;
        state = ((u32)((SaveDataState *)gDuel_awPlayerDeck)->duelist_code) %
            CREDITS_SECRET_NUMBER_COUNT;
        table = (u16 *)gCredits_awSecretNumbers;
        number = &table[state * 2];
        D_801D5608[0].pair.lo = number[0];
        D_801D5608[0].pair.hi = number[1];
        TextBox_CreateFlagged(0, 0x23, 0x10, 0x70, 0x120, 0x20, 8);
    }
    func_80039794();
    textbox = (u8 *)D_800EB0F8;
    if ((*(u16 *)(textbox + 0x34) & 8) == 0) {
        TextBox_Destroy(textbox);
        D_8009B26E = 2;
    }
    return;

run_credits_scene:
    if ((state & 0x80) == 0) {
        D_8009B26E = state | 0x80;
        D_8009B0C0 = one;
        func_800530C4();
        func_800533D8();
        func_80059C9C();
        return;
    }
    func_80059CD0();
    func_80059CE4();
}
