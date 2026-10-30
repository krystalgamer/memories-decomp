#include "../../types.h"
#include "image12048_chains.h"

void func_8013D22C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[3][9];
    PSXLONG interpolation;
    PSXLONG flag;
    Chains12048State *state = (Chains12048State *)context;
    Chains12048Chain *chain = state->chains;
    POLY_FT4 *poly;
    SVECTOR *pos;
    SVECTOR *edge;
    GsOT *ot;
    s16 i, j;
    s16 amp;
    s32 angle;
    s32 wave, sway;
    s32 limit;
    s32 spread;
    s32 dx, dy;

    ot = func_80058F10();
    angle = ratan2(state->target[2], state->target[0]) + 0xC00;
    ratan2(state->target[1], state->target[0]);
    for (i = 0; i < 3; i++, chain++) {
        wave = chain->wave;
        sway = chain->sway;
        for (j = 0; j < 9;) {
            amp = rcos(wave) * 64 >> 12;
            amp += rcos(sway) * 32 >> 12;
            if (j == 0) {
                chain->u.f.pos[0].vx = rcos(chain->yaw) * (rsin(chain->pitch) * 384 >> 12) >> 12;
                chain->u.f.pos[0].vy = rcos(chain->pitch) * 384 >> 12;
                chain->u.f.pos[0].vz = rsin(chain->yaw) * (rsin(chain->pitch) * 384 >> 12) >> 12;
                chain->dir[0] = -chain->u.f.pos[0].vx;
                chain->dir[1] = -chain->u.f.pos[0].vy;
                chain->dir[2] = -chain->u.f.pos[0].vz;
            } else {
                j[chain->u.f.pos].vx = chain->u.f.pos[0].vx + chain->dir[0] * j / 4 +
                                   (rcos(angle) * (rcos(chain->pitch) * amp >> 12) >> 12);
                j[chain->u.f.pos].vy = chain->u.f.pos[0].vy + chain->dir[1] * j / 4 +
                                   (rsin(chain->pitch) * amp >> 12);
                j[chain->u.f.pos].vz = chain->u.f.pos[0].vz + chain->dir[2] * j / 4 +
                                   (rsin(angle) * (rcos(chain->pitch) * amp >> 12) >> 12);
            }
            spread = 24;
            j[chain->u.f.edge].vx = j[chain->u.f.pos].vx + (rcos(angle) * spread >> 12);
            pos = &j[chain->u.span];
            edge = &pos[18];
            edge->vy = pos->vy;
            edge->vz = pos->vz + (rsin(angle) * spread >> 12);
            j++;
            sway += 1700;
            wave = j * 768;
        }
    }

    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->offset.vx;
    matrix.t[1] = state->offset.vy;
    matrix.t[2] = state->offset.vz;
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

    chain = state->chains;
    for (i = 0; i < 3; i++, chain++) {
        for (j = 0; j < 9; j++) {
            if (j == 8) {
                chain->depth[j] = RotTransPers4(&chain->u.f.pos[j - 1], &chain->u.f.pos[j], &chain->u.f.pos[j - 1], &chain->u.f.pos[j],
                                                &chain->u.f.sxy[j - 1], &chain->u.f.sxy[j],
                                                &chain->u.f.sxy[j - 1], &chain->u.f.sxy[j],
                                                &interpolation, &flags[i][j]);
                RotTransPers(&chain->u.f.edge[j], &chain->edge_sxy[j], &interpolation, &flag);
                dx = (s16)chain->u.f.sxy[j] - (s16)chain->u.f.sxy[j - 1];
                dy = (s16)(chain->u.f.sxy[j] >> 16) - (s16)(chain->u.f.sxy[j - 1] >> 16);
                chain->u.f.angle[j] = ratan2(dy, dx) + 0xC00;
                chain->width[j] = (s16)chain->edge_sxy[j] - (s16)chain->u.f.sxy[j];
                chain->dx[j] = rcos(chain->u.f.angle[j]) * chain->width[j] >> 12;
                chain->dy[j] = rsin(chain->u.f.angle[j]) * chain->width[j] >> 12;
            } else {
                chain->depth[j] = RotTransPers4(&chain->u.f.pos[j], &chain->u.f.pos[j + 1], &chain->u.f.pos[j], &chain->u.f.pos[j + 1],
                                                &chain->u.f.sxy[j], &chain->u.f.sxy[j + 1],
                                                &chain->u.f.sxy[j], &chain->u.f.sxy[j + 1],
                                                &interpolation, &flags[i][j]);
                RotTransPers(&chain->u.f.edge[j], &chain->edge_sxy[j], &interpolation, &flag);
                dx = (s16)chain->u.f.sxy[j + 1] - (s16)chain->u.f.sxy[j];
                dy = (s16)(chain->u.f.sxy[j + 1] >> 16) - (s16)(chain->u.f.sxy[j] >> 16);
                chain->u.f.angle[j] = ratan2(dy, dx) + 0xC00;
                chain->width[j] = (s16)chain->edge_sxy[j] - (s16)chain->u.f.sxy[j];
                chain->dx[j] = rcos(chain->u.f.angle[j]) * chain->width[j] >> 12;
                chain->dy[j] = rsin(chain->u.f.angle[j]) * chain->width[j] >> 12;
            }
        }
    }

    chain = state->chains;
    for (i = 0; i < 3; i++, chain++) {
        limit = 0;
        if (chain->end >= 0) {
            limit = chain->end;
        }
        for (j = chain->start, poly = state->polys; j < limit; j++, poly = state->polys) {
            if (!(j & 1)) {
                poly++;
            }
            if (j == 0) {
                poly->x0 = (s16)chain->u.f.sxy[j];
                poly->y0 = (chain->u.f.sxy[j] >> 16);
                poly->x1 = (s16)chain->u.f.sxy[j + 1] + chain->dx[j + 1];
                poly->y1 = (chain->u.f.sxy[j + 1] >> 16) + chain->dy[j + 1];
                poly->x2 = (s16)chain->u.f.sxy[j];
                poly->y2 = (chain->u.f.sxy[j] >> 16);
                poly->x3 = (s16)chain->u.f.sxy[j + 1] - chain->dx[j + 1];
                poly->y3 = (chain->u.f.sxy[j + 1] >> 16) - chain->dy[j + 1];
            } else if (j + 1 == 8) {
                poly->x0 = (s16)chain->u.f.sxy[j] + chain->dx[j];
                poly->y0 = (chain->u.f.sxy[j] >> 16) + chain->dy[j];
                poly->x1 = (s16)chain->u.f.sxy[j + 1];
                poly->y1 = (chain->u.f.sxy[j + 1] >> 16);
                poly->x2 = (s16)chain->u.f.sxy[j] - chain->dx[j];
                poly->y2 = (chain->u.f.sxy[j] >> 16) - chain->dy[j];
                poly->x3 = (s16)chain->u.f.sxy[j + 1];
                poly->y3 = (chain->u.f.sxy[j + 1] >> 16);
            } else {
                poly->x0 = (s16)chain->u.f.sxy[j] + chain->dx[j];
                poly->y0 = (chain->u.f.sxy[j] >> 16) + chain->dy[j];
                poly->x1 = (s16)chain->u.f.sxy[j + 1] + chain->dx[j + 1];
                poly->y1 = (chain->u.f.sxy[j + 1] >> 16) + chain->dy[j + 1];
                poly->x2 = (s16)chain->u.f.sxy[j] - chain->dx[j];
                poly->y2 = (chain->u.f.sxy[j] >> 16) - chain->dy[j];
                poly->x3 = (s16)chain->u.f.sxy[j + 1] - chain->dx[j + 1];
                poly->y3 = (chain->u.f.sxy[j + 1] >> 16) - chain->dy[j + 1];
            }
            poly->r0 = chain->color[j].r;
            poly->g0 = chain->color[j].g;
            poly->b0 = chain->color[j].b;
            if (chain->depth[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)chain->depth[j]);
            }
        }
        if (chain->stage < 4 && chain->end < 9) {
            chain->end += 2;
            if (chain->end >= 8) {
                chain->end = 8;
                if (chain->start < 8) {
                    chain->start += 2;
                    if (chain->start >= 8) {
                        chain->end = -8;
                        chain->start = 0;
                        chain->yaw += 1200;
                        chain->pitch += 300;
                        if (state->phase >= 6) {
                            chain->stage++;
                            if (i + 1 == 3 && chain->stage == 4) {
                                state->phase = 7;
                            }
                        }
                    }
                }
            }
        }
        chain->wave += 0x180;
        chain->sway += 0x300;
    }
}
