#ifndef MEMORIES_DECOMP_MODEL_VARIANT_H
#define MEMORIES_DECOMP_MODEL_VARIANT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

/* The MODEL.MRG variant modules (stages 7-10 of Model_LoadMonsterMerge) were
 * compiled with the GCC 2.7.2 CDK compiler (profile gcc_2_7_2_cdk_g0). Their
 * helpers receive the module's work area as a byte pointer and reach its
 * fields at fixed offsets; the record arrays inside it are typed below. */
#define MODEL_VARIANT_WORD(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define MODEL_VARIANT_HALF(p, o) (*(s16 *)((u8 *)(p) + (o)))

/* The command row the quad, quad-pair, swarm and bloom helpers reach through
 * their timing pointer: the frame limit at + 0x20 is compared with the
 * elapsed time before a finished quad is reset. */
typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} ModelVariantQuadTiming;

/* Like ModelVariantQuadTiming, but the frame limit sits one word later, at
 * + 0x24. */
typedef struct {
    u8 unknown_00[0x24];
    u32 limit;
} ModelVariantQuadTimingLong;

/* The rays command row: the pass count at + 0x18; the helper finishes when
 * its index reaches it. */
typedef struct {
    u8 unknown_00[0x18];
    u16 count;
} ModelVariantRayTiming;

/* The swarm command row with the group count, the growth rate and the frame
 * limit a finished quad waits for. */
typedef struct {
    u8 unknown_00[0x1C];
    s32 count;
    u8 unknown_20[4];
    u32 rate;
    u8 unknown_28[4];
    u32 limit;
} ModelVariantSwarmRateTiming;

/* One 0x2FC-byte group of ten quads drawn by the quads helpers: the four
 * corner rows, a shared colour, and per quad the size, angle, done flag and
 * reset flag. */
typedef struct {
    SVECTOR a[10];
    SVECTOR b[10];
    SVECTOR c[10];
    SVECTOR d[10];
    u8 unknown_140[0x50];
    u8 color[4];
    s32 size[10];
    s32 angle[10];
    s32 done[10];
    u8 unknown_20C[0x28];
    s32 reset[10];
    u8 unknown_25C[0xA0];
} ModelVariantQuadGroupView;

/* One group of 32 swarm quads: corners, a shared colour, and per quad the
 * growth, done flag and reset flag. */
typedef struct {
    SVECTOR a[32];
    SVECTOR b[32];
    SVECTOR c[32];
    SVECTOR d[32];
    u8 unknown_400[0x100];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32];
    s32 done[32];
    u8 unknown_614[0x80];
    s32 reset[32];
} ModelVariantSwarmGroup;

/* One group of 32 quads: corners, a shared colour, growth and done flags;
 * ModelVariantSwarmGroup without the trailing reset flags. */
typedef struct {
    SVECTOR a[32];
    SVECTOR b[32];
    SVECTOR c[32];
    SVECTOR d[32];
    u8 unknown_400[0x100];
    u8 color[4];
    u8 unknown_504[0x10];
    s32 size[32];
    s32 done[32];
} ModelVariantSwarmGroupShort;

/* A 0x20-byte anchor whose position becomes the translation of a sheet,
 * ribbon or quad-pair matrix. */
typedef struct {
    VECTOR position;
    u8 unknown_10[0x10];
} ModelVariantAnchor;

/* An eight-byte copy of the view metrics (Model_GetViewMetricsBuffer) that
 * the header 630-683 entries and image 60348 take each frame; the entries
 * that use it turn their sprites with the yaw at +2 and the pitch at +4. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} ModelVariantViewSnapshot;

/* The same eight-byte view-metrics copy kept as bytes, as the header
 * 628-676 entries and image 9840 lay it out: the entries that use it read
 * the yaw at +2 and the pitch at +4 through s16 casts. Not interchangeable
 * with ModelVariantViewSnapshot: this form is byte-aligned, which changes
 * how GCC copies it, and each entry matches only with its own form. */
typedef struct {
    u8 bytes[8];
} ModelVariantViewSnapshotBytes;

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

/* One 0x4C-byte effect element of the header 630-682 entries: an active
 * flag, a colour, texture coordinates, a draw mode, its age and lifetime,
 * the coordinate unit it is placed through, a rotation, a scale and a
 * position, then a last vector. `target` is a record-view label, not a
 * fixed role: most entries keep a point the element moves towards there,
 * but e.g. header 630's sparks keep a velocity and header 678's bubbles
 * the arc height in target.vy (see each entry header); headers 645, 646,
 * 658, 659, 663 and 682 only clear it. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} ModelVariantElement;

/* ModelVariantElement with the last vector labelled `spin`, as headers 650
 * and 652 name it; the label is a record view: header 650 keeps the
 * sprites' spin speed in spin.vz, header 652 the dome row count and the
 * spark rise (see each entry header). */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR spin;
} ModelVariantSpinElement;

