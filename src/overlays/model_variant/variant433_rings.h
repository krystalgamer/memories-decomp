#ifndef MODEL_VARIANT433_RINGS_H
#define MODEL_VARIANT433_RINGS_H
#include "../../types.h"
#include "model_variant.h"

/* Header 433's 0x208-byte ring record: nine points, their screen positions
 * and edge copies, per-point widths, depths, offsets and growth amounts. */
typedef union {
    PSXLONG packed;
    DVECTOR point;
} Variant433RingScreen;

typedef struct {
    SVECTOR p[9];
    Variant433RingScreen s[9];
    s32 angle[9];
    SVECTOR e[9];
    Variant433RingScreen es[9];
    s32 width[9];
    u8 color[4];
    u8 pad120[0x4];
    u8 pad128[0x70];
    s32 depth[9];
    s16 xo[9];
    s16 yo[9];
    s32 amount[9];
    u8 pad204[0x4];
} Variant433Ring;

void ModelVariant433_DrawRings(u8 *context);
#endif
