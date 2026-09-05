#include "../types.h"

#include "duel_card.h"

typedef struct {
    u16 x;
    u16 y;
} Pair;

typedef struct {
    char pad_00[4];
    u16 y;
} NewObject;

extern u8 D_8009B1D5;
extern u16 D_8009B220;
extern Pair D_80090800[2][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
extern u8 D_800907D8[];
extern char D_8009AF40[];
extern s16 D_8009B1A8;
extern u8 D_80090AD4[];
extern void (*D_80090A5C[])(void);

extern int func_80024E24(void);
extern NewObject *func_8002C68C(int);
extern void func_8008E870(char *, int);
extern void func_80024954(DuelCardRecord *);

void func_80026A3C(void)
{
    int i;

    if (!func_80024E24()) {
        NewObject *object = func_8002C68C(0x17);
        int side = D_8009B1D5 ^ 1;

        object->y = D_80090800[side][0].y;
        func_8008E870(D_8009AF40, (short)object->y);
    } else {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            int position =
                i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
            DuelCardRecord *entry =
                &D_801A7AD8[D_800907D8[position]];

            if (entry->flags & 0x8000)
                func_80024954(entry);
        }
        D_8009B220 = 0;
    }
}

int func_80026B34(void)
{
    u16 flags = D_8009B220;

    if (flags & 0x8000) {
        u8 *indices = D_80090AD4;
        void (**callbacks)(void);
        int index = indices[D_8009B1A8] * 2;

        if (flags & 0x4000)
            index++;
        callbacks = D_80090A5C;
        callbacks[index]();
    }
    return D_8009B220;
}