/* ModelVariantElement with a growth vector in place of the target, as
 * headers 656 and 665 use it. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} ModelVariantGrowthElement;

/* ModelVariantElement with an offset from a centre in place of the
 * target, as headers 657, 660 and 662 use it. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR offset;
} ModelVariantOffsetElement;

/* The 0x4C-byte burst record of header 647, shared by headers 627-673:
 * the ModelVariantElement fields with the colour as separate bytes and a
 * growth vector last (the flight or cone growth; headers 632, 634 and 647
 * only clear it). */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} ModelVariantBurst;

/* The 0x50-byte record of headers 674 and 675: ModelVariantBurst with a
 * tick counter after the lifetime. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u16 ticks, unknown_12;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} ModelVariantTickedBurst;

/* The 0x50-byte element of headers 655, 661 and 667: ModelVariantElement
 * with a done flag and a count after the lifetime: the streaks (655, 661)
 * and ribbons (667) clear both at setup, test done and grow count, which
 * shapes their geometry; header 667's fans also test and set done. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u8 done, count, unknown_12[2];
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_40;
} ModelVariantCountedElement;

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
 * outer colour on the fourth, a scale and two words the entry handlers
 * clear. */
typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
    s32 size;
    s32 field_8C;
    s32 field_90;
    u8 unknown_94[4];
} ModelVariantSheet;

/* A 0x250-byte primary record the sheet helpers read for its active flag and
 * size. */
typedef struct {
    u8 unknown_00[0x188];
    s32 active;
    s32 size;
    u8 unknown_190[0xC0];
} ModelVariantSheetPrimary;

/* A 0x2B0-byte primary record the sheet helpers read for its active flag and
 * size. */
typedef struct {
    u8 unknown_000[0x1C8];
    s32 active;
    s32 size;
    u8 unknown_1D0[0xE0];
} ModelVariantSheetPrimaryWide;

/* ModelVariantSheet with its sixteen corners as one points[16] array and its
 * colours as CVECTORs: outer is the colour of the fourth corner and inner that
 * of the other three, as FrenchModelVariant400_DrawSheets and the header-335
 * and 336 sheet drawers read them. (The header-337, 400 and 410 entries named
 * the two colours the other way round; the header-410 ring drawer's
 * Family410Ring keeps that naming.) */
typedef struct {
    SVECTOR points[16];
    CVECTOR outer, inner;
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
} ModelVariantPointSheet;

/* One 0x1C8-byte band of the bands helper: three rows of nine points, the
 * screen coordinates RotTransPers3 writes for them, two colour rows and the
 * nine depths. */
typedef struct {
    SVECTOR a[9];
    SVECTOR b[9];
    SVECTOR c[9];
    long sa[9];
    long sb[9];
    long sc[9];
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
    long sa[9];
    long sb[9];
    long sc[9];
    u8 ca[9][4];
    u8 cb[9][4];
    u8 pad18C[0x18];
    s32 otz[9];
    u8 pad1C8[0x24];
} ModelVariantBandPadded;

/* Header 397's band record: five points per row in 0x11C bytes, with the
 * flag word RotTransPers3 writes for each point kept after the depths. */
typedef struct {
    SVECTOR a[5];
    SVECTOR b[5];
    SVECTOR c[5];
    long sa[5];
    long sb[5];
    long sc[5];
    u8 ca[5][4];
    u8 cb[5][4];
    u8 padDC[0x18];
    s32 otz[5];
    long flag[5];
} ModelVariantBandShort;

/* The 0x1C8-byte ModelVariantBand seen from the entry handlers: the inner
 * (ModelVariantBand's ca) and outer (cb) colour rows, set from the entry
 * config's inner and outer colours, and the scale vector and words its pad
 * holds. */
typedef struct {
    u8 unknown_00[0x144];
    CVECTOR inner[9], outer[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x24];
} ModelVariantColorBand;

/* ModelVariantColorBand in the 0x1EC-byte ModelVariantBandPadded record. */
typedef struct {
    u8 unknown_00[0x144];
    CVECTOR inner[9], outer[9];
    VECTOR scale;
    s16 field_19C, field_19E;
    s32 field_1A0;
    u8 unknown_1A4[0x48];
} ModelVariantColorBandPadded;

