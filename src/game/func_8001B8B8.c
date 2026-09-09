#include "../types.h"
#include "card_constants.h"
#include "display_object.h"
#include "duel_display.h"
#include "duel_hand.h"

void func_8001B8B8(u8 *arg0) {
    s32 i;
    for (i = 0; i < HAND_SIZE; i++)
        if (D_800EA030[i].active_09 == 0)
            ((DisplayObject *)D_800EA030[i].object)->field_0C =
                DUEL_DISPLAY_COLOR_DIMMED;
    if (arg0[0x15] == 0)
        ((DisplayObject *)D_800EA030[(s8)arg0[0xE]].object)->field_0C =
            DUEL_DISPLAY_COLOR_NORMAL;
}
