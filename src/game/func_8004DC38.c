#include "../types.h"
#include "model.h"
#include "func_8004DC38.h"

/* The GsSEQ fields read here are the ones func_8004DC38 writes: ti is the
 * command the part stopped at, tframe the length of that command and rframe
 * the time left in it. 0x6000 is the clamp func_8004DC38 applies in mode
 * 0x3C. */
s32 func_8004DB14(ModelSlot *p, s32 i)
{
    s32 acc;
    s32 n;
    ModelSlotPart *e;
    s32 *tbl;
    s32 *lim;
    s32 *q;
    s32 k;
    s32 v;

    acc = 0;
    n = 1;
    e = p->field_1E0[i];
    if (e == 0) {
        return 0;
    }
    lim = p->field_DD8 + e->ti;
    if (*lim < 0) {
        return 0;
    }
    tbl = p->field_DD8;
    for (n = 1; n < 10; n++) {
        k = p->field_2C8[n][i];
        q = tbl + k;
        if (k != 0xFFFF && lim >= q - 1 &&
            tbl + *(u16 *)(q - 1) >= lim) {
            break;
        }
    }
    if (n < 10) {
        while (q < lim) {
            acc += ((u8 *)q)[2];
            q++;
        }
        v = e->rframe;
        if (v != 0x6000 && v != 0x7000) {
            acc += (e->tframe - v) / 16;
        }
    } else {
        n = 0;
    }
    return (n << 24) | acc;
}

/* Seeks channel i of model slot p to position pos within sequence n. The
 * sequence's entry index comes from field_2C8[n][i] and its step count from
 * field_750[n].values[i]. The command list at field_DD8 holds 4-byte entries
 * whose byte 2 is a duration in sixteenths, and the halfword of the entry
 * before a sequence's first is its length. The walk consumes pos modulo one
 * step's length, entry by entry. It stops at the first entry that outlasts
 * the remainder, at a negative entry, or at the end, then skips forward over
 * non-negative entries to find the next stop. The part's GsSEQ receives the
 * stop index (ti), the entry before it (ci), the entry length (tframe) and
 * the time left in it (rframe); mode 0x3C clamps that last value to 0x6000. */
void func_8004DC38(ModelSlot *p, s32 i, s32 n, u32 pos) {
    s32 *tbl;
    ModelSlotPart *e;
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

    /* Both channel offsets are materialised up front and never read by
       name: cse reuses them for the field_1E0 and field_2C8 indexing just
       below, and that is what moves p out of $a0 into $t3 at entry.
       Dropping either one changes the entry sequence. */
    off4 = i * 4;
    off2 = i * 2;
    k = p->field_2C8[n][i];
    tbl = p->field_DD8;
    e = p->field_1E0[i];
    q = tbl + k;
    if (e == 0) {
        return;
    }
    if (k == 0xFFFF) {
        return;
    }
    /* Read twice rather than through a local: cse folds the second read,
       and the extra reference is what puts the address in $v1. */
    if (p->field_750[n].values[i] == 0) {
        return;
    }
    step = p->field_750[n].values[i] * 16;
    rem = pos % step;
    cp = q - 1;
    if (rem == 0) {
        if (pos != 0 && p->field_E16 != 0x3C) {
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
        if (p->field_E16 != 0x3C) {
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
    x = q - p->field_DD8;
    e->ti = x;
    if (x != 0) {
        x = x - 1;
    } else {
        x = v - p->field_DD8;
    }
    e->ci = x;
    y = ((u8 *)q)[2] << 4;
    z = y - rem;
    e->tframe = y;
    e->rframe = z;
    if (p->field_E16 != 0x3C) {
        return;
    }
    if ((s16)z < e->tframe) {
        return;
    }
    e->rframe = 0x6000;
}
