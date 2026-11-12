#include "../../types.h"
#include "variant336_entry.h"

#ifndef MODEL_VARIANT336_CLUT_X
#define MODEL_VARIANT336_CLUT_X 640
#endif

s32 FrenchModelVariant336_HandleRequest(SVECTOR *point, s32 command)
{
    Variant336EntryState *work = (Variant336EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ModelVariantProjection projection;
    Variant336EntryRecord *record = work->records;
    Variant337EntryStreamer *streamer = work->streamers;
    ModelVariantPointSheet *sheet = work->sheets;
    GsOT *ot = Graphics_GetDefaultOrderingTable();
    POLY_GT4 *textured = work->packets.textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_FT4 *extra = work->extra_textured;
    POLY_G3 *triangle = &work->packets.triangle;
    POLY_G4 *quad = &work->packets.quad;
    s32 result = 0;
    s32 sheet_index, sheet_point;
    s32 i, j;
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
        work->config = gFrenchModelVariant336_aConfigs + command;
        work->part0 = Model_GetSlotDataEntry(work->slot, work->config->part0);
        work->part1 = Model_GetSlotDataEntry(work->slot, work->config->part1);
        if (work->slot == 0) {
            setVector(&work->aim.target, 0, -300, -450);
        } else {
            setVector(&work->aim.target, 0, -300, 450);
        }
        GsGetLwUnit(work->part0, &work->matrix0);
        GsGetLwUnit(work->part1, &work->matrix1);
        work->origin[0] = (work->matrix0.t[0] + work->matrix1.t[0]) / 2;
        work->origin[1] = (work->matrix0.t[1] + work->matrix1.t[1]) / 2;
        work->origin[2] = (work->matrix0.t[2] + work->matrix1.t[2]) / 2;
        work->aim.direction.vx = work->aim.target.vx - work->origin[0];
        work->aim.direction.vy = work->aim.target.vy - work->origin[1];
        work->aim.direction.vz = work->aim.target.vz - work->origin[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT336_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT336_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant336_aImages[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant336_aImages[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant336_aImages[1]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant336_aImages[2]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant336_aImages[3]);
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant336_aImages[8]);
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
        for (i = 0; i < 4; i++, record++) {
            setVector(&record->style.scale, 4096, 4096, 4096);
            record->style.inner.r = 128;
            record->style.inner.g = 128;
            record->style.inner.b = 128;
            record->style.outer.r = 0;
            record->style.outer.g = 16;
            record->style.outer.b = 128;
            record->style.field_18 = 0;
            record->style.field_1C = 0;
            record->style.count = 0;
            record->state = 0;
            record->count = 0;
            record->extent = 1024;
        }
        for (sheet_index = 0; sheet_index < 8; sheet_index++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->points[sheet_point], -256, -256, 0);
                    setVector(&sheet->points[sheet_point + 4], 0, -256, 0);
                    setVector(&sheet->points[sheet_point + 8], -256, 0, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->points[sheet_point], 256, -256, 0);
                    setVector(&sheet->points[sheet_point + 4], 256, 0, 0);
                    setVector(&sheet->points[sheet_point + 8], 0, -256, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->points[sheet_point], 256, 256, 0);
                    setVector(&sheet->points[sheet_point + 4], 0, 256, 0);
                    setVector(&sheet->points[sheet_point + 8], 256, 0, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->points[sheet_point], -256, 256, 0);
                    setVector(&sheet->points[sheet_point + 4], -256, 0, 0);
                    setVector(&sheet->points[sheet_point + 8], 0, 256, 0);
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
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->field_1FE8 = 0;
        work->sheet_count = 1;
        work->segment_start = 0;
        work->segment_end = 0;
        work->wave0 = 0;
        work->wave1 = 0;
        work->extent = 1024;
        work->clock.frame_count = 0;
        work->clock.frame = 0;
        work->clock.animation_frame = 0;
        work->clock.step = 0;
        work->phase = 0;
        work->clock.fade = 0;
        work->brightness = 2048;
        work->command = command;
        work->rotation[0] = 0;
        work->rotation[1] = 0;
        work->rotation[2] = 0;
    } else {
        view = Model_GetCameraViewBuffer();
        work->aim.view_delta.vx = view->vrx - view->vpx;
        work->aim.view_delta.vy = view->vry - view->vpy;
        work->aim.view_delta.vz = view->vrz - view->vpz;
        Square0(&work->aim.view_delta, &squared);
        work->clock.angles[0] = ratan2(work->aim.view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[1] = -ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->aim.target);
            work->aim.target.vy = 0;
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->aim.target);
            work->aim.target.vy = 0;
        }
        if (work->clock.frame < work->config->end) {
            GsGetLwUnit(work->part0, &work->matrix0);
            GsGetLwUnit(work->part1, &work->matrix1);
            work->origin[0] = (work->matrix0.t[0] + work->matrix1.t[0]) / 2;
            work->origin[1] = (work->matrix0.t[1] + work->matrix1.t[1]) / 2;
            work->origin[2] = (work->matrix0.t[2] + work->matrix1.t[2]) / 2;
            work->aim.direction.vx = work->aim.target.vx - work->origin[0];
            work->aim.direction.vy = work->aim.target.vy - work->origin[1];
            work->aim.direction.vz = work->aim.target.vz - work->origin[2];
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->origin[0], work->origin[1], work->origin[2]);
        RotTransPers(&position, (long *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->aim.target.vx, work->aim.target.vy, work->aim.target.vz);
        RotTransPers(&target, (long *)&projection.target,
                     &projection.interpolation, &projection.flag);
        dx = projection.target.vx - screen_x;
        work->aim.screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->aim.screen_delta.vy = dy;
        if (work->config->start < work->clock.frame) {
            FrenchModelVariant336_DrawStreamers((u8 *)point);
        }
        if (work->phase > 0) {
            FrenchModelVariant336_DrawRibbon((u8 *)point);
            FrenchModelVariant336_DrawSheets((u8 *)point);
        }
        work->clock.frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->clock.animation_frame) {
            work->clock.frame += Model_GetFrameStep();
            work->clock.step = Model_GetFrameStep();
            work->clock.animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->brightness, work->brightness, work->brightness);
    if (work->phase < 6) {
        if (work->brightness > 0) {
            work->brightness -= 128;
            if (work->brightness <= 0) {
                work->brightness = 0;
            }
        }
    } else {
        work->brightness = work->clock.fade << 5;
    }
    if (work->phase == 2) {
        result = 4;
    } else if (work->phase == 3) {
        result = 1;
        work->phase = 6;
    } else if (work->phase == 6) {
        if (work->clock.fade < 64) {
            work->clock.fade += work->clock.step;
            if (work->clock.fade >= 64) {
                work->clock.fade = 64;
                work->phase = 7;
            }
        }
    } else if (work->phase == 7) {
        result = 2;
    }
    return result;
}
