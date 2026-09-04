#include "../types.h"

typedef struct {
    s16 field_00;
    u8 pad_02[2];
    u8 field_04;
    u8 pad_05[0x1B];
} ResetEntry;

typedef struct {
    u8 bytes[0x118];
} Pool;

typedef struct {
    u8 pad_00[0x1C];
    u8 flags;
    u8 pad_1D[3];
} PoolEntry;

extern u8 D_8009B260;
extern Pool D_800EAD88;

void func_8002C598(void)
{
    ResetEntry *entry;
    Pool *pool;
    int count;
    int fill;

    D_8009B260 = 0;
    count = 8;
    fill = -1;
    pool = &D_800EAD88;
    entry = (ResetEntry *)((u8 *)pool + 0x18);
    do {
        entry->field_04 = 0;
        entry->field_00 = fill;
        entry++;
    } while (--count != 0);
}

PoolEntry *func_8002C5CC(void)
{
    PoolEntry *entry = (PoolEntry *)&D_800EAD88;
    int count = 8;

    for (;;) {
        if (!(entry->flags & 0x80)) {
            return entry;
        }
        if (--count == 0) {
            return 0;
        }
        entry++;
    }
}
