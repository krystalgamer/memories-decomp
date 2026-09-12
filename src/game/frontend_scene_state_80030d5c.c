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

/* Frontend scene state 0x80030D5C. D_8009B2EB carries bit 0x80 for
   "already entered" and bit 0x40 while the selected scene or sound runs. */

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
