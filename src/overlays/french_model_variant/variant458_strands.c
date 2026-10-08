#include "../../types.h"
#include "variant458_strands.h"

void func_8013C150(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[6][9];
    PSXLONG p;
    PSXLONG flag;
    Variant458StrandView *work;
    POLY_FT4 *poly;
    GsOT *ot;
    s16 i;
    s32 phase;
    s32 base;
    s32 turn;
    s16 radius;
    s16 k;
    s32 wave;
    s32 sway;
    s32 count;
    Variant458Strand *strand;
    s32 dx;
    s32 dy;
    s32 row;

    work = (Variant458StrandView *)ctx;
    ot = func_80058F10();
    turn = ratan2(work->axis_z, work->axis_x) + 3072;
    ratan2(work->axis_y, work->axis_z);
    radius = work->size / 128;
    strand = work->strands;
    for (i = 0, base = 0; i < 6; i++, strand++, base = i * 4096 / 6) {
        if (!(i & 1)) {
            phase = -work->phase_a;
        } else {
            phase = work->phase_a;
        }
        for (k = 0; k < 9; k++, phase += 1300) {
            if (k == 0 || k == 8) {
                sway = 0;
            } else if (!(i & 1)) {
                sway = base - k * 768 + work->phase_b;
            } else {
                sway = base - k * 768 - work->phase_b;
            }
            rsin(sway);
            rsin(phase);
            setVector(&strand->a[k], work->directions[i].vx * k / 8, work->directions[i].vy * k / 8,
                      work->directions[i].vz * k / 8);
            setVector(&strand->b[k], strand->a[k].vx + (rcos(turn) * radius >> 12), strand->a[k].vy,
                      strand->a[k].vz + (rsin(turn) * radius >> 12));
            row = i;
            setVector(&rot, 0, 0, 0);
            m.t[0] = work->origins[row].vx;
            m.t[1] = work->origins[row].vy;
            m.t[2] = work->origins[row].vz;
            setVector(&scale, 4096, 4096, 4096);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            if (k == 8) {
                strand->depth[k] = RotTransPers4(&strand->a[k - 1], &strand->a[k], &strand->a[k - 1], &strand->a[k],
                                                 &strand->sa[k - 1].packed, &strand->sa[k].packed,
                                                 &strand->sa[k - 1].packed, &strand->sa[k].packed, &p, &flags[row][k]);
                RotTransPers(&strand->b[k], &strand->sb[k].packed, &p, &flag);
                dx = strand->sa[k].point.vx - strand->sa[k - 1].point.vx;
                dy = strand->sa[k].point.vy - strand->sa[k - 1].point.vy;
                strand->angle[k] = ratan2(dy, dx) - 0x400;
                strand->width[k] = strand->sb[k].point.vx - strand->sa[k].point.vx;
                strand->ox[k] = rcos(strand->angle[k]) * strand->width[k] >> 12;
                strand->oy[k] = rsin(strand->angle[k]) * strand->width[k] >> 12;
            } else {
                strand->depth[k] = RotTransPers4(&strand->a[k], &strand->a[k + 1], &strand->a[k], &strand->a[k + 1],
                                                 &strand->sa[k].packed, &strand->sa[k + 1].packed,
                                                 &strand->sa[k].packed, &strand->sa[k + 1].packed, &p, &flags[row][k]);
                RotTransPers(&strand->b[k], &strand->sb[k].packed, &p, &flag);
                dx = strand->sa[k + 1].point.vx - strand->sa[k].point.vx;
                dy = strand->sa[k + 1].point.vy - strand->sa[k].point.vy;
                strand->angle[k] = ratan2(dy, dx) - 0x400;
                strand->width[k] = strand->sb[k].point.vx - strand->sa[k].point.vx;
                strand->ox[k] = rcos(strand->angle[k]) * strand->width[k] >> 12;
                strand->oy[k] = rsin(strand->angle[k]) * strand->width[k] >> 12;
            }
            wave = base - k * 1024 + work->phase_b;
            strand->sa[k].packed += rcos(strand->angle[k]) * (rsin(wave) * 8 >> 12) >> 12;
            strand->sa[k].packed += (rsin(strand->angle[k]) * (rsin(wave) * 8 >> 12) >> 12) << 16;
        }
    }
    strand = work->strands;
    for (i = 0; i < 6; i++, strand++) {
        count = 0;
        if (strand->count >= 0) {
            count = strand->count;
        }
        for (k = 0, poly = work->quads; k < count; k++, poly = work->quads) {
            if ((s16)(k % 2) == 1) {
                poly++;
            }
            poly->x0 = strand->sa[k].point.vx + strand->ox[k];
            poly->y0 = (strand->sa[k].packed >> 16) + strand->oy[k];
            poly->x1 = strand->sa[k + 1].point.vx + strand->ox[k + 1];
            poly->y1 = (strand->sa[k + 1].packed >> 16) + strand->oy[k + 1];
            poly->x2 = strand->sa[k].point.vx - strand->ox[k];
            poly->y2 = (strand->sa[k].packed >> 16) - strand->oy[k];
            poly->x3 = strand->sa[k + 1].point.vx - strand->ox[k + 1];
            poly->y3 = (strand->sa[k + 1].packed >> 16) - strand->oy[k + 1];
            poly->r0 = strand->color.r;
            poly->g0 = strand->color.g;
            poly->b0 = strand->color.b;
            if (strand->depth[k] >= 0 && flags[i][k] >= 0) {
                func_8005B260((u32 *)poly, ot, (u16)strand->depth[k], 1);
            }
        }
        if (work->state == 3 && strand->count < 8) {
            strand->count += work->speed;
            if (strand->count >= 8) {
                strand->count = 8;
                if (i + 1 == 6) {
                    work->state = 4;
                }
            }
        }
    }
    work->phase_a += work->speed * 192;
    work->phase_b += work->speed * 384;
    work->spin += 64;
}
