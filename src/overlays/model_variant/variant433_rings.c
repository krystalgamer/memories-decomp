#include "../../types.h"
#include "variant433_rings.h"

/* Header 433's rings: sixteen rings of nine points grown from the centre by
 * a per-point amount, projected with RotTransPers4 and drawn as quads from
 * the projected edges. The second `ring->amount[j] = limit;` in the
 * then-arm is dead (reload's post-pass deletes the repeated store), but the
 * register copy reload makes for it stays and fills the j == 8 branch's
 * delay slot as in the retail code. */
void ModelVariant433_DrawRings(u8 *ctx)
{
    PSXLONG p;
    SVECTOR rot;
    u8 *words;
    u8 *work;
    GsOT *ot;
    VECTOR scale;
    PSXLONG flag;
    MATRIX m;
    MATRIX ls;
    s32 ang1;
    s16 i, j;
    s32 ang2;
    GsCOORDINATE2 coord;
    POLY_FT4 *quad;
    Variant433Ring *ring;
    s32 dx, limit;
    s32 yaw, pitch;
    s16 reach;
    SVECTOR *pt;
    s32 dy;
    s32 size;
    s32 row;
    SVECTOR *edge;
    u8 *halves;
    s32 wave;
    PSXLONG flags[16][9];
    s32 level;
    s32 radius;
    s32 half_radius;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x4298), MODEL_VARIANT_WORD(work, 0x4290)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x4294), MODEL_VARIANT_WORD(work, 0x4290)) + 0xC00;
    if (!(MODEL_VARIANT_WORD(work, 0x42A8) & 1)) {
        size = MODEL_VARIANT_HALF(work, 0x42CE);
        if (size < 0) {
            size += 0x7F;
        }
        size = (u32)size >> 7;
    } else {
        size = MODEL_VARIANT_HALF(work, 0x42CE) * 12;
        if (size < 0) {
            size += 0x3FF;
        }
        size = (u32)size >> 10;
    }
    ang1 = 0;
    i = 0;
    ang2 = 0;
    reach = (s16)size;
    limit = 0x400;
    ring = (Variant433Ring *)(work + 0x1518);
    for (; i < 16; i++, ring++, ang1 += 0x708, ang2 += 0x514) {
        wave = MODEL_VARIANT_WORD(work, 0x42E4);
        for (j = 0; j < 9; j++, wave += 0x514) {
            level = ring->amount[j];
            radius = 0x400;
            if (level > 0) {
                radius = 0;
                if (level < 0x400) {
                    radius = limit - level;
                }
            }
            ((SVECTOR *)(j * 8 + (s32)ring))->vx = (rcos(ang1) * (half_radius = radius / 2) >> 12) + (rcos(pitch + yaw + ang1) * (rsin(wave) << 5 >> 12) >> 12);
            ((SVECTOR *)(j * 8 + (s32)ring))->vy = (rsin(ang1) * half_radius >> 12) + (rsin(pitch + ang1) * (rsin(wave) << 5 >> 12) >> 12);
            ((SVECTOR *)(j * 8 + (s32)ring))->vz = (rsin(ang2) * half_radius >> 12) + (rsin(yaw + ang2) * (rsin(wave) << 5 >> 12) >> 12);
            pt = (SVECTOR *)((u8 *)ring + j * 8);
            edge = (SVECTOR *)((u8 *)pt + 0x90);
            edge->vx = pt->vx + (rcos(yaw) * reach >> 12);
            edge->vy = pt->vy;
            edge->vz = pt->vz + (rsin(yaw) * reach >> 12);
            level = ring->amount[j];
            if (level <= 0x400) {
                ring->amount[j] += MODEL_VARIANT_WORD(work, 0x42B4) << 7;
                if (ring->amount[j] >= 0x400) {
                    ring->amount[j] = limit;
                    if ((u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x42BC), 0x20) < (u32)MODEL_VARIANT_WORD(work, 0x42AC)) {
                        ring->amount[j] = limit;
                        if (j == 8) {
                            *(s32 *)((u8 *)ring + 0x204) = 1;
                        }
                    } else if (j == 8) {
                        for (j = 0; j < 9; j++) {
                            ring->amount[j] = -(j << 7);
                        }
                    }
                }
            }
        }
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x4268);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x426C);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x4270);
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
    ring = (Variant433Ring *)(work + 0x1518);
    for (i = 0; i < 16; i++, ring++) {
        for (j = 0; j < 9; j++) {
#ifdef VERSION_FRENCH
            /* The French header-450 rings index the screen, offset and depth
             * arrays directly, use the loop index in the j == 8 arm, and
             * have no folded `+ 1` displacements. */
            if (j == 8) {
                ring->depth[j] = RotTransPers4(&ring->p[7], &ring->p[8], &ring->p[7], &ring->p[8],
                                               &ring->s[7].packed, &ring->s[8].packed, &ring->s[7].packed, &ring->s[8].packed,
                                               &p, &flags[i][8]);
                RotTransPers(&ring->e[8], &ring->es[8].packed, &p, &flag);
                dx = ring->s[j].point.vx - ring->s[7].point.vx;
                dy = ring->s[j].point.vy - ring->s[7].point.vy;
                ring->angle[j] = ratan2(dy, dx) - 0x400;
                ring->width[j] = ring->es[j].point.vx - ring->s[j].point.vx;
                ring->xo[8] = rcos(ring->angle[j]) * ring->width[j] >> 12;
                ring->yo[8] = rsin(ring->angle[j]) * ring->width[j] >> 12;
            } else {
                ring->depth[j] = RotTransPers4(&ring->p[j], &ring->p[j + 1], &ring->p[j], &ring->p[j + 1],
                                               &ring->s[j].packed, &ring->s[j + 1].packed, &ring->s[j].packed, &ring->s[j + 1].packed,
                                               &p, &flags[i][j]);
                RotTransPers(&ring->e[j], &ring->es[j].packed, &p, &flag);
                dx = ring->s[j + 1].point.vx - ring->s[j].point.vx;
                dy = ring->s[j + 1].point.vy - ring->s[j].point.vy;
#else
            if (j == 8) {
                row = 0;
                ring->depth[8] = RotTransPers4(&ring->p[7], &ring->p[8], &ring->p[7], &ring->p[8],
                                               &ring->s[7].packed, &ring->s[8].packed, &ring->s[7].packed, &ring->s[8].packed,
                                               &p, &flags[i][8]);
                RotTransPers(&ring->e[8], &ring->es[8].packed, &p, &flag);
                dx = ring->s[8].point.vx - ring->s[7].point.vx;
                dy = ring->s[8].point.vy - ring->s[7].point.vy;
                ring->angle[8] = ratan2(dy, dx) - 0x400;
                ring->width[8] = ring->es[8].point.vx - ring->s[8].point.vx;
                ring->xo[8] = rcos(ring->angle[8]) * ring->width[8] >> 12;
                ring->yo[8] = rsin(ring->angle[8]) * ring->width[8] >> 12;
            } else {
                ring->depth[j] = RotTransPers4(&ring->p[j], &ring->p[j + 1], &ring->p[j], &ring->p[j + 1],
                                               &ring->s[j].packed, &ring->s[j + 1].packed, &ring->s[j].packed, &ring->s[j + 1].packed,
                                               &p, &(&flags[0][(row = i * 9)])[j]);
                RotTransPers(&ring->e[j], &ring->es[j].packed, &p, &flag);
                dx = (&ring->s[j])[1].point.vx - ring->s[j].point.vx;
                dy = (&ring->s[j])[1].point.vy - ring->s[j].point.vy;
#endif
                ring->angle[j] = ratan2(dy, dx) - 0x400;
                ring->width[j] = ring->es[j].point.vx - ring->s[j].point.vx;
                ring->xo[j] = rcos(ring->angle[j]) * ring->width[j] >> 12;
                ring->yo[j] = rsin(ring->angle[j]) * ring->width[j] >> 12;
            }
        }
    }
    ring = (Variant433Ring *)(work + 0x1518);
    for (i = 0; i < 16; i++, ring++) {
        for (j = 0; j < 8; j++) {
            if ((s16)(j % 2) == (MODEL_VARIANT_WORD(work, 0x42A8) & 1)) {
                quad = (POLY_FT4 *)(work + 0x41E0);
            } else {
                quad = (POLY_FT4 *)(work + 0x4208);
            }
#ifdef VERSION_FRENCH
            quad->x0 = ring->s[j].point.vx + ring->xo[j];
            quad->y0 = (ring->s[j].packed >> 16) + ring->yo[j];
            quad->x1 = ring->s[j + 1].point.vx + ring->xo[j + 1];
            quad->y1 = (ring->s[j + 1].packed >> 16) + ring->yo[j + 1];
            quad->x2 = ring->s[j].point.vx - ring->xo[j];
            quad->y2 = (ring->s[j].packed >> 16) - ring->yo[j];
            quad->x3 = ring->s[j + 1].point.vx - ring->xo[j + 1];
            quad->y3 = (ring->s[j + 1].packed >> 16) - ring->yo[j + 1];
            quad->r0 = ring->color[0];
            quad->g0 = ring->color[1];
            quad->b0 = ring->color[2];
            if (ring->depth[j] >= 0 && flags[i][j] >= 0 && ring->amount[j] > 0 && *(s32 *)((u8 *)ring + 0x204) == 0) {
                GsSortPoly(quad, ot, (u16)ring->depth[j]);
#else
            words = (u8 *)(j * 4 + (s32)ring);
            halves = (u8 *)(j * 2 + (s32)ring);
            quad->x0 = *(s16 *)(words + 0x48) + *(s16 *)(halves + 0x1BC);
            quad->y0 = *(s16 *)(halves + 0x1CE) + (*(s32 *)(words + 0x48) >> 16);
            quad->x1 = *(s16 *)(words + 0x4C) + *(s16 *)(halves + 0x1BE);
            quad->y1 = *(s16 *)(halves + 0x1D0) + (*(s32 *)(words + 0x4C) >> 16);
            quad->x2 = *(s16 *)(words + 0x48) - *(s16 *)(halves + 0x1BC);
            quad->y2 = (*(s32 *)(words + 0x48) >> 16) - *(s16 *)(halves + 0x1CE);
            quad->x3 = *(s16 *)(words + 0x4C) - *(s16 *)(halves + 0x1BE);
            quad->y3 = (*(s32 *)(words + 0x4C) >> 16) - *(s16 *)(halves + 0x1D0);
            quad->r0 = ring->color[0];
            quad->g0 = ring->color[1];
            quad->b0 = ring->color[2];
            if (*(s32 *)(words + 0x198) >= 0 && flags[i][j] >= 0 && *(s32 *)(words + 0x1E0) > 0 && *(s32 *)((u8 *)ring + 0x204) == 0) {
                GsSortPoly(quad, ot, (u16)*(s32 *)(words + 0x198));
#endif
            }
        }
    }
}
