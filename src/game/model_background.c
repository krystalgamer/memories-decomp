#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_background.h"
#include "screen_projection.h"
#include "model_graphics_state.h"
#include "model_update_background_scroll.h"
#include "camera_view.h"
#include "ordering_tables.h"

#define CVECTOR_VIEW(color) ((CVECTOR *)(color))

#ifndef BACKGROUND_RENDER_CLIP_LIMIT
#define BACKGROUND_RENDER_CLIP_LIMIT 241
#endif

void Model_DrawBackground(void)
{
    CVECTOR colors[5] = {
        {128, 128, 128, 0},
        {96, 64, 64, 0},
        {64, 42, 42, 0},
        {32, 21, 21, 0},
        {0, 0, 0, 0}
    };
    static const BackgroundNormals initial_normals = {
        {{4096, 0, 0, 0}, {0, -4096, 0, 0}}
    };
    BackgroundNormals normals = initial_normals;
    SVECTOR vertices[8];
    GsSPRITE sprite;
    POLY_G4 polygons[3];
    PSXLONG projection[4];
    ModelSlot *slot = &gModel_aSlots[2];
    u8 vbase;
    s32 height;
    s32 ubase;
    s16 y;
    u16 pitch_bits;
    s32 minimum_y;
    s32 amount;
    s32 phase;
    s32 cosine;
    s32 sine;
    s32 first_depth;
    s32 second_depth;

    vbase = gModel_pBackgroundRecord->texture_v_page.bytes.v;
    ubase = (gModel_pBackgroundRecord->texture_u_page & 0x3F) <<
        (2 - ((gModel_pBackgroundRecord->texture_height_flags >> 13) & 3));
    height = slot->field_D18->matrix.t[1] + 300;
    if (!slot->is_active)
        return;
    if (slot->ambient_rgb_scale[0] == -4096 &&
        slot->ambient_rgb_scale[1] == -4096 && slot->ambient_rgb_scale[2] == -4096)
        return;
    Model_UpdateBackgroundScroll(0);
    sprite.attribute = 0;
    if (gModel_pBackgroundRecord->texture_height_flags & 0xE000)
        sprite.attribute = 0x01000000;
    GsSetFlatLight(0, (GsF_LIGHT *)&slot->field_D70[0]);
    GsSetFlatLight(1, (GsF_LIGHT *)&slot->field_D70[1]);
    GsSetFlatLight(2, (GsF_LIGHT *)&slot->field_D70[2]);
    GsSetAmbient(slot->ambient_rgb_scale[0], slot->ambient_rgb_scale[1], slot->ambient_rgb_scale[2]);
    GsSetLightMatrix(&gModel_CameraCoordinateUnit.matrix);
    NormalColorCol(&normals.values[0], &colors[0], CVECTOR_VIEW(&sprite.r));
    sprite.tpage = ((gModel_pBackgroundRecord->texture_height_flags >> 6) & 0x180) |
        ((gModel_pBackgroundRecord->texture_v_page.packed & 0x100) >> 4) |
        ((gModel_pBackgroundRecord->texture_u_page & 0x3FF) >> 6) |
        ((gModel_pBackgroundRecord->texture_v_page.packed & 0x200) << 2);
    sprite.cx = gModel_pBackgroundRecord->clut_x;
    sprite.cy = gModel_pBackgroundRecord->clut_y;
    sprite.w = 64;
    pitch_bits = *(u16 *)&gModel_wBackgroundOffsetY;
    y = pitch_bits - 48;
    sprite.u = ubase;
    sprite.v = vbase;
    sprite.h = 256;
    sprite.y = y;
    minimum_y = -(gModel_pBackgroundRecord->texture_height_flags & 0x1FFF);
    if ((s16)pitch_bits >= minimum_y && y >= minimum_y) {
        if (y > 0)
            sprite.y = 0;
        if (sprite.y < 0) {
            sprite.v = __builtin_abs(sprite.y);
            sprite.y = 0;
        }
        {
            s32 end = sprite.y + 256;
            s32 edge = (gModel_pBackgroundRecord->texture_height_flags & 0x1FFF) + (s16)pitch_bits;
            s32 full_height;
            s32 amount;
            if (edge < BACKGROUND_RENDER_CLIP_LIMIT) {
                if (edge >= end)
                    goto second_clip;
            } else if (end < BACKGROUND_RENDER_CLIP_LIMIT) {
                goto second_clip;
            }
            full_height = sprite.h;
            amount = sprite.y + full_height -
                ((gModel_pBackgroundRecord->texture_height_flags & 0x1FFF) + gModel_wBackgroundOffsetY);
            sprite.h = full_height - (amount < full_height ? amount : full_height);
        }
second_clip:
        {
            s32 end = sprite.v + sprite.h;
            s32 edge = gModel_pBackgroundRecord->texture_height_flags & 0x1FFF;
            s32 full_height;
            s32 amount;
            if ((u32)edge < BACKGROUND_RENDER_CLIP_LIMIT) {
                if (edge < end)
                    goto trim_texture;
                goto tiles;
            }
            if (end < BACKGROUND_RENDER_CLIP_LIMIT)
                goto tiles;
trim_texture:
            full_height = sprite.h;
            amount =
                sprite.v + full_height - (gModel_pBackgroundRecord->texture_height_flags & 0x1FFF);
            sprite.h = full_height - (amount < full_height ? amount : full_height);
        }
tiles:
        sprite.x = (s16)*(u16 *)&gModel_wBackgroundScrollX / 10 -
            160 * (u16)(gModel_pBackgroundRecord->texture_width / 0x500u);
        while (sprite.x < 320) {
            if (sprite.x + sprite.w > 0)
                GsSortFastSprite(&sprite, gGraphics_apOrderingTables[3],
                    (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
            amount = (sprite.u + sprite.w) %
                (u16)(gModel_pBackgroundRecord->texture_width / 10u);
            sprite.x += sprite.w;
            sprite.u = amount + ubase;
        }
    }
    if (!(gModel_pBackgroundRecord->flags & 1))
        return;
    NormalColorCol(&normals.values[1], &colors[1],
                   CVECTOR_VIEW(&polygons[0].r0));
    NormalColorCol(&normals.values[1], &colors[1],
                   CVECTOR_VIEW(&polygons[0].r1));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[0].r2));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[0].r3));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[1].r0));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[1].r1));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[1].r2));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[1].r3));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[2].r0));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[2].r1));
    NormalColorCol(&normals.values[1], &colors[4],
                   CVECTOR_VIEW(&polygons[2].r2));
    NormalColorCol(&normals.values[1], &colors[4],
                   CVECTOR_VIEW(&polygons[2].r3));
    setPolyG4(&polygons[0]);
    setPolyG4(&polygons[1]);
    setPolyG4(&polygons[2]);
    vertices[0].vx = 1000;
    vertices[2].vx = 1600;
    vertices[4].vx = 2200;
    vertices[0].vy = height;
    vertices[0].vz = 0;
    vertices[2].vy = height;
    vertices[2].vz = 0;
    vertices[4].vy = height;
    vertices[4].vz = 0;
    vertices[6].vx = 2800;
    vertices[6].vy = height;
    vertices[6].vz = 0;
    vertices[1].vy = height;
    vertices[3].vy = height;
    vertices[5].vy = height;
    vertices[7].vy = height;
    GsSetLsMatrix(Model_GetLightSourceMatrix());
    phase = 256;
    do {
        cosine = rcos(phase);
        sine = rsin(phase);
        vertices[1].vx = 1000 * cosine / 4096;
        vertices[1].vz = 1000 * sine / 4096;
        vertices[3].vx = 1600 * cosine / 4096;
        vertices[3].vz = 1600 * sine / 4096;
        vertices[5].vx = 2200 * cosine / 4096;
        vertices[5].vz = 2200 * sine / 4096;
        vertices[7].vx = 2800 * cosine / 4096;
        vertices[7].vz = 2800 * sine / 4096;
        first_depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&polygons[0].x0, (PSXLONG *)&polygons[0].x1,
            (PSXLONG *)&polygons[0].x2, (PSXLONG *)&polygons[0].x3,
            &projection[0], &projection[1]);
        second_depth = RotAverage4(&vertices[4], &vertices[5], &vertices[6], &vertices[7],
            (PSXLONG *)&polygons[2].x0, (PSXLONG *)&polygons[2].x1,
            (PSXLONG *)&polygons[2].x2, (PSXLONG *)&polygons[2].x3,
            &projection[2], &projection[3]);
        polygons[1].x0 = (u16)polygons[0].x2;
        polygons[1].y0 = (u16)polygons[0].y2;
        polygons[1].x1 = (u16)polygons[0].x3;
        polygons[1].y1 = (u16)polygons[0].y3;
        polygons[1].x2 = (u16)polygons[2].x0;
        polygons[1].y2 = (u16)polygons[2].y0;
        polygons[1].x3 = (u16)polygons[2].x1;
        polygons[1].y3 = (u16)polygons[2].y1;
        if (first_depth >= 0) {
            GsSortPoly(&polygons[0], gGraphics_apOrderingTables[3],
                (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
        } else if (second_depth < 0) {
            goto advance;
        }
        GsSortPoly(&polygons[1], gGraphics_apOrderingTables[3],
            (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
        if (second_depth >= 0)
            GsSortPoly(&polygons[2], gGraphics_apOrderingTables[3],
                (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
advance:
        phase += 256;
        vertices[0] = vertices[1];
        vertices[2] = vertices[3];
        vertices[4] = vertices[5];
        vertices[6] = vertices[7];
    } while (phase <= 4096);
}

/* These historical COMMON definitions retain the post-mfhi delay slot.
 * The graphics state binds to its strong .sdata owner and the snapshot
 * names to existing image/layout symbols; they add no replacement storage.
 * The named profile's --use-comm-section spelling is load-bearing. */
ModelBackgroundRecord *G32 gModel_pBackgroundRecord;
s16 gModel_wBackgroundScrollX;
s16 gModel_wBackgroundOffsetY;

ModelBytes8 gModel_wViewDistance;
s16 gModel_wViewYaw;
s16 gModel_wViewPitch;
ModelBytes8 gModel_ViewSnapshot;
s16 gModel_wViewYawSnapshot;
s16 gModel_wViewPitchSnapshot;

/*
 * Direct signed angle differences and the semantic absolute-value predicate
 * recover the initial loads and branch-delay copy without register bindings.
 * Each difference is between signed halfwords, so abs cannot receive INT_MIN.
 * All 496 bytes match with the existing uniform G8 split COMMON profile.
 */
void Model_UpdateBackgroundScroll(s32 force)
{
    s32 dy;
    s32 dz;
    s32 t;
    s32 a;
    s32 b;
    s32 u;
    s32 r;
    s32 m;

    dy = gModel_wViewYaw - gModel_wViewYawSnapshot;
    dz = gModel_wViewPitch - gModel_wViewPitchSnapshot;
    if (__builtin_abs(dy) >= 0x801) {
        if (dy > 0) {
            dy -= MODEL_ANGLE_FULL_TURN;
        } else {
            dy += MODEL_ANGLE_FULL_TURN;
        }
    }
    if (force != 0 || dy != 0) {
        m = gModel_pBackgroundRecord->texture_width;
        gModel_wBackgroundScrollX = (dy * 1280 / 384 + gModel_wBackgroundScrollX + m) % m;
    }
    if (force != 0 || dz != 0) {
        a = gModel_wViewPitch;
        if (a < 0x801) {
            goto low;
        }
        t = a - MODEL_ANGLE_FULL_TURN;
        if (t < 0) {
            goto neg_side;
        }
        r = -0x139;
        if (t < MODEL_ANGLE_QUARTER_TURN) {
            goto compute;
        }
        goto store;
neg_side:
        r = -0x139;
        if (MODEL_ANGLE_FULL_TURN - a >= MODEL_ANGLE_QUARTER_TURN) {
            goto store;
        }
compute:
        b = gModel_wViewPitch;
        u = b - MODEL_ANGLE_FULL_TURN;
        if (u < 0) {
            goto sub_form;
        }
        r = -(u * 26) / 85;
        goto store;
sub_form:
        r = -((MODEL_ANGLE_FULL_TURN - b) * 26) / 85;
        goto store;
low:
        if (a >= MODEL_ANGLE_QUARTER_TURN) {
            goto clamp_pos;
        }
        r = (a * 26) / 85;
        goto store;
clamp_pos:
        r = 0x139;
store:
        gModel_wBackgroundOffsetY = r;
    }
    gModel_ViewSnapshot = gModel_wViewDistance;
}
