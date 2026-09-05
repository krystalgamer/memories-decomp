#include "../types.h"

struct Obj {
    char pad6A[0x6A];
    u8 f6A;
};

struct Pair {
    s16 x;
    s16 y;
};

struct Entry {
    s16 *ptr;
    u8 pad4[28 - 4];
};

/* D_8015C424 must stay a padded struct object rather than a base pointer.
   A struct member offset this large is split at a 0x8000 boundary, giving the
   lui/ori of 0x48000 plus 0x36B8 as the load displacement. Pointer arithmetic
   folds the whole offset into an oversized displacement that the assembler
   expands through $at, which is one instruction shorter and does not match. */
struct Blob {
    u8 pad0[0x4B6B8];
    struct Entry table[1];
};

extern u8 *func_800249E0(s32 a, s32 b);
extern struct Obj *func_80024C1C(s32 cardId, s32 x, s32 y);
extern struct Blob D_8015C424;
extern struct Pair D_800908A0[];

void func_80024D34(s32 arg0, s32 arg1)
{
    u8 *out;
    s32 idx;
    s16 *entry;
    struct Obj *obj;

    out = func_800249E0(arg0, arg1);
    idx = arg0;
    if (idx & 0x80) {
        idx = (idx & 0x7F) + 0xF;
    }
    entry = D_8015C424.table[idx].ptr;
    obj = func_80024C1C(entry[0], D_800908A0[idx].x, D_800908A0[idx].y);
    *(struct Obj **)out = obj;
    obj->f6A = (u8)idx;
}
