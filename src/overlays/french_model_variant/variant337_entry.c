#include "../../types.h"
#include "variant337_entry.h"

#ifndef MODEL_VARIANT337_CLUT_X
#define MODEL_VARIANT337_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant337EntryState *work = (Variant337EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant337EntryRecord *record = work->records;
    Variant337EntryRing *ring = work->rings;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
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
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant337EntryConfig *)(D_8013CF5C + 0xFC) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT337_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT337_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CF5C + 0xC4));
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CF5C + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CF5C + 0x1C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CF5C + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CF5C + 0x54));
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
            ring->inner.r = work->config->inner.r;
            ring->inner.g = work->config->inner.g;
            ring->inner.b = work->config->inner.b;
            ring->outer.r = work->config->outer.r;
            ring->outer.g = work->config->outer.g;
            ring->outer.b = work->config->outer.b;
            ring->scale = 0;
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
        work->field_D4C = 1024;
        work->field_D40 = 0;
        work->field_D42 = 0;
        work->field_D44 = 0;
        work->field_D5C = 0;
        work->field_D60 = 0;
        work->field_D64 = 0;
        work->field_D68 = 0;
        work->field_D50 = 0;
        work->field_D54 = 0;
        work->field_D58 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->fade = 0;
        work->field_D78 = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        work->view_delta.vx = view->vrx - view->vpx;
        work->view_delta.vy = view->vry - view->vpy;
        work->view_delta.vz = view->vrz - view->vpz;
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        work->screen_delta.vx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->start <= work->frame) {
            func_8013B9A4((u8 *)point);
            func_8013C278((u8 *)point);
        }
        if (work->phase >= 3) {
            func_8013C738((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    func_800595C8(2, work->field_D78, work->field_D78, work->field_D78);
    if (work->phase < 6) {
        if (work->field_D78 > 0) {
            work->field_D78 -= 128;
            if (work->field_D78 <= 0) {
                work->field_D78 = 0;
            }
        }
    } else {
        work->field_D78 = work->fade << 5;
    }
    if (work->phase >= 2 && work->phase <= 4) {
        result = 4;
    } else if (work->phase == 5) {
        result = 1;
        work->phase = 6;
    } else if (work->phase == 6) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 7;
            }
        }
    } else if (work->phase == 7) {
        result = 2;
    }
    return result;
}
