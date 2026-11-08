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

/* A 0x2FC-byte group of ten quads: corner arrays a..d, a position per quad,
 * a colour, and per quad a size, an angle, a done flag and an initialized
 * flag. The French header-470 entry builds one at work + 0x988 and hands the
 * work area to SpanishModelVariant470_DrawQuads, which draws the corners in
 * the colour, grows size, sets done and clears initialized when a quad wraps;
 * the header-419, 428, 429, 471 and 478 quads helpers read it the same way. */
typedef struct {
    SVECTOR a[10], b[10], c[10], d[10], positions[10];
    CVECTOR color;
    s32 size[10], angle[10], done[10], field_20C[10], initialized[10];
    u8 unknown_25C[0xA0];
} ModelVariantQuadGroup;

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

/* A 0x714-byte swarm of 32 sprites: corner arrays v0..v3, an origin per
 * sprite, a colour, and per sprite a scale and a done flag (drawn, for
 * instance, by FrenchModelVariant430_DrawSwarm). */
typedef struct {
    SVECTOR v0[32], v1[32], v2[32], v3[32], origin[32];
    CVECTOR color;
    u8 unknown_504[0x10];
    s32 scale[32];
    s32 field_594[32];
    s32 field_614[32];
    s32 done[32];
} ModelVariantSwarm;

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

/* One 0x28-byte command row of the wave-grid entries: the wave's colour and
 * the ranges its grid colours are drawn from (unused by header 138), the
 * sprites' colour, then the sprite size, the bounce height, the sway and
 * drift of the ring, the spin speed, the grid size, the base height, the wave
 * amplitude, the period, the fade time, the lead time and the frame the fade
 * starts. */
typedef struct {
    u8 red, green, blue;
    u8 low[3], high[3];
    u8 sprite_red, sprite_green, sprite_blue;
    s16 sprite_size, bounce, sway, drift, speed, grid_size, base, amplitude, period, unknown_1E;
    s16 fade, unknown_22, lead, start;
} ModelVariantWaveConfig;

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

/* One 0x3C-byte command row: the burst colour, the ray count and shape, the
 * spark count, size and speed, the glow size, the debris and ember sizes,
 * speeds and counts, the colour fade step, the delay, and the timing and
 * offsets of the camera presets. */
typedef struct {
    u8 r, g, b, pad_3;
    u16 ray_count;
    s16 ray_width, ray_base, ray_height;
    u16 spark_max;
    s16 spark_size;
    u16 spark_speed;
    u16 glow_size;
    s16 debris_size;
    u16 debris_speed;
    u16 debris_count;
    u16 ember_count;
    s16 ember_size;
    u16 ember_speed;
    s16 fade;
    s16 pad_22;
    u16 delay;
    u16 camera_times[2];
    s16 camera_args[9];
} ModelVariantBurstConfig;

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
 * each drawn as a GsGLINE from inner[k] to outer[k]. The ring drawers light
 * the inner end with inner_color and fade it with angle; the spoke drawers
 * light the outer end with outer_color and scale the spokes instead. angle
 * runs to 0x400 and count is bumped each time it wraps. ModelVariantColorRing
 * is the same record with CVECTOR colours. */
typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 inner_color[4];
    u8 outer_color[4];
    s32 angle;
    s32 count;
} ModelVariantRing;

/* One 0x98-byte element of header 397's sheet array: four POLY_GT4 quads
 * given by corner arrays v0..v3, an inner colour on three corners and an
 * outer colour on the fourth, a scale and two words the entry handlers
 * clear. The header-321, 324, 337, 376 and 432 sheet helpers draw quad j
 * from v0[j]..v3[j], light the first three corners with inner and the fourth
 * with outer, and grow size as the scale; the header-415 and 341 entries
 * (ModelVariantSheet) and the header-337, 338 and 393 entries
 * (ModelVariantPointSheet) pass them these records. */
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
 * and 336 sheet drawers read them. (The header-337, 400 and 410 entries and
 * the Exodia entries named the two colours the other way round; the
 * header-410 ring drawer's Family410Ring and the Exodia ring drawers'
 * ExodiaRing keep that naming.) */
