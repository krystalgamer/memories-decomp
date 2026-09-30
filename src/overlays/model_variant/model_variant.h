#ifndef MEMORIES_DECOMP_MODEL_VARIANT_H
#define MEMORIES_DECOMP_MODEL_VARIANT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

/* The MODEL.MRG variant modules (stages 7-10 of Model_LoadMonsterMerge) were
 * compiled with the GCC 2.7.2 CDK compiler (profile gcc_2_7_2_cdk_g0). Their
 * helpers receive the module's work area as a byte pointer and reach its
 * fields at fixed offsets; the record arrays inside it are typed below. */
#define MODEL_VARIANT_WORD(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define MODEL_VARIANT_HALF(p, o) (*(s16 *)((u8 *)(p) + (o)))

/* One 0x90-byte element of the quad array at work + 0x1B4C: a four-corner
 * quad whose two colours and growing size drive a Gouraud-shaded POLY_G4. */
typedef struct {
    u8 pad0[0x58];
    SVECTOR corner[4];
    u8 inner[4];
    u8 outer[4];
    s32 size;
    u8 pad84[0x0C];
} ModelVariantQuad;

/* One 0x90-byte element of the ring array at work + 0x15AC: eight spokes,
 * each drawn as a GsGLINE from inner[k] to outer[k]. */
typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 color[4];
    u8 pad84[4];
    s32 angle;
    s32 count;
} ModelVariantRing;

/* One 0x90-byte element of the spoke-ring array in header 397's work area:
 * like ModelVariantRing, but the colour sits one word earlier and the ring
 * is scaled rather than swept. */
typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 pad80[4];
    u8 color[4];
    s32 angle;
    s32 count;
} ModelVariantSpokeRing;

/* One 0x98-byte element of header 397's sheet array: four POLY_GT4 quads
 * given by corner arrays v0..v3, an inner colour on three corners and an
 * outer colour on the fourth, and a scale. */
typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
    s32 size;
    u8 pad8C[0x0C];
} ModelVariantSheet;

/* One 0x1C8-byte band of the bands helper: three rows of nine points, the
 * screen coordinates RotTransPers3 writes for them, two colour rows and the
 * nine depths. */
typedef struct {
    SVECTOR a[9];
    SVECTOR b[9];
    SVECTOR c[9];
    PSXLONG sa[9];
    PSXLONG sb[9];
    PSXLONG sc[9];
    u8 ca[9][4];
    u8 cb[9][4];
    u8 pad18C[0x18];
    s32 otz[9];
} ModelVariantBand;

/* Header 418's band record: the same fields in a 0x1EC-byte record. */
typedef struct {
    SVECTOR a[9];
    SVECTOR b[9];
    SVECTOR c[9];
    PSXLONG sa[9];
    PSXLONG sb[9];
    PSXLONG sc[9];
    u8 ca[9][4];
    u8 cb[9][4];
    u8 pad18C[0x18];
    s32 otz[9];
    u8 pad1C8[0x24];
} ModelVariantBandPadded;

/* One 0x74-byte ribbon of header 418: a two-point spine, a copy of it moved
 * sideways, both projected, and per point the screen angle, the projected
 * width and the width's screen offset. */
typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 unknown_40[0x24];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
} ModelVariantRibbon;

/* Header 425's 0x6C-byte ribbon: the same fields with eight bytes less
 * between the widths and the depths. */
typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 unknown_40[0x1C];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
} ModelVariantRibbonShort;

/* One 0x7C-byte strand of header 443: thirteen points fanned from the origin. */
typedef struct {
    SVECTOR point[13];
    u8 pad68[0x14];
} ModelVariantStrand;

/* One 0x260-byte web of header 418: two 6x6 point grids joined line by line,
 * a colour and a scale. */
typedef struct {
    SVECTOR near[6][6];
    SVECTOR far[6][6];
    CVECTOR color;
    u8 unknown_244[0x10];
    s32 scale;
    u8 unknown_258[8];
} ModelVariantWeb;

/* Header 425's 0x200-byte web: 5x6 near and far grids. */
typedef struct {
    SVECTOR near[5][6];
    SVECTOR far[5][6];
    CVECTOR color;
    u8 unknown_1E4[0x10];
    s32 scale;
    u8 unknown_1F8[8];
} ModelVariantWebSmall;

/* Header 397's 0x1A0-byte web: 4x6 near and far grids. */
typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    u8 unknown_184[0x10];
    s32 scale;
    s32 done;
    u8 unknown_19C[4];
} ModelVariantWebNarrow;

/* The same strand in a 0x84-byte record, as the header-458 images lay it out. */
typedef struct {
    SVECTOR point[13];
    u8 pad68[0x1C];
} ModelVariantStrandWide;

/* One 0x9C-byte sheet of header 443's sheet set: ModelVariantSheet plus a
 * shown flag. */
typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
    s32 size;
    u8 pad8C[0x0C];
    s32 shown;
} ModelVariantSheetSet;

/* One 0x118-byte curtain of header 422: two rows of seventeen points drawn
 * as sixteen POLY_GT4 strips, a scale and a wrap count. */
typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    s32 scale;
    s32 count;
} ModelVariantCurtain;

#endif
