#ifndef MEMORIES_DECOMP_MODEL_VARIANT675US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT675US_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the ring radius, the bolt
 * reach, the ring colour and the bolt colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, bolt_red, bolt_green, bolt_blue;
} Model675UsCommand;

/* One 0x50-byte effect record: a ring, a bolt, a fading bolt trail or a
 * spark curtain. Bolts keep their trail cursor in ticks and their target
 * point in growth. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u16 ticks, unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} Model675UsElement;

/* The work area: the command, the slot, the packets, the flash square, the
 * upper, lower and middle ring outlines, the 17-point fan with its centre,
 * the upper and lower curtain edges (nine rows of sixteen bolts), the model
 * origin, the flags, the record counts, and the rings, bolts, bolt trails and
 * spark curtains. */
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
    POLY_GT4 shaded;
    POLY_GT3 fan_tri;
    GsLINE line;
    SVECTOR square[4];
    SVECTOR ring_upper[17];
    SVECTOR ring_lower[17];
    SVECTOR ring_middle[17];
    SVECTOR fan[17];
    SVECTOR center;
    SVECTOR curtain_upper[9][16];
    SVECTOR curtain_lower[9][16];
    SVECTOR origin;
    u8 flags[5];
    u8 ring_count, bolt_count, trail_count;
    Model675UsElement rings[3];
    Model675UsElement bolts[16];
    Model675UsElement trails[8][16];
    Model675UsElement sparks[16];
} Model675UsState;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model675UsViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013DAF8[5];
extern Model675UsCommand D_8013DB84[];

s32 func_8013B004(u8 *context, s32 command);

#endif
