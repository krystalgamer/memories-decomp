#include "../types.h"
#include "card_constants.h"
#include "duel_display.h"

typedef struct { u8 *p; u8 pad4[5]; u8 flag; u8 pad10[2]; } Rec12;
extern Rec12 D_800EA030[HAND_SIZE];
void func_8001B8B8(u8 *arg0) {
    s32 i;
    for (i = 0; i < HAND_SIZE; i++)
        if (D_800EA030[i].flag == 0)
            *(u32 *)(D_800EA030[i].p + 0xC) = DUEL_DISPLAY_COLOR_DIMMED;
    if (arg0[0x15] == 0)
        *(u32 *)(D_800EA030[(s8)arg0[0xE]].p + 0xC) =
            DUEL_DISPLAY_COLOR_NORMAL;
}
