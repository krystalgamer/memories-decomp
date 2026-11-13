#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant336_entry.h"

void FrenchModelVariant336_DrawStrand(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant336EntryState *work;
    GsLINE *line;
    ModelVariantStrandWide *strand;
    s32 i;
    s32 j;
    s32 a;
    s32 b;
    s32 r;

    work = (Variant336EntryState *)ctx;
    strand = (ModelVariantStrandWide *)work->unknown_1AB0;
    line = (GsLINE *)work->unknown_1F10;
    if (work->phase >= 2) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->aim.target.vx;
        m.t[1] = work->aim.target.vy;
        m.t[2] = work->aim.target.vz;
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
        i = 0;
        a = i;
        do {
            for (j = 0, b = 0; j < 13; j++, b += 0x708) {
                r = (j << 10) / 12;
                strand->point[j].vx = rcos(a) * r >> 12;
                strand->point[j].vy = (u32)rsin(b) >> 9;
                strand->point[j].vz = rsin(a) * r >> 12;
            }
            i++;
            a = (i << 12) / 6;
            strand++;
        } while (i < 6);
        strand = (ModelVariantStrandWide *)work->unknown_1AB0;
        for (i = 0; i < 6; i++, strand++) {
            for (j = work->segment_start;
                 j < work->segment_end; j++) {
                line->attribute = 0x50000000;
                RotTransPers4(&strand->point[j], &strand->point[j + 1],
                              &strand->point[j], &strand->point[j + 1],
                              (long *)&line->x0, (long *)&line->x1,
                              (long *)&line->x0, (long *)&line->x1,
                              &p, &flag);
                line->g = 0x40;
                line->r = 0;
                line->b = 0x80;
            }
        }
        if (work->segment_end < 13) {
            work->segment_end += (u32)work->clock.step >> 1;
            if (work->segment_end >= 12) {
                work->segment_end = 12;
                if (work->segment_start < 13) {
                    work->segment_start += (u32)work->clock.step >> 1;
                    if (work->segment_start >= 12) {
                        work->segment_end = 0;
                        work->segment_start = 0;
                    }
                }
            }
        }
    }
}