/* One 0x74-byte ribbon of header 418: a two-point spine, a copy of it moved
 * sideways, both projected, and per point the screen angle, the projected
 * width and the width's screen offset. */
typedef struct {
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
    s32 width[2];
    u8 unknown_40[0x24];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
} ModelVariantRibbon;

/* The ribbon command row: the fade window at + 0x24. */
typedef struct {
    u8 unknown00[0x24];
    u32 fade_start;
    u32 fade_end;
} ModelVariantRibbonFadeTiming;

/* Header 425's 0x6C-byte ribbon: the same fields with eight bytes less
 * between the widths and the depths. */
typedef struct {
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
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

/* Header 397's 0x1A0-byte web: 4x6 near and far grids, a colour, three words
 * the entry handlers clear, a scale and a done flag. */
typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    s32 field_184, field_188, field_18C;
    u8 unknown_190[4];
    s32 scale;
    s32 done;
    s32 field_19C;
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
    s32 field_8C;
    s32 field_90;
    u8 unknown_94[4];
    s32 shown;
} ModelVariantSheetSet;

/* The sheet command row: the window in which the sheets grow and the later
 * window in which they shrink again. */
typedef struct {
    u8 unknown_00[0x18];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_20[4];
    u32 shrink_start;
    u32 shrink_end;
} ModelVariantSheetTiming;

/* ModelVariantSheetTiming with the shrink window one word later. */
typedef struct {
    u8 unknown_00[0x18];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_20[8];
    u32 shrink_start;
    u32 shrink_end;
} ModelVariantSheetTimingWide;

/* One 0x118-byte curtain of header 422: two rows of seventeen points drawn
 * as sixteen POLY_GT4 strips, a scale the drawers grow to 0x1000 and a count
 * of the times it wrapped. The entry handlers start the scales staggered at
 * -(n * 4096 / 3) or similar. */
typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    s32 scale;
    s32 count;
} ModelVariantCurtain;

/* The RotTransPers scratch record of the entry handlers: a projected screen
 * point, the interpolation value and flag RotTransPers writes, and the
 * projected screen point of the target. */
typedef struct {
    DVECTOR projected;
    long interpolation, flag;
    DVECTOR target;
} ModelVariantProjection;

/* The pulsing companion record the lines, rays, tube and mesh helpers of
 * headers 417-459 read only for its size at + 0x88: it scales the rays,
 * widens the tube or mesh radius, or gates the lines. */
typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} ModelVariantPulse;

/* One 0x90-byte pulse group: four rows of four points and the size. */
typedef struct {
    SVECTOR points[4][4];
    s32 size;
    u8 unknown_84[0xC];
} ModelVariantPulseGroup;

/* The pulse command row: the group count, the growth window and the fade
 * window. */
typedef struct {
    u8 unknown_00[0xC];
    u16 count;
    u8 unknown_0E[0x1E];
    u32 start;
    u32 end;
    u8 unknown_34[4];
    u32 fade_start;
    u32 fade_end;
} ModelVariantPulseTiming;

/* ModelVariantProjection with the target's projected screen point kept as
 * the packed word RotTransPers writes: the handlers split it with (u16) and
 * >> 16 to get the screen delta. */
typedef struct {
    DVECTOR projected;
    long interpolation, flag, target;
} ModelVariantPackedProjection;

/* The paired-sheet command row: the first sheet count, the pairing flag and
 * the fade window. */
typedef struct {
    u8 unknown_00[0x1C];
    s32 first_count;
    s32 paired;
    u8 unknown_24[0x0C];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_38[8];
} ModelVariantPairedSheetTiming;

/* One 0x3A8-byte record of the array at work + 0xED0 of header 427, read by
 * the mesh and panel helpers only for its progress at + 0x240. */
typedef struct {
    u8 unknown_000[0x240];
    s32 progress;
    u8 unknown_244[0x164];
} ModelVariantProgressRecord;

/* One 0x90-byte fan record: a centre point and two rings of five points
 * (point[0], point[1..5], point[6..10]), four quad corners, the inner and
 * outer colours, the growing size and a done flag. */
typedef struct {
    SVECTOR point[11];
    SVECTOR corner[4];
    u8 inner[4];
    u8 outer[4];
    s32 size;
    s32 field_84;
    s32 done;
    u8 unknown_8C[4];
} ModelVariantFan;

/* A 0x4F8-byte record read only for its size at + 0x4EC, which scales the
 * funnels or curtains drawn over it. */
typedef struct {
    u8 unknown_000[0x4EC];
    s32 size;
    u8 unknown_4F0[8];
} ModelVariantGridScale;

/* The funnel and sheet command row of headers 411 and 444: the growth window
 * and the fade window. */
