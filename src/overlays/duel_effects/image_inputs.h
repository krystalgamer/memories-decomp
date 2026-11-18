#ifndef MEMORIES_DECOMP_DUEL_EFFECT_IMAGE_INPUTS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_IMAGE_INPUTS_H

#include "../../types.h"
#include "../../psyq/libgs.h"

/* PAL loads the effect palettes at a different VRAM position. */
#ifdef VERSION_EUROPE
#define DUEL_EFFECT_CLUT(ntsc_x, ntsc_y, pal_x, pal_y) pal_x, pal_y
#else
#define DUEL_EFFECT_CLUT(ntsc_x, ntsc_y, pal_x, pal_y) ntsc_x, ntsc_y
#endif

/* Several effect units each define their own copy of this table. */
#define DUEL_EFFECT_TEXTURE_IMAGES { \
    { 9, 512, 256, 32, 64, NULL, DUEL_EFFECT_CLUT(0, 240, 640, 208), 256, 1, NULL }, \
    { 8, 544, 256, 8, 64, NULL, DUEL_EFFECT_CLUT(0, 241, 640, 209), 16, 1, NULL }, \
    { 9, 512, 320, 32, 64, NULL, DUEL_EFFECT_CLUT(0, 242, 640, 210), 256, 1, NULL }, \
    { 8, 552, 256, 16, 64, NULL, DUEL_EFFECT_CLUT(16, 241, 656, 209), 16, 1, NULL }, \
    { 8, 544, 320, 32, 64, NULL, DUEL_EFFECT_CLUT(32, 241, 672, 209), 16, 1, NULL }, \
    { 8, 512, 384, 16, 128, NULL, DUEL_EFFECT_CLUT(48, 241, 688, 209), 16, 1, NULL }, \
    { 8, 528, 384, 16, 128, NULL, DUEL_EFFECT_CLUT(64, 241, 704, 209), 16, 1, NULL }, \
    { 8, 544, 384, 16, 64, NULL, DUEL_EFFECT_CLUT(80, 241, 720, 209), 16, 1, NULL }, \
    { 8, 544, 448, 24, 64, NULL, DUEL_EFFECT_CLUT(96, 241, 736, 209), 16, 1, NULL }, \
    { 8, 568, 256, 8, 64, NULL, DUEL_EFFECT_CLUT(112, 241, 752, 209), 16, 1, NULL }, \
    { 8, 560, 384, 16, 64, NULL, DUEL_EFFECT_CLUT(128, 241, 768, 209), 16, 1, NULL }, \
    { 8, 576, 256, 8, 128, NULL, DUEL_EFFECT_CLUT(144, 241, 784, 209), 16, 1, NULL }, \
    { 8, 568, 448, 8, 32, NULL, DUEL_EFFECT_CLUT(160, 241, 800, 209), 16, 1, NULL }, \
    { 8, 584, 256, 16, 128, NULL, DUEL_EFFECT_CLUT(176, 241, 816, 209), 16, 1, NULL }, \
    { 9, 616, 256, 24, 128, NULL, DUEL_EFFECT_CLUT(192, 241, 832, 209), 256, 1, NULL }, \
    { 8, 600, 256, 16, 64, NULL, DUEL_EFFECT_CLUT(240, 241, 880, 209), 16, 1, NULL }, \
    { 8, 576, 384, 32, 64, NULL, DUEL_EFFECT_CLUT(0, 243, 640, 211), 16, 1, NULL }, \
    { 8, 600, 320, 16, 64, NULL, DUEL_EFFECT_CLUT(16, 243, 656, 211), 16, 1, NULL }, \
    { 8, 624, 384, 16, 128, NULL, DUEL_EFFECT_CLUT(32, 243, 672, 211), 16, 1, NULL }, \
    { 8, 612, 384, 12, 128, NULL, DUEL_EFFECT_CLUT(48, 243, 688, 211), 16, 1, NULL }, \
    { 8, 576, 448, 32, 64, NULL, DUEL_EFFECT_CLUT(64, 243, 704, 211), 16, 1, NULL }, \
}

/* Mode 20 overlaps the low halfword of the first variant image's pmode. */
typedef union {
    u16 modes[21];
    struct {
        u16 leading_modes[20];
        GsIMAGE images[5];
    } variant;
} DuelEffectImageInputs;

typedef char DuelEffectImageInputs_size_must_be_0xB4[
    sizeof(DuelEffectImageInputs) == 0xB4 ? 1 : -1
];
typedef char DuelEffectImageInputs_images_offset_must_be_0x28[
    OFFSETOF(DuelEffectImageInputs, variant.images) == 0x28 ? 1 : -1
];

#endif
