#ifndef SPANISH_MODEL_VARIANT443_MESH_H
#define SPANISH_MODEL_VARIANT443_MESH_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[9][9];
    u8 unknown_288[0x288];
    CVECTOR colors[9];
    u8 unknown_534[0x18];
} Mesh443;

typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} Companion443;

typedef struct {
    u8 unknown_00[0x48];
    u32 grow_start;
    u32 grow_end;
} Timing443;

typedef struct {
    u8 unknown_00[0xC80];
    Mesh443 mesh[1];
    u8 unknown_11CC[0x189C];
    Companion443 companion;
    u8 unknown_2AF4[0x8B8];
    POLY_G4 quad;
    u8 unknown_33D0[0x1D0];
    s32 origin[3];
    u8 unknown_35AC[0xC];
    s32 path[3];
    u8 unknown_35C4[0x34];
    DVECTOR projected;
    u8 unknown_35FC[0xC];
    s32 direction[3];
    u8 unknown_3614[0x10];
    u32 time;
    u8 unknown_3628[4];
    u32 step;
    u8 unknown_3630[4];
    Timing443 *G32 timing;
    u8 unknown_3638[0x1C];
    s32 progress;
    s32 angle;
    u8 unknown_365C[0x3C];
    s32 phase;
} Mesh443State;

#endif
