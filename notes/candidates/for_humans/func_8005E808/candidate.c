#include "../../../../src/types.h"

extern u8 *D_8009B074;
extern u8 D_800F5768[];

extern void func_8005FB30(u8 *);
extern s32 func_80058DD8(s32);
extern void Model_CopySlotU16Values(s32, void *);
extern s32 SquareRoot0(s32);
extern void func_8005EBF4(u8 *, s32, s32, s32, void *);

/* Recomputes a model's audible radius. The magnitude of the s16 at +0x20 sets
   the base radius at +0x22, capped at 0x4000, and when that field is negative
   the two 8-byte descriptors that follow the header are each scored and the
   largest result wins: type 0x80/0x81 measures the straight distance from the
   descriptor's offset position to the matching entry in D_800F5768, type 1
   sums the segment lengths of a ten-point path filled in by func_8005EBF4,
   and type 4 scales a fixed 6284 per unit by the descriptor's second word. */
void func_8005E808(u8 *p)
{
    s16 pos[3];
    s16 buf[10][3];
    u8 *e;
    u8 *q;
    u8 *g;
    s32 i;
    s32 j;
    s32 off;
    s32 a;
    s32 k;
    s32 sum;
    s32 t;
    u32 d;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 slot;
    u8 *prev;
    s32 n;
    u16 v;

    n = *(s16 *)(p + 0x20);
    a = n;
    if (n < 0) {
        a = -a;
    }
    t = a;
    if (a <= 0) {
        t = 1;
    }
    k = t * 2;
    func_8005FB30(p);
    *(s16 *)(p + 0x24) = 0;
    if (D_8009B074[0x26] != 0) {
        return;
    }
    p[0x26] = 1;
    if (a >= 0x4000) {
        *(s16 *)(p + 0x22) = 0x4000;
    } else {
        *(s16 *)(p + 0x22) = k;
    }
    if (*(s16 *)(p + 0x20) >= 0) {
        return;
    }

    e = p;
    g = D_800F5768;
    for (i = 0; i < 2; i++, e += 8, g += 8) {
        q = e + 2;
        sum = 0;
        switch (*(s16 *)(q + 4)) {
        case 0x80:
        case 0x81:
            slot = (s16)(*(u16 *)(q + 4) & 0xFF7F);
            if (func_80058DD8(slot) != 1) {
                continue;
            }
            Model_CopySlotU16Values(slot, pos);
            pos[0] = pos[0] + *(u16 *)e;
            pos[1] = pos[1] + *(u16 *)q;
            pos[2] = pos[2] + *(u16 *)(q + 2);
            dx = pos[0] - *(s16 *)g;
            dy = pos[1] - *(s16 *)(g + 2);
            dz = pos[2] - *(s16 *)(g + 4);
            d = k * SquareRoot0(dx * dx + dy * dy + dz * dz) / 1000;
            if (*(u16 *)(p + 0x22) < d) {
                *(s16 *)(p + 0x22) = d;
            }
            continue;
        case 1:
            for (j = 0; j < 10; j++) {
                func_8005EBF4(p, i, j, 0x1E, buf[j]);
            }
            off = 0;
            for (j = 1; j < 10; j++) {
                prev = (u8 *)buf + off;
                dx = *(s16 *)prev - buf[j][0];
                dy = *(s16 *)(prev + 2) - buf[j][1];
                dz = *(s16 *)(prev + 4) - buf[j][2];
                sum += SquareRoot0(dx * dx + dy * dy + dz * dz);
                off += 6;
            }
            d = k * sum / 1000;
            break;
        case 4:
            v = *(u16 *)(D_800F5768 + 0x10);
            if (*(s16 *)(D_800F5768 + 0x10) < *(s16 *)e) {
                v = *(u16 *)e;
            }
            t = (s16)v * 6284 / 1000;
            t = k * t;
            slot = *(s16 *)q;
            if (slot < 0) {
                slot = -slot;
            }
            d = (u32)(t * slot) / 4096000;
            break;
        default:
            continue;
        }
        if (*(u16 *)(p + 0x22) < d) {
            *(s16 *)(p + 0x22) = d;
        }
    }
}
