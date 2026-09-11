#include "../types.h"
#include "card_constants.h"
#include "data_transfer_request.h"
#include "duel_effect_mode_7.h"
#include "input.h"
#include "save_data.h"
#include "script_command_busy.h"

#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "text_box_runtime.h"
#include "func_80039794.h"
#include "func_8003B6AC.h"
#include "display_object.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "script_state.h"
#include "main_services.h"
#include "../unmatched.h"
#include "func_8002EE94.h"
#include "duel_effect_mark_object_if_active.h"

int func_8002EE5C(void)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == 0) {
            return 0;
        }
        i++;
        entry++;
    }
    return 1;
}

