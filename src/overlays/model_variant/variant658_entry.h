#ifndef MEMORIES_DECOMP_MODEL_VARIANT658_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT658_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x1C-byte command row: the start delay, the darkening time, the frame
 * that ends the burst/spark emission, the size of the halo and of the cone,
 * then the halo, the burst/spark and the flash colours. */
typedef struct {
    s16 delay;
    u16 unknown_02;
    s32 fade_duration;
    s32 emit_end;
    u16 radius, height;
    u8 color[9];
    u8 unknown_19[3];
} Model658Command;

/* One 0x4C-byte halo: a sprite and a two-band ring of strips plus a cloud
 * of shards, all placed through a camera-relative coordinate unit. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_3C;
} Model658Halo;

/* One 0x2C-byte screen-space particle moving from start to end. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v;
    u16 elapsed, duration, unknown_0A;
    VECTOR start;
    VECTOR end;
} Model658Particle;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s32 darken;
    s32 brighten;
    s32 projection;
    s16 delay;
    u16 radius, height;
    u16 tpage[3], unknown_24;
    u16 clut[3], unknown_2C[2];
    POLY_FT4 sprite;
    POLY_GT4 strip;
    POLY_GT3 shard;
    GsGLINE line;
    SVECTOR corners[4];
    SVECTOR base;
    SVECTOR apex;
    SVECTOR ring[17];
    SVECTOR cone[17];
    SVECTOR flash_corners[4];
    SVECTOR inner[33];
    SVECTOR outer[33];
    SVECTOR middle[33];
    SVECTOR shards[3][8][8];
    /* Burst cursor, spark cursor, first trigger, spark timer, restore
     * request and second trigger. */
    u8 flags[6];
    u8 fade;
    u8 halo_count, burst_count, spark_count, unknown_B4A[2];
    Model658Halo halos[16];
    Model658Particle bursts[128];
    Model658Particle sparks[32];
    Model658Particle flash;
} Model658State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model658ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013E03C[3];
extern Model658Command D_8013E090[];

s32 func_8013B004(u8 *context, s32 command);

#endif
