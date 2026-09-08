#include "../../types.h"
#include "../../game/card_list_rows.h"
#include "trade_helpers.h"

typedef struct {
    s16 id;
    u16 count;
} MainMenuCardCount;

extern MainMenuCardCount D_801845FC[];
extern u16 D_80185C8C[][2];
extern u8 *D_801845E0;

void MainMenu_RebuildTradeInventoryRows(s32 side)
{
    s32 flags;

    flags = D_801845E0[0x69] - 4;
    func_80060E70((u16 *)(side * 2888 + (s32)D_801845FC + D_80185C8C[side][0] * 4), side,
                  flags & (1 << side), flags);
}
