#ifndef FRENCH_MODEL_VARIANT446_ENTRY_H
#define FRENCH_MODEL_VARIANT446_ENTRY_H

#include "../../types.h"
#include "variant402_entry.h"
#include "../../game/model_get_frame_step.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u16 count;
    u8 unknown_0E[2];
    s32 width, inner_radius, outer_radius;
    u8 unknown_1C[0xC];
    s32 extent;
    u32 draw_start, duration, expanded, fade, end, rays_start, rays_end;
    u32 unknown_48;
} EntryConfig446;

typedef struct {
    u8 unknown_00[0x78];
    CVECTOR inner[3], outer[3];
    u8 unknown_90[0x28];
} EntryRay446;

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    VECTOR direction;
    s32 size, finished, field_19C;
} EntryLines446;

typedef struct {
    SVECTOR points[3][5];
    PSXLONG projected[3][5];
    s16 field_B4, field_B6;
    s32 field_B8, depth[5];
} EntryStrip446;

typedef struct {
    SVECTOR points[4][4];
    VECTOR scale;
} EntryPulse446;

typedef struct {
    SVECTOR inner[17], outer[17];
    s32 progress, state;
} EntryRing446;

typedef struct {
    EntryRay446 rays[16];
    EntryLines446 lines[3];
    EntryStrip446 strips[1];
    EntryPulse446 pulses[2];
    u8 unknown_1250[0x2C0];
    EntryRing446 rings[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra, ring_packet;
    POLY_FT4 flat[2];
    u8 unknown_1AD0[0x38];
    GsGLINE line;
    MATRIX matrix, second_matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, elapsed, animation, step;
    s32 fade;
    EntryConfig446 *G32 config;
    u32 unknown_1BA8;
    GsCOORDUNIT *G32 parts[3];
    s32 part_index;
    CVECTOR inner, outer;
    s16 spin, field_1BC6, field_1BC8, field_1BCA;
    s16 width, radius, progress, field_1BD2;
    s32 ray_size, ray_length, phase, field_1BE0;
    s16 slot, command;
} EntryState446;

extern u8 D_8013E6D4[];
void func_8013BE00(u8 *context);
void func_8013C2F8(u8 *context);
void func_8013C95C(u8 *context);
void func_8013CFE8(u8 *context);
void func_8013D4BC(u8 *context);
void func_8013DCAC(u8 *context);

#endif
