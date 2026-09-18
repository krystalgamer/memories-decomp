#ifndef MEMORIES_DECOMP_MODEL_DISC_EFFECT_H
#define MEMORIES_DECOMP_MODEL_DISC_EFFECT_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "model_geometry_tables.h"

/* State for Model_UpdateDiscEffect, one of the four handlers in the pointer
   table at D_800114E8. arg1 >= 0 builds a disc of 67 points: a centre at a
   third of the radius and two 33-point rings at the radius and at four fifths
   of it, with the three decimal digits of arg1 turned into the centre colour.
   Negative arg1 draws the disc as 32 Gouraud triangles and advances level
   (-2) or scale (any other value). */
typedef struct {
    ModelDiscEffectConfig *table;
    SVECTOR points[67];
    u8 r;
    u8 g;
    u8 b;
    u8 pad21F;
    s32 unk220;
    s32 level;
    s32 scale;
    u8 frame;
} ModelDiscEffectState;

/* Sixteen bytes of rodata copied whole into the handler's scale vector. */
extern VECTOR D_8001187C;

s32 Model_UpdateDiscEffect(ModelDiscEffectState *arg0, s32 arg1);

#endif
