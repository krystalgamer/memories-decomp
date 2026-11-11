#include "../../types.h"
#include "variant337_entry.h"
#include "variant337_entry_calls.h"

#ifndef MODEL_VARIANT337_CLUT_X
#define MODEL_VARIANT337_CLUT_X 640
#endif

s32 FrenchModelVariant337_HandleRequest(SVECTOR *point, s32 command)
{
    Variant337EntryState *work = (Variant337EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ModelVariantProjection projection;
    Variant337EntryRecord *record = work->records;
    ModelVariantPointSheet *ring = work->rings;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->packets.textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->packets.triangle;
    POLY_G4 *quad = &work->packets.quad;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j, angle;
    u16 tpage, clut;
    s32 flat_texture, flat_page;
    s32 packed;
    GsRVIEW2 *view;
    s16 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dy;

    if (command >= 0) {
        work->tail.slot = Model_GetActiveSlotIndex();
        work->config = gFrenchModelVariant337_aConfigs + command;
        work->part = Model_GetSlotDataEntry(work->tail.slot, work->config->part);
        if (work->tail.slot == 0) {
            setVector(&work->aim.target, 0, -300, -450);
        } else {
            setVector(&work->aim.target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->aim.direction.vx = work->aim.target.vx - work->matrix.t[0];
        work->aim.direction.vy = work->aim.target.vy - work->matrix.t[1];
        work->aim.direction.vz = work->aim.target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT337_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT337_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant337_aImages[7]);
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant337_aImages[5]);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant337_aImages[1]);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant337_aImages[2]);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant337_aImages[3]);
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
        for (i = 0; i < 1; i++, record++) {
            setVector(&record->scale, 4096, 4096, 4096);
            record->inner.r = 192;
            record->inner.g = 192;
            record->inner.b = 192;
            record->outer.r = 0;
            record->outer.g = 16;
            record->outer.b = 128;
            record->field_238 = 0;
            record->field_23C = 0;
            record->field_23A = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = 1024 + (i + 1) * 512;
            } else {
                angle = 1024 - i * 512;
            }
            record->velocity.vx = (rcos(angle) << 8) >> 12;
            record->velocity.vy = (rsin(angle) << 8) >> 12;
            record->velocity.vz = 0;
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
            ring->outer.r = work->config->inner.r;
            ring->outer.g = work->config->inner.g;
            ring->outer.b = work->config->inner.b;
            ring->inner.r = work->config->outer.r;
            ring->inner.g = work->config->outer.g;
            ring->inner.b = work->config->outer.b;
            ring->size = 0;
            ring->field_8C = 0;
            ring->field_90 = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->width = 1024;
        work->field_D48 = 1024;
        work->streamer.length = 1024;
        work->ribbon_length = 0;
        work->field_D42 = 0;
        work->field_D44 = 0;
        work->ribbon_wave = 0;
        work->ribbon_phase = 0;
        work->spin = 0;
        work->field_D68 = 0;
        work->streamer.rotation_x = 0;
        work->streamer.rotation_y = 0;
        work->streamer.rotation_z = 0;
        work->clock.frame_count = 0;
        work->clock.frame = 0;
        work->clock.animation_frame = 0;
        work->clock.step = 0;
        work->tail.phase = 0;
        work->clock.fade = 0;
        work->tail.tint = 2048;
        work->tail.command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        work->aim.view_delta.vx = view->vrx - view->vpx;
        work->aim.view_delta.vy = view->vry - view->vpy;
        work->aim.view_delta.vz = view->vrz - view->vpz;
        Square0(&work->aim.view_delta, &squared);
        work->clock.angles[0] = ratan2(work->aim.view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[1] = -ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) - 1024;
        if (work->tail.slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->aim.target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->aim.target);
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->aim.direction.vx = work->aim.target.vx - work->matrix.t[0];
        work->aim.direction.vy = work->aim.target.vy - work->matrix.t[1];
        work->aim.direction.vz = work->aim.target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (long *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->aim.target.vx, work->aim.target.vy, work->aim.target.vz);
        RotTransPers(&target, (long *)&projection.target,
                     &projection.interpolation, &projection.flag);
        work->aim.screen_delta.vx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->aim.screen_delta.vy = dy;
        if (work->config->start <= work->clock.frame) {
            ModelVariant320_DrawRibbon((u8 *)point);
            SpanishModelVariant337_DrawRings((u8 *)point);
        }
        if (work->tail.phase >= 3) {
            ModelVariant320_DrawStreamers((u8 *)point);
        }
        work->clock.frame_count++;
        frame = Model_GetSlotAnimationFrame(work->tail.slot);
        if (frame != work->clock.animation_frame) {
            work->clock.frame += Model_GetFrameStep();
            work->clock.step = Model_GetFrameStep();
            work->clock.animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tail.tint, work->tail.tint, work->tail.tint);
    if (work->tail.phase < 6) {
        if (work->tail.tint > 0) {
            work->tail.tint -= 128;
            if (work->tail.tint <= 0) {
                work->tail.tint = 0;
            }
        }
    } else {
        work->tail.tint = work->clock.fade << 5;
    }
    if (work->tail.phase >= 2 && work->tail.phase <= 4) {
        result = 4;
    } else if (work->tail.phase == 5) {
        result = 1;
        work->tail.phase = 6;
    } else if (work->tail.phase == 6) {
        if (work->clock.fade < 64) {
            work->clock.fade += work->clock.step;
            if (work->clock.fade >= 64) {
                work->clock.fade = 64;
                work->tail.phase = 7;
            }
        }
    } else if (work->tail.phase == 7) {
        result = 2;
    }
    return result;
}
