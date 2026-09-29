#ifndef MEMORIES_DECOMP_MODEL_VARIANT_H
#define MEMORIES_DECOMP_MODEL_VARIANT_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

/* The MODEL.MRG variant modules (stages 7-10 of Model_LoadMonsterMerge) were
 * compiled with the GCC 2.7.2 CDK compiler (profile gcc_2_7_2_cdk_g0). Their
 * helpers receive the module's work area as a byte pointer and reach its
 * fields at fixed offsets; the record arrays inside it are typed below. */
#define MODEL_VARIANT_WORD(p, o) (*(s32 *)((u8 *)(p) + (o)))

/* One 0x90-byte element of the quad array at work + 0x1B4C: a four-corner
 * quad whose two colours and growing size drive a Gouraud-shaded POLY_G4. */
typedef struct {
    u8 pad0[0x58];
    SVECTOR corner[4];
    u8 inner[4];
    u8 outer[4];
    s32 size;
    u8 pad84[0x0C];
} ModelVariantQuad;

/* One 0x90-byte element of the ring array at work + 0x15AC: eight spokes,
 * each drawn as a GsGLINE from inner[k] to outer[k]. */
typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 color[4];
    u8 pad84[4];
    s32 angle;
    s32 count;
} ModelVariantRing;

#endif
