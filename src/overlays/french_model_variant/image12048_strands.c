#include "../../types.h"
#include "image12048_strands.h"

void FrenchModelVariant403_DrawStrands(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[5][7];
    PSXLONG interpolation;
    PSXLONG flag;
    Strands12048State *state = (Strands12048State *)context;
    Strands12048Strand *strand;
    POLY_FT4 *poly;
    SVECTOR *pos;
    SVECTOR *edge;
    GsOT *ot;
    s16 i, j;
    s32 twist;
    s32 sway;
    s32 offset;
    s32 tilt;
    s32 angle;
    s32 bend;
    s16 spread;
    s32 dx, dy;

    ot = func_80058F10();
    angle = ratan2(state->target[2], state->target[0]) + 0xC00;
    ratan2(state->target[1], state->target[0]);
    if (state->phase > 0) {
        if (state->phase < 4) {
            spread = 24;
        } else {
            spread = state->width / 16;
        }
        offset = 0;
        strand = state->strands;
        for (i = 0; i < state->timing->count; i++, strand++, offset = i * 3 * 2048 / state->timing->count) {
            if (state->phase < 4) {
                if (i == 0) {
                    tilt = 0x400;
                } else if (i == 1) {
                    tilt = 0x200;
                } else if (i == 2) {
                    tilt = 0x600;
                } else if (i == 3) {
                    tilt = 0;
                } else {
                    tilt = 0x800;
                }
            } else {
                tilt = 0x400;
            }
            sway = -state->sway;
            for (j = 0; j < 7;) {
                if (j == 0 || j == 6) {
                    twist = 0;
                    bend = 0;
                } else {
                    twist = offset - j * 4096 / 6 + state->twist;
                    bend = j * 2048 / 6 + offset;
                }
                j[strand->pos].vx = state->reach[i].vx * j / 6 +
                                    (rsin(bend) * (rcos(tilt + 0x800) * 128 >> 12) >> 12) +
                                    (rcos(angle) * ((rsin(twist) * 64 >> 12) + (rsin(sway) * 32 >> 12)) >> 12);
                j[strand->pos].vy = state->reach[i].vy * j / 6;
                j[strand->pos].vz = state->reach[i].vz * j / 6 +
                                    (rsin(angle) * ((rsin(twist) * 64 >> 12) + (rsin(sway) * 32 >> 12)) >> 12);
                j[strand->edge].vx = j[strand->pos].vx + (rcos(angle) * spread >> 12);
                pos = &j[strand->pos];
                edge = &pos[14];
                edge->vy = pos->vy;
                edge->vz = pos->vz + (rsin(angle) * spread >> 12);
                sway += 1300;
                j++;
            }

            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->origins[i].vx;
            matrix.t[1] = state->origins[i].vy;
            matrix.t[2] = state->origins[i].vz;
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);

            for (j = 0; j < 7; j++) {
                if (j == 6) {
                    strand->depth[j] = RotTransPers4(&strand->pos[j - 1], &strand->pos[j], &strand->pos[j - 1], &strand->pos[j],
                                                     &strand->sxy[j - 1], &strand->sxy[j],
                                                     &strand->sxy[j - 1], &strand->sxy[j],
                                                     &interpolation, &flags[i][j]);
                    RotTransPers(&strand->edge[j], &strand->edge_sxy[j], &interpolation, &flag);
                    dx = (s16)strand->sxy[j] - (s16)strand->sxy[j - 1];
                    dy = (s16)(strand->sxy[j] >> 16) - (s16)(strand->sxy[j - 1] >> 16);
                    strand->angle[j] = ratan2(dy, dx) - 0x400;
                    strand->width[j] = (s16)strand->edge_sxy[j] - (s16)strand->sxy[j];
                    strand->dx[j] = rcos(strand->angle[j]) * strand->width[j] >> 12;
                    strand->dy[j] = rsin(strand->angle[j]) * strand->width[j] >> 12;
                } else {
                    strand->depth[j] = RotTransPers4(&strand->pos[j], &strand->pos[j + 1], &strand->pos[j], &strand->pos[j + 1],
                                                     &strand->sxy[j], &strand->sxy[j + 1],
                                                     &strand->sxy[j], &strand->sxy[j + 1],
                                                     &interpolation, &flags[i][j]);
                    RotTransPers(&strand->edge[j], &strand->edge_sxy[j], &interpolation, &flag);
                    dx = (s16)strand->sxy[j + 1] - (s16)strand->sxy[j];
                    dy = (s16)(strand->sxy[j + 1] >> 16) - (s16)(strand->sxy[j] >> 16);
                    strand->angle[j] = ratan2(dy, dx) - 0x400;
                    strand->width[j] = (s16)strand->edge_sxy[j] - (s16)strand->sxy[j];
                    strand->dx[j] = rcos(strand->angle[j]) * strand->width[j] >> 12;
                    strand->dy[j] = rsin(strand->angle[j]) * strand->width[j] >> 12;
                }
            }
        }

        strand = state->strands;
        for (i = 0; i < state->timing->count; i++, strand++) {
            for (j = 0; j < state->length[i]; j++) {
                if (state->mode[i] == 3) {
                    poly = &state->polys[2];
                } else if (state->mode[i] == 4) {
                    poly = &state->polys[4];
                } else {
                    poly = state->polys;
                }
                if (!(state->frame & 1)) {
                    if (!(j & 1)) {
                        poly++;
                    }
                } else if (j % 2 == 1) {
                    poly++;
                }
                if (j + 1 == 6) {
                    poly->x0 = (s16)strand->sxy[j] + strand->dx[j];
                    poly->y0 = (strand->sxy[j] >> 16) + strand->dy[j];
                    poly->x1 = (s16)strand->sxy[j + 1];
                    poly->y1 = (strand->sxy[j + 1] >> 16);
                    poly->x2 = (s16)strand->sxy[j] - strand->dx[j];
                    poly->y2 = (strand->sxy[j] >> 16) - strand->dy[j];
                    poly->x3 = (s16)strand->sxy[j + 1];
                    poly->y3 = (strand->sxy[j + 1] >> 16);
                } else {
                    poly->x0 = (s16)strand->sxy[j] + strand->dx[j];
                    poly->y0 = (strand->sxy[j] >> 16) + strand->dy[j];
                    poly->x1 = (s16)strand->sxy[j + 1] + strand->dx[j + 1];
                    poly->y1 = (strand->sxy[j + 1] >> 16) + strand->dy[j + 1];
                    poly->x2 = (s16)strand->sxy[j] - strand->dx[j];
                    poly->y2 = (strand->sxy[j] >> 16) - strand->dy[j];
                    poly->x3 = (s16)strand->sxy[j + 1] - strand->dx[j + 1];
                    poly->y3 = (strand->sxy[j + 1] >> 16) - strand->dy[j + 1];
                }
                poly->r0 = strand->color.r;
                poly->g0 = strand->color.g;
                poly->b0 = strand->color.b;
                if (strand->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(poly, ot, (u16)strand->depth[j]);
                }
            }
            if (state->phase == 1) {
                if (state->length[i] < 6) {
                    state->length[i] += state->step >> 1;
                    if (state->length[i] >= 6) {
                        state->length[i] = 6;
                    }
                } else if (state->fade[i] < 0x400) {
                    state->fade[i] += state->step * 128;
                    if (state->fade[i] >= 0x400) {
                        state->fade[i] = 0x400;
                        state->mode[i] = 2;
                    }
                } else if (state->mode[i] == 2 || state->mode[i] == 3) {
                    state->mode[i]++;
                } else if (state->mode[i] == 4) {
                    state->mode[i] = 5;
                    state->length[i] = 0;
                    if (i + 1 == state->timing->count) {
                        state->phase = 2;
                    }
                }
            } else {
                if (i + 1 == state->timing->count && state->phase == 2) {
                    state->phase = 3;
                }
                if (state->phase >= 4 && state->length[i] < 6) {
                    state->length[i] += state->step >> 1;
                    if (state->length[i] >= 6) {
                        state->length[i] = 6;
                    }
                }
            }
        }
        if (state->phase == 5) {
            state->width = 0x400 - ((state->time - state->timing->start) << 10) / state->span;
        }
    }
    state->sway += state->step * 384;
    state->twist += state->step * 192;
}
