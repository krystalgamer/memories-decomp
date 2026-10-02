#include "../../types.h"
#include "variant338_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant338EntryState *work = (Variant338EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant338EntryProjection projection;
    Variant337EntryRecord *record = work->records;
    Variant337EntryRing *ring = work->rings;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_FT4 *extra = work->extra_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j, angle;
    u16 tpage, clut;
    u16 flat_texture;
    s32 packed;
    s32 flat_page, extra_page;
    GsRVIEW2 *view;
    s16 screen_x, dx;
    s32 screen_y;
    u32 frame;
    s32 dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant338EntryConfig *)(D_8013D70C + 0xFC) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        if (work->config->mode == 1) {
            func_800593D0(work->slot, work->config->part, work->config->vertex,
                          &work->vertex_position);
            work->matrix.t[0] = work->vertex_position.vx;
            work->matrix.t[1] = work->vertex_position.vy;
            work->matrix.t[2] = work->vertex_position.vz;
        } else {
            GsGetLwUnit(work->part, &work->matrix);
        }
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D70C + 0xC4));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D70C + 0x8C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D70C + 0x1C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D70C + 0x38));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D70C + 0x54));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D70C + 0xE0));
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
        SetPolyFT4(extra);
        extra_page = packed >> 16;
        extra->tpage = extra_page;
        setUV4(extra, 0, 224, 31, 224, 0, 255, 31, 255);
        extra->clut = packed;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 1);
        extra++;
        SetPolyFT4(extra);
        extra->tpage = extra_page;
        setUV4(extra, 32, 224, 63, 224, 32, 255, 63, 255);
        extra->clut = packed;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 1);
        for (i = 0; i < 5; i++, record++) {
            setVector(&record->scale, 4096, 4096, 4096);
            record->inner.r = work->config->ribbon.r;
            record->inner.g = work->config->ribbon.g;
            record->inner.b = work->config->ribbon.b;
            record->outer.r = 0;
            record->outer.g = 16;
            record->outer.b = 128;
            record->field_238 = 0;
            record->field_23C = 0;
            record->field_23A = -(i * 16);
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = 1024 + (i + 1) * 1024 / 6;
            } else {
                angle = 1024 - i * 1024 / 6;
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
        work->field_1F50 = 1024;
        work->field_1F48 = 0;
        work->field_1F4A = 0;
        work->field_1F4C = 0;
        work->field_1F60 = 0;
        work->field_1F64 = 0;
        work->field_1F54 = 0;
        work->field_1F58 = 0;
        work->field_1F5C = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->fade = 0;
        work->field_1F74 = 2048;
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
        if (work->config->mode == 1) {
            func_800593D0(work->slot, work->config->part, work->config->vertex,
                          &work->vertex_position);
            work->matrix.t[0] = work->vertex_position.vx;
            work->matrix.t[1] = work->vertex_position.vy;
            work->matrix.t[2] = work->vertex_position.vz;
        } else {
            GsGetLwUnit(work->part, &work->matrix);
        }
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
        dx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        func_8013BBA0((u8 *)point);
        if (work->config->start <= work->frame) {
            func_8013C6D8((u8 *)point);
        }
        if (work->phase >= 2) {
            func_8013CED4((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    func_800595C8(2, work->field_1F74, work->field_1F74, work->field_1F74);
    if (work->phase < 6) {
        if (work->field_1F74 > 0) {
            work->field_1F74 -= 128;
            if (work->field_1F74 <= 0) {
                work->field_1F74 = 0;
            }
        }
    } else {
        work->field_1F74 = work->fade << 5;
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
