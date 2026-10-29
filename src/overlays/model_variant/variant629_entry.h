#ifndef MEMORIES_DECOMP_MODEL_VARIANT629_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT629_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/graphics_packet_submission.h"

/* One 0xE-byte command row, indexed by command 0..4. */
typedef struct {
    u16 duration;   /* 0x0 -> work 0x8 */
    u16 width;      /* 0x2 -> work 0xA */
    u16 height;     /* 0x4 -> work 0xC */
    u16 radius;     /* 0x6 -> work 0xE */
    u8 color_b[3];  /* 0x8 -> elements b */
    u8 color_a[3];  /* 0xB -> elements a and c */
} Model629Config;

typedef struct {
    u8 f00;
    u8 r, g, b;          /* 0x01 */
    u8 u;
    u8 v;
    u8 pad06[2];
    s32 f08;             /* 0x08 */
    s16 f0C;
    u16 f0E;
    u16 f10;
    s16 pad12;
    GsCOORDUNIT *G32 coord; /* 0x14 */
    SVECTOR rot;         /* 0x18 */
    VECTOR scale;        /* 0x20 */
    VECTOR pos;          /* 0x30 */
    VECTOR v40;          /* 0x40 */
    VECTOR v50;          /* 0x50 */
} Model629Element;       /* 0x60 */

typedef struct {
    SVECTOR sv[2];
} Model629Pair;          /* 0x10 */

typedef struct {
    SVECTOR p[2][2];
} Model629Petal;         /* 0x20 */

typedef struct {
    u8 command;          /* 0x000 */
    u8 pad001;
    u16 frame;           /* 0x002 */
    s32 slot;            /* 0x004 */
    s16 duration;        /* 0x008 */
    u16 width;           /* 0x00A */
    u16 height;          /* 0x00C */
    u16 radius;          /* 0x00E */
    u16 tpage0;          /* 0x010 */
    u16 tpage1;          /* 0x012 */
    u16 pad014;
    u16 clut0;           /* 0x016 */
    u16 clut1;           /* 0x018 */
    u16 pad01A;
    POLY_G4 g4;          /* 0x01C */
    POLY_GT4 poly;       /* 0x040 */
    POLY_FT4 ft4;        /* 0x074 */
    SVECTOR quad[4];     /* 0x09C */
    Model629Pair pairs[9];   /* 0x0BC */
    Model629Petal petals[10]; /* 0x14C */
    MATRIX mats[9];      /* 0x28C */
    u8 flags[5];         /* 0x3AC */
    u8 count_a;          /* 0x3B1 */
    u8 count_b;          /* 0x3B2 */
    u8 pad3B3;
    Model629Element a[10];   /* 0x3B4 */
    Model629Element b[8];    /* 0x774 */
    Model629Element c[32];   /* 0xA74 */
} Model629State;

typedef struct {
    u16 distance;
    s16 yaw;
    s16 pitch;
    u16 pad;
} Model629ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013DC80[2];
extern Model629Config D_8013DCB8[];

s32 func_8013B004(u8 *context, s32 command);

#endif
