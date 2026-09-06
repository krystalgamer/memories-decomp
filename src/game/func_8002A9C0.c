#include "../types.h"
#include "../psyq/libgte.h"

extern void func_8004036C(void *);
extern void func_80041F90(u8 *, s32, s32, u8 *);
extern void func_8005B260(u8 *, s32, u16, s32);

void func_8002A9C0(u8 *p, s32 arg1)
{
    s32 sp28;
    s32 sp2C;
    u8 *q;
    u8 *r;
    u8 *b0;
    u8 *b1;
    u8 *b2;
    u8 *b3;
    s32 n;
    s32 e;
    s32 m;
    s32 z;
    s32 k;
    s32 v;
    s32 w;

    n = *(u16 *)(p + 0x60) - 0x10;
    *(s16 *)(p + 0x60) = n;
    if ((s16)n <= 0) {
        func_8004036C(p);
        return;
    }

    r = (u8 *)0x1F800398;
    p[0xE] = p[0x60];
    func_80041F90(p, *(s16 *)(p + 0x30) + *(s16 *)(p + 0x18),
                  *(s16 *)(p + 0x32) + *(s16 *)(p + 0x1A),
                  (u8 *)0x1F800398);

    q = (u8 *)0x1F8002A0;
    b0 = (u8 *)0x1F800300;
    m = 0x55555555;
    b2 = (u8 *)0x1F800310;
    b3 = (u8 *)0x1F800318;
    b1 = (u8 *)0x1F800308;

    z = *(s32 *)(p + 0xC);
    q[3] = 6;
    *(s32 *)(q + 0x18) = m;
    *(s32 *)(q + 4) = z;
    q[7] = 0x4C;

    v = *(u16 *)(p + 0x30) - *(u16 *)r;
    *(s16 *)b2 = v;
    *(s16 *)b0 = v;
    v += *(u16 *)(p + 0x3C);
    *(s16 *)b3 = v;
    *(s16 *)(b0 + 8) = v;

    w = *(u16 *)(p + 0x32) - *(u16 *)(r + 2);
    *(s16 *)(b1 + 2) = w;
    *(s16 *)(b0 + 2) = w;
    e = *(u16 *)(p + 0x3E);
    *(s16 *)(b3 + 4) = 0;
    *(s16 *)(b2 + 4) = 0;
    *(s16 *)(b1 + 4) = 0;
    *(s16 *)(b0 + 4) = 0;

    w += e;
    *(s16 *)(b3 + 2) = w;
    *(s16 *)(b0 + 0x12) = w;

    RotAverage4(
        (SVECTOR *)0x1F800300, (SVECTOR *)0x1F800308,
        (SVECTOR *)0x1F800310, (SVECTOR *)0x1F800318,
        (long *)0x1F8002A8, (long *)0x1F8002AC,
        (long *)0x1F8002B4, (long *)0x1F8002B0,
        (long *)&sp28, (long *)&sp2C
    );

    func_8005B260((u8 *)0x1F8002A0, arg1, *(u16 *)(p + 0x14), 1);
    q[3] = 3;
    q[7] = 0x40;
    k = *(u16 *)(p + 0x14);
    *(s32 *)(q + 0xC) = *(s32 *)(q + 0x14);
    func_8005B260((u8 *)0x1F8002A0, arg1, k, 1);
}
