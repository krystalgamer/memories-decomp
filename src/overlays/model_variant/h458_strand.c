#include "../../types.h"

#include "h458_strand.h"
#include "../model_variant/h458_entry_pal.h"

/* Builds six strands of thirteen points fanned around the variant origin and draws
 * the visible span of each as GsLINE segments, then advances
 * the span. */
void ModelVariant458_DrawStrand(Variant475EntryState *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant475EntryState *work;
    GsLINE *line;
    ModelVariantStrandWide *strand;
    s32 i;
    s32 j;
    s32 a;
    s32 b;
    s32 r;
    SVECTOR *v;
    s32 otz;

    work = ctx;
    strand = work->strands;
    line = &work->line;
    if (work->tail.phase >= 2) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->target.vx;
        m.t[1] = work->target.vy;
        m.t[2] = work->target.vz;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        Strand458_RotMatrix(&rot, &m);
        Strand458_ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        Strand458_GsGetLs(&coord, &ls);
        Strand458_GsSetLsMatrix(&ls);
        i = 0;
        a = i;
        do {
            for (j = 0, b = 0; j < 13; j++, b += 0x708) {
                r = (j << 10) / 12;
                strand->point[j].vx = Strand458_rcos(a) * r >> 12;
                strand->point[j].vy = (u32)Strand458_rsin(b) >> 9;
                strand->point[j].vz = Strand458_rsin(a) * r >> 12;
            }
            i++;
            a = (i << 12) / 6;
            strand++;
        } while (i < 6);
        strand = work->strands;
        for (i = 0; i < 6; i++, strand++) {
            for (j = work->strand_start; j < work->strand_end; j++) {
                line->attribute = 0x50000000;
                otz = Strand458_RotTransPers4(&strand->point[j], &strand->point[j + 1], &strand->point[j],
                                    &strand->point[j + 1], (long *)&line->x0, (long *)&line->x1,
                                    (long *)&line->x0, (long *)&line->x1, &p, &flag);
                line->g = 0x40;
                line->r = 0;
                line->b = 0x80;
                if (otz > 0) {
                    if (otz < 0x800) {
                        Strand458_GsSortLine(line, Strand458_func_80058F10(), otz);
                    }
                }
            }
        }
        if (work->strand_end < 13) {
            work->strand_end += ((u32)work->clock.step >> 1);
            if (work->strand_end >= 12) {
                work->strand_end = 12;
                if (work->strand_start < 13) {
                    work->strand_start += ((u32)work->clock.step >> 1);
                    if (work->strand_start >= 12) {
                        work->strand_end = 0;
                        work->strand_start = 0;
                    }
                }
            }
        }
    }
}
