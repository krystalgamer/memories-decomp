#ifndef MEMORIES_DECOMP_MODEL_VARIANT653_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT653_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_set_slot_shadow_enabled.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the radius, the height, the
 * shell colour and the line colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue;
    u8 line_red, line_green, line_blue;
} Model653Command;

/* One 0x4C-byte effect record. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} Model653Strip;

/* The work area: the command, the slot, the textured quad, the cone
 * triangle, the line, the shell grid, the cone ring and the sphere grid.
 * The flags hold the next shell, the drop counter and the cones-released
 * flag. Then come the record counts, the other slot's position and four
 * record arrays: the swinging emitters, the shells they drop, the cones and
 * the line spheres. Shells, cones and spheres are indexed by the sum of the
 * emitter and child indices. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s32 other_slot;
    s32 shadow;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_26;
    POLY_GT4 quad;
    POLY_G3 tri;
    GsGLINE line;
    SVECTOR shell[17][7];
    SVECTOR ring[17];
    SVECTOR sphere[9][9];
    u8 flags[5];
    u8 emitter_count, shell_count, cone_count;
    SVECTOR target;
    Model653Strip emitters[1];
    Model653Strip shells[32];
    Model653Strip cones[8];
    Model653Strip spheres[8];
} Model653State;

extern MATRIX D_800FE168;
extern GsIMAGE gModelVariant653_D250C;
extern Model653Command gModelVariant653_D2528[];

s32 ModelVariant653_HandleRequest(u8 *context, s32 command);

#endif
