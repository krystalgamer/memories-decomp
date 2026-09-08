#include "../../types.h"
#include "../../game/card_constants.h"
#include "../../game/text_box_lifecycle.h"

typedef struct {
    u8 unk_00[0x30];
    s16 x;
    s16 y;
} FreeDuelWidget;

extern FreeDuelWidget *gFreeDuel_pCursorWidget;
extern FreeDuelWidget *gFreeDuel_pThumbWidget;
extern s8 gFreeDuel_bCursorColumn;
extern s8 gFreeDuel_bCursorRow;
extern s8 gFreeDuel_bTargetColumn;
extern s16 D_8009B32E;
extern u8 gFreeDuel_abGridAvailable[];
extern u8 D_800EB0F8[];
extern s16 D_801D0000[];
extern s16 gGraphics_sViewportY;

typedef struct {
    u32 lo;
    u32 hi;
} Pair;

extern Pair D_801D5608;
extern void func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void func_80039A60(u8 *);

void FreeDuel_UpdateScrollbar(void)
{
    FreeDuelWidget *cursor = gFreeDuel_pCursorWidget;
    s32 relative = cursor->y - gGraphics_sViewportY;

    if (relative < 0x28) {
        gGraphics_sViewportY = cursor->y - 0x28;
    }
    if (relative >= 0x91) {
        gGraphics_sViewportY = cursor->y - 0x90;
    }
    gFreeDuel_pThumbWidget->y = (cursor->y - 0x28) * 72 / 364 + 7;
}

void FreeDuel_PlaceCursor(FreeDuelWidget *w, s32 arm)
{
    s32 col;
    s32 index;
    s32 param;
    s16 trunc;
    u8 *panel;
    s16 *base;
    s16 *slot;

    col = gFreeDuel_bCursorColumn;
    panel = D_800EB0F8;
    w->x = col * 56 + 20;
    w->y = gFreeDuel_bCursorRow * 52 + 40;
    TextBox_Destroy(panel);
    if (arm == 0) {
        return;
    }
    index = gFreeDuel_bTargetColumn +
            gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT;
    if (gFreeDuel_abGridAvailable[index] == 0) {
        return;
    }
    slot = &D_8009B32E;
    trunc = index - 31960;
    *slot = trunc;
    param = trunc;
    if (index != 0) {
        param = 12;
        base = D_801D0000;
        D_801D5608.lo = base[index * 2 + 910];
        D_801D5608.hi = base[index * 2 + 911];
    }
    func_80035BE4(0, param, 16, 204, 288, 16);
    func_80039A60(panel);
}
