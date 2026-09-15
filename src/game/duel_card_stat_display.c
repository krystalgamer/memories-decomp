#include "../types.h"
#include "display_object_config.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "display_object_helpers.h"
#include "duel_card.h"
#include "duel_card_stat_display.h"

void func_800316F0(GsSPRITE *sprite, GsOT *ot, const u8 *digits, s32 count)
{
    s32 i;

    sprite->v = 0x70;
    i = count - 1;
    if (i >= 0) {
        do {
            u8 value = digits[i];

            if (value < 0xA) {
                sprite->u = value * 8 - 0x80;
                GsSortFastSprite(sprite, ot, 0);
            }
            *(u16 *)&sprite->x += 8;
            i--;
        } while (i >= 0);
    }
}
