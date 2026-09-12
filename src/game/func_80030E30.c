#define D_8009B36A_IN_DATA
#define D_8009B254_IN_DATA
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

/* The later frontend scene states, in address order. They are steps of the
   same state machine as func_80030C10 and func_80030CB0 in
   frontend_scene_states.c; the first three here use its flags byte
   D_8009B2EB, which carries bit 0x80 for "already entered", so the first call
   does the setup and each later call polls for completion and clears the byte
   on the way out. The five are contiguous and are the whole gcc_2_8_1_g8 run
   between the matching func_80030D5C in
   frontend_scene_state_80030d5c.c and func_80030FA0. */

void func_80030E30(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B254 = 3;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void func_80030E7C(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B254 = 4;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void func_80030EC8(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
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

void func_80030F40(void)
{
    D_8009B36A = 0x71D0;
    D_8009B368 = 0;
    func_80024DC8(-1, 1, 0x8000, 0x8000);
}

void func_80030F80(void)
{
    func_80033C90();
}
