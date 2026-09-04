#include "../../types.h"

extern s8 D_8009B366;
extern s8 D_8009B367;
extern s8 D_8009B36C;
extern s16 D_8009B32E;
extern u8 D_800EB0F8[];
extern u8 gFreeDuel_abGridAvailable[];
extern u8 D_801D0000[];
extern s32 D_801D5608[];
extern void func_80035B7C(void *);
extern void func_80035BE4(int, int, int, int, int, int);
extern void func_80039A60(void *);

void FreeDuel_PlaceCursor(u8 *widget, int show)
{
    u8 *panel = D_800EB0F8;
    s16 value;
    int index;
    u8 *entry;

    *(s16 *)(widget + 0x30) = D_8009B366 * 56 + 0x14;
    *(s16 *)(widget + 0x32) = D_8009B367 * 52 + 0x28;
    func_80035B7C(panel);

    if (show) {
        index = D_8009B36C + D_8009B367 * 5;
        if (gFreeDuel_abGridAvailable[index]) {
            value = index - 0x7CD8;
            D_8009B32E = value;
            if (index != 0) {
                value = 0xC;
                entry = D_801D0000 + index * 4;
                D_801D5608[0] = *(s16 *)(entry + 0x71C);
                D_801D5608[1] = *(s16 *)(entry + 0x71E);
            }
            func_80035BE4(0, value, 0x10, 0xCC, 0x120, 0x10);
            func_80039A60(panel);
        }
    }
}
