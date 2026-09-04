#include "../types.h"

extern s16 gDuel_awRecentCardDrops[];

extern void func_800137E4(void);
extern void File_RequestAsyncTransfer(s32, s32, s32, s32, void *, s32, s32);
extern void func_80032184(void);

void func_80032328(void)
{
    File_RequestAsyncTransfer(0, 0, 0x2189, 0x4C, func_80032184, 0, 0);
    func_800137E4();
}

void func_80032370(void)
{
    register s16 *source __asm__("$6");
    register u8 *base __asm__("$4");
    register s32 i __asm__("$5");
    register s16 *current __asm__("$3");
    s16 *destination;

    __asm__ volatile(
        "lui $2,%%hi(gDuel_awRecentCardDrops)\n\t"
        "addiu $6,$2,%%lo(gDuel_awRecentCardDrops)"
        : "=r"(source)
        :
        : "$2");
    base = (u8 *)source - 0x56C;
    i = 15;
    current = source + 15;
    for (; i >= 0; i--, current--) {
        if (*current != 0 && base[*current - 1] == 0)
            *current = 0;
    }
    destination = source;
    for (i = 0; i < 16; i++, source++) {
        if (*source) {
            if (source != destination) {
                *destination = *source;
                *source = 0;
            }
            destination++;
        }
    }
}
