#include "../types.h"
#include "../ygo_types.h"
#include "func_8003B5C8.h"
#include "text_constants.h"

extern TextDecimalDigitKeyBlock D_80010330;
extern u16 D_800EAFF8[];
extern const u32 D_801D9004[];

/* Copies the ten two-byte Shift-JIS digit keys from D_80010330 into a local
   buffer and walks them as big-endian u16 keys. Each key is looked up in the
   4-byte-stride table at D_801D9004 (key in the low halfword, a zero word
   terminating) and the 1-based
   match index is written to D_800EAFF8. The lookup is skipped entirely when the
   table is empty.

   D_801D9004 is const so that the guard read hoists out of the loop: GCC treats
   a load from unchanging memory as loop-invariant regardless of the stores in
   the body, which is what leaves one shared %hi in the preheader feeding both
   the guard load and the per-iteration table address. */
void func_8003B5C8(void) {
    TextDecimalDigitKeyBlock buf;
    u16 *out;
    u16 *q;
    const u32 *e;
    s32 p;
    s32 end;
    s32 i;
    s32 n;
    s32 key;

    out = D_800EAFF8;
    i = 1;
    p = (s32)buf.bytes;
    end = (s32)buf.bytes + 2 * TEXT_DECIMAL_RADIX;
    buf = D_80010330;

    do {
        e = D_801D9004;
        n = 1;
        key = (*(u8 *)p << 8) | buf.bytes[i];
        if (D_801D9004[0] != 0) {
            q = out;
        search:
            if (key == *(u16 *)e) {
                *q = n;
                goto found;
            }
            e++;
            n++;
            if (*e != 0) {
                goto search;
            }
        found:
            ;
        }
        out++;
        p += 2;
        i += 2;
    } while (p < end);
}
