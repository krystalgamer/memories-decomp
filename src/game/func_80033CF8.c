#include "../types.h"
#include "../psyq/libgte.h"
#include "sorted_entry.h"

void func_80033CF8(s32 dx, s32 dy, s32 dz) {
    SortedEntry *p;
    s32 q;
    s32 v;

    D_8009B310->distance = SquareRoot0(dx * dx + dz * dz);
    q = ratan2(dx, dz) / 16;
    if (q >= 256) {
        q = 255;
    }
    v = q << 8;

    p = D_8009B310;
    p->packed = v | (dy >> 4);
    p->append_index = D_8009B314;
    p->sorted_position = 0;

    D_8009B310 = p + 1;
    D_8009B314 = D_8009B314 + 1;
}
