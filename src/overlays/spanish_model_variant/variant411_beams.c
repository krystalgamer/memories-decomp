#include "../../types.h"

#include "../model_variant/variant411_beams.h"

/* From phase 4 on, builds sixteen two-point beams in four tilt groups around
 * the spin, grows each point to 0x400, projects each spine and a copy of it
 * moved along the view, and draws each beam as two POLY_GT4 halves fading to
 * black whose depth lies inside the ordering-table range. */
void func_8013C57C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Beam411State *state;
    ModelVariantSheet *sheet;
    Beam411 *beam;
    POLY_GT4 *quad;
    GsOT *ot;
    s16 i;
    s16 j;
    s16 h;
    s16 size;
    s32 turn;
    s32 tilt;
    s32 angle;
    s16 length;
    s32 dx;
    s32 dy;

    beam = ((Beam411State *)context)->beams;
    state = (Beam411State *)context;
    ot = func_80058F10();
    turn = ratan2(state->direction[2], state->direction[0]) + 0xC00;
    ratan2(state->direction[1], state->direction[0]);
    sheet = &state->sheet;
    quad = &state->quad;
    if (state->phase >= 4) {
        for (i = 0, angle = state->spin; i < 16; i++, beam++, angle += 0x400) {
            if (i < 4) {
                tilt = 0x100;
                if (i == 3) {
                    angle += 400;
                }
            } else if (i < 8) {
                tilt = 0x300;
                if (i == 7) {
                    angle += 400;
                }
            } else if (i < 12) {
                tilt = 0x500;
                if (i == 11) {
                    angle += 400;
                }
            } else if (i < 16) {
                tilt = 0x700;
            }
            for (j = 0; j < 2; j++) {
                if (j == 0) {
                    length = 16;
                } else {
                    length = 64;
                }
                h = (rsin(tilt) << 10) * j >> 12;
                setVector(&beam->a[j], rcos(angle) * (h * j) >> 12, (rcos(tilt) << 10) * j >> 12,
                          rsin(angle) * (h * j) >> 12);
                setVector(&beam->b[j], beam->a[j].vx + (rcos(turn) * length >> 12), beam->a[j].vy,
                          beam->a[j].vz + (rsin(turn) * length >> 12));
                if (beam->grow[j] < 0x400) {
                    beam->grow[j] += 80;
                    if (beam->grow[j] >= 0x400) {
                        beam->grow[j] = 0x400;
                    }
                }
            }
        }
        if (!(state->frame & 1)) {
            size = sheet->size;
        } else {
            size = sheet->size - 512;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = state->position[0];
        matrix.t[1] = state->position[1];
        matrix.t[2] = state->position[2];
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        beam = state->beams;
        for (i = 0; i < 16; i++, beam++) {
            if (beam->age >= 11) {
                for (j = 0; j < 2; j++) {
                    if (j == 1) {
                        beam->otz[j] = RotTransPers4(&beam->a[j - 1], &beam->a[j], &beam->a[j - 1], &beam->a[j],
                                                     &beam->sa[j - 1], &beam->sa[j], &beam->sa[j - 1], &beam->sa[j],
                                                     &interpolation, &flag);
                        RotTransPers(&beam->b[j], &beam->sb[j], &interpolation, &flag);
                        dx = (s16)beam->sa[j] - (s16)beam->sa[j - 1];
                        dy = (beam->sa[j] >> 16) - (beam->sa[j - 1] >> 16);
                        beam->angle[j] = ratan2(dy, dx) - 0x400;
                        beam->width[j] = (s16)beam->sb[j] - (s16)beam->sa[j];
                        beam->ox[j] = rcos(beam->angle[j]) * beam->width[j] >> 12;
                        beam->oy[j] = rsin(beam->angle[j]) * beam->width[j] >> 12;
                    } else {
                        beam->otz[j] = RotTransPers4(&beam->a[j], &beam->a[j + 1], &beam->a[j], &beam->a[j + 1],
                                                     &beam->sa[j], &beam->sa[j + 1], &beam->sa[j], &beam->sa[j + 1],
                                                     &interpolation, &flag);
                        RotTransPers(&beam->b[j], &beam->sb[j], &interpolation, &flag);
                        dx = (s16)beam->sa[j + 1] - (s16)beam->sa[j];
                        dy = (beam->sa[j + 1] >> 16) - (beam->sa[j] >> 16);
                        beam->angle[j] = ratan2(dy, dx) - 0x400;
                        beam->width[j] = (s16)beam->sb[j] - (s16)beam->sa[j];
                        beam->ox[j] = rcos(beam->angle[j]) * beam->width[j] >> 12;
                        beam->oy[j] = rsin(beam->angle[j]) * beam->width[j] >> 12;
                    }
                }
            }
        }
        beam = state->beams;
        for (i = 0; i < 16; i++, beam++) {
            if (beam->age >= 11) {
                for (j = 0; j < 1; j++) {
                    quad->x0 = beam->sa[j] + beam->ox[j];
                    quad->y0 = (beam->sa[j] >> 16) + beam->oy[j];
                    quad->x1 = beam->sa[j + 1] + beam->ox[j + 1];
                    quad->y1 = (beam->sa[j + 1] >> 16) + beam->oy[j + 1];
                    quad->x2 = beam->sa[j];
                    quad->y2 = beam->sa[j] >> 16;
                    quad->x3 = beam->sa[j + 1];
                    quad->y3 = beam->sa[j + 1] >> 16;
                    setRGB0(quad, beam->inner[0], beam->inner[1], beam->inner[2]);
                    setRGB1(quad, 0, 0, 0);
                    setRGB2(quad, beam->outer[0], beam->outer[1], beam->outer[2]);
                    setRGB3(quad, 0, 0, 0);
                    if (beam->otz[j] >= 0) {
                        if (beam->otz[j] < 0x800) {
                            GsSortPoly(quad, ot, beam->otz[j]);
                        }
                    }
                }
                for (j = 0; j < 1; j++) {
                    quad->x0 = beam->sa[j] - beam->ox[j];
                    quad->y0 = (beam->sa[j] >> 16) - beam->oy[j];
                    quad->x1 = beam->sa[j + 1] - beam->ox[j + 1];
                    quad->y1 = (beam->sa[j + 1] >> 16) - beam->oy[j + 1];
                    quad->x2 = beam->sa[j];
                    quad->y2 = beam->sa[j] >> 16;
                    quad->x3 = beam->sa[j + 1];
                    quad->y3 = beam->sa[j + 1] >> 16;
                    setRGB0(quad, beam->inner[0], beam->inner[1], beam->inner[2]);
                    setRGB1(quad, 0, 0, 0);
                    setRGB2(quad, beam->outer[0], beam->outer[1], beam->outer[2]);
                    setRGB3(quad, 0, 0, 0);
                    if (beam->otz[j] > 0) {
                        if (beam->otz[j] < 0x800) {
                            GsSortPoly(quad, ot, beam->otz[j]);
                        }
                    }
                }
            }
            beam->age++;
        }
    }
    state->spin += state->step * 8;
}
