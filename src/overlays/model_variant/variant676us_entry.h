#ifndef MEMORIES_DECOMP_MODEL_VARIANT676US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT676US_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x10-byte command row: the start delay, the size of the sprites and
 * the spread of the lattice, then the flash/shard, burst and ring/wave
 * colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[9];
    u8 unknown_0F;
} Model676UsCommand;

/* One 0x50-byte element: a flash, a shard, a burst, a ring or a wave. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u16 ticks, unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model676UsElement;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[5];
    u16 clut[5];
    u16 unknown_26;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    POLY_GT3 fan;
    SVECTOR corners[4];
    SVECTOR ring[17];
    SVECTOR centre;
    SVECTOR shard_corners[4];
    SVECTOR burst_corners[4];
    SVECTOR arc_lower[17];
    SVECTOR arc_upper[17];
    SVECTOR ring_corners[4];
    /* Inner and outer lattice points: [side][layer][column][row]. */
    SVECTOR lattice[2][5][8][8];
    SVECTOR ring_inner[17];
    SVECTOR ring_outer[17];
    SVECTOR wave_inner[17];
    SVECTOR wave_outer[17];
    /* [4] records that the completion has already been reported once. */
    u8 flags[5];
    u8 flash_count, shard_count, burst_count, spoke_count;
    u8 unknown_18F5[3];
    Model676UsElement flashes[3];
    Model676UsElement shards[32];
    Model676UsElement bursts[3];
    Model676UsElement rings[3];
    Model676UsElement waves[3];
} Model676UsState;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model676UsViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013E490[6];
extern Model676UsCommand D_8013E538[];

s32 func_8013B004(u8 *context, s32 command);

#endif
