#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant444_entry.h"

#ifndef MODEL_VARIANT444_CLUT_X
#define MODEL_VARIANT444_CLUT_X 640
#endif

s32 FrenchModelVariant444_HandleRequest(SVECTOR *point, s32 command)
{
    Variant444EntryState *work = (Variant444EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ModelVariantProjection projection;
    ModelVariantTubeView *tube = work->tubes;
    Variant444EntryColorRecord *record = work->records;
    ModelVariantSheet *sheet = work->sheets;
    ModelVariantFunnel *funnel = work->funnels;
    ModelVariantSpokeColorRing *spoke = work->spokes;
    ModelVariantBeam *beam = work->beams;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *plain = &work->plain;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *paged = &work->paged;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    s32 n, row, i, j, k, angle, base_angle;
    s32 r, g, b;
    u16 tpage, clut;
    s32 packed, flat_page;
    u16 paged_clut, flat_clut;
    s32 paged_page;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y, dx, dy;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->clock.frame_count = 0;
        work->config = gFrenchModelVariant444_aConfigs + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT444_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT444_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant444_aImages[7]);
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant444_aImages[5]);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant444_aImages[1]);
        paged_page = packed >> 16;
        paged_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gFrenchModelVariant444_aImages[3]);
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
        flat->clut = flat_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 0, 176, 0, 223, 48, 176, 48, 223);
        flat->clut = flat_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        SetPolyGT4(plain);
        SetSemiTrans(plain, 1);
        SetShadeTex(plain, 0);
        SetPolyGT4(extra);
        SetSemiTrans(extra, 0);
        SetShadeTex(extra, 0);
        SetPolyGT4(paged);
        paged->tpage = paged_page;
        setUV4(paged, 128, 207, 255, 207, 128, 128, 255, 128);
        paged->clut = paged_clut;
        SetSemiTrans(paged, 1);
        SetShadeTex(paged, 0);

        for (n = 0; n < 2; n++, funnel++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&funnel->inner[j], (u32)rcos(angle) >> 4,
                          (u32)rsin(angle) >> 4, 0);
                setVector(&funnel->outer[j], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, -64);
            }
            if (n == 1) {
                funnel->size = 0;
            } else {
                funnel->size = -n * 4096;
            }
            MODEL_VARIANT_WORD(funnel, 0x114) = 0;
        }
        for (i = 0; i < 1; i++, tube++) {
            setVector(&tube->scale, 4096, 4096, 4096);
            for (j = 0; j < 9; j++) {
                if (j % 2 == 0) {
                    r = 96; g = 96; b = 96;
                } else {
                    r = 0; g = 0; b = 128;
                }
                tube->colors[j].r = r;
                tube->colors[j].g = g;
                tube->colors[j].b = b;
            }
            tube->field_544 = 0;
            tube->field_548 = 0;
            tube->field_546 = 0;
        }
        for (i = 0; i < 16; i++, record++) {
            if (i % 8 == 0) {
                r = 128; g = 0; b = 0; record->field_48 = 0;
            } else if (i % 8 == 1) {
                r = 128; g = 64; b = 0; record->field_48 = 2;
            } else if (i % 8 == 2) {
                r = 128; g = 128; b = 0; record->field_48 = 3;
            } else if (i % 8 == 3) {
                r = 0; g = 128; b = 0; record->field_48 = 1;
            } else if (i % 8 == 4) {
                r = 0; g = 128; b = 128; record->field_48 = 4;
            } else if (i % 8 == 5) {
                r = 0; g = 0; b = 128; record->field_48 = 7;
            } else if (i % 8 == 6) {
                r = 128; g = 0; b = 128; record->field_48 = 5;
            } else {
                r = 128; g = 0; b = 64; record->field_48 = 6;
            }
            record->color_44.r = r;
            record->color_44.g = g;
            record->color_44.b = b;
            record->color_40.r = r + 127;
            record->color_40.g = g + 127;
            record->color_40.b = b + 127;
        }
        for (n = 0; n < 2; n++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -256, -256, 0);
                    setVector(&sheet->v1[k], 0, -256, 0);
                    setVector(&sheet->v2[k], -256, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 256, -256, 0);
                    setVector(&sheet->v1[k], 256, 0, 0);
                    setVector(&sheet->v2[k], 0, -256, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 256, 256, 0);
                    setVector(&sheet->v1[k], 0, 256, 0);
                    setVector(&sheet->v2[k], 256, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -256, 256, 0);
                    setVector(&sheet->v1[k], -256, 0, 0);
                    setVector(&sheet->v2[k], 0, 256, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 64;
            sheet->inner[2] = 255;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
        }
        for (row = 0, base_angle = 0, j = 0; row < 4;
             row++, spoke++, base_angle += 256) {
            for (n = 0, angle = base_angle; n < 8; n++, j++, angle = base_angle + n * 512) {
                setVector(&spoke->inner[n], (u32)rcos(angle) >> 4,
                          (u32)rsin(angle) >> 4, 0);
                setVector(&spoke->outer[n], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
                if (j % 8 == 0) {
                    r = 255; g = 0; b = 0;
                } else if (j % 8 == 1) {
                    r = 255; g = 128; b = 0;
                } else if (j % 8 == 2) {
                    r = 255; g = 255; b = 0;
                } else if (j % 8 == 3) {
                    r = 0; g = 255; b = 0;
                } else if (j % 8 == 4) {
                    r = 0; g = 255; b = 255;
                } else if (j % 8 == 5) {
                    r = 0; g = 0; b = 255;
                } else if (j % 8 == 6) {
                    r = 255; g = 0; b = 255;
                } else {
                    r = 255; g = 0; b = 128;
                }
                spoke->colors[n].r = r;
                spoke->colors[n].g = g;
                spoke->colors[n].b = b;
            }
            spoke->inner_color.r = 64;
            spoke->inner_color.g = 128;
            spoke->inner_color.b = 255;
            spoke->outer_color.r = 0;
            spoke->outer_color.g = 0;
            spoke->outer_color.b = 0;
            spoke->angle = 1024 - row * 256;
            spoke->count = 0;
        }
        for (i = 0; i < 2; i++, beam++) {
            setVector(&beam->scale, 4096, 4096, 4096);
            beam->inner.r = 255;
            beam->inner.g = 255;
            beam->inner.b = 255;
            beam->outer.r = 0;
            beam->outer.g = 16;
            beam->outer.b = 128;
        }
        work->inner.r = 128;
        work->inner.g = 128;
        work->inner.b = 128;
        work->outer.r = 0;
        work->outer.g = 0;
        work->outer.b = 128;
        work->field_1668 = 0;
        work->width = 1024;
        work->angle = 0;
        work->field_1674 = 0;
        work->field_1678 = 0;
        work->progress = 0;
        work->field_1658 = 0;
        work->field_165A = 0;
        work->clock.frame_count = 0;
        work->clock.frame = 0;
        work->clock.animation_frame = 0;
        work->clock.step = 0;
        work->clock.fade = 0;
        work->tint = 2048;
        work->phase = 0;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta,
                  view->vrx - view->vpx,
                  view->vry - view->vpy,
                  view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->clock.angles[0] = ratan2(work->view_delta.vy,
                                SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (long *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (long *)&projection.target,
                     &projection.interpolation, &projection.flag);
        dx = (u16)projection.target.vx - screen_x;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->sheet_start < work->clock.frame) {
            SpanishModelVariant444_DrawSheets((u8 *)point);
            SpanishModelVariant444_DrawFunnels((u8 *)point);
        }
        if (work->config->tube_start < work->clock.frame) {
            FrenchModelVariant175_DrawTube((u8 *)point);
        }
        work->clock.frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->clock.animation_frame) {
            work->clock.frame += Model_GetFrameStep();
            work->clock.step = Model_GetFrameStep();
            work->clock.animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 5) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->clock.fade << 5;
    }
    if (work->phase >= 2 && work->phase <= 3) {
        result = 4;
    } else if (work->phase == 4) {
        result = 1;
        work->phase = 5;
    } else if (work->phase == 5) {
        work->clock.fade += work->clock.step;
        if (work->clock.fade >= 64) {
            work->clock.fade = 64;
            work->phase = 6;
        }
    } else if (work->phase == 6) {
        result = 2;
    }
    return result;
}
