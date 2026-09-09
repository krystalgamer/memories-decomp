#include "../types.h"
#include "func_8004DC38.h"

/* Seeks channel i of model slot p to position pos within sequence n. The
 * sequence's entry index comes from the 0x74-stride table at +0x2C8 and its
 * step count from the 0x76-stride table at +0x750; the entry list at
 * +0xDD8 holds 4-byte entries whose byte 2 is a duration in sixteenths and
 * whose halfword before the list is its length. The walk consumes pos
 * modulo one step's length entry by entry, stopping at the first entry
 * that outlasts the remainder, at a negative entry, or at the end, then
 * skips forward over non-negative entries to find the next stop. The
 * channel object at +0x1E0 receives the stop index (+0x16), the entry
 * before it (+0x14), the entry length (+0x12) and the time left in it
 * (+0x10); mode 0x3C clamps that last value to 0x6000. */
void func_8004DC38(u8 *p, s32 i, s32 n, u32 pos) {
    s32 *tbl;
    u8 *e;
    s32 *q;
    s32 *cp;
    s32 *v;
    s32 k;
    s32 step;
    s32 rem;
    s32 len;
    s32 idx;
    s32 x;
    s32 y;
    s32 z;
    s32 d;
    s32 off4;
    s32 off2;
    s32 row;
    s32 row2;

    /* Both channel offsets are materialised up front; that is what moves
       p out of $a0 into $t3 at entry. */
    off4 = i * 4;
    off2 = i * 2;
    row = n * 0x74;
    k = *(u16 *)(p + (off2 + row) + 0x2C8);
    tbl = *(s32 **)(p + 0xDD8);
    e = *(u8 **)(p + off4 + 0x1E0);
    q = tbl + k;
    if (e == (u8 *)0) {
        return;
    }
    if (k == 0xFFFF) {
        return;
    }
    row2 = n * 0x76;
    /* Read twice rather than through a local: cse folds the second read,
       and the extra reference is what puts the address in $v1. */
    if (*(u16 *)(p + (off2 + row2) + 0x750) == 0) {
        return;
    }
    step = *(u16 *)(p + (off2 + row2) + 0x750) * 16;
    rem = pos % step;
    cp = q - 1;
    if (rem == 0) {
        if (pos != 0 && p[0xE16] != 0x3C) {
            rem = step;
        }
    }
    /* The guard and the loop both read the length from memory. jump.c
       duplicates the loop's exit test at its entry, cse folds it against
       the guard, and the loop optimiser hoists the mode byte, the 0x3C
       and the in-loop length read (which cse2 then turns into the copy
       retail keeps in $a3). A named length local defeats all of that. */
    idx = 0;
    if (idx >= *(u16 *)cp) {
        return;
    }
    for (; idx < *(u16 *)cp; idx++) {
        if (p[0xE16] != 0x3C) {
            d = rem - (((u8 *)q)[2] << 4);
            if (d <= 0) {
                break;
            }
        } else {
            d = rem - (((u8 *)q)[2] << 4);
            if (d < 0) {
                break;
            }
        }
        rem = rem - (((u8 *)q)[2] << 4);
        if (q[1] < 0) {
            break;
        }
        q++;
    }

    len = *(u16 *)cp;
    if (idx >= len) {
        return;
    }
    /* A goto loop: a while would be rotated and undo the advance on the
       exit path. The advance sits before the count test on purpose, since
       retail's back-branch delay slot runs it on the exit path too. */
    v = q;
scan:
    if (v[1] >= 0) {
        idx++;
        v++;
        if (idx < len) {
            goto scan;
        }
    }
    x = q - *(s32 **)(p + 0xDD8);
    *(s16 *)(e + 0x16) = x;
    if (x != 0) {
        x = x - 1;
    } else {
        x = v - *(s32 **)(p + 0xDD8);
    }
    *(s16 *)(e + 0x14) = x;
    y = ((u8 *)q)[2] << 4;
    z = y - rem;
    *(s16 *)(e + 0x12) = y;
    *(s16 *)(e + 0x10) = z;
    if (p[0xE16] != 0x3C) {
        return;
    }
    if ((s16)z < *(u16 *)(e + 0x12)) {
        return;
    }
    *(s16 *)(e + 0x10) = 0x6000;
}
