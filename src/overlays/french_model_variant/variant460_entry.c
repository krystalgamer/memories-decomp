#include "../../types.h"
#include "variant460_entry.h"

/* The North American build (header 443) reads its CLUT from column 512. */
#ifndef MODEL_VARIANT460_CLUT_X
#define MODEL_VARIANT460_CLUT_X 640
#endif

s32 FrenchModelVariant460_HandleRequest(SVECTOR *point, s32 command)
{
    Variant460EntryState *work = (Variant460EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ModelVariantProjection projection;
    Variant460EntryRecord *record = work->records;
    ModelVariantSheetSet *sheet = work->sheets;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->packets.textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->packets.triangle;
    POLY_G4 *quad = &work->packets.quad;
    POLY_GT4 *extra = &work->extra;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j, angle;
    s32 angle2;
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
        work->config = gFrenchModelVariant460_aConfigs + command;
        for (ring_point = 0; ring_point < 8; ring_point++) {
            work->parts[ring_point] = Model_GetSlotDataEntry(work->tail.slot, work->config->parts[ring_point]);
        }
        if (work->tail.slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        for (ring_point = 0; ring_point < 8; ring_point++) {
            setVector(&work->targets[ring_point], work->target.vx, work->target.vy, work->target.vz);
            GsGetLwUnit(work->parts[ring_point], &work->matrices[ring_point]);
            setVector(&work->directions[ring_point],
                      work->targets[ring_point].vx - work->matrices[ring_point].t[0],
                      work->targets[ring_point].vy - work->matrices[ring_point].t[1],
                      work->targets[ring_point].vz - work->matrices[ring_point].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT460_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT460_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant460_aImages[7]);
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant460_aImages[5]);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant460_aImages[1]);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant460_aImages[2]);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant460_aImages[3]);
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
        for (i = 0; i < 8; i++, record++) {
            setVector(&record->scale, 4096, 4096, 4096);
            record->inner.r = work->config->ribbon.r;
            record->inner.g = work->config->ribbon.g;
            record->inner.b = work->config->ribbon.b;
            record->outer.r = work->config->outer.r;
            record->outer.g = work->config->outer.g;
            record->outer.b = work->config->outer.b;
            record->field_238 = 0;
            record->field_23C = 0;
            record->count = -(i * 16);
            record->field_244 = 0;
            record->state = 0;
            record->length = 1024;
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = 1024 + (i + 1) * 1024 / 9;
            } else {
                angle = 1024 - i * 1024 / 9;
            }
            record->velocity.vx = (rcos(angle) << 8) >> 12;
            record->velocity.vy = (rsin(angle) << 8) >> 12;
            record->velocity.vz = 0;
        }
        for (ring_index = 0; ring_index < 16; ring_index++, sheet++) {
            for (ring_point = 0; ring_point < 4; ring_point++) {
                if (ring_point == 0) {
                    setVector(&sheet->v0[ring_point], -128, -128, 0);
                    setVector(&sheet->v1[ring_point], 0, -128, 0);
                    setVector(&sheet->v2[ring_point], -128, 0, 0);
                    setVector(&sheet->v3[ring_point], 0, 0, 0);
                } else if (ring_point == 1) {
                    setVector(&sheet->v0[ring_point], 128, -128, 0);
                    setVector(&sheet->v1[ring_point], 128, 0, 0);
                    setVector(&sheet->v2[ring_point], 0, -128, 0);
                    setVector(&sheet->v3[ring_point], 0, 0, 0);
                } else if (ring_point == 2) {
                    setVector(&sheet->v0[ring_point], 128, 128, 0);
                    setVector(&sheet->v1[ring_point], 0, 128, 0);
                    setVector(&sheet->v2[ring_point], 128, 0, 0);
                    setVector(&sheet->v3[ring_point], 0, 0, 0);
                } else if (ring_point == 3) {
                    setVector(&sheet->v0[ring_point], -128, 128, 0);
                    setVector(&sheet->v1[ring_point], -128, 0, 0);
                    setVector(&sheet->v2[ring_point], 0, 128, 0);
                    setVector(&sheet->v3[ring_point], 0, 0, 0);
                }
            }
            sheet->outer[0] = work->config->sheet.r;
            sheet->outer[1] = work->config->sheet.g;
            sheet->outer[2] = work->config->sheet.b;
            sheet->inner[0] = work->config->outer.r;
            sheet->inner[1] = work->config->outer.g;
            sheet->inner[2] = work->config->outer.b;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
            sheet->shown = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = work->config->streamer.r;
                streamer->color[j].g = work->config->streamer.g;
                streamer->color[j].b = work->config->streamer.b;
            }
            streamer->field_2A8 = 0;
        }
        work->width = 1024;
        work->field_2EAC = 1024;
        work->field_2EA4 = 0;
        work->field_2EA6 = 0;
        work->field_2EA8 = 0;
        work->ribbon_wave = 0;
        work->ribbon_phase = 0;
        work->field_2EB0 = 0;
        work->field_2EB4 = 0;
        work->field_2EB8 = 0;
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
        work->view_delta.vx = view->vrx - view->vpx;
        work->view_delta.vy = view->vry - view->vpy;
        work->view_delta.vz = view->vrz - view->vpz;
        Square0(&work->view_delta, &squared);
        work->clock.angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->tail.slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        for (ring_point = 0, angle = 0, angle2 = 0; ring_point < 8; ring_point++, angle += 800, angle2 += 1300) {
            if (work->config->mode == 1 && ring_point != 0) {
                setVector(&work->targets[ring_point],
                          work->target.vx + ((rcos(angle + angle2) << 7) >> 12),
                          work->target.vy + ((rsin(angle) << 7) >> 12),
                          work->target.vz + ((rsin(angle2) << 7) >> 12));
            } else {
                setVector(&work->targets[ring_point], work->target.vx, work->target.vy, work->target.vz);
            }
            GsGetLwUnit(work->parts[ring_point], &work->matrices[ring_point]);
            setVector(&work->directions[ring_point],
                      work->targets[ring_point].vx - work->matrices[ring_point].t[0],
                      work->targets[ring_point].vy - work->matrices[ring_point].t[1],
                      work->targets[ring_point].vz - work->matrices[ring_point].t[2]);
        }
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (long *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (long *)&projection.target,
                     &projection.interpolation, &projection.flag);
        work->screen_delta.vx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->phase3 < work->clock.frame && work->tail.phase == 2) {
            work->tail.phase = 3;
        }
        if (work->config->phase4 < work->clock.frame && work->tail.phase == 3) {
            work->tail.phase = 4;
        }
        if (work->config->ribbon_start <= work->clock.frame) {
            ModelVariant443_DrawRibbons((u8 *)point);
        }
        if (work->config->sheet_start <= work->clock.frame && work->tail.phase < 6) {
            ModelVariant443_DrawSheets((Variant460EntryState *)point);
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
