#include "../../types.h"

typedef struct {
    u8 pad0[20];
    s16 f14;
    u8 pad16[26];
    s16 f30;
    s16 f32;
    u8 pad34[8];
    u16 f3C;
    u16 f3E;
} Record;

extern void func_8005B260(void *, void *, u16, s32);

void func_801681A0(Record *r, void *ot)
{
    u8 *poly;
    u8 *line;
    s32 pri;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    s32 xm1;
    s32 xp3;
    s32 right;

    poly = (u8 *)0x1F800000;
    line = (u8 *)0x1F800040;
    *(u32 *)(poly + 4) = 0x0000FF00;
    *(u32 *)(line + 4) = 0x0000FF00;
    *(u32 *)(line + 12) = 0;
    pri = r->f14;
    x = r->f30;
    y = r->f32;
    w = r->f3C;
    h = r->f3E;
    poly[3] = 5;
    poly[7] = 0x48;
    *(u32 *)(poly + 20) = 0x55555555;
    line[3] = 4;
    line[7] = 0x50;
    xm1 = x - 1;
    xp3 = x + 3;
    *(s16 *)(poly + 12) = xm1;
    *(s16 *)(poly + 8) = xm1;
    *(s16 *)(poly + 16) = xp3;
    *(s16 *)(poly + 18) = y - 1;
    *(s16 *)(poly + 14) = y - 1;
    *(s16 *)(poly + 10) = y + 3;
    func_8005B260(poly, ot, pri, 1);
    right = x + w;
    *(s16 *)(poly + 12) = right + 1;
    *(s16 *)(poly + 8) = right + 1;
    *(s16 *)(poly + 16) = right - 3;
    func_8005B260(poly, ot, pri, 1);
    *(s16 *)(poly + 18) = y + h + 1;
    *(s16 *)(poly + 14) = y + h + 1;
    *(s16 *)(poly + 10) = y + h - 3;
    func_8005B260(poly, ot, pri, 1);
    *(s16 *)(poly + 12) = xm1;
    *(s16 *)(poly + 8) = xm1;
    *(s16 *)(poly + 16) = xp3;
    func_8005B260(poly, ot, pri, 1);
    *(s16 *)(line + 16) = x + (w >> 1);
    *(s16 *)(line + 8) = x + (w >> 1);
    *(s16 *)(line + 10) = y + 2;
    *(s16 *)(line + 18) = 0;
    func_8005B260(line, ot, pri, 1);
    *(s16 *)(line + 10) = y + h - 2;
    *(s16 *)(line + 18) = 192;
    func_8005B260(line, ot, pri, 1);
    *(s16 *)(line + 18) = y + (h >> 1);
    *(s16 *)(line + 10) = y + (h >> 1);
    *(s16 *)(line + 8) = x + 2;
    *(s16 *)(line + 16) = 0;
    func_8005B260(line, ot, pri, 1);
    *(s16 *)(line + 8) = right - 2;
    *(s16 *)(line + 16) = 320;
    func_8005B260(line, ot, pri, 1);
}
