#ifndef MEMORIES_DECOMP_MODEL_VARIANT437_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT437_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/camera_view.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"

/* One 48-byte header-437 command record: three colours, the three slot
 * part indices and the frames at which the blooms start and the bloom
 * origins stop following the model. */
typedef struct {
    u8 color[3][4];
    u8 unknown0C[4];
    u8 parts[4];
    u8 unknown14[8];
    u32 bloom_start;
    u8 unknown20[4];
    u32 follow_end;
    u8 unknown28[8];
} Variant437Config;

/* One 0x2A4-byte bloom group (eight at work + 0x000), the same layout the
 * bloom helper draws. */
typedef struct {
    SVECTOR a[12];
    SVECTOR b[12];
    SVECTOR c[12];
    SVECTOR d[12];
    u8 unknown180[0x60];
    u8 color[4];
    u8 unknown1E4[0x10];
    s32 size[12];
    s32 done[12];
    s32 extra[12];
    VECTOR origin;
    VECTOR direction;
} Variant437Group;

/* One 0xA8-byte nine-step ribbon (sixteen at work + 0x1520). */
typedef struct {
    u8 unknown00[0x48];
    u8 color[9][4];
    s32 unknown6C[3];
    u8 unknown78[4];
    s32 offset[9];
    s32 unknownA0;
    s32 unknownA4;
} Variant437Ribbon;

/* One 0x1A0-byte four-ring cone (three at work + 0x1FA0). */
typedef struct {
    SVECTOR inner[4][6];
    SVECTOR outer[4][6];
    u8 color[4];
    s32 unknown184[3];
    u8 unknown190[4];
    s32 angle;
    s32 unknown198;
    s32 unknown19C;
} Variant437Cone;

/* The 0xA0-byte two-step beam at work + 0x2480. */
typedef struct {
    u8 unknown00[0x68];
    s32 offset[2];
    u8 color[2][4];
    u8 glow[2][4];
    VECTOR scale;
    s16 rx, ry;
    s32 unknown94;
    u8 unknown98[8];
} Variant437Beam;

/* One 0x98-byte four-corner tile (two at work + 0x2520). */
typedef struct {
    SVECTOR a[4];
    SVECTOR b[4];
    SVECTOR c[4];
    SVECTOR d[4];
    u8 color[4];
    u8 glow[4];
    s32 unknown88[3];
    u8 unknown94[4];
} Variant437Tile;

/* One 0x90-byte eight-point disc (six at work + 0x2650, four at + 0x29B0). */
typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 color[4];
    u8 glow[4];
    s32 angle;
    s32 unknown8C;
} Variant437Disc;

/* The 0x90-byte five-point star at work + 0x2BF0. */
typedef struct {
    SVECTOR points[11];
    SVECTOR quad[4];
    u8 color[4];
    u8 glow[4];
    s32 unknown80[3];
    u8 unknown8C[4];
} Variant437Star;

typedef struct {
    Variant437Group groups[8];
    Variant437Ribbon ribbons[16];
    Variant437Cone cones[3];
    Variant437Beam beam;
    Variant437Tile tiles[2];
    Variant437Disc discs[6];
    Variant437Disc rings[4];
    Variant437Star star;
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    u8 unknown2D28[0x34];
    POLY_FT4 flat[2];
    POLY_FT4 bloom;
    u8 unknown2DD4[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR aim;
    VECTOR delta;
    u8 unknown2E40[0x10];
    DVECTOR projected;
    VECTOR view;
    s32 pitch;
    s32 yaw;
    s32 frame;
    u32 time;
    s32 last_frame;
    s32 step;
    s32 fade;
    Variant437Config *G32 config;
    u8 unknown2E84[4];
    GsCOORDUNIT *G32 parts[3];
    s16 unknown2E94, unknown2E96, unknown2E98, unknown2E9A;
    s32 unknown2E9C;
    s32 unknown2EA0;
    s32 phase;
    SVECTOR orbit;
    s32 ambient;
    s16 slot;
    s16 command;
} Variant437State;

extern GsIMAGE gModelVariant437_D188C[];
extern Variant437Config gModelVariant437_D1988[];

s32 ModelVariant437_HandleRequest(u8 *context, s32 command);
void FrenchModelVariant454_DrawBlooms(u8 *context);

#endif