typedef struct {
    u8 unknown_00[0xC];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_14[4];
    u32 fade_start;
    u32 fade_end;
} ModelVariantGrowFadeTiming;

/* Header 438's command row, read through one pointer at work + 0x22B4 by both
 * helpers: the sheets grow in the window at + 0x1C, and the lines cycle their
 * colour in the same window. */
typedef struct {
    u8 unknown_00[0x1C];
    u32 grow_start;
    u32 grow_end;
} ModelVariantGrowTiming;

/* One 0x1A0-byte group of lines: two sets of four six-point lines, a colour,
 * the size and the finished flag. */
typedef struct {
    SVECTOR points[2][4][6];
    u8 color[4];
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} ModelVariantLineGroup;

/* ModelVariantLineGroup with the colour typed as a CVECTOR. */
typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    u8 unknown_184[0x10];
    s32 size;
    s32 finished;
    u8 unknown_19C[4];
} ModelVariantLineGroupRgb;

/* The lines command row: the duration at + 0x30. */
typedef struct {
    u8 unknown_00[0x30];
    u32 duration;
} ModelVariantLineTiming;

/* One 0x6C-byte ray: a two-point spine and a copy offset across the view,
 * both projected, the screen angles and widths, the depths and the width's
 * screen offset. */
typedef struct {
    SVECTOR points[2];
    long screen[2];
    s32 angles[2];
    SVECTOR offset_points[2];
    long offset_screen[2];
    s32 projected_width[2];
    u8 unknown_40[0x1C];
    s32 depth[2];
    s16 width_x[2];
    s16 width_y[2];
} ModelVariantOffsetRay;

/* One 0x7C-byte spiral record: two inner and two outer colours after 0x50
 * unknown bytes. The DrawSpiral helpers (e.g. SpanishModelVariant421_DrawSpiral)
 * and FrenchModelVariant383_DrawRays, which draws sixteen of them as rays,
 * read these records. */
typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[0x1C];
} ModelVariantSpiral;

/* One 0xA8-byte trail record: nine colours, a ramp from bright to the base
 * colour, and nine staggered start offsets (a delay or -(j * 256)) the entry
 * handlers set per point. */
typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color[9];
    s32 field_6C, field_70, field_74, field_78, offset[9], field_A0, field_A4;
} ModelVariantTrail;

/* The 0x40-byte stack block below the entry handlers' projection locals;
 * only its last two halfwords, the texture CLUT words, are written. */
typedef struct {
    u8 unknown_00[60];
    u16 texture_cluts[2];
} ModelVariantClutStack;

/* One 0x90-byte shape: an origin, inner and outer rings of five points, four
 * corners and the inner and outer colours. */
typedef struct {
    SVECTOR origin, inner[5], outer[5], corners[4];
    CVECTOR inner_color, outer_color;
    s32 field_80, field_84, field_88;
    u8 unknown_8C[4];
} ModelVariantShape;

/* One 0x1A8-byte curtain: three seventeen-point loops, two colours, a scale
 * and a growth word. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    u8 color_a[4], color_b[4];
    s32 scale, growth;
} ModelVariantTripleCurtain;

/* The tube command row: the expansion and fade windows. */
typedef struct {
    u8 unknown_00[0x10];
    s32 expansion_start;
    s32 expansion_end;
    s32 fade_start;
    s32 fade_end;
} ModelVariantTubeTiming;

/* One 0x54C-byte tube or mesh: nine rings of nine points swept along the
 * path, and one colour per ring. */
typedef struct {
    SVECTOR points[9][9];
    u8 unknown_288[0x288];
    CVECTOR colors[9];
    u8 unknown_534[0x18];
} ModelVariantTubeView;

/* Header 398's 0x1E4-byte framebuffer ring: three rows of seventeen points, a
 * colour per point (cycling through eight hues), a scale the drawer grows to
 * 0x1000 and the count of times it wrapped. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR color[17];
    s32 scale, count;
} ModelVariantScreenRing;

/* ModelVariantSpiral in a 0x84-byte record. */
typedef struct {
    u8 unknown_00[0x50];
    CVECTOR inner[2], outer[2];
    u8 unknown_60[0x24];
} ModelVariantSpiralWide;

/* One 0x7C-byte spiral arm: a two-point spine and a copy of it moved along
 * the view, both projected, per point the screen angle, the projected width
 * and its screen offset, and two colour rows. */
typedef struct {
    u8 unknown00[0x10];
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
    s32 width[2];
    u8 cb[2][4];
    u8 ca[2][4];
    u8 unknown60[4];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown74[8];
} ModelVariantSpiralArm;