typedef struct {
    SVECTOR points[16];
    CVECTOR outer, inner;
    s32 size, field_8C, field_90;
    u8 unknown_94[4];
} ModelVariantPointSheet;

/* One 0x98-byte sheet: a 4x4 grid of points, inner and outer colours and a
 * scale vector. */
typedef struct {
    SVECTOR points[4][4];
    CVECTOR inner, outer;
    VECTOR scale;
} ModelVariantScaledSheet;

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

/* The ribbon colour bytes of headers 373 and 383: the outer and inner
 * intensities. */
typedef struct {
    u8 unknown_00[0x80];
    u8 outer;
    u8 unknown_81[3];
    u8 inner;
    u8 unknown_85[3];
} ModelVariantRibbonColors;

/* The ribbon command row: the fade window at + 0x24. */
typedef struct {
    u8 unknown00[0x24];
    u32 fade_start;
    u32 fade_end;
} ModelVariantRibbonFadeTiming;

/* The ribbon command row of headers 425 and 426: the growth and fade
 * windows. */
typedef struct {
    u8 unknown00[0x1C];
    u32 grow_start;
    u32 grow_end;
    u8 unknown24[4];
    u32 fade_start;
    u32 fade_end;
} ModelVariantRibbonGrowTiming;

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

/* One 0x6C-byte ribbon span of headers 373 and 383: two points and their
 * edge copies, both projected, the angles and widths, the depths and the
 * screen offsets. */
typedef struct {
    SVECTOR points[2];
    long projected[2];
    s32 angle[2];
    SVECTOR edges[2];
    long edge_projected[2];
    s32 width[2];
    u8 unknown_40[0x1C];
    long depth[2];
    s16 x_offset[2];
    s16 y_offset[2];
} ModelVariantRibbonSpan;

/* The ribbon tail: the completion flag, the target it moves to and the
 * per-step delta. */
typedef struct {
    s32 completed;
    VECTOR target;
    VECTOR delta;
} ModelVariantRibbonTail;

/* One 0x360-byte ribbon: two colours, a scale vector and a position. */
typedef struct {
    u8 unknown_000[0x220];
    CVECTOR colors[2];
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x88];
    VECTOR position;
    u8 unknown_2D8[0x88];
} ModelVariantLongRibbon;

/* One 0x250-byte ribbon: inner and outer colours, a scale vector, a count, a
 * state, a length and a position. */
typedef struct {
    u8 unknown_000[0x160];
    CVECTOR inner, outer;
    VECTOR scale;
    s32 field_178, field_17C, count, field_184, state, length;
    u8 unknown_190[0x58];
    VECTOR position;
    u8 unknown_1F8[0x58];
} ModelVariantCountedRibbon;

/* A 0x6C-byte ribbon record: a colour and three scale words. */
typedef struct {
    u8 unknown_00[0x48];
    CVECTOR color;
    s32 scale[3];
    u8 unknown_58[4];
    s16 field_5C, field_5E;
    s32 field_60;
    u8 unknown_64[8];
} ModelVariantShortRibbon;

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

/* One 0x274-byte streamer: a thirteen-point spine and its copy moved along
 * the view, both projected, with per-point angle, width, colour, depth and
 * screen offset. */
typedef struct {
    SVECTOR a[13];
    long sa[13];
    s32 angle[13];
    SVECTOR b[13];
    long sb[13];
    s32 width[13];
    u8 unknown_01A0[0x34];
    CVECTOR color[13];
    u8 unknown_0208[4];
    s32 depth[13];
    s16 ox[13];
    s16 oy[13];
} ModelVariantShortStreamer;

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

/* One 0x84-byte spiral or petal arm: a two-point spine and a copy of it
 * moved along the view, both projected, per point the screen angle, the
 * projected width and its screen offset, and two colour rows. */
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
    long flag[2];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown7C[8];
} ModelVariantFlaggedSpiralArm;

/* One 0x378-byte streamer: a seventeen-point spine and a copy of it moved
 * along the view, both projected, and per point the screen angle, the
 * projected width, a colour, the projection flag, the depth and the width's
 * screen offset. */
