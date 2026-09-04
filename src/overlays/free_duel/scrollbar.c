#include "../../types.h"

typedef struct {
    u8 unk_00[0x32];
    s16 y;
} FreeDuelWidget;

extern FreeDuelWidget *gFreeDuel_pCursorWidget;
extern FreeDuelWidget *gFreeDuel_pThumbWidget;
extern s16 D_8009B148;

void FreeDuel_UpdateScrollbar(void)
{
    FreeDuelWidget *cursor = gFreeDuel_pCursorWidget;
    s32 relative = cursor->y - D_8009B148;

    if (relative < 0x28) {
        D_8009B148 = cursor->y - 0x28;
    }
    if (relative >= 0x91) {
        D_8009B148 = cursor->y - 0x90;
    }
    gFreeDuel_pThumbWidget->y = (cursor->y - 0x28) * 72 / 364 + 7;
}
