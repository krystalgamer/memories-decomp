#include "../../types.h"

#include "variant407_entry.h"

/* The two builds spell the per-petal reads differently: the French build
 * indexes the arrays, the North American one adds the index before the
 * array base, which reverses the address addu's operands. */
#if defined(VERSION_FRENCH)
#define PETAL_SCALE(w, i) ((w)->petals.scale[i])
#define PETAL_DONE(w, i) ((w)->petals.field_854[i])
#else
#define PETAL_SCALE(w, i) (*((i) + (w)->petals.scale))
#define PETAL_DONE(w, i) (*((i) + (w)->petals.field_854))
#endif

/* Draws 48 petals: each a POLY_G4 grown from the variant centre along its own
 * velocity and swung around a circle, faded past half size, and cycles each
 * petal's size through the timing record's phases. */
void ModelVariant407_DrawPetals(u8 *ctx)
{
    SVECTOR rot;
    /* The target reserves 16 unused bytes between rot and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant407EntryState *work;
    /* A second view of the state; folding it into work changes the
     * register allocation. */
    Variant407EntryState *base;
    GsOT *ot;
    s16 i;
    s16 sweep;
    s32 done;
    u8 r;
    u8 g;
    u8 b;
    MATRIX *lp;
    POLY_G4 *poly;
    Variant407EntryConfig *conf;
    s16 phase;
    s32 size;
    s32 fade;
    s16 radius;
    s16 lift;
    s32 ang;
    s32 otz;

    work = (Variant407EntryState *)ctx;
    base = (Variant407EntryState *)ctx;
    poly = &base->sprite.g4;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
#if !defined(VERSION_FRENCH)
    lp = &ls;
#endif
    i = 0;
    sweep = 0;
    phase = 0;
#if defined(VERSION_FRENCH)
    lp = &ls;
#endif
    ratan2(base->direction.vy, base->direction.vz);
    ratan2(base->screen_delta.vy, base->screen_delta.vx);
    do {
        size = PETAL_SCALE(work, i);
        if (size > 0) {
            if (size > 0x200) {
                fade = 0x400 - size;
                r = work->petals.color.r * fade / 512;
                g = work->petals.color.g * fade / 512;
                b = work->petals.color.b * fade / 512;
            } else {
                r = work->petals.color.r;
                g = work->petals.color.g;
                b = work->petals.color.b;
            }
            radius = base->config->petal_radius -
                     (rcos(PETAL_SCALE(work, i)) * base->config->petal_radius >> 12);
            lift = (rsin(PETAL_SCALE(work, i)) * 0x1000 >> 12) + 0x180;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            fade = 0x400;
            ang = phase + (sweep - fade);
            m.t[0] = base->matrix.t[0] + base->offsets[i].vx * PETAL_SCALE(work, i) / 512 + (rcos(ang) * radius >> 12);
            m.t[1] = base->matrix.t[1] + base->offsets[i].vy * PETAL_SCALE(work, i) / 512 + (rsin(ang) * radius >> 12);
            m.t[2] = base->matrix.t[2] + base->offsets[i].vz * PETAL_SCALE(work, i) / 512;
            scale.vx = lift;
            scale.vy = lift;
            scale.vz = lift;
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, lp);
            GsSetLsMatrix(lp);
            ReadRotMatrix(lp);
            RotMatrix(&rot, lp);
            ScaleMatrix(lp, &scale);
            SetRotMatrix(lp);
            otz = RotTransPers4(&work->petals.v0[i], &work->petals.v1[i],
                                &work->petals.v2[i], &work->petals.v3[i],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            poly->r0 = r;
            poly->g0 = g;
            poly->b0 = b;
            if (otz >= 0 && flag >= 0 && PETAL_DONE(work, i) == 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (PETAL_SCALE(work, i) < 0x400) {
            conf = base->config;
            PETAL_SCALE(work, i) += (u32)(conf->field_20 * base->clock.step) >> 1;
            if (PETAL_SCALE(work, 0) >= 0x200 && base->tail.phase == 0) {
                base->tail.phase = 2;
            }
            if (PETAL_SCALE(work, i) >= 0x400) {
                if (base->config->field_28 > base->clock.frame) {
                    PETAL_SCALE(work, i) -= 0x400;
                    work->petals.done[i] = 0;
                } else {
                    PETAL_SCALE(work, i) = 0x400;
                    PETAL_DONE(work, i) = 1;
                }
            } else if (PETAL_SCALE(work, i) <= 0 &&
                       base->config->field_28 <= base->clock.frame) {
                PETAL_SCALE(work, i) = 0x400;
                PETAL_DONE(work, i) = 1;
            }
        }
        done *= PETAL_DONE(work, i);
        if (i + 1 == 48 && done == 1 && base->tail.phase == 2) {
            base->tail.phase = 5;
        }
        i++;
        sweep += 0x40;
        phase = (s16)(i % 5) * 0x1000 / 5;
    } while (i < 48);
}
