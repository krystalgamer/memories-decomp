#include "../../types.h"

#include "variant458_strand.h"

/* Header 552's leftover copy of the header-458 strands (variant458_strand.c)
 * over the layout it was compiled against (Variant552StrandWork): it reads
 * its origin as words, advances the span by one and sorts at a quarter
 * depth. */
/* Builds six strands of thirteen points fanned around the variant origin and draws
 * the visible span of each as GsLINE segments, then advances
 * the span. */
void ModelVariant458_DrawStrand(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant552StrandWork *work;
    GsLINE *line;
    ModelVariantStrandWide *strand;
    s32 i;
    s32 j;
    s32 a;
    s32 b;
    s32 r;
    SVECTOR *v;
    s32 otz;

    work = (Variant552StrandWork *)ctx;
    strand = work->strands;
    line = &work->line;
    if (work->phase >= 2) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->origin[0];
        m.t[1] = work->origin[1];
        m.t[2] = work->origin[2];
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
            for (j = work->span_first; j < work->span_last; j++) {
                line->attribute = 0x50000000;
                otz = Strand458_RotTransPers4(&strand->point[j], &strand->point[j + 1], &strand->point[j],
                                    &strand->point[j + 1], (long *)&line->x0, (long *)&line->x1,
                                    (long *)&line->x0, (long *)&line->x1, &p, &flag);
                line->g = 0x40;
                line->r = 0;
                line->b = 0x80;
                if (otz > 0) {
                    if (otz < 0x800) {
                        Strand458_GsSortLine(line, Strand458_func_80058F10(), (otz >> 2));
                    }
                }
            }
        }
        if (work->span_last < 13) {
            work->span_last += 1;
            if (work->span_last >= 12) {
                work->span_last = 12;
                if (work->span_first < 13) {
                    work->span_first += 1;
                    if (work->span_first >= 12) {
                        work->span_last = 0;
                        work->span_first = 0;
                    }
                }
            }
        }
    }
}
