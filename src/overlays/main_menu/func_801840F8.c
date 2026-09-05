#include "../../types.h"

typedef struct {
    s16 id;
    u16 count;
} MainMenuCardCount;

extern MainMenuCardCount D_801845FC[];

void func_801840F8(s32 slot, s32 id, u32 amount)
{
    MainMenuCardCount *p;
    MainMenuCardCount *entry;
    u32 total;
    s32 offset;
    s32 i;

    i = 0;
    offset = slot * 2888;
    p = D_801845FC;
    while (i < 722) {
        entry = (MainMenuCardCount *)(offset + (s32)p);
        if (entry->id == id) {
            total = entry->count + amount;
            if (total < 0xFB) {
                entry->count = total;
            }
            return;
        }
        p++;
        i++;
    }
}