typedef struct {
    SVECTOR a[17];
    long sa[17];
    s32 angle[17];
    SVECTOR b[17];
    long sb[17];
    s32 width[17];
    u8 unknown220[0x44];
    u8 color[17][4];
    u8 unknown2A8[0x04];
    long flag[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} ModelVariantFlaggedStreamer;

/* One 0x118-byte funnel or ring: seventeen inner points at radius 64 and
 * seventeen outer points at radius 256, joined into quads and scaled by
 * size. */
typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    u8 unknown_114[4];
} ModelVariantFunnel;

/* One 0x24-byte command row, picked by command % 100: the particle and glow
 * colours (headers 165 and 166 do not read the glow colour), the slot part
 * the particles start from, the particle count, the particle half size, the
 * target sphere radius, the glow half size, the ring radius, the rise, fade
 * and pulse durations, the gather and burst durations, the spacing between
 * particles, the spin rate (unused by 165 and 166) and the three start
 * times. */
typedef struct {
    u8 red, green, blue, glow_red, glow_green, glow_blue, part, count;
    s16 size, radius, glow_size, ring_radius, rise, fade, pulse, gather, burst, spacing, spin;
    s16 delay, delay2, start;
} ModelVariantGatherConfig;

/* A 0x4F8-byte record read only for its size at + 0x4EC, which scales the
 * funnels or curtains drawn over it. */
typedef struct {
    u8 unknown_000[0x4EC];
    s32 size;
    u8 unknown_4F0[8];
} ModelVariantGridScale;

/* One 0x4F8-byte grid sheet: nine rows of seventeen points, a colour per
 * row, the size and the done flag. */
typedef struct {
    SVECTOR points[9][17];
    u8 colors[9][4];
    s32 size;
    u8 unknown_4F0[4];
    s32 done;
} ModelVariantGridSheet;

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

/* One 0xB8-byte record whose three inner and three outer colours the entry
 * seeds as ModelVariantSpiral's are (inner grey, outer the bright colour). */
typedef struct {
    u8 unknown_00[0x78];
    CVECTOR inner[3], outer[3];
    u8 unknown_90[0x28];
} ModelVariantRay;

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
 * path, one colour per ring, and the scale vector the entry handlers reset
 * to 4096. */
typedef struct {
    SVECTOR points[9][9];
    u8 unknown_288[0x288];
    CVECTOR colors[9];
    VECTOR scale;
    s16 field_544, field_546;
    s32 field_548;
} ModelVariantTubeView;

/* One 0x3C-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, its rays and sparks,
 * the two fade steps, the two bolts' launch times and the CLUT mode of
 * the second and fourth textures. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 ray_r, ray_g, ray_b;
    u8 part, unknown_0D;
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 ray_spread;
    u16 ray_width;
    u16 ray_count;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_33;
    u8 ray_fade, unknown_35;
    u16 start;
    u16 second_start;
    u16 mode;
} ModelVariantTwinBoltConfig;

/* Header 398's 0x1E4-byte framebuffer ring: three rows of seventeen points, a
 * colour per point (cycling through eight hues), a scale the drawer grows to
 * 0x1000 and the count of times it wrapped. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR color[17];
    s32 scale, count;
} ModelVariantScreenRing;

/* One 0x7C-byte ray of headers 373 and 383: two points and their edge
 * copies, both projected, the angles and widths, the inner and outer
 * colours, the depths and the screen offsets. */
typedef struct {
    u8 unknown_00[0x10];
    SVECTOR points[2];
    long projected[2];
    s32 angle[2];
    SVECTOR edges[2];
    long edge_projected[2];
    s32 width[2];
    CVECTOR inner[2];
    CVECTOR outer[2];
    u8 unknown_60[4];
    s32 depth[2];
    s16 x_offset[2];
    s16 y_offset[2];
    u8 unknown_74[8];
} ModelVariantShadedRay;

/* One 0x1A8-byte screen-textured ring: three rows of seventeen points, inner
 * and outer colours, a scale and a wrap count. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR inner, outer;
    s32 scale, count;
} ModelVariantTwoToneScreenRing;

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

/* One 0x12C-byte beam record: inner and outer colours and a scale vector. */
typedef struct {
    u8 unknown_00[0xD8];
    CVECTOR inner, outer;
    VECTOR scale;
    u8 unknown_F0[0x3C];
} ModelVariantBeam;

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

