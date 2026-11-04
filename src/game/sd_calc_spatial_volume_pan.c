#include "../types.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "sd_calc_spatial_volume_pan.h"

/* Distance-to-camera fade and pan for one sound source: the square root of
 * the distance from the listener record at gModel_RView, minus 700, sets the
 * volume byte (0xFF down to 0x10), and the signed projection onto the
 * listener direction, divided by 16 and clamped to +-0x7F, sets the pan. */
void SD_CalcSpatialVolumePan(s32 source_x, s32 source_z, u8 *volume, u8 *pan) {
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

    a = gModel_RView.vpx - source_x;
    b = gModel_RView.vpz - source_z;
    h = SquareRoot0(a * a + b * b);
    q = gModel_RView.vpz;
    c = gModel_RView.vrx * q;
    d = gModel_RView.vrz - q;
    f = gModel_RView.vpx - gModel_RView.vrx;
    g = c - gModel_RView.vpx * gModel_RView.vrz;
    n = SquareRoot0(d * d + f * f);
    h = h - 0x2BC;

    *volume = 0xFF;
    *pan = 0;

    if (h > 0) {
        v = 0xFF - h / 12;
        if (v < 0x10) {
            v = 0x10;
        }
        *volume = v;
    }

    if (n != 0) {
        h = (source_x * d + source_z * f + g) / n;
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
        *pan = w;
    }
}
