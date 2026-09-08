#include "../types.h"
#include "menu_record.h"

extern void (*D_80090F68[])(u8 *);
extern void func_8003B378(u8 *, s32);

void func_8003B50C(s32 arg0) {
    u8 *r = (u8 *)D_800EB010;
    s32 i = MENU_RECORD_COUNT - 1;
    void (**t)(u8 *) = D_80090F68;

    do {
        if (*(s8 *)(r + 0x30) >= 0) {
            if (r[0x32] & 0x40) {
                func_8003B378(r, arg0);
            }

            if (r[0x33] != 0) {
                t[r[0x33] & 0x1F](r);
            }
        }
        r += 0x4C;
        i--;
    } while (i >= 0);
}
