#ifndef MEMORIES_DECOMP_MODEL_BACKGROUND_H
#define MEMORIES_DECOMP_MODEL_BACKGROUND_H

#include "../types.h"
#include "../psyq/libgte.h"

/* Selected 0xB2-byte background record at D_8009AF88. The background
 * renderer and angle updater both use the texture width at +0xA6. */
typedef struct {
    u8 pad_00[0xA6];
    u16 texture_width;
    u8 pad_A8[0x0A];
} ModelBackgroundRecord;

typedef char ModelBackgroundRecord_texture_width_offset_must_be_0xA6[
    ((u32)&(((ModelBackgroundRecord *)0)->texture_width)) == 0xA6 ? 1 : -1
];
typedef char ModelBackgroundRecord_size_must_be_0xB2[
    sizeof(ModelBackgroundRecord) == 0xB2 ? 1 : -1
];

typedef struct {
    SVECTOR values[2];
} BackgroundNormals;

/* Draws from model slot 2 and the selected 0xB2-byte model record.
 * Both the active record and slot coordinate must be valid before the
 * active-byte guard. Texture depth must be 0..2, a visible tile period
 * nonzero, and ordering-table length below 32. See
 * notes/model-background-renderer.md for the measured input contract. */
void func_8004DE24(void);

#endif
