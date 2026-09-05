#include "../types.h"

typedef struct SpeedEntry {
    u16 size;
    u16 speed;
} SpeedEntry;

extern SpeedEntry D_8009151C[64];
extern s32 D_800E9EB4;

extern void func_8007E600(s32, void *, s32);
extern s32 func_8007E710(void *);
extern void func_8005B8A0(void *, s32, s32, s32, s32, s32);

void func_8005C388(s32 index, s32 arg1, s32 speed, s32 arg3, s32 arg4)
{
    s32 buffer[2];
    SpeedEntry *base;
    SpeedEntry *entry;
    s32 i;
    s32 total;

    base = D_8009151C;
    entry = &base[index];
    if (entry->speed != 0) {
        speed = entry->speed;
    }
    i = 0;
    total = 0;
    if (index > 0) {
        do {
            total += base[i].size;
            i++;
        } while (i < index);
    }
    func_8007E600(D_800E9EB4 + total, buffer, total);
    {
        register SpeedEntry *table asm("a1");
        register void *buffer_arg asm("a0");
        s32 value;

        value = func_8007E710(buffer);
        buffer_arg = buffer;
        table = D_8009151C;
        value += table[index].size;
        func_8005B8A0(buffer, arg1, speed, value, arg3, arg4);
    }
}
