#ifndef MEMORIES_DECOMP_PSYQ_LIBGTE_ABI_VARIANTS_H
#define MEMORIES_DECOMP_PSYQ_LIBGTE_ABI_VARIANTS_H

#include "libgte.h"

extern long NormalClip_800879A0(void *primitive) asm("NormalClip");
extern long RotAverageNclip3_nom_80089CF0(
    SVECTOR *v0,
    SVECTOR *v1,
    SVECTOR *v2,
    SVECTOR *v3
) asm("RotAverageNclip3_nom");

#endif
