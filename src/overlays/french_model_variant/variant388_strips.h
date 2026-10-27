#ifndef FRENCH_MODEL_VARIANT388_STRIPS_H
#define FRENCH_MODEL_VARIANT388_STRIPS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* The 0xF8-byte strip unit at the start of the work area: five cross
 * sections of three points each, their packed screen positions, and the
 * projection flags and depths. */
typedef struct {
    SVECTOR a[5];
    SVECTOR b[5];
    SVECTOR c[5];
    PSXLONG sa[5];
    PSXLONG sb[5];
    PSXLONG sc[5];
    u8 unknown_B4[0x1C];
    PSXLONG flag[5];
    PSXLONG otz[5];
} Strip388Unit;

/* The timing record addressed by the word at +0x186C. */
typedef struct {
    u32 unknown_00[3];
    u16 frame;
    u16 unknown_0E;
    u32 unknown_10;
    u32 grow_start;
    u32 grow_end;
    u32 shrink_start;
    u32 shrink_end;
} Strip388Timing;

#endif
