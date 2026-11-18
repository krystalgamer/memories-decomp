#ifndef MEMORIES_DECOMP_DUEL_EFFECT_DRAWING_H
#define MEMORIES_DECOMP_DUEL_EFFECT_DRAWING_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/display_object_system.h"
#include "drawing_helpers.h"
#include "packet_helpers.h"
#include "textured_quads.h"

typedef struct {
    POLY_GT4 polygon;
    u8 alignment_gap[4];
    SVECTOR vertices[4];
} DuelEffectGradientQuad;

/* Primitives paired with their word layout, as display_object_system.h's
   DisplayObject*Packet unions: the GTE projections store each screen x/y
   pair through a long *. */
typedef union {
    POLY_G3 prim;
    struct {
        u32 tag;
        u32 rgb0;
        long xy0;
        u32 rgb1;
        long xy1;
        u32 rgb2;
        long xy2;
    } w;
} DuelEffectG3Packet;

typedef union {
    POLY_F4 prim;
    struct {
        u32 tag;
        u32 rgb0;
        long xy0;
        long xy1;
        long xy2;
        long xy3;
    } w;
} DuelEffectF4Packet;

typedef union {
    GsGLINE prim;
    struct {
        u32 attribute;
        long xy0;
        long xy1;
    } w;
} DuelEffectGLinePacket;

#endif
