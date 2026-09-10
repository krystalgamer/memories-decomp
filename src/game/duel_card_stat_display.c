#include "../types.h"
#include "display_object_config.h"
#include "display_object_api.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "display_object_helpers.h"
#include "duel_card.h"
#include "duel_card_stat_display.h"

void func_800316F0(u8 *arg0, s32 arg1, u8 *arg2, s32 arg3)
{
    s32 i;

    arg0[0xF] = 0x70;
    i = arg3 - 1;
    if (i >= 0) {
        do {
            u8 value = arg2[i];

            if (value < 0xA) {
                arg0[0xE] = value * 8 - 0x80;
                GsSortFastSprite((GsSPRITE *)arg0, (GsOT *)arg1, 0);
            }
            *(u16 *)(arg0 + 4) += 8;
            i--;
        } while (i >= 0);
    }
}
