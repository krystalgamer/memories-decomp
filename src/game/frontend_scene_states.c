#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#define D_8009B36A_IN_DATA
#define GDUEL_BEFFECTSTATE_IN_DATA
#define D_8009B368_IN_DATA
#define D_8009B3EA_IN_DATA
#define D_8009B3ED_IN_DATA
#include "../types.h"
#include "mem_card.h"
#include "frontend_debug_tables.h"
#include "duel_effect.h"
#include "duel_side_state.h"
#include "duel_effect_mode_7.h"
#include "duel_interface_setup.h"
#include "file_transfer.h"
#include "func_80024DC8.h"
#include "save_data_update_trade_load.h"
#include "movie_playback_control.h"
#include "../unmatched.h"
#include "main_services.h"
#include "frontend_scene_states.h"
#include "main_mode_state.h"

/* The frontend's eight contiguous scene states, in address order. They are
   the complete gcc_2_8_1_g8 run between DebugMenu_UpdateCampaignEntry and
   DebugMenu_EnterMappedMode.
   D_8009B2EB carries the entered and asynchronous-pending state shared by the
   setup/polling steps. */

void func_80030C10(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDebug_nSceneOrSoundID = D_8009B2E8;
        func_80030250(D_80090D0C, 0x14, 0, 0, 0xC, 2, 1);
    }
    result = func_80030294();
    if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
        } else {
            D_8009B363[0] = 0;
            D_8009B26C[0] = 5;
            D_8009B2E8 = *(u8 *)&gDebug_nSceneOrSoundID;
        }
    }
}

void DebugMenu_UpdateTitleEntry(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDebug_nSceneOrSoundID = D_8009B26D;
        func_80030250(D_80090D28, 0x15, 0, 0, 0x12, 2, 1);
    }
    result = func_80030294();
    if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
        } else {
            D_8009B268 = 1;
            D_8009B26C[0] = 8;
            D_8009B26D = *(u8 *)&gDebug_nSceneOrSoundID;
        }
    }
}

void DebugMenu_UpdateMovieEntry(void)
{
    {
        u8 flags = D_8009B2EB;

        if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
            D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
            gDebug_nSceneOrSoundID = 0;
            func_80030250(D_80090D44, 0x1D, 0, 0, 5, 2, 1);
        }
    }
    {
        u8 flags = D_8009B2EB;

        if (flags & FRONTEND_STEP_FLAG_ASYNC_PENDING) {
            u32 word = D_8009B0F4_abs;
            if ((word & 0x02000000) == 0)
                D_8009B2EB = flags & ~FRONTEND_STEP_FLAG_ASYNC_PENDING;
        } else {
            int result = func_80030294();

            if (result) {
                if (result < 0)
                    D_8009B2EB = 0;
                else {
                    func_8003594C(gDebug_nSceneOrSoundID);
                    D_8009B2EB |= FRONTEND_STEP_FLAG_ASYNC_PENDING;
                }
            }
        }
    }
}

void DebugMenu_UpdateLoadEntry(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDuel_bEffectState = DUEL_EFFECT_STATE_NOOP_3;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void DebugMenu_UpdateSaveEntry(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDuel_bEffectState = DUEL_EFFECT_STATE_NOOP_4;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void DebugMenu_UpdateTradeEntry(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
        D_8009B3ED = 0;
        D_8009B3EA = 0;
    }
    result = SaveData_UpdateTradeLoad();
    if (result != 0) {
        if (result == 1) {
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
            D_8009B26C[0] = 14;
        }
        D_8009B2EB = 0;
    }
}

void DebugMenu_EnterDuel(void)
{
    D_8009B36A = 0x71D0;
    D_8009B368 = 0;
    func_80024DC8(-1, 1, 0x8000, 0x8000);
}

void DebugMenu_EnterDeckEditor(void)
{
    func_80033C90();
}
