#include "../types.h"
#include "model.h"

typedef struct {
    short x;
    short y;
    short z;
    short w;
} Vec;

typedef struct StatRec {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
} StatRec;

typedef struct Buf {
    char pad[6];
    s16 f6;
} Buf;

typedef struct {
    s16 a;
    s16 b;
    s16 c;
    s16 d;
} Data;

extern u8 D_80091550[];
extern StatRec D_80091570[];
extern s8 D_8009B07A;
extern u8 D_8009B07B;
extern u8 D_8009B07C;

extern unsigned short *func_800591FC(void);
extern unsigned short *func_80059208(void);
extern void Model_CopySlotU16Values(int, short *);
extern void func_80058434(int, int, int, int, int);
extern void func_80059000(s32 a0, Buf *a1);
extern void func_8005D994(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
extern void func_8005F070(int);
extern void func_8005F91C(s32, void *, void *, s32);
extern void func_80059EBC(int);

void func_8005F3B8(int mode, int y, int a, int b, Vec *offset)
{
    unsigned short *p = func_800591FC();
    unsigned short *q = func_80059208();
    if (D_8009B07B == 1 && D_8009B07C == 1)
        return;
    if (mode >= 0) {
        short v[4];
        Model_CopySlotU16Values(mode, v);
        ((int *)p)[3] = v[0];
        ((int *)p)[4] = v[1];
        ((int *)p)[5] = v[2];
    }
    if (offset) {
        int sign = -1;
        if (mode <= 0)
            sign = 1;
        ((int *)p)[3] += offset->x * sign;
        ((int *)p)[4] += offset->y;
        ((int *)p)[5] += offset->z * sign;
    }
    q[0] = y;
    q[1] = (a + MODEL_ANGLE_FULL_TURN +
            (mode <= 0 ? 3 * MODEL_ANGLE_QUARTER_TURN :
                         MODEL_ANGLE_QUARTER_TURN)) %
        MODEL_ANGLE_FULL_TURN;
    q[2] = (b + MODEL_ANGLE_FULL_TURN) % MODEL_ANGLE_FULL_TURN;
    func_80058434(1, MODEL_ANGLE_FULL_TURN, 0, 0, 1);
    func_8005F070(1);
    func_80059EBC(-1);
}

int func_8005F564(void)
{
    if (D_8009B07A < 0) {
        return 0;
    }
    return D_8009B07A++ > 0;
}

void func_8005F588(int value)
{
    if (D_8009B07B != 1 || D_8009B07C != 1) {
        if (value == 0) {
            D_8009B07A = -1;
        } else {
            D_8009B07A = 0;
        }
    }
}

/* If D_8009B07B==1 and D_8009B07C matches it, bail early. Otherwise reads
   D_80091570[arg1].f0 as a base stat value; if arg0 (level?) < 2, scales
   the stat by a growth ratio derived from func_80059000's output clamped
   to >=50 then offset by -300, divided by 750, and adds the delta. Passes
   the (possibly adjusted) value plus f2/f4 on to setup_positional_sfx. */
void func_8005F5C8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    StatRec *rec = &D_80091570[arg1];
    s32 s0;

    if (D_8009B07B == 1 && D_8009B07C == D_8009B07B) {
        return;
    }

    s0 = rec->f0;

    if (arg0 < 2) {
        Buf buf;
        s32 v1;

        func_80059000(arg0, &buf);
        if (buf.f6 < 0x32) {
            buf.f6 = 0x32;
        }
        buf.f6 -= 300;
        v1 = buf.f6;
        if (v1 != 0) {
            s32 divisor = 750;
            s32 half = (v1 > 0) ? (s0 / 2) : s0;
            s0 += (v1 * half) / divisor;
        }
    }

    func_8005D994(arg0, s0, rec->f2, rec->f4, arg2, arg3);
}

void func_8005F714(s32 a, s32 b, s32 c)
{
    void *x = a < 0 ? 0 : D_80091550 + a * 8;
    void *y = b < 0 ? 0 : D_80091550 + b * 8;
    s32 flag;

    if (D_8009B07B == 1 && D_8009B07C == 1) {
        return;
    }
    flag = (s8)D_8009B07A;
    if (flag < 0) {
        flag = 0;
    } else {
        D_8009B07A++;
        flag = flag > 0;
    }
    func_8005F91C(flag, x, y, c);
}

void func_8005F7B0(s32 value, s32 arg)
{
    Data d = {value, 0, 0, 5};
    s32 flag;
    s32 next;

    if (D_8009B07B == 1 && D_8009B07C == 1) {
        return;
    }
    flag = (s8)D_8009B07A;
    next = (u8)D_8009B07A;
    if (flag < 0) {
        flag = 0;
    } else {
        D_8009B07A = next + 1;
        flag = flag > 0;
    }
    func_8005F91C(flag, &d, &d, arg);
}

void func_8005F828(s32 count, void *p1, void *p2, s32 arg3)
{
    s8 raw172;
    s32 flag;

    if (D_8009B07B == 1 && D_8009B07C == D_8009B07B) {
        return;
    }

    raw172 = D_8009B07A;
    if (raw172 < 0) {
        flag = 0;
    } else {
        D_8009B07A = (u8)D_8009B07A + 1;
        flag = raw172 > 0;
    }

    if (count == 0) {
        return;
    }

    do {
        func_8005F91C(flag, p1, p2, arg3);

        if (p1 != 0) {
            p1 = (u8 *)p1 + 8;
        }
        if (p2 != 0) {
            p2 = (u8 *)p2 + 8;
        }

        if (p1 != 0 && *(s16 *)((u8 *)p1 + 6) != 0) {
            goto cont;
        }
        if (p2 == 0) {
            return;
        }
        if (*(s16 *)((u8 *)p2 + 6) == 0) {
            return;
        }
cont:
        if (count > 0) {
            count--;
        }
        flag = 1;
    } while (count != 0);
}
