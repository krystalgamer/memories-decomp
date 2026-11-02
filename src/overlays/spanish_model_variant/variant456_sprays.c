#include "../../types.h"

#include "variant456_sprays.h"

/* Sixteen sprays of seventeen points. Each spray points along its own
 * direction, turned by i * 1300 and i * 700 plus its spin, and every point
 * moves out to its growth, capped at 1024; a copy of each point is moved 12
 * units along the view. Growth rises by step * 128; when the last point is
 * full the spin advances by 800 and, before the restart frame of the row at
 * +0x2330, the spray restarts with its points staggered by 64. The points
 * are projected and swayed sideways along their screen normals by the phases
 * at +0x235C and +0x2358, and each segment is drawn as a POLY_FT4 as wide as
 * the projected offset, tapering to a point at both ends. */
void SpanishModelVariant456_DrawSprays(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    SVECTOR points[16][17];
    PSXLONG screen[16][17];
    s32 angle[16][17];
    SVECTOR offset_points[16][17];
    PSXLONG offset_screen[16][17];
    s32 width[16][17];
    PSXLONG flags[16][17];
    s32 depth[16][17];
    s16 ox[16][17];
    s16 oy[16][17];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    Spray456 *spray;
    POLY_FT4 *poly;
    GsOT *ot;
    s16 i;
    s16 j;
    s32 wave;
    s32 pitch;
    s32 turn;
    s32 radius;
    s32 phase;
    s32 sway;
    s32 swing;
    s32 length;
    s32 dx;
    s32 dy;

    work = ctx;
    spray = (Spray456 *)(work + 0x440);
    poly = (POLY_FT4 *)(work + 0x2204);
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x230C), MODEL_VARIANT_WORD(work, 0x2304)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x2308), MODEL_VARIANT_WORD(work, 0x2304));
    length = 12;
    for (i = 0, pitch = 0, phase = 0; i < 16; i++, spray++, phase += 1300, pitch += 700) {
        wave = MODEL_VARIANT_WORD(work, 0x2358);
        for (j = 0; j < 17; j++, wave += 1300) {
            radius = 0;
            if (spray->growth[j] > 0) {
                radius = 1024;
                if (spray->growth[j] < 1024) {
                    radius = spray->growth[j];
                }
            }
            setVector(&points[i][j], rcos(phase + pitch + spray->spin) * radius >> 12, rsin(phase + spray->spin) * radius >> 12, rsin(pitch + spray->spin) * radius >> 12);
            setVector(&offset_points[i][j], points[i][j].vx + (rcos(turn) * length >> length), points[i][j].vy, points[i][j].vz + (rsin(turn) * length >> length));
            if (spray->growth[j] <= 1024) {
                spray->growth[j] += MODEL_VARIANT_WORD(work, 0x2328) << 7;
                if (spray->growth[j] >= 1024) {
                    spray->growth[j] = 1024;
                    if (j == 16) {
                        if ((*(Sprays456Timing *G32 *)(work + 0x2330))->restart <= MODEL_VARIANT_WORD(work, 0x2320)) {
                            spray->growth[16] = 1024;
                        } else {
                            for (j = 0; j < 17; j++) {
                                spray->growth[j] = -(j * 64);
                            }
                        }
                        spray->spin += 800;
                    }
                }
            }
        }
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x22DC);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x22E0);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x22E4);
    scale.vx = 0x1000;
    scale.vy = 0x1000;
    scale.vz = 0x1000;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    spray = (Spray456 *)(work + 0x440);
    for (i = 0, pitch = MODEL_VARIANT_WORD(work, 0x235C); i < 16; i++, pitch += 1800, spray++) {
        for (j = 0; j < 17; j++) {
            radius = spray->growth[j]; /* read but never used */
            if (j == 16) {
                depth[i][j] = RotTransPers4(&points[i][15], &points[i][16], &points[i][15], &points[i][16],
                                            &screen[i][15], &screen[i][16], &screen[i][15], &screen[i][16],
                                            &p, &flags[i][j]);
                RotTransPers(&offset_points[i][16], &offset_screen[i][16], &p, &flag);
                dx = (s16)screen[i][j] - (s16)screen[i][j - 1];
                dy = (screen[i][j] >> 16) - (screen[i][j - 1] >> 16);
            } else {
                depth[i][j] = RotTransPers4(&points[i][j], &points[i][j + 1], &points[i][j], &points[i][j + 1],
                                            &screen[i][j], &screen[i][j + 1], &screen[i][j], &screen[i][j + 1],
                                            &p, &flags[i][j]);
                RotTransPers(&offset_points[i][j], &offset_screen[i][j], &p, &flag);
                dx = (s16)screen[i][j + 1] - (s16)screen[i][j];
                dy = (screen[i][j + 1] >> 16) - (screen[i][j] >> 16);
            }
            angle[i][j] = ratan2(dy, dx) - 0x400;
            width[i][j] = (s16)offset_screen[i][j] - (s16)screen[i][j];
            ox[i][j] = rcos(angle[i][j]) * width[i][j] >> 12;
            oy[i][j] = rsin(angle[i][j]) * width[i][j] >> 12;
            if (j == 0 || j == 16) {
                phase = 0;
            } else {
                phase = pitch - j * 384;
            }
            sway = (s32)((u32)width[i][j] << 5) / 12;
            swing = (s32)((u32)width[i][j] << 6) / 12;
            screen[i][j] += rcos(angle[i][j]) * ((rsin(phase) * sway >> 12) + (rsin(wave) * swing >> 12)) >> 12;
            screen[i][j] = (u32)screen[i][j] +
                           ((u32)(rsin(angle[i][j]) * ((rsin(phase) * sway >> 12) + (rsin(wave) * swing >> 12)) >> 12) << 16);
        }
    }
    spray = (Spray456 *)(work + 0x440);
    for (i = 0; i < 16; i++, spray++) {
        poly->r0 = spray->color.r;
        poly->g0 = spray->color.g;
        poly->b0 = spray->color.b;
        poly = (POLY_FT4 *)(work + 0x2204);
        for (j = 0; j < 16; j++) {
            if (j == 0) {
                poly->x0 = screen[i][j];
                poly->y0 = screen[i][j] >> 16;
                poly->x1 = screen[i][j + 1] + ox[i][j + 1];
                poly->y1 = (screen[i][j + 1] >> 16) + oy[i][j + 1];
                poly->x2 = screen[i][j];
                poly->y2 = screen[i][j] >> 16;
                poly->x3 = screen[i][j + 1] - ox[i][j + 1];
                poly->y3 = (screen[i][j + 1] >> 16) - oy[i][j + 1];
            } else if (j + 1 == 16) {
                poly->x0 = screen[i][j] + ox[i][j];
                poly->y0 = (screen[i][j] >> 16) + oy[i][j];
                poly->x1 = screen[i][j + 1];
                poly->y1 = screen[i][j + 1] >> 16;
                poly->x2 = screen[i][j] - ox[i][j];
                poly->y2 = (screen[i][j] >> 16) - oy[i][j];
                poly->x3 = screen[i][j + 1];
                poly->y3 = screen[i][j + 1] >> 16;
            } else {
                poly->x0 = screen[i][j] + ox[i][j];
                poly->y0 = (screen[i][j] >> 16) + oy[i][j];
                poly->x1 = screen[i][j + 1] + ox[i][j + 1];
                poly->y1 = (screen[i][j + 1] >> 16) + oy[i][j + 1];
                poly->x2 = screen[i][j] - ox[i][j];
                poly->y2 = (screen[i][j] >> 16) - oy[i][j];
                poly->x3 = screen[i][j + 1] - ox[i][j + 1];
                poly->y3 = (screen[i][j + 1] >> 16) - oy[i][j + 1];
            }
            if (depth[i][j] >= 0 && flags[i][j] >= 0 && spray->growth[j] > 0 && spray->hidden == 0) {
                GsSortPoly(poly, ot, depth[i][j]);
            }
            if (!(j & 1)) {
                poly++;
            } else {
                poly--;
            }
        }
    }
}
