#ifndef MEMORIES_DECOMP_MODEL_VARIANT458_STRAND_H
#define MEMORIES_DECOMP_MODEL_VARIANT458_STRAND_H

#include "../../types.h"
#include "model_variant.h"

/* The strand helper's resident calls. Each image that builds
 * variant458_strand.c binds these names in its symbols file: at the current
 * resident addresses in the header-458 images and at the earlier layout's
 * addresses in the header-552 images, whose leftover copy was linked against
 * that layout. */
MATRIX *Strand458_RotMatrix(SVECTOR *r, MATRIX *m);
MATRIX *Strand458_ScaleMatrix(MATRIX *m, VECTOR *v);
void Strand458_GsGetLs(GsCOORDINATE2 *m, MATRIX *out);
void Strand458_GsSetLsMatrix(MATRIX *mp);
int Strand458_rcos(int a);
int Strand458_rsin(int a);
long Strand458_RotTransPers4(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3,
                             long *sxy0, long *sxy1, long *sxy2, long *sxy3,
                             long *p, long *flag);
void Strand458_GsSortLine(GsLINE *lp, GsOT *ot, unsigned short pri);
GsOT *Strand458_func_80058F10(void);

/* The work layout the header-552 leftover copy (variant552_strand.c) was
 * compiled against: six strands, the line packet, the origin words, the
 * visible span and the phase. The header-402 images that keep that copy
 * have a different entry state; no image's state has this layout. */
typedef struct {
    u8 unknown_000[0x480];
    ModelVariantStrandWide strands[6];
    u8 unknown_798[0xF8];
    GsLINE line;
    u8 unknown_8A0[0x50];
    s32 origin[3];
    u8 unknown_8FC[0x52];
    s16 span_first, span_last;
    u8 unknown_952[0xA];
    s32 phase;
} Variant552StrandWork;

#endif
