#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "gpu_packets.h"
#include "func_80042C08.h"

/* Builds the mask one quadrant step at a time. The eight POLY_G4 quads sit in
 * the scratchpad at 0x1F800020: quads 0-3 are the inner ring, black at the
 * +0x48 radius and fading to white at the +0x4A radius, and quads 4-7 are
 * the outer ring, white from the +0x4A radius out to 240. Each ring has one
 * quad per quadrant (1 and 3 on the right, 0 and 2 on the left, 1 and 0 at
 * the top). The radii are scaled by the object's +0x44/+0x46 pair, 4096 being
 * 1.0. Every pass computes the next edge from rcos/rsin of 0x100 more (a
 * sixteenth of a turn) and links all eight quads through func_8005B260 in
 * mode 2, before that edge becomes the next pass's first one.
 *
 * The chained assignments are what reproduce retail. GCC gives each chain
 * its own block-local value register, and it lifts the addresses of quads 1,
 * 4, 5, 6 and 7 into pseudos of its own. Those are the spilled pointers at
 * sp+0x1C..0x28 and in $fp. Named pointer locals allocate differently. */
void func_80042C08(DisplayObject *object, GsOT *ot)
{
    s32 depth;
    s32 inner;
    s32 outer;
    s32 *radius = (s32 *)0x1F800000;
    s32 *scale = (s32 *)0x1F800010;
    POLY_G4 *poly = (POLY_G4 *)0x1F800020;
    s32 i;
    s32 j;
    s32 cx;
    s32 cy;

    scale[0] = object->field_44.h.field_44;
    scale[1] = object->field_44.h.field_46;
    depth = (s16)object->field_14;
    cx = (s16)object->field_30.h.field_30;
    cy = (s16)object->field_30.h.field_32;
    inner = object->field_48.h.field_48;
    outer = object->field_48.h.field_4A;

    for (i = 0; i < 4; i++) {
        *(u32 *)&poly[i].r0 = 0;
        *(u32 *)&poly[i].r2 = 0;
    }
    for (i = 4; i < 8; i++) {
        *(u32 *)&poly[i].r0 = 0xFFFFFF;
        *(u32 *)&poly[i].r2 = 0xFFFFFF;
    }
    for (i = 0; i < 8; i++) {
        poly[i].y1 = cy;
        poly[i].y0 = cy;
        *(u32 *)&poly[i].r1 = 0xFFFFFF;
        *(u32 *)&poly[i].r3 = 0xFFFFFF;
        setPolyG4(&poly[i]);
    }

    radius[0] = inner * scale[0] / 4096;
    radius[1] = outer * scale[0] / 4096;
    radius[2] = scale[0] * 240 / 4096;

    poly[3].x0 = poly[1].x0 = radius[0] + cx;
    poly[3].x1 = poly[1].x1 = poly[7].x0 = poly[5].x0 = radius[1] + cx;
    poly[7].x1 = poly[5].x1 = radius[2] + cx;
    poly[2].x0 = poly[0].x0 = cx - radius[0];
    poly[2].x1 = poly[0].x1 = poly[6].x0 = poly[4].x0 = cx - radius[1];
    poly[6].x1 = poly[4].x1 = cx - radius[2];

    for (i = 0x100; i < 0x401; i += 0x100) {
        radius[0] = inner * rcos(i) / 4096;
        radius[0] = radius[0] * scale[0] / 4096;
        radius[1] = outer * rcos(i) / 4096;
        radius[1] = radius[1] * scale[0] / 4096;
        radius[2] = rcos(i) * 240 / 4096;
        radius[2] = radius[2] * scale[0] / 4096;

        poly[3].x2 = poly[1].x2 = radius[0] + cx;
        poly[3].x3 = poly[1].x3 = poly[7].x2 = poly[5].x2 = radius[1] + cx;
        poly[7].x3 = poly[5].x3 = radius[2] + cx;
        poly[2].x2 = poly[0].x2 = cx - radius[0];
        poly[2].x3 = poly[0].x3 = poly[6].x2 = poly[4].x2 = cx - radius[1];
        poly[6].x3 = poly[4].x3 = cx - radius[2];

        radius[0] = inner * rsin(i) / 4096;
        radius[0] = radius[0] * scale[1] / 4096;
        radius[1] = outer * rsin(i) / 4096;
        radius[1] = radius[1] * scale[1] / 4096;
        radius[2] = rsin(i) * 240 / 4096;
        radius[2] = radius[2] * scale[1] / 4096;

        poly[2].y2 = poly[3].y2 = radius[0] + cy;
        poly[2].y3 = poly[3].y3 = poly[6].y2 = poly[7].y2 = radius[1] + cy;
        poly[6].y3 = poly[7].y3 = radius[2] + cy;
        poly[0].y2 = poly[1].y2 = cy - radius[0];
        poly[0].y3 = poly[1].y3 = poly[4].y2 = poly[5].y2 = cy - radius[1];
        poly[4].y3 = poly[5].y3 = cy - radius[2];

        for (j = 0; j < 8; j++) {
            func_8005B260((u32 *)&poly[j], ot, (u16)depth, 2);
            *(s32 *)&poly[j].x0 = *(s32 *)&poly[j].x2;
            *(s32 *)&poly[j].x1 = *(s32 *)&poly[j].x3;
        }
    }
}
