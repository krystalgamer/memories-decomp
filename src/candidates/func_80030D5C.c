/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/frontend_scene_states.c.
 */
#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#define D_8009B36A_IN_DATA
#define GDUEL_BEFFECTSTATE_IN_DATA
#define D_8009B368_IN_DATA
#define D_8009B3EA_IN_DATA
#define D_8009B3ED_IN_DATA
#include "../types.h"
#include "../game/mem_card.h"
#include "../game/frontend_debug_tables.h"
#include "../game/duel_effect.h"
#include "../game/duel_side_state.h"
#include "../game/duel_effect_mode_7.h"
#include "../game/duel_interface_setup.h"
#include "../game/file_transfer.h"
#include "../game/func_80024DC8.h"
#include "../game/save_data_update_trade_load.h"
#include "../game/movie_playback_control.h"
#include "../unmatched.h"
#include "../game/main_services.h"
#include "../game/frontend_scene_states.h"

/* The frontend's scene states, in address order. Every one of them is a step
   of the same state machine: the flags byte D_8009B2EB carries bit 0x80 for
   "already entered", so the first call does the setup and each later call
   polls for completion and clears the byte on the way out. The eight are
   contiguous and are the whole gcc_2_8_1_g8 run between func_80030998 and
   func_80030FA0. */

void func_80030D5C(void)
{
    {
        u8 flags = D_8009B2EB;

        if ((flags & 0x80) == 0) {
            D_8009B2EB = flags | 0x80;
            gDebug_nSceneOrSoundID = 0;
            func_80030250(D_80090D44, 0x1D, 0, 0, 5, 2, 1);
        }
    }
    {
        u8 flags = D_8009B2EB;

        if (flags & 0x40) {
            register u32 word asm("$2");

            __asm__ volatile(
                "lui $2,%%hi(D_8009B0F4)\n\t"
                "lw $2,%%lo(D_8009B0F4)($2)"
                : "=r"(word)
                :
                : "memory"
            );
            if ((word & 0x02000000) == 0)
                D_8009B2EB = flags & 0xBF;
        } else {
            int result = func_80030294();

            if (result) {
                if (result < 0)
                    D_8009B2EB = 0;
                else {
                    func_8003594C(gDebug_nSceneOrSoundID);
                    D_8009B2EB |= 0x40;
                }
            }
        }
    }
}

