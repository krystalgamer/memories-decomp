#include "../types.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "func_8005A6A8.h"

/* Distance-to-camera fade and pan for one sound source: the square root of
 * the distance from the listener record at D_800F56F0, minus 700, sets the
 * volume byte (0xFF down to 0x10), and the signed projection onto the
 * listener direction, divided by 16 and clamped to +-0x7F, sets the pan. */
void func_8005A6A8(s32 arg0, s32 arg1, u8 *arg2, u8 *arg3) {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 f;
    s32 g;
    s32 h;
    s32 n;
    s32 q;
    s32 z;
    s32 u;
    s32 v;
    s32 w;

    a = D_800F56F0.vpx - arg0;
    b = D_800F56F0.vpz - arg1;
    h = SquareRoot0(a * a + b * b);
    q = D_800F56F0.vpz;
    c = D_800F56F0.vrx * q;
    d = D_800F56F0.vrz - q;
    f = D_800F56F0.vpx - D_800F56F0.vrx;
    g = c - D_800F56F0.vpx * D_800F56F0.vrz;
    n = SquareRoot0(d * d + f * f);
    h = h - 0x2BC;

    *arg2 = 0xFF;
    *arg3 = 0;

    if (h > 0) {
        v = 0xFF - h / 12;
        if (v < 0x10) {
            v = 0x10;
        }
        *arg2 = v;
    }

    if (n != 0) {
        h = (arg0 * d + arg1 * f + g) / n;
        w = h / 16;
        v = w;
        w = v < 0;
        if (w) {
            v = -v;
        }
        if (v >= 0x80) {
            v = 0x7F;
        }
        w = v;
        if (h <= 0) {
            w = -w;
        }
        *arg3 = w;
    }
}
