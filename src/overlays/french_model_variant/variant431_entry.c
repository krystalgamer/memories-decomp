#include "../../types.h"
#include "variant431_entry.h"

#ifndef MODEL_VARIANT431_CLUT_X
#define MODEL_VARIANT431_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant431EntryState *work = (Variant431EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant431EntryBand *band = work->bands;
    ModelVariantSheet *sheet = work->sheets;
    ModelVariantRing *ring = work->rings;
    ModelVariantSpokeRing *spoke = work->spokes;
    Variant414Fan *fan = work->fans;
    ModelVariantWebNarrow *web = work->webs;
    POLY_GT4 *textured = work->textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *extra = &work->extra;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    s32 outer, web_angle, row;
    s32 i, j, k, angle, base_angle;
    s32 near_radius, near_y, far_radius, far_y;
    s32 near_scale, far_scale;
    u16 tpage, clut;
    s32 flat_texture, flat_page, packed;
    GsRVIEW2 *view;
    u16 screen_x, screen_y;
    s16 dx;
    s32 dy;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant431EntryConfig *)(D_8013DF48 + 0xFC) + command;
        for (i = 0; i < 5; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        for (i = 0; i < 5; i++) {
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i],
                      work->target.vx - work->matrices[i].t[0],
                      work->target.vy - work->matrices[i].t[1],
                      work->target.vz - work->matrices[i].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT431_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT431_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DF48 + 0xC4));
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DF48 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DF48 + 0x1C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DF48 + 0xE0));
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
        for (i = 0; i < 5; i++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 5; j++) {
                if (j == 4) {
                    band->ca[j].r = 0;
                    band->ca[j].g = 0;
                    band->ca[j].b = 0;
                    band->cb[j].r = 0;
                    band->cb[j].g = 0;
                    band->cb[j].b = 0;
                } else {
                    band->ca[j].r = 128;
                    band->ca[j].g = 64;
                    band->ca[j].b = 0;
                    band->cb[j].r = 128;
                    band->cb[j].g = 128;
                    band->cb[j].b = 120;
                }
                band->progress[j] = -j * 256 - (i << 8);
            }
            band->field_EC = 0;
            band->field_F0 = 0;
            band->field_EE = 0;
            band->field_108 = 1024;
        }
        near_scale = 256;
        far_scale = 192;
        for (outer = 0, web_angle = 0; outer < 3; outer++, web++, web_angle += 300) {
            for (row = 0, j = 2048 / 5; row < 4; row++, j = (row + 1) * 2048 / 5) {
                near_radius = rsin(j) * near_scale >> 12;
                near_y = rcos(j) * near_scale >> 12;
                far_radius = rsin(j) * far_scale >> 12;
                far_y = rcos(j) * far_scale >> 12;
                for (j = 0, angle = web_angle; j < 6; j++, angle = web_angle + j * 4096 / 6) {
                    setVector(&web->near[row][j], rcos(angle) * near_radius >> 12,
                              near_y, rsin(angle) * near_radius >> 12);
                    setVector(&web->far[row][j], rcos(angle) * far_radius >> 12,
                              far_y, rsin(angle) * far_radius >> 12);
                }
            }
            web->color.r = 255;
            web->color.g = 255;
            web->color.b = 255;
            MODEL_VARIANT_WORD(web, 0x184) = 0;
            MODEL_VARIANT_WORD(web, 0x188) = 0;
            MODEL_VARIANT_WORD(web, 0x18C) = 0;
            web->scale = -(outer * 8192 / 3);
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (outer = 0; outer < 5; outer++, fan++) {
            setVector(&fan->point[0], 0, 0, 0);
            for (j = 0, angle = 0; j < 5; j++, angle = j * 1024) {
                setVector(&fan->point[j + 1], (u32)rcos(angle) >> 6,
                          (u32)rsin(angle) >> 8, 0);
                setVector(&fan->point[j + 6], (u32)(rcos(angle) * 3) >> 10,
                          (u32)(rsin(angle) * 5) >> 9, 0);
            }
            fan->inner[0] = 255;
            fan->inner[1] = 192;
            fan->inner[2] = 192;
            fan->outer[0] = 64;
            fan->outer[1] = 64;
            fan->outer[2] = 0;
            fan->size = 0;
            MODEL_VARIANT_WORD(fan, 0x64) = 0;
            MODEL_VARIANT_WORD(fan, 0x68) = 0;
            fan->done = 0;
        }
        for (outer = 0; outer < 6; outer++, sheet++) {
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
            sheet->inner[0] = 255;
            sheet->inner[1] = 128;
            sheet->inner[2] = 0;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
        }
        for (row = 0, base_angle = 0; row < 6; row++, ring++, base_angle += 256) {
            for (outer = 0, angle = base_angle; outer < 8; angle += 512, outer++) {
                setVector(&ring->inner[outer], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&ring->outer[outer], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
            }
            ring->color[0] = 64;
            ring->color[1] = 128;
            ring->color[2] = 255;
            ring->pad84[0] = 0;
            ring->pad84[1] = 0;
            ring->pad84[2] = 0;
            ring->angle = row * 1024 / 6;
            ring->count = 0;
        }
        for (row = 0, base_angle = 0; row < 4; base_angle += 200, row++, spoke++) {
            for (outer = 0, angle = base_angle; outer < 8; angle += 512, outer++) {
                setVector(&spoke->inner[outer], (u32)(rcos(angle) * 3) >> 5,
                          (u32)(rsin(angle) * 3) >> 5, 0);
                setVector(&spoke->outer[outer], (u32)rcos(angle) >> 3, (u32)rsin(angle) >> 3, 0);
            }
            spoke->pad80[0] = 0;
            spoke->pad80[1] = 0;
            spoke->pad80[2] = 0;
            spoke->color[0] = 255;
            spoke->color[1] = 192;
            spoke->color[2] = 64;
            spoke->angle = 1024 - row * 256;
            spoke->count = 0;
        }
        work->field_18CC = 0;
        work->field_18D0 = 1024;
        work->field_18D2 = 1024;
        work->field_18D4 = 0;
        work->start_index = 0;
        work->field_18DC = 0;
        work->field_18CE = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
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
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        if (work->slot == 0) {
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        for (i = 0; i < 5; i++) {
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i],
                      work->target.vx - work->matrices[i].t[0],
                      work->target.vy - work->matrices[i].t[1],
                      work->target.vz - work->matrices[i].t[2]);
            setVector(&position, work->matrices[i].t[0], work->matrices[i].t[1], work->matrices[i].t[2]);
            RotTransPers(&position, (PSXLONG *)&projection.target,
                         &projection.interpolation, &projection.flag);
            dx = screen_x - projection.target.vx;
            dy = screen_y - projection.target.vy;
            work->screen_x[i] = dx;
            work->screen_y[i] = dy;
        }
        if (work->config->first_start <= work->frame) {
            func_8013C338((u8 *)point);
        }
        if (work->config->timings[0] <= work->frame) {
            func_8013D444((u8 *)point);
        }
        if (work->config->timings[work->start_index] <= work->frame) {
            func_8013C7C8((u8 *)point);
        }
        if (work->phase >= 2) {
            func_8013BF1C((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    func_800595C8(2, work->tint, work->tint, work->tint);
    if (work->phase < 7) {
        if (work->tint > 0) {
            work->tint -= 256;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->phase >= 2 && work->phase <= 4) {
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