/* One 0x34-byte entry config: outer and inner colours, the model part, and
 * the frames the drawing, ribbon, travel, fade and spiral phases start or
 * end at. */
typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, ribbon_start, travel_start, travel_end, fade_start, fade_end;
    u32 spiral_start;
} ModelVariantSpiralConfig;

/* One 0x40-byte entry config: outer, inner and web colours, the model parts,
 * the first count and paired flag, and the frames the phases start or end
 * at. */
typedef struct {
    CVECTOR outer, inner, web_color;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    s32 first_count, paired;
    u32 draw_start, grow_end, field_2C, phase3_end, matrix_end;
    u8 unknown_38[8];
} ModelVariantGrowConfig;

/* One 0x38-byte entry config: outer, inner and web colours, two model parts,
 * a limit and the frames the drawing, matrix and move phases start or end
 * at. */
typedef struct {
    CVECTOR outer, inner, web_color;
    u8 unknown_0C[4], parts[2], unknown_12[0xA];
    u32 draw_start, limit, field_24, field_28, matrix_end, move_start, move_end;
} ModelVariantMoveConfig;

/* One 0x38-byte entry config: ModelVariantWebConfig's colours, parts and start
 * frame, a limit, and the frames the origin and sweep phases end or start
 * at. */
typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start;
    u32 limit;
    u8 unknown_24[8];
    u32 origin_end;
    u32 sweep_start, sweep_end;
} ModelVariantSweepConfig;

/* A 0x5B4-byte ring of 24 quads: corner arrays v0..v3, positions, a colour,
 * and per quad a size, a progress word and an initialized flag. The French
 * header-461 and 469 entries build it at work + 0xB90 and + 0x1614 and pass
 * the work area to SpanishModelVariant461_DrawRing and
 * SpanishModelVariant469_DrawQuadRing, which draw v0..v3 and grow size. */
typedef struct {
    SVECTOR v0[24], v1[24], v2[24], v3[24], positions[24];
    CVECTOR color;
    u8 unknown_3C4[0x10];
    s32 size[24], progress[24], field_494[24], field_4F4[24], initialized[24];
} ModelVariantQuadRing;

/* The first 0x434 bytes of a ring of 24 quads given by corner arrays v0..v3,
 * and a size per quad, as the header-447, 462 and 463 ring helpers read it.
 * The same reads at the same offsets as ModelVariantQuadRing, but no decompiled
 * entry is known to hand these helpers a full ModelVariantQuadRing. */
typedef struct {
    SVECTOR v0[24];
    SVECTOR v1[24];
    SVECTOR v2[24];
    SVECTOR v3[24];
    u8 unknown_300[0xD4];
    s32 size[24];
} ModelVariantQuadRingView;

/* One 0x88-byte sheet of four quads: four rows of four corners; each quad
 * takes color[1] on its first three corners and color[0] on the fourth. */
typedef struct {
    SVECTOR points[4][4];
    CVECTOR color[2];
} ModelVariantQuadSheet;

/* One 0x334-byte streamer: seventeen colours the entry handlers reset to
 * grey. */
typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0x88];
} ModelVariantStreamer;

/* The strip of headers 373 and 383: three rows of two points, their screen
 * coordinates and two depths. */
typedef struct {
    SVECTOR points[3][2];
    long projected[3][2];
    u8 unknown_48[0x1C];
    long depth[2];
} ModelVariantStrip;

/* The strip command row: the pass count, the width and the start, expanded,
 * fade and end times. */
typedef struct {
    u8 unknown_00[0xC];
    u16 iterations;
    u8 unknown_0E[2];
    s32 width;
    u8 unknown_14[0x1C];
    u32 start;
    u32 expanded;
    u32 fade;
    u32 end;
} ModelVariantStripTiming;

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

/* The command row of the header 373 and 383 ribbon and strip helpers, read
 * only for the pass count at + 0x0C: the helper finishes when its index
 * reaches it. */
