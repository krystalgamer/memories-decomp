#include "../../types.h"
#include "variant459_entry.h"

/* Header 459's form of the variant curtains: each frame rebuilds the two rows of
 * every timed curtain around the rotating angle at work + 0x30C8, turned to
 * face the variant's direction and placed at its position in work + 0x2F8C.
 * Curtain 3 and curtains below half size keep their colours, and the rest
 * fade with size. Before phase 3 each scale grows with the clock at
 * work + 0x3084 toward the timing record's limit, which counts a cycle;
 * otherwise it shrinks back between the record's fade start and fade end. */
void SpanishModelVariant459_DrawCurtains(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant459EntryState *work;
    ModelVariantCurtain *curtain;
    GsOT *ot;
    s32 i;
    s32 lift;
    s32 yaw;
    s32 pitch;
    s32 radius;
    POLY_GT4 *poly;
    s32 j;
    s32 otz;
    s32 size;
    s32 angle;
    ModelVariantSheet *sheet;
    s32 fade;
    Variant459EntryConfig *rec;
    s32 start;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 r1;
    u8 g1;
    u8 b1;

    work = (Variant459EntryState *)ctx;
    curtain = work->rings;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->aim.direction.vz, work->aim.direction.vx) + 0x800;
    sheet = work->sheets;
    poly = &work->ray;
    pitch = ratan2(work->aim.direction.vy, work->aim.direction.vz) + 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    lift = (((s32)work->clock.frame_count & 1) << 5) * sheet->size / 4096;
    for (i = 0; i < work->config->count; i++, curtain++) {
        radius = lift + 0x80;
        if (curtain->scale > 0) {
            size = curtain->scale;
            for (j = 0, angle = work->curtain_angle; j < 17;
                 j++, angle = work->curtain_angle + j * 256) {
                setVector(&curtain->a[j], (u32)rcos(angle) >> 6, (u32)rsin(angle) >> 6, 0);
                setVector(&curtain->b[j], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, -(lift + 0x40));
            }
            rot.vx = -pitch;
            rot.vy = yaw;
            rot.vz = 0;
            m.t[0] = work->points[i].vx;
            m.t[1] = work->points[i].vy;
            m.t[2] = work->points[i].vz;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            if (i == 3 || curtain->scale < 0x800) {
                /* The colours load as one do-while block, which keeps the
                 * target's register allocation. */
                do {
                    r0 = work->curtain_inner_color.r;
                    g0 = work->curtain_inner_color.g;
                    b0 = work->curtain_inner_color.b;
                    r1 = work->curtain_outer_color.r;
                    g1 = work->curtain_outer_color.g;
                    b1 = work->curtain_outer_color.b;
                } while (0);
            } else {
                fade = (s16)(0x1000 - curtain->scale);
                r0 = work->curtain_inner_color.r * fade / 2048;
                g0 = work->curtain_inner_color.g * fade / 2048;
                b0 = work->curtain_inner_color.b * fade / 2048;
                r1 = work->curtain_outer_color.r * fade / 2048;
                g1 = work->curtain_outer_color.g * fade / 2048;
                b1 = work->curtain_outer_color.b * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                otz = RotTransPers4(&curtain->a[j], &curtain->a[j + 1], &curtain->b[j], &curtain->b[j + 1],
                                    (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                                    (long *)&poly->x3, &p, &flag);
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r1;
                poly->g2 = g1;
                poly->b2 = b1;
                poly->r3 = r1;
                poly->g3 = g1;
                poly->b3 = b1;
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, (u16)otz);
                }
            }
        }
        if (curtain->scale < work->config->limit && work->phase < 3) {
            curtain->scale = work->config->limit * work->clock.frame / work->config->end;
            if (curtain->scale >= work->config->limit) {
                curtain->scale = work->config->limit;
                curtain->count++;
            }
        } else if (work->clock.frame >= (u32)(start = (rec = work->config)->fade_start)) {
            curtain->scale = rec->limit - rec->limit * (work->clock.frame - start) / (rec->fade_end - start);
        }
    }
    work->curtain_angle += work->clock.step * 80;
}
