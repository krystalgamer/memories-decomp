#include "../../types.h"
#include "variant335_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Model335EntryState *work = (Model335EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Model335EntryRecord *record = work->records;
    Model335EntrySheet *sheet = work->sheets;
    Variant335Ring *ring = work->rings;
    Variant337EntryStreamer *streamer = work->streamers;
    Model335EntryShortStreamer *short_streamer = work->short_streamers;
    GsOT *ot = func_80058F10();
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_FT4 *extra = work->extra_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 geometry_index, sheet_point;
    s32 i, j;
    s32 angle;
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
        work->config = (Model335EntryConfig *)(D_8013D8EC + 0x7CC) + command;
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
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8EC + 0x794));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8EC + 0x75C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8EC + 0x6EC));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8EC + 0x708));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8EC + 0x724));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8EC + 0x7B0));
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
        for (i = 0; i < 3; i++, record++) {
            setVector(&record->scale, 4096, 4096, 4096);
            record->inner.r = 128;
            record->inner.g = 128;
            record->inner.b = 128;
            record->outer.r = 0;
            record->outer.g = 16;
            record->outer.b = 128;
            record->field_238 = 0;
            record->field_23C = 0;
            record->field_23A = 0;
        }
        for (geometry_index = 0; geometry_index < 4; geometry_index++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->points[sheet_point], -128, -128, 0);
                    setVector(&sheet->points[sheet_point + 4], 0, -128, 0);
                    setVector(&sheet->points[sheet_point + 8], -128, 0, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->points[sheet_point], 128, -128, 0);
                    setVector(&sheet->points[sheet_point + 4], 128, 0, 0);
                    setVector(&sheet->points[sheet_point + 8], 0, -128, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->points[sheet_point], 128, 128, 0);
                    setVector(&sheet->points[sheet_point + 4], 0, 128, 0);
                    setVector(&sheet->points[sheet_point + 8], 128, 0, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->points[sheet_point], -128, 128, 0);
                    setVector(&sheet->points[sheet_point + 4], -128, 0, 0);
                    setVector(&sheet->points[sheet_point + 8], 0, 128, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                }
            }
            sheet->outer.r = 255;
            sheet->outer.g = 255;
            sheet->outer.b = 255;
            sheet->inner.r = 0;
            sheet->inner.g = 64;
            sheet->inner.b = 255;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        for (geometry_index = 0; geometry_index < 3; geometry_index++, ring++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&ring->a[j], (u32)rcos(angle) / 8, 0, (u32)rsin(angle) / 8);
                setVector(&ring->b[j], (u32)rcos(angle) / 4, 0, (u32)rsin(angle) / 4);
                setVector(&ring->c[j], (u32)rcos(angle) * 9 / 32, 0, (u32)rsin(angle) * 9 / 32);
                ring->outer[0] = 128;
                ring->outer[1] = 192;
                ring->inner[0] = 255;
                ring->inner[1] = 255;
                ring->inner[2] = 255;
                ring->outer[2] = 255;
            }
            ring->scale = 4096 - geometry_index * 4096 / 3;
            ring->cycles = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        for (i = 0; i < 6; i++, short_streamer++) {
            for (j = 0; j < 17; j++) {
                short_streamer->color[j].r = 128;
                short_streamer->color[j].g = 128;
                short_streamer->color[j].b = 128;
            }
            short_streamer->field_2A8 = 0;
        }
        work->width = 1024;
        work->extent = 1024;
        work->field_2E78 = 0;
        work->field_2E6C = 0;
        work->field_2E70 = 0;
        work->field_2E74 = 0;
        work->count = 0;
        work->field_2E8E = 0;
        work->field_2E90 = 0;
        work->wave0 = 0;
        work->wave1 = 0;
        work->rotation[0] = 0;
        work->rotation[1] = 0;
        work->rotation[2] = 0;
        work->field_2E94 = 0;
        work->field_2E96 = 0;
        work->brightness = 2048;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->fade = 0;
        work->phase = 0;
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
            work->target.vy = 0;
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
            work->target.vy = 0;
        }
        if (work->frame < work->config->end) {
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
            dx = projection.target.vx - screen_x;
            dy = projection.target.vy - screen_y;
            work->screen_delta.vx = dx;
            work->screen_delta.vy = dy;
        }
        if (work->config->start <= work->frame) {
            if (work->phase < 7) {
                func_8013BB98((u8 *)point);
                func_8013C534((u8 *)point);
            }
            if (work->phase >= 5) {
                func_8013D0D8((u8 *)point);
                func_8013CBC4((u8 *)point);
            }
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
        work->field_2E74 += work->step * 32;
    }
    func_800595C8(2, work->brightness, work->brightness, work->brightness);
    if (work->phase < 10) {
        if (work->brightness > 0) {
            work->brightness -= 128;
            if (work->brightness <= 0) {
                work->brightness = 0;
            }
        }
    } else {
        work->brightness = work->fade << 5;
    }
    if (work->phase >= 6 && work->phase <= 8) {
        result = 4;
    } else if (work->phase == 9) {
        result = 1;
        work->phase = 10;
    } else if (work->phase == 10) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 11;
            }
        }
    } else if (work->phase == 11) {
        result = 2;
    }
    return result;
}
