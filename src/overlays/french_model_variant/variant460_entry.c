#include "../../types.h"
#include "variant460_entry.h"

/* The North American build (header 443) reads its CLUT from column 512. */
#ifndef MODEL_VARIANT460_CLUT_X
#define MODEL_VARIANT460_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant460EntryState *work = (Variant460EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant460EntryRecord *record = work->records;
    ModelVariantSheetSet *sheet = work->sheets;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
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
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant460EntryConfig *)(D_8013D8CC + 0xFC) + command;
        for (ring_point = 0; ring_point < 8; ring_point++) {
            work->parts[ring_point] = Model_GetSlotDataEntry(work->slot, work->config->parts[ring_point]);
        }
        if (work->slot == 0) {
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
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8CC + 0xC4));
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8CC + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8CC + 0x1C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8CC + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D8CC + 0x54));
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
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
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
        work->field_2EBC = 0;
        work->field_2EC0 = 0;
        work->field_2EB0 = 0;
        work->field_2EB4 = 0;
        work->field_2EB8 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->fade = 0;
        work->field_2ED0 = 2048;
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
        if (work->config->phase3 < work->frame && work->phase == 2) {
            work->phase = 3;
        }
        if (work->config->phase4 < work->frame && work->phase == 3) {
            work->phase = 4;
        }
        if (work->config->ribbon_start <= work->frame) {
            func_8013BCFC((u8 *)point);
        }
        if (work->config->sheet_start <= work->frame && work->phase < 6) {
            func_8013C7EC((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    func_800595C8(2, work->field_2ED0, work->field_2ED0, work->field_2ED0);
    if (work->phase < 6) {
        if (work->field_2ED0 > 0) {
            work->field_2ED0 -= 128;
            if (work->field_2ED0 <= 0) {
                work->field_2ED0 = 0;
            }
        }
    } else {
        work->field_2ED0 = work->fade << 5;
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
