/*
 * Current best under gcc_2_8_1_g8: 344/348 instructions and opcode distance
 * 4. The first 87 instructions, 0x58 frame, seven spill slots and register
 * roles match. Residual: one &poly[3] addiu, one &poly[5] saved-register copy
 * and two nops. Five named quad pointers and two alternating load temporaries
 * are required; pinning &poly[5] to $30 miscompiles by overwriting the
 * scratch-pad base.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../game/display_object.h"
#include "../game/gpu_packets.h"

void func_80042C08(DisplayObject *object, s32 arg1)
{
    s32 *r = (s32 *)0x1F800000;
    s32 *sc = (s32 *)0x1F800010;
    POLY_G4 *poly = (POLY_G4 *)0x1F800020;
    POLY_G4 *q1;
    POLY_G4 *q4;
    POLY_G4 *q5;
    POLY_G4 *q6;
    POLY_G4 *q7;
    s32 i;
    s32 j;
    s32 a;
    s32 x48;
    s32 x4A;
    s32 u14;
    s32 cx;
    s32 cy;
    s32 h0;
    s32 h1;

    sc[0] = object->field_44.h.field_44;
    sc[1] = object->field_44.h.field_46;
    u14 = (s16)object->field_14;
    cx = (s16)object->field_30.h.field_30;
    cy = (s16)object->field_30.h.field_32;
    x48 = object->field_48.h.field_48;
    x4A = object->field_48.h.field_4A;

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

    a = sc[0];
    r[0] = x48 * a / 4096;
    r[1] = x4A * a / 4096;
    r[2] = sc[0] * 240 / 4096;

    q1 = &poly[1];
    q4 = &poly[4];
    q5 = &poly[5];
    q6 = &poly[6];
    q7 = &poly[7];

    h0 = *(u16 *)&r[0] + cx;
    q1->x0 = h0;
    poly[3].x0 = h0;
    h0 = *(u16 *)&r[1] + cx;
    q5->x0 = h0;
    q7->x0 = h0;
    q1->x1 = h0;
    poly[3].x1 = h0;
    h0 = *(u16 *)&r[2] + cx;
    q5->x1 = h0;
    q7->x1 = h0;
    h0 = cx - *(u16 *)&r[0];
    poly[0].x0 = h0;
    poly[2].x0 = h0;
    h1 = cx - *(u16 *)&r[1];
    q4->x0 = h1;
    q6->x0 = h1;
    poly[0].x1 = h1;
    poly[2].x1 = h1;
    h1 = cx - *(u16 *)&r[2];
    q4->x1 = h1;
    q6->x1 = h1;

    for (i = 0x100; i < 0x401; i += 0x100) {
        r[0] = x48 * rcos(i) / 4096;
        r[0] = r[0] * sc[0] / 4096;
        r[1] = x4A * rcos(i) / 4096;
        r[1] = r[1] * sc[0] / 4096;
        r[2] = rcos(i) * 240 / 4096;
        r[2] = r[2] * sc[0] / 4096;

        h0 = *(u16 *)&r[0] + cx;
        q1->x2 = h0;
        poly[3].x2 = h0;
        h0 = *(u16 *)&r[1] + cx;
        q5->x2 = h0;
        q7->x2 = h0;
        q1->x3 = h0;
        poly[3].x3 = h0;
        h0 = *(u16 *)&r[2] + cx;
        q5->x3 = h0;
        q7->x3 = h0;
        h0 = cx - *(u16 *)&r[0];
        poly[0].x2 = h0;
        poly[2].x2 = h0;
        h1 = cx - *(u16 *)&r[1];
        q4->x2 = h1;
        q6->x2 = h1;
        poly[0].x3 = h1;
        poly[2].x3 = h1;
        h1 = cx - *(u16 *)&r[2];
        q4->x3 = h1;
        q6->x3 = h1;

        r[0] = x48 * rsin(i) / 4096;
        r[0] = r[0] * sc[1] / 4096;
        r[1] = x4A * rsin(i) / 4096;
        r[1] = r[1] * sc[1] / 4096;
        r[2] = rsin(i) * 240 / 4096;
        r[2] = r[2] * sc[1] / 4096;

        h0 = *(u16 *)&r[0] + cy;
        poly[3].y2 = h0;
        poly[2].y2 = h0;
        h0 = *(u16 *)&r[1] + cy;
        q7->y2 = h0;
        q6->y2 = h0;
        poly[3].y3 = h0;
        poly[2].y3 = h0;
        h0 = *(u16 *)&r[2] + cy;
        q7->y3 = h0;
        q6->y3 = h0;
        h0 = cy - *(u16 *)&r[0];
        q1->y2 = h0;
        poly[0].y2 = h0;
        h1 = cy - *(u16 *)&r[1];
        q5->y2 = h1;
        q4->y2 = h1;
        q1->y3 = h1;
        poly[0].y3 = h1;
        h1 = cy - *(u16 *)&r[2];
        q5->y3 = h1;
        q4->y3 = h1;

        for (j = 0; j < 8; j++) {
            func_8005B260((u32 *)&poly[j], (GsOT *)arg1, (u16)u14, 2);
            *(s32 *)&poly[j].x0 = *(s32 *)&poly[j].x2;
            *(s32 *)&poly[j].x1 = *(s32 *)&poly[j].x3;
        }
    }
}
