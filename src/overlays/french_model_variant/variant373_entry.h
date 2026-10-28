#ifndef FRENCH_MODEL_VARIANT373_ENTRY_H
#define FRENCH_MODEL_VARIANT373_ENTRY_H

#include "../../types.h"
#include "variant402_entry.h"
#include "variant373_layers.h"
#include "variant373_grid.h"
#include "variant373_points.h"
#include "variant373_strip.h"
#include "variant373_ribbons.h"
#include "variant373_quads.h"
#include "variant373_rays.h"
#include "../../game/model_get_frame_step.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u16 count;
    u8 unknown_0E[2];
    u32 layers_start, grid_start, field_18, field_1C, field_20, points_start;
} EntryConfig373;

typedef struct {
    u8 unknown[0x3A];
    u16 clut2, clut1, clut0;
} EntryTextureStack373;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag, target;
} EntryProjection373;

typedef struct {
    SVECTOR points[16], corners[4], rotation;
    s32 scale;
    u8 unknown_AC[8];
} EntryPointGroup373;

typedef struct {
    SVECTOR points[3][2];
    PSXLONG projected[3][2];
    CVECTOR color;
    VECTOR scale;
    s16 field_5C, field_5E;
    s32 field_60;
    PSXLONG depth[2];
} EntryStrip373;

typedef struct {
    SVECTOR inner[8], outer[8];
    CVECTOR inner_color, outer_color;
    s32 size, field_8C;
} EntryRing373;

typedef union {
    POLY_GT4 grid[8];
    struct {
        u8 unknown_00[0x34];
        POLY_FT4 pair[2], tiles[8];
    } flat;
} EntryPackets373;

typedef struct {
    Family373Grid grid;
    Family373Layer layers[5];
    EntryPointGroup373 groups[3];
    EntryStrip373 strips[1];
    Family373QuadGroup quadrants[1];
    Family373Ribbon ribbons[8];
    EntryRing373 rings[4];
    Family373Ray rays[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 shared[2], layer_packets[16];
    EntryPackets373 packets;
    POLY_FT4 point_packet;
    u8 unknown_238C[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation;
    s32 step, fade;
    EntryConfig373 *G32 config;
    u32 field_241C;
    GsCOORDUNIT *G32 parts[3];
    s32 index, grid_scale, translation, rotation, oscillation, grid_brightness;
    s32 scale, brightness, fields_244C[5];
    s16 field_2460, field_2462, field_2464, field_2466, field_2468, field_246A;
    s16 field_246C, field_246E, field_2470, field_2472, factor, width;
    s32 angle, field_247C, texture_offset, phase, field_2488, field_248C;
    u8 unknown_2490[8];
    s32 ambient;
    s16 slot, command;
} EntryState373;

extern u8 D_8013EEF0[];

#endif
