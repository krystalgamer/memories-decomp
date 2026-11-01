#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT397_BEAMS_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT397_BEAMS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

/* Command row view: the record count and three release frames. */
typedef struct {
    u8 unknown_00[0x24];
    s32 count;
    u8 unknown_28[0xC];
    u32 grow_end;
    u32 hide_at;
    u32 release_at;
} Variant397BeamsConfig;

/* One 0x2D4-byte beam: a thirteen-point spine and its view-shifted copy,
 * both projected, with per-point angle, width, projection flag, depth and
 * screen offset, and the beam colour. */
typedef struct {
    SVECTOR a[13];
    PSXLONG sa[13];
    s32 angle[13];
    SVECTOR b[13];
    PSXLONG sb[13];
    s32 width[13];
    u8 color[4];
    u8 unknown_1A4[0x238 - 0x1A4];
    PSXLONG flag[13];
    s32 otz[13];
    s16 ox[13];
    s16 oy[13];
} Variant397Beam;

/* Minimum observed context extent, not an allocation capacity; the layout
 * matches the North American header-380 entry's view. */
typedef struct {
    Variant397Beam beams[5];
    ModelVariantSheetSet sheets[5];
    u8 unknown_1130[0x18C8 - 0x1130];
    POLY_FT4 flat[6];
    u8 unknown_19B8[0x10];
    MATRIX matrices[5];
    SVECTOR target;
    VECTOR targets[5];
    VECTOR directions[5];
    s16 screen_dx[5], screen_dy[5];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, span, fade;
    Variant397BeamsConfig *G32 config;
    u32 field_1B58;
    void *G32 parts[5];
    s16 length[5], field_1B7A[5], mode[5], done[5];
    s16 field_1B98, field_1B9A, width;
    s32 field_1BA0[5], field_1BB4[5], field_1BC8[5], field_1BDC[5];
    s32 wave, bend, phase;
} Variant397BeamsState;

void FrenchModelVariant397_DrawBeams(u8 *ctx);
#endif