typedef struct {
    u8 unknown_00[0xC];
    u16 count_0C;
} ModelVariantCountConfig;

/* ModelVariantColorRing with a colour per spoke before the inner and outer
 * colours (0xB0 bytes). */
typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR colors[8], inner_color, outer_color;
    s32 angle, count;
} ModelVariantSpokeColorRing;

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

/* One 0x14-byte command row, picked by command % 100: the beam and particle
 * colours, the slot part, the ring radius, the particle half size, the spin
 * rate, the fade, the duration and the delay. */
typedef struct {
    u8 beam_r, beam_g, beam_b;
    u8 particle_r, particle_g, particle_b;
    u8 part, unknown_07;
    s16 radius, particle_half_size, spin, fade, duration, delay;
} ModelVariantBeamParticleConfig;

/* One 0x26-byte command row of the beam-and-ring entries: the beam and ring
 * colours, the beam height, half width and spread, the ring radius,
 * thickness and height, and the durations, spacings and start times of the
 * beams, rings and flash. */
typedef struct {
    u8 beam_color[3];
    u8 ring_color[3];
    u8 unknown_06[2];
    s16 height;
    s16 half_width;
    s16 spread;
    s16 radius;
    s16 thickness;
    s16 ring_height;
    s16 beam_duration;
    s16 ring_span;
    s16 ring_duration;
    s16 flash_duration;
    s16 beam_spacing;
    s16 ring_spacing;
    s16 ring_begin;
    s16 beam_begin;
    s16 flash_begin;
} ModelVariantBeamRingConfig;

/* One 0x48-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolt leaves from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around the bolt, the smoke puffs, the impact glow, its rays, the shock
 * fan, the sparks, the two fade steps, the two fan growth rates and the
 * start time. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 ray_r, ray_g, ray_b;
    u8 part;
    u8 unknown_0D;
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 ray_spread;
    u16 ray_width;
    u16 ray_count;
    u16 fan_radius;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_39;
    u8 ray_fade, unknown_3B;
    u32 grow;
    u32 grow_late;
    u16 start;
    u16 unknown_46;
} ModelVariantBoltFanConfig;

/* One 0x3C-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, its rays and sparks,
 * the two fade steps and the spawn period, start and end. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 ray_r, ray_g, ray_b;
    u8 part, unknown_0D;
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 ray_spread;
    u16 ray_width;
    u16 ray_count;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_33;
    u8 ray_fade, unknown_35;
    u16 period;
    u16 start;
    u16 end;
} ModelVariantBoltStreamConfig;

/* One 0x58-byte command row: the muzzle sprite colour, the bolt, glow and
 * flare colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, the shock fan, the
 * sparks, the fade step, the fan growth rates, the four launch times and
 * the camera cue times. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 unknown_09[3];
    u8 part;
    u8 unknown_0D[3];
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 unknown_28;
    u16 fan_radius;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_37;
    u32 unknown_38;
    u32 grow;
    u32 grow_late;
    u16 start;
    u16 launch[3];
    u16 unknown_4C[2];
    u16 cue[3];
    u16 unknown_56;
} ModelVariantBoltVolleyConfig;

/* A 0xA0-byte pair of shaded sprites sharing one scale: per sprite a
 * staggered start offset (-(k * 768)) and the entry config's two colours,
 * then the scale vector and two rotation halfwords. ModelVariantBandPair is
 * the same record with CVECTOR colours. */
typedef struct {
    u8 unknown_00[0x68];
    s32 offset[2];
    u8 inner[2][4];
    u8 outer[2][4];
    VECTOR scale;
    s16 rx, ry;
    s32 field_94;
    u8 unknown_98[8];
} ModelVariantBeamPair;

/* Nine 17-point rings of a dome, each with its own colour (ModelVariantMesh
 * with CVECTOR colours). */
typedef struct {
    SVECTOR points[9][17];
    CVECTOR colors[9];
} ModelVariantDome;

/* A 0x4EC-byte mesh of nine rows of seventeen points and one colour per row,
 * drawn as RotTransPers4 quads between neighbouring rows. */
typedef struct {
    SVECTOR rows[9][17];
    u8 colors[9][4];
} ModelVariantMesh;

#endif
