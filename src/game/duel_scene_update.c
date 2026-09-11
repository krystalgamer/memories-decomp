#define AI_OPPONENT_ID_ARRAY
#include "../types.h"
#include "ai_opponent_data.h"
#include "duel_side_state.h"
#include "input.h"
#include "duel_check_quit_input.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "func_80039794.h"
#include "duel_scene_callbacks.h"
#include "duel_scene_update.h"
#include "duel_effect.h"
#include "duel_effect_request.h"
#include "func_8002C6C8.h"
#include "../unmatched.h"
#include "duel_magic_effect_dispatch.h"

void func_80024388(void)
{
    int value = 0;

    if (gDuel_bOpponentID[0] < 0) {
        value = D_8009B1D5;
        if (D_8009B238 >= 0) {
            value = D_8009B238;
        }
    }
    if (value != 0) {
        Input_BackupPad1AndUsePad2();
        func_80024200();
        Input_RestorePad1FromBackup();
    } else {
        func_80024200();
    }
}
