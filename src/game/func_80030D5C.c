#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
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
            u32 word = D_8009B0F4_abs;
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
