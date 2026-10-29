#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant432_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT432_CLUT_X
#define MODEL_VARIANT432_CLUT_X 512
#endif

/* Header 432's entry: the French header-437 entry with 0x380-byte ribbons,
 * a 192-unit sheet and a midpoint origin. The sheet and band helpers build
 * from the French header-449 sources; the ribbon and streamer helpers stay
 * generated assembly. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant432EntryState *work = (Variant432EntryState *)point;
    /* Preserve the native 224-byte frame, including two unused matrices. */
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant432EntryProjection projection;
    Variant432EntryRecord *record = work->records;
    Variant432EntryRibbon *ribbon = work->ribbons;
    Sheet437 *sheet = work->sheets;
    Variant437Streamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *extra = work->extra;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 i, j;
    u16 tpage, clut, flat_texture;
    s32 packed, flat_page, extra_page;
    s32 k;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant432EntryConfig *)(D_8013DB64 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        result = 6;
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[1], &work->origin_a);
        GsGetLwUnit(work->parts[2], &work->origin_b);
        setVector(&work->direction, work->origin_b.t[0] - work->origin_a.t[0],
                  work->origin_b.t[1] - work->origin_a.t[1],
                  work->origin_b.t[2] - work->origin_a.t[2]);
        GsGetLwUnit(work->parts[0], &work->matrix);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT432_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT432_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013DB64 + 0xC4));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013DB64 + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013DB64 + 0x1C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013DB64 + 0x38));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013DB64 + 0x54));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013DB64 + 0xE0));
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
        for (i = 0; i < 1; i++, record++) {
            setVector(&record->scale, 4096, 4096, 4096);
            for (j = 0; j < 3; j++) {
                record->inner[j].r = 0;
                record->inner[j].g = 64;
                record->inner[j].b = 255;
                record->outer[j].r = 255;
                record->outer[j].g = 255;
                record->outer[j].b = 255;
                record->progress[j] = 128 - j * 128;
            }
            record->field_A0 = 0;
            record->field_A4 = 0;
            record->field_A2 = 0;
        }
        for (i = 0; i < 4; i++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->inner.r = 128;
            ribbon->inner.g = 128;
            ribbon->inner.b = 128;
            ribbon->outer.r = 0;
            ribbon->outer.g = 16;
            ribbon->outer.b = 128;
            ribbon->field_238 = 0;
            ribbon->field_23A = 0;
            ribbon->field_23C = 0;
            ribbon->state = 0;
            ribbon->count = 0;
            if (!(i & 1)) {
                ribbon->start = -i * 8;
                ribbon->end = 0;
            } else {
                ribbon->start = i * 8 + 16;
                ribbon->end = 16;
            }
        }
        for (k = 0; k < 1; k++, sheet++) {
            s32 j;
            if (k == 0) {
                for (j = 0; j < 4; j++) {
                    if (j == 0) {
                        setVector(&sheet->v0[j], -192, -192, 0);
                        setVector(&sheet->v1[j], 0, -192, 0);
                        setVector(&sheet->v2[j], -192, 0, 0);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    } else if (j == 1) {
                        setVector(&sheet->v0[j], 192, -192, 0);
                        setVector(&sheet->v1[j], 192, 0, 0);
                        setVector(&sheet->v2[j], 0, -192, 0);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    } else if (j == 2) {
                        setVector(&sheet->v0[j], 192, 192, 0);
                        setVector(&sheet->v1[j], 0, 192, 0);
                        setVector(&sheet->v2[j], 192, 0, 0);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    } else if (j == 3) {
                        setVector(&sheet->v0[j], -192, 192, 0);
                        setVector(&sheet->v1[j], -192, 0, 0);
                        setVector(&sheet->v2[j], 0, 192, 0);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    }
                }
            } else {
                for (j = 0; j < 4; j++) {
                    if (j == 0) {
                        setVector(&sheet->v0[j], 0, -192, -192);
                        setVector(&sheet->v1[j], 0, -192, 0);
                        setVector(&sheet->v2[j], 0, 0, -192);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    } else if (j == 1) {
                        setVector(&sheet->v0[j], 0, -192, 192);
                        setVector(&sheet->v1[j], 0, 0, 192);
                        setVector(&sheet->v2[j], 0, -192, 0);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    } else if (j == 2) {
                        setVector(&sheet->v0[j], 0, 192, 192);
                        setVector(&sheet->v1[j], 0, 192, 0);
                        setVector(&sheet->v2[j], 0, 0, 192);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    } else if (j == 3) {
                        setVector(&sheet->v0[j], 0, 192, -192);
                        setVector(&sheet->v1[j], 0, 0, -192);
                        setVector(&sheet->v2[j], 0, 192, 0);
                        setVector(&sheet->v3[j], 0, 0, 0);
                    }
                }
            }
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 64;
            sheet->inner[2] = 255;
        }
        for (i = 0; i < 3; i++, streamer++) {
            for (j = 0; j < 13; j++) {
                streamer->color[j][0] = 128;
                streamer->color[j][1] = 128;
                streamer->color[j][2] = 128;
            }
            MODEL_VARIANT_WORD(streamer, 0x208) = 0;
        }
        work->field_1C08 = 4096;
        work->field_1C0A = 1024;
        work->field_1C0C = 0;
        work->field_1C10 = 1024;
        work->field_1C14 = 0;
        work->field_1C18 = 0;
        work->spin = 0;
        work->factor = 0;
        work->field_1B94 = 0;
        work->scale = 0;
        work->spread = 0;
        work->field_1C20 = 0;
        work->field_1C22 = 0;
        work->phase_a = 0;
        work->phase_b = 0;
        work->length = 1024;
        work->rotation_x = 0;
        work->rotation_y = 0;
        work->rotation_z = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->span = work->config->scale_end - work->config->scale_start;
        work->fade = 0;
        work->tint = 2048;
        work->phase = 0;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[1], &work->origin_a);
        GsGetLwUnit(work->parts[2], &work->origin_b);
        setVector(&work->direction, work->origin_b.t[0] - work->origin_a.t[0],
                  work->origin_b.t[1] - work->origin_a.t[1],
                  work->origin_b.t[2] - work->origin_a.t[2]);
        GsGetLwUnit(work->parts[0], &work->matrix);
        if (work->elapsed < work->config->scale_end) {
            setVector(&work->origin, work->matrix.t[0],
                      (work->origin_b.t[1] + work->origin_a.t[1]) / 2, work->matrix.t[2]);
        }
        setVector(&work->path, work->target.vx - work->origin.vx,
                  work->target.vy - work->origin.vy, work->target.vz - work->origin.vz);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        dx = (u16)projection.target.vx - screen_x;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->start <= work->elapsed) {
            func_8013C244((u8 *)point);
            func_8013CB78((u8 *)point);
        }
        if (work->config->scale_start < work->elapsed) {
            func_8013BD24((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 11) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->phase >= 3 && work->phase <= 6) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 11;
    } else if (work->phase == 11) {
        result = 0;
        work->fade += work->step;
        if (work->fade >= 64) {
            work->fade = 64;
            work->phase = 12;
        }
    } else if (work->phase == 12) {
        result = 2;
    }
    return result;
}
