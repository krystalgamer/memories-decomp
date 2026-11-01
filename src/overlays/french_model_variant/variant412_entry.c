#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant412_entry.h"

#ifndef MODEL_VARIANT412_CLUT_X
#define MODEL_VARIANT412_CLUT_X 640
#endif

s32 FrenchModelVariant412_HandleRequest(SVECTOR *point, s32 command)
{
    Variant412EntryState *work = (Variant412EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1, unused_matrix2;
    SVECTOR position, target;
    VECTOR squared;
    Variant402EntryProjection projection;
    Variant412Trail *trail = work->trails;
    Variant402EntryRecord *record = work->records;
    Variant402EntryRing *ring = work->rings;
    Family402Band *band = work->bands;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *band_packet = &work->band_packet;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    s32 i, j, n, ring_index, ring_point, angle, scale;
    s32 r, b;
    u16 tpage, clut, unknown_page, band_texture, flat_texture;
    s32 flat_page, band_page, packed;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant412EntryConfig *)(gFrenchModelVariant412_D27A8 + 0xFC) + command;
        work->root_part = (GsCOORDINATE2 *)Model_GetSlotDataEntry(work->slot, work->config->root_part);
        for (i = 0; i < 6; i++) {
            work->parts[i] = (GsCOORDINATE2 *)Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLw(work->root_part, &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT412_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT412_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant412_D27A8 + 0xC4));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant412_D27A8 + 0x8C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant412_D27A8 + 0x1C));
        band_page = packed >> 16;
        band_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant412_D27A8 + 0x54));
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = tpage;
        setUV4(textured, 126, 0, 127, 0, 126, 63, 127, 63);
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        textured++;
        SetPolyGT4(textured);
        textured->tpage = tpage;
        setUV4(textured, 64, 0, 127, 0, 64, 63, 127, 63);
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 0, 128, 0, 175, 48, 128, 48, 175);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 0, 176, 0, 223, 48, 176, 48, 223);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        SetPolyGT4(extra);
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(band_packet);
        band_packet->tpage = unknown_page;
        setUV4(band_packet, 0, 96, 0, 127, 127, 96, 127, 127);
        /* Retail reads an unwritten stack halfword before replacing this packet. */
        band_packet->clut = unused_matrix1.t[2];
        SetSemiTrans(band_packet, 1);
        SetShadeTex(band_packet, 0);
        SetPolyGT4(band_packet);
        band_packet->tpage = band_page;
        setUV4(band_packet, 128, 207, 255, 207, 128, 128, 255, 128);
        band_packet->clut = band_texture;
        SetSemiTrans(band_packet, 1);
        SetShadeTex(band_packet, 0);

        for (n = 0; n < 1; n++, record++) {
            record->field_48 = 0;
            record->field_4C = 0;
            record->field_4A = 0;
        }
        for (n = 0; n < 1; n++, trail++) {
            for (i = 0; i < 6; i++) {
                r = (5 - i) * 128 / 5;
                b = r + i * 128 / 5;
                for (j = 0; j < 30; j++) {
                    setVector(&trail->original[i][j], 0, 0, 0);
                    setVector(&trail->moved[i][j], 0, 0, 0);
                    trail->inner[i][j].r = r;
                    trail->inner[i][j].g = r;
                    trail->inner[i][j].b = b;
                    trail->outer[i][j].r = r;
                    trail->outer[i][j].g = r;
                    trail->outer[i][j].b = b;
                }
            }
            for (j = 0; j < 30; j++) {
                trail->progress[j] = 0;
                trail->angle[j] = 0;
            }
        }
        for (ring_index = 0; ring_index < 2; ring_index++, ring++) {
            for (ring_point = 0; ring_point < 4; ring_point++) {
                if (ring_point == 0) {
                    setVector(&ring->points[ring_point], -128, -128, 0);
                    setVector(&ring->points[ring_point + 4], 0, -128, 0);
                    setVector(&ring->points[ring_point + 8], -128, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 1) {
                    setVector(&ring->points[ring_point], 128, -128, 0);
                    setVector(&ring->points[ring_point + 4], 128, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, -128, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 2) {
                    setVector(&ring->points[ring_point], 128, 128, 0);
                    setVector(&ring->points[ring_point + 4], 0, 128, 0);
                    setVector(&ring->points[ring_point + 8], 128, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 3) {
                    setVector(&ring->points[ring_point], -128, 128, 0);
                    setVector(&ring->points[ring_point + 4], -128, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, 128, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                }
            }
            ring->field_80 = 0;
            ring->field_84 = 0;
            ring->field_88 = 0;
        }
        for (ring_index = 0, scale = 0; ring_index < 2; ring_index++, band++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                band->inner[j].vx = (u32)rcos(angle) >> 4;
                band->inner[j].vy = (u32)rsin(angle) >> 4;
                band->inner[j].vz = 0;
                setVector(&band->outer[j], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, -192);
            }
            band->scale = scale;
            band->cycles = 0;
            scale -= 2048;
        }
        work->width = 1024;
        work->field_1B5E = 0;
        work->inner.r = 192;
        work->inner.g = 192;
        work->inner.b = 192;
        work->outer.r = 0;
        work->outer.g = 0;
        work->outer.b = 0;
        work->field_1B54 = 0;
        work->field_1B56 = 0;
        work->field_1B58 = 0;
        work->field_1B5A = 0;
        work->field_1B60 = 0;
        work->field_1B62 = 0;
        work->frame_count = 0;
        work->current_animation_frame = 0;
        work->elapsed = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->captured_count = 0;
        work->field_1B1C = 0;
        work->fade = 0;
        work->tint = 2048;
        work->phase = 0;
        work->field_1B68 = 0;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        work->current_animation_frame = Model_GetSlotAnimationFrame(work->slot);
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLw(work->root_part, &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        dx = (u16)projection.target.vx;
        work->part_index = 0;
        work->screen_delta.vx = dx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->part_count != 0) {
            do {
                GsGetLw(work->root_part, &work->matrix);
                work->direction.vx = work->target.vx - work->matrix.t[0];
                work->direction.vy = work->target.vy - work->matrix.t[1];
                work->direction.vz = work->target.vz - work->matrix.t[2];
                work->part_index++;
            } while (work->part_index < work->config->part_count);
        }
        if (work->config->capture_start <= work->elapsed) {
            FrenchModelVariant412_DrawTrails((u8 *)point);
        }
        work->frame_count++;
        if (work->current_animation_frame != work->animation_frame) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = work->current_animation_frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 2) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->config->duration <= work->elapsed && work->phase == 0) {
        result = 4;
    } else if (work->phase == 1) {
        result = 1;
        work->phase = 2;
    } else if (work->phase == 2) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 3;
            }
        }
    }
    if (work->phase == 3) {
        result = 2;
    }
    return result;
}
