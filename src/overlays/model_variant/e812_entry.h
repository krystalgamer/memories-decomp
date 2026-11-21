#ifndef MEMORIES_DECOMP_MODEL_VARIANT629_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT629_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
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

/* One 0x60-byte element of the a (petal fans), b (shaded quads) and c
 * (rising sprites) arrays: an active flag, a colour faded over the second
 * half of its life, the texture coordinates of its sprite cell (read by c), the draw mode passed to
 * Graphics_SubmitDrawModePacket (b), the number of petal segments drawn so
 * far (a; grows to 9), its age and lifetime, its transform, a velocity (c
 * rises by velocity.vy per frame step) and the position c returns to each
 * time its sprite cell wraps. */
typedef struct {
    u8 active;
    u8 r, g, b;          /* 0x01 */
    u8 u;
    u8 v;
    u8 pad06[2];
    s32 mode;            /* 0x08 */
    s16 segments;        /* 0x0C */
    u16 elapsed;         /* 0x0E */
    u16 duration;        /* 0x10 */
    s16 pad12;
    GsCOORDUNIT *coord; /* 0x14 */
    SVECTOR rot;         /* 0x18 */
    VECTOR scale;        /* 0x20 */
    VECTOR pos;          /* 0x30 */
    VECTOR velocity;     /* 0x40 */
    VECTOR origin;       /* 0x50 */
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


extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant629_aImages[2];
extern Model629Config gModelVariant629_aConfigs[];

s32 ModelVariant629_HandleRequest(Model629State *context, s32 command);

#endif
