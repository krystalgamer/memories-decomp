#include "../types.h"

extern s32 gDuel_adwCardStats[];
extern void func_8003201C(s32);
extern void func_80032C48(void *);

void func_800320BC(s32 arg0, s32 arg1)
{
    s32 *record;
    /* Keep one induction pointer instead of folding accesses around entry-2. */
    register s16 *entry asm("$6");
    register s32 index asm("$4") = 0;
    {
        s32 *base = gDuel_adwCardStats;
        s32 record_index = arg1 - 1;

        record = &base[record_index];
    }
    entry = (s16 *)(arg0 + 0x2D58);
    do {
        if (((u8 *)entry)[5] == 0) {
            ((u8 *)entry)[5] = 1;
            entry[-2] = (s16)arg1;
            ((u8 *)entry)[2] = (u8)((*record >> 26) & 0x1F);
            entry[-1] = (s16)((*record & 0x1FF) * 10);
            entry[0] = (s16)(((*record >> 9) & 0x1FF) * 10);
            func_80032C48((void *)(arg0 + 0x2D50));
            func_8003201C(arg0);
            return;
        }
        index++;
        entry += 8;
    } while (index < 40);
}
