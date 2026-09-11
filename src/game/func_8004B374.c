#define D_8009B458_IN_DATA
#include "../types.h"
#include "../psyq/libspu.h"
#include "func_8004A7C0.h"
#include "sound_sequence_constants.h"
#include "sound.h"

/* D_8009B458 is held in its own register and D_80011434 is materialized
 * through cc1psx's address pair.
 *
 * The 21 differences the first pure C sat at were ONE mechanism: gcc hoisted
 * arg1's per-iteration `andi` out of the call-bearing loop into a saved
 * register. The loop is written with `goto` (label at the top, conditional
 * `goto top;` at the bottom) because gcc 2.8's loop pass runs only between
 * the LOOP_BEG/LOOP_END notes of for/while/do -- a goto loop has none, so
 * nothing is hoisted, which is what retail shows. Then, in order:
 *   29 -> 22  `a1 = arg1;` named at the top, masked at its use
 *   22 -> 11  `mask = 0;` written before `i = 0;`
 *   13 ->  5  `do { } while (0);` round the loop's bottom four statements
 *             (the permuter's find; it stops off/tbl rotating)
 *    5 ->  0  `i = 0; mask = 0;` -- two zeroings rather than `i = mask;`;
 *             gcc makes the copy itself and puts it in the guard's delay slot
 */

void func_8004B374(s32 arg0, s32 arg1) {
    u8 *base;
    s32 mask;
    s32 i;
    s32 off;
    s32 *tbl;
    s32 v0;
    s32 t1;
    u8 *p;
    s32 a1;

    a1 = arg1;
    base = (u8 *)D_8009B458;
    i = 0;
    mask = 0;
    if (*(s16 *)(base + 0x510) > 0) {
        t1 = arg0 & 0xFF;
        tbl = D_80011434;
        off = 0;
        top:
            p = base + off;
            if (p[0x183] == t1 && p[0x185] == (u8)a1) {
                func_8004A7C0(i);
                mask |= *tbl;
            }
            tbl++;
            do {
                off += 0x28;
                base = (u8 *)D_8009B458;
                i++;
                if (i < *(s16 *)(base + 0x510)) goto top;
            } while (0);
    }

    if (mask != 0) {
        do {
            SpuSetKey(SPU_OFF, mask);
            v0 = SpuGetKeyStatus(mask);
        } while (v0 != SPU_OFF_ENV_ON && v0 != SPU_OFF);
    }

    {
        u8 *q = (u8 *)D_8009B458 + ((u8)arg0) * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
        q[4] = 0;
    }
}
