#ifndef MEMORIES_DECOMP_IMAGE9840_ENTRY_H
#define MEMORIES_DECOMP_IMAGE9840_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    s16 delay;
    u16 radius, length, start_radius, end_radius;
    u8 red, green, blue;
    u8 secondary_red, secondary_green, secondary_blue;
} Image9840Command;

typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale, position, growth;
} Image9840Part;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, length, start_radius, end_radius;
    u16 tpage;
    u8 unknown_14[4];
    u16 clut;
    u8 unknown_1A[6];
    POLY_G3 triangle;
    POLY_GT4 quad;
    SVECTOR square[4];
    SVECTOR inner[5];
    SVECTOR first_ring[17];
    SVECTOR second_ring[17];
    SVECTOR burst_ring[17];
    u8 flags[5];
    u8 primary_count, secondary_count, burst_count;
    Image9840Part primary[2];
    Image9840Part secondary[10];
    Image9840Part bursts[32];
} Image9840State;

typedef struct {
    u8 bytes[8];
} Image9840ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CD54;
extern Image9840Command D_8013CD70[];

s32 func_8013B004(u8 *context, s32 command);

#endif
