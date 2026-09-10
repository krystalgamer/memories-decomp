#include "../types.h"
#include "sorted_entry.h"
#include "func_80035598.h"

/* The original qsort call is unprototyped; a prototype changes argument setup. */
void func_800355C8(void)
{
    u8 *base = (u8 *)D_8009B304;
    u8 *entry = base;
    u32 count = D_8009B314;
    u32 sorted_count;
    u32 index;

    D_8009B308 = count;
    D_8009B30C &= ~4;

    if (count >= 2) {
        s32 (*compare)(const u32 *, const u32 *) = func_80035598;

        qsort(entry, count, 8, compare);
    }

    sorted_count = D_8009B308;
    index = 0;

    if (sorted_count != 0) {
        do {
            *(u16 *)(base + *(s16 *)(entry + 4) * 8 + 6) = index;
            index++;
            entry += 8;
        } while (index < sorted_count);
    }

    D_8009B314 = 0;
    D_8009B310 = D_8009B304;
}
