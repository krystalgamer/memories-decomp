#ifndef MEMORIES_DECOMP_MODEL_VARIANT673_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT673_ENTRY_H

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
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the spread radius, the rise
 * height, the quad colour and the fan colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, fan_red, fan_green, fan_blue;
} Model673Command;

/* One 0x4C-byte quad record flying from position along growth over its
 * lifetime. */
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
} Model673Quad;

/* The work area: the command, the slot, the packets, the quad corners, the
 * fan ring and its centre and corners, the slot position, the flags, and
 * three record arrays: the rising quads, the falling quads and the fans. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_FT4 flat;
    POLY_GT3 fan;
    SVECTOR square[4];
    SVECTOR ring[17];
    SVECTOR centre;
    SVECTOR corners[4];
    s16 position[3];
    u16 unknown_146;
    u8 flags[5];
    u8 count;
    u8 unknown_14E[2];
    Model673Quad rising[64];
    Model673Quad falling[64];
    Model673Quad fans[64];
} Model673State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model673ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013CB30[];
#endif
extern GsIMAGE D_8013CB48[3];
extern Model673Command D_8013CB9C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
