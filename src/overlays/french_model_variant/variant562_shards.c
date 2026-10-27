#include "../../types.h"

#include "variant562_shards.h"

#define SHARD_COUNT(work) ((*(Variant562ShardInfo *G32 *)((work) + 0x3230))->count)

/* Places each shard's two halves at origin + direction * scale / 1024 (scale
 * clamped at zero), turned by the ratan2 heading plus a quarter turn, and
 * projects them with RotTransPers3. A second pass sorts one white-to-grey
 * POLY_GT4 per shard whose second scale is below 0x800, then grows both
 * scales by step * 128 up to 0x800 while the shard state is 1, moving the
 * phase from 1 to 2 once the first scale reaches 0x400. The empty loops keep
 * the retail scheduling of the heading and the clamp. */
void func_8013C2CC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[3][2];
    PSXLONG p;
    GsOT *ot;
    s32 angle;
    u8 *work;
    POLY_GT4 *poly;
    Variant562Shard *rec;
    s16 j;
    s16 i;
    s32 v;
    s32 s;

    work = ctx;
    ot = func_80058F10();
    v = ratan2(*(s16 *)(work + 0x31FE), *(s16 *)(work + 0x31FC));
    poly = (POLY_GT4 *)(work + 0x2E08);
    do {
    } while (0);
    angle = v + 0x800;
    rec = (Variant562Shard *)work;
    for (j = 0; j < SHARD_COUNT(work); j++, rec++) {
        for (i = 0; i < 2; i++) {
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (i == 0) {
                v = rec->scale[0];
            } else {
                v = rec->scale[1];
            }
            s = 0;
            if (v >= 0) {
                do {
                    s = v;
                } while (0);
            }
            m.t[0] = *(s16 *)(work + 0x31E4) + *(s32 *)(work + 0x31EC) * s / 1024;
            m.t[1] = *(s16 *)(work + 0x31E6) + *(s32 *)(work + 0x31F0) * s / 1024;
            m.t[2] = *(s16 *)(work + 0x31E8) + *(s32 *)(work + 0x31F4) * s / 1024;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            ReadRotMatrix(&ls);
            RotMatrix(&rot, &ls);
            ScaleMatrix(&ls, &scale);
            SetRotMatrix(&ls);
            rec->otz[i] = RotTransPers3(&rec->v[0][i], &rec->v[1][i], &rec->v[2][i],
                                        &rec->sxy[0][i], &rec->sxy[1][i], &rec->sxy[2][i],
                                        &p, &flags[j][i]);
        }
    }

    rec = (Variant562Shard *)work;
    for (j = 0; j < SHARD_COUNT(work); j++, rec++) {
        if (rec->scale[1] < 0x800) {
            poly->r0 = 0xFF;
            poly->g0 = 0xFF;
            poly->b0 = 0xFF;
            poly->r1 = 0x10;
            poly->g1 = 0x10;
            poly->b1 = 0x10;
            poly->r2 = 0xFF;
            poly->g2 = 0xFF;
            poly->b2 = 0xFF;
            poly->r3 = 0x10;
            poly->g3 = 0x10;
            poly->b3 = 0x10;
            for (i = 0; i < 1; i++) {
                poly->x0 = rec->sxy[0][i];
                poly->y0 = rec->sxy[0][i] >> 16;
                poly->x1 = rec->sxy[0][i + 1];
                poly->y1 = rec->sxy[0][i + 1] >> 16;
                poly->x2 = rec->sxy[2][i];
                poly->y2 = rec->sxy[2][i] >> 16;
                poly->x3 = rec->sxy[2][i + 1];
                poly->y3 = rec->sxy[2][i + 1] >> 16;
                if (rec->otz[i] >= 0 && flags[j][i] >= 0) {
                    GsSortPoly(poly, ot, rec->otz[i]);
                }
            }
        }
        if (rec->state == 1) {
            if (rec->scale[0] < 0x800) {
                if (rec->scale[0] >= 0x400 && *(s32 *)(work + 0x3254) == 1) {
                    *(s32 *)(work + 0x3254) = 2;
                }
                rec->scale[0] += *(s32 *)(work + 0x3224) * 128;
                if (rec->scale[0] >= 0x800) {
                    rec->scale[0] = 0x800;
                }
            }
            if (rec->scale[1] < 0x800) {
                rec->scale[1] += *(s32 *)(work + 0x3224) * 128;
                if (rec->scale[1] >= 0x800) {
                    rec->scale[1] = 0x800;
                }
            }
        }
    }
}
