#ifndef MEMORIES_DECOMP_MODEL_BACKGROUND_H
#define MEMORIES_DECOMP_MODEL_BACKGROUND_H

#include "../types.h"
#include "../psyq/libgte.h"

typedef union {
    u16 packed;
    struct {
        u8 v;
        u8 upper;
    } bytes;
} ModelBackgroundVPage;

/* The selector also chooses the auxiliary model file; the scene controller
 * and VRAM animation step switch on the same byte. Flags bit 0 enables the
 * floor bands and shadow clipping, while bit 1 disables model shadows.
 * The renderer reads +0xAC both as a V byte and as packed texture-page bits. */
typedef struct ModelBackgroundRecord {
    u8 pad_00[0xA0];
    u8 selector;
    u8 flags;
    s16 shadow_y_offset;
    u16 stage_height;
    u16 texture_width;
    /* Low 13 bits bound vertical clipping; high bits select texture modes. */
    u16 field_A8;
    /* Low six bits feed U; the next four contribute to the texture page. */
    u16 field_AA;
    ModelBackgroundVPage texture_v_page;
    u16 clut_x;
    u16 clut_y;
} ModelBackgroundRecord;

typedef char ModelBackgroundVPage_size_must_be_2[
    sizeof(ModelBackgroundVPage) == 2 ? 1 : -1
];
typedef char ModelBackgroundVPage_v_offset_must_be_0[
    ((u32)&(((ModelBackgroundVPage *)0)->bytes.v)) == 0 ? 1 : -1
];
typedef char ModelBackgroundVPage_upper_offset_must_be_1[
    ((u32)&(((ModelBackgroundVPage *)0)->bytes.upper)) == 1 ? 1 : -1
];
typedef char ModelBackgroundRecord_selector_offset_must_be_0xA0[
    ((u32)&(((ModelBackgroundRecord *)0)->selector)) == 0xA0 ? 1 : -1
];
typedef char ModelBackgroundRecord_flags_offset_must_be_0xA1[
    ((u32)&(((ModelBackgroundRecord *)0)->flags)) == 0xA1 ? 1 : -1
];
typedef char ModelBackgroundRecord_shadow_y_offset_must_be_0xA2[
    ((u32)&(((ModelBackgroundRecord *)0)->shadow_y_offset)) == 0xA2 ? 1 : -1
];
typedef char ModelBackgroundRecord_stage_height_offset_must_be_0xA4[
    ((u32)&(((ModelBackgroundRecord *)0)->stage_height)) == 0xA4 ? 1 : -1
];
typedef char ModelBackgroundRecord_texture_width_offset_must_be_0xA6[
    ((u32)&(((ModelBackgroundRecord *)0)->texture_width)) == 0xA6 ? 1 : -1
];
typedef char ModelBackgroundRecord_field_A8_offset_must_be_0xA8[
    ((u32)&(((ModelBackgroundRecord *)0)->field_A8)) == 0xA8 ? 1 : -1
];
typedef char ModelBackgroundRecord_field_AA_offset_must_be_0xAA[
    ((u32)&(((ModelBackgroundRecord *)0)->field_AA)) == 0xAA ? 1 : -1
];
typedef char ModelBackgroundRecord_texture_v_page_offset_must_be_0xAC[
    ((u32)&(((ModelBackgroundRecord *)0)->texture_v_page)) == 0xAC ? 1 : -1
];
typedef char ModelBackgroundRecord_clut_x_offset_must_be_0xAE[
    ((u32)&(((ModelBackgroundRecord *)0)->clut_x)) == 0xAE ? 1 : -1
];
typedef char ModelBackgroundRecord_clut_y_offset_must_be_0xB0[
    ((u32)&(((ModelBackgroundRecord *)0)->clut_y)) == 0xB0 ? 1 : -1
];
typedef char ModelBackgroundRecord_size_must_be_0xB2[
    sizeof(ModelBackgroundRecord) == 0xB2 ? 1 : -1
];

typedef struct {
    SVECTOR values[2];
} BackgroundNormals;

/* Draws the model-scene backdrop. Unless slot 2 is inactive (is_active) or
 * its three ambient words are all -4096, it calls
 * Model_UpdateBackgroundScroll(0) to update the scroll, lights a colour from
 * slot 2's flat lights and ambient and, unless the vertical offset
 * gModel_wBackgroundOffsetY clips the band away, sorts 64-wide GsSPRITE tiles of up to 256
 * rows across the 320-pixel screen from x = gModel_wBackgroundScrollX / 10 less a
 * width-derived base, wrapping U within the record's texture width. If bit 0
 * of record byte 0xA1 is set it then sorts sixteen 256-step segments of three
 * Gouraud POLY_G4 floor bands (radii 1000-1600-2200-2800) at slot 2's
 * coordinate Y + 300, shaded from the dimmer colour constants down to black.
 *
 * Draws from model slot 2 and the selected 0xB2-byte model record.
 * Both the active record and slot coordinate must be valid before the
 * active-byte guard. Texture depth must be 0..2, a visible tile period
 * nonzero, and ordering-table length below 32. See
 * notes/model-background-renderer.md for the measured input contract. */
void Model_DrawBackground(void);

#endif
