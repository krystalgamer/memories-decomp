#include "../types.h"
#include "duel_card.h"

struct Obj {
    char pad_00[0x6A];
    u8 f6A;
};

struct Coords {
    s16 x;
    s16 y;
};

struct Blob {
    char pad_0000[0x36B4];
    DuelCardRecord record;
};

extern u8 D_8015C424[];
extern struct Coords D_800908A0[];
extern u8 *func_800249E0(s32, s32);
extern struct Obj *func_80024C1C(s32, s32, s32);

void func_80024D34(s32 a, s32 b)
{
    u8 *slot;
    s32 idx;
    u8 *tb;
    struct Blob *blob;
    struct Obj *obj;

    slot = func_800249E0(a, b);
    idx = a;
    if ((idx & 0x80) != 0) {
        idx = (idx & 0x7F) + 0xF;
    }
    tb = D_8015C424;
    blob = (struct Blob *)(tb + idx * sizeof(DuelCardRecord) + 0x48000);
    obj = func_80024C1C(*(s16 *)blob->record.data, D_800908A0[idx].x,
                        D_800908A0[idx].y);
    *(struct Obj **)slot = obj;
    obj->f6A = idx;
}
