#include "../../types.h"

typedef struct { u8 pad0[48]; s16 x; s16 y; } Widget;

extern s8 D_8009B366;
extern s8 D_8009B367;
extern s8 D_8009B36C;
extern s16 D_8009B32E;
extern u8 gFreeDuel_abGridAvailable[];
extern u8 D_800EB0F8[];
extern s16 D_801D0000[];
typedef struct { u32 lo; u32 hi; } Pair;
extern Pair D_801D5608;
extern void TextBox_Destroy(u8 *);
extern void func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void func_80039A60(u8 *);

void FreeDuel_PlaceCursor(Widget *w, s32 arm)
{
    s32 col;
    s32 index;
    s32 param;
    s16 trunc;
    u8 *panel;
    s16 *base;
    s16 *slot;

    col = D_8009B366;
    panel = D_800EB0F8;
    w->x = col * 56 + 20;
    w->y = D_8009B367 * 52 + 40;
    TextBox_Destroy(panel);
    if (arm == 0) {
        return;
    }
    index = D_8009B36C + D_8009B367 * 5;
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
