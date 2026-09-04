#include "../types.h"
#include "duel_card.h"

typedef struct {
    char pad[0x6A];
    u8 index;
} DuelSelectionObject;

typedef struct {
    DuelSelectionObject *ptr;
} DuelSelectionSource;

extern u8 D_800907D8[];
extern u8 D_8009B1D5;

extern s32 func_8001EFD4(DuelSelectionObject *, DuelSelectionObject *);

int func_8002778C(DuelSelectionSource *source)
{
    int count = 0;
    int slot = 5;

    do {
        int position = slot + D_8009B1D5 * 20;
        DuelCardRecord *entry = &D_801A7AD8[D_800907D8[position]];

        if (entry->flags & 0x8000) {
            count++;
            if (func_8001EFD4(source->ptr, entry->object) > 0)
                return ((DuelSelectionObject *)entry->object)->index;
        }
        slot++;
    } while (slot < 10);

    if (count == 0)
        return D_800907D8[D_8009B1D5 * 20 + 7];
    return -1;
}

s32 func_800278A0(void *arg0)
{
    s32 i;
    s32 found = 0;
    DuelCardRecord *entry = 0;

    for (i = 5; i < 10; i++) {
        u8 row = D_800907D8[i + D_8009B1D5 * 20];
        entry = &D_801A7AD8[row];
        if (entry->flags & 0x8000) {
            found++;
            if (!(entry->flags & 0x1000)) {
                s32 result = func_8001EFD4(
                    *(DuelSelectionObject **)arg0, entry->object);
                if (result > 0) {
                    return ((DuelSelectionObject *)entry->object)->index;
                }
            }
        }
    }

    if (found == 0) {
        return D_800907D8[D_8009B1D5 * 20 + 7];
    }
    return -1;
}
