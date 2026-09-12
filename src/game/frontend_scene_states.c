#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
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

/* The first two of the frontend's scene states, in address order. Both are
   steps of the same state machine: the flags byte D_8009B2EB carries bit 0x80
   for "already entered", so the first call does the setup and each later call
   polls for completion and clears the byte on the way out. The two are
   contiguous and are the whole gcc_2_8_1_g8 run between func_80030998, which
   is generated assembly, and the matching func_80030D5C in
   frontend_scene_state_80030d5c.c. The states after it are in
   func_80030E30.c. */

extern u8 D_8009B26C[];

void func_80030C10(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
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

void func_80030CB0(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
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
