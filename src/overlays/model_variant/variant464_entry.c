#include "../../types.h"
#include "variant464_entry.h"

/* The French images load the effect palette from VRAM x = 640. */
#ifdef VERSION_FRENCH
#define MODEL_VARIANT464_CLUT_X 640
#else
#define MODEL_VARIANT464_CLUT_X 512
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant464EntryState *work = (Variant464EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant464EntryProjection projection;
    Variant464EntryDome *dome = &work->dome;
    Variant464EntryFunnel *funnel = work->funnels;
    Variant464EntryBand *band = work->bands;
    ModelVariantSheet *sheet = work->sheets;
    Variant464EntryStreamer *streamer = work->streamers;
    Variant464EntrySpiral *spiral = work->spirals;
    POLY_GT4 *special = &work->special;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_GT4 *beams = work->beams;
    s32 result = 0;
    u16 flat_clut, other_tpage, other_clut, tpage, clut;
    s32 packed, n, flat_page, blue;
    s32 i, j, k, sheet_point, angle, a, amount, radius, height;
    GsRVIEW2 *view;
    Variant464EntryConfig *config;
    u32 screen_x, frame;
    s32 screen_y;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant464EntryConfig *)(D_8013CE0C + 0x1C) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT464_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT464_CLUT_X, 244);
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)D_8013CE0C);
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CE0C - 0x38));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CE0C - 0xA8));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CE0C - 0x8C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CE0C - 0x70));
        other_tpage = GetTPage(1, 1, 896, 0);
        other_clut = GetClut(MODEL_VARIANT464_CLUT_X, 244);
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
        setUV4(flat, 128, 128, 255, 128, 128, 223, 255, 223);
        flat->clut = flat_clut;
        SetSemiTrans(flat, 0);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 128, 128, 255, 128, 128, 223, 255, 223);
        flat->clut = flat_clut;
        SetSemiTrans(flat, 0);
        SetShadeTex(flat, 1);
        SetPolyGT4(beams);
        beams->tpage = flat_page;
        setUV4(beams, 128, 223, 223, 223, 128, 128, 223, 128);
        beams->clut = flat_clut;
        SetSemiTrans(beams, 1);
        SetShadeTex(beams, 0);
        beams++;
        SetPolyGT4(beams);
        beams->tpage = flat_page;
        setUV4(beams, 192, 223, 223, 223, 192, 128, 223, 128);
        beams->clut = flat_clut;
        SetSemiTrans(beams, 1);
        SetShadeTex(beams, 0);
        SetPolyGT4(special);
        special->tpage = other_tpage;
        setUV4(special, 0, 191, 31, 191, 0, 128, 31, 128);
        special->clut = other_clut;
        SetSemiTrans(special, 1);
        SetShadeTex(special, 0);
        for (n = 0, angle = 0, blue = 64; n < 9; angle = n << 8) {
            radius = (s32)(rsin(angle) << 8) >> 12;
            height = (s32)(rcos(angle) << 8) >> 12;
            for (j = 0, a = 0; j < 17; a = j << 8) {
                setVector(&dome->points[n][j], rcos(a) * radius >> 12, -height,
                    rsin(a) * radius >> 12);
                j++;
            }
            dome->color[n].b = blue;
            blue += 8;
            dome->color[n].g = dome->color[n].r = n * 8;
            n++;
        }
        for (i = 0; i < 1; i++, band++) {
            for (j = 0, amount = 0; j < 33; amount -= 128, j++) {
                band->amount[j] = amount;
                band->offset[j] = amount;
                if (j < 28) {
                    band->color[j].r = 128;
                    band->color[j].g = 128;
                    band->color[j].b = 128;
                    band->secondary[j].r = 0;
                    band->secondary[j].g = 16;
                    band->secondary[j].b = 128;
                } else {
                    band->color[j].r = (32 - j) << 5;
                    band->color[j].g = (32 - j) << 5;
                    band->color[j].b = (32 - j) << 5;
                    band->secondary[j].r = 0;
                    band->secondary[j].g = (32 - j) << 2;
                    band->secondary[j].b = (32 - j) << 5;
                }
            }
            setVector(&band->scale, 4096, 4096, 4096);
            band->field_640 = 0;
            band->field_644 = 0;
            band->field_642 = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i / 2 * 2 == i - 1) {
                angle = 1024 + (i + 1) * 512;
            } else {
                angle = 1024 - i * 512;
            }
            band->heading[0] = (s32)(rcos(angle) << 8) >> 12;
            band->heading[1] = (s32)(rsin(angle) << 8) >> 12;
            band->heading[2] = 0;
        }
        for (k = 0; k < 2; k++, funnel++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&funnel->inner[j], (u32)rcos(angle) >> 4, 0,
                    (u32)rsin(angle) >> 4);
                setVector(&funnel->outer[j], (u32)(rcos(angle) * 5) >> 6, -64,
                    (u32)(rsin(angle) * 5) >> 6);
            }
            /* Both arms store zero; the split keeps the retail store before k++. */
            if (k == 1) {
                funnel->size = 0;
            } else {
                funnel->size = 0;
            }
            funnel->field_114 = 0;
        }
        for (i = 0; i < 12; i++, spiral++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    spiral->outer[j].r = 0;
                    spiral->outer[j].g = 0;
                    spiral->outer[j].b = 0;
                    spiral->inner[j].r = 0;
                    spiral->inner[j].g = 0;
                    spiral->inner[j].b = 0;
                } else {
                    spiral->outer[j].r = 255;
                    spiral->outer[j].g = 0;
                    spiral->outer[j].b = 255;
                    spiral->inner[j].r = 128;
                    spiral->inner[j].g = 128;
                    spiral->inner[j].b = 128;
                }
            }
        }
        for (k = 0; k < 2; k++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->v0[sheet_point], -128, -128, 0);
                    setVector(&sheet->v1[sheet_point], 0, -128, 0);
                    setVector(&sheet->v2[sheet_point], -128, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->v0[sheet_point], 128, -128, 0);
                    setVector(&sheet->v1[sheet_point], 128, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, -128, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->v0[sheet_point], 128, 128, 0);
                    setVector(&sheet->v1[sheet_point], 0, 128, 0);
                    setVector(&sheet->v2[sheet_point], 128, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->v0[sheet_point], -128, 128, 0);
                    setVector(&sheet->v1[sheet_point], -128, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, 128, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
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
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->inner.r = work->config->inner.r;
        work->inner.g = work->config->inner.g;
        work->inner.b = work->config->inner.b;
        work->outer.r = work->config->outer.r;
        work->outer.g = work->config->outer.g;
        work->outer.b = work->config->outer.b;
        work->field_20A8 = 0;
        work->field_2058 = 0;
        work->field_205C = 0;
        work->field_2064 = 0;
        work->field_2068 = 0;
        work->field_206C = 0;
        work->field_2070 = 0;
        work->field_2074 = 1024;
        work->field_2078 = 0;
        work->field_207C = 0;
        work->field_2088 = 0;
        work->field_208A = 0;
        work->field_208C = 0;
        work->field_208E = 0;
        work->width = 1024;
        work->field_2060 = 1024;
        work->field_2050 = 0;
        work->field_2052 = 0;
        work->field_20AC = 0;
        work->field_20B0 = 0;
        work->field_2090 = 1024;
        work->field_20B4 = 0;
        work->field_20B8 = 0;
        work->progress = 0;
        work->field_2094 = 0;
        work->field_2098 = 0;
        work->field_209C = 0;
        work->field_2080 = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->field_20C0 = 0;
        work->field_20C4 = 0;
        work->fade = 0;
        work->ambient = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx,
            view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx);
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
            &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &projection.target,
            &projection.interpolation, &projection.flag);
        work->screen_delta.vx = (u16)projection.target - screen_x;
        work->screen_delta.vy = (projection.target >> 16) - screen_y;
        if (work->config->draw_start <= work->elapsed) {
            func_8013C708((u8 *)point);
            if (work->phase < 6) {
                func_8013BE3C((u8 *)point);
            }
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
        config = work->config;
        if (work->elapsed >= config->grow_start && work->progress < 1024) {
            work->progress = ((work->elapsed - config->grow_start) << 10)
                / (config->grow_end - config->grow_start);
            if (work->progress >= 1024) {
                work->progress = 1024;
            }
        }
    }
    setVector(&work->camera, -512, -256, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->camera, 20);
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 6) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if ((u32)(work->phase - 2) < 3) {
        result = 4;
    } else if (work->phase == 6) {
        result = 1;
        work->phase = 7;
    } else if (work->phase == 7) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 8;
            }
        }
    } else if (work->phase == 8) {
        result = 2;
    }
    return result;
}