/* One 0x30-byte entry config: the inner, outer and web colours, the model
 * parts, the frame the webs start at and the frame the sheet/band burst
 * starts at. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, burst_start;
    u8 unknown_24[0xC];
} ModelVariantWebConfig;

/* One 0xC-byte row of a header 636-682 entry's command table, indexed by
 * the request command: the start delay, a radius, a height (each entry
 * gives the two sizes its own role; see the table's declaration) and two
 * RGB colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} ModelVariantTwoColorCommand;

/* The 0x10-byte command row of headers 635, 657, 662, 676 and 681: the
 * start delay, a radius, a height and three RGB colours, then a pad byte. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[9];
    u8 unknown_0F;
} ModelVariantThreeColorCommand;

/* The 0xC-byte command row of headers 647 and 651: the start delay, a
 * radius, a height and one RGB colour; the last three bytes are unused. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, unknown_09[3];
} ModelVariantOneColorCommand;

/* The 0xC-byte command row of headers 632 and 654: the start delay, the
 * radius, a value the entries copy but never read, the main colour and
 * the sphere colour. */
typedef struct {
    s16 delay;
    u16 radius, unknown_04;
    u8 red, green, blue, sphere_red, sphere_green, sphere_blue;
} ModelVariantSphereCommand;

/* The 0xC-byte command row of headers 633 and 634: the start delay, the
 * radius, the height (header 634 copies it but never reads it), the main
 * colour and the spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, spark_red, spark_green, spark_blue;
} ModelVariantSparkCommand;

/* A 0x5B4-byte ring of 24 quads: corner arrays v0..v3, positions, a colour,
 * and per quad a size, a progress word and an initialized flag. */
typedef struct {
    SVECTOR v0[24], v1[24], v2[24], v3[24], positions[24];
    CVECTOR color;
    u8 unknown_3C4[0x10];
    s32 size[24], progress[24], field_494[24], field_4F4[24], initialized[24];
} ModelVariantQuadRing;

/* One 0x334-byte streamer: seventeen colours the entry handlers reset to
 * grey. */
typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} ModelVariantStreamer;

/* Ten sprites: corner arrays v0..v3, an origin per sprite, a colour, and per
 * sprite a scale and a done flag. */
typedef struct {
    SVECTOR v0[10], v1[10], v2[10], v3[10], origin[10];
    CVECTOR color;
    s32 scale[10];
    s32 field_1BC[10];
    s32 field_1E4[10];
    s32 field_20C[10];
    s32 done[10];
    u8 unknown_25C[0xA0];
} ModelVariantSpriteSet;

/* One 0x20-byte spark of headers 680 and 682: an animated sprite pinned
 * to a model point through its coordinate unit. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, unknown_05;
    u16 elapsed, duration;
    u16 unknown_0A;
    GsCOORDUNIT *coord;
    VECTOR position;
} ModelVariantSpark;

/* One 0x90-byte ring of eight spokes with CVECTOR colours: inner and outer
 * spoke ends, the inner and outer colours (the entry handlers set them to
 * (64, 128, 255) and black), the angle the spokes sweep through 0..0x400 and
 * a count. ModelVariantRing is the same record with u8[4] colours; its
 * drawers advance angle and bump count when angle wraps. */
typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    CVECTOR inner_color;
    CVECTOR outer_color;
    s32 angle;
    s32 count;
} ModelVariantColorRing;

/* One 0xA0-byte two-point band record: per point a staggered start offset
 * (-(j * 768)) and two colours, then a scale vector. ca takes the first
 * colour of the entry config and cb the second (the header-461 family's
 * config calls these outer and inner, the header-402 and 478 configs inner
 * and outer). */
typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    CVECTOR ca[2];
    CVECTOR cb[2];
    VECTOR scale;
    s16 field_90, field_92;
    s32 field_94;
    u8 unknown_98[8];
} ModelVariantBandPair;

/* One 0xB4-byte three-point band record: per point a staggered start offset
 * and inner and outer colours, then a scale vector. */
typedef struct {
    u8 unknown_00[0x6C];
    s32 offset[3];
    CVECTOR inner[3], outer[3];
    VECTOR scale;
    s16 field_A0, field_A2;
    s32 field_A4;
    u8 unknown_A8[0xC];
} ModelVariantBandTriple;

/* A 0x4EC-byte mesh of nine rows of seventeen points and one colour per row,
 * drawn as RotTransPers4 quads between neighbouring rows. */
typedef struct {
    SVECTOR rows[9][17];
    u8 colors[9][4];
} ModelVariantMesh;

#endif
