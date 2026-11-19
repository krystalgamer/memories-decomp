#ifndef MEMORIES_DECOMP_DUELIST_PORTRAIT_H
#define MEMORIES_DECOMP_DUELIST_PORTRAIT_H

#include "../types.h"

#define DUELIST_PORTRAIT_IMAGE_SIZE 0x900
#define DUELIST_PORTRAIT_CLUT_SIZE 0x80

/* One duelist portrait record as the WA.MRG portrait packages store it in
 * the shared work arena: a 48x48 8-bit image followed by its 64x1 CLUT row,
 * each uploaded with LoadImage2. Campaign_LoadScenePackage walks 25 of them
 * for the dialog portraits and FreeDuel_Init 40 for the duelist grid
 * (notes/mrg-files.md). */
typedef struct DuelistPortrait {
    u32 image[DUELIST_PORTRAIT_IMAGE_SIZE / sizeof(u32)];
    u32 clut[DUELIST_PORTRAIT_CLUT_SIZE / sizeof(u32)];
} DuelistPortrait;

typedef char DuelistPortrait_size_must_be_0x980[
    sizeof(DuelistPortrait) == 0x980 ? 1 : -1
];

#endif
