#include "../../types.h"
#include "variant464_entry.h"

#ifndef MODEL_VARIANT464_CLUT_X
#define MODEL_VARIANT464_CLUT_X 640
#endif

s32 FrenchModelVariant464_HandleRequest(SVECTOR *point, s32 command)
{
    Variant464EntryState *work = (Variant464EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant464EntryPrimary *primary = work->primary;
    ModelVariantSheetSet *sheet = work->sheets;
    ModelVariantRing *ring = work->rings;
    ModelVariantSpokeRing *spoke = work->spokes;
    Variant464EntryFan *fan = work->fans;
    ModelVariantWebNarrow *web = work->webs;
    Variant464EntryScreenRing *screen_ring = work->screen_rings;
    ModelVariantCurtain *curtain = work->curtains;
    POLY_GT4 *textured = work->textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
        POLY_GT4 *curtain_poly = &work->curtain_poly;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    u16 curtain_clut;
    s32 outer, web_angle, row;
    s32 i, j, k, angle, base_angle;
    s32 r, g, b;
    s32 packed, curtain_page;
    s32 near_radius, near_y, far_radius, far_y;
    s32 near_scale, far_scale;
    u16 tpage, clut;
    u16 flat_texture;
    s32 flat_page;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant464EntryConfig *)(gFrenchModelVariant464_D2DAC + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT464_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT464_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant464_D2DAC + 0xC4));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant464_D2DAC + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant464_D2DAC + 0x1C));
        curtain_page = packed >> 16;
        curtain_clut = packed;
        func_80059A50(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant464_D2DAC + 0x54));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant464_D2DAC + 0x0));
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = packed >> 16;
        setUV4(textured, 96, 255, 96, 128, 127, 255, 127, 128);
        textured->clut = packed;
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
        SetPolyGT4(curtain_poly);
        curtain_poly->tpage = curtain_page;
        setUV4(curtain_poly, 128, 207, 255, 207, 128, 128, 255, 128);
        curtain_poly->clut = curtain_clut;
        SetSemiTrans(curtain_poly, 1);
        SetShadeTex(curtain_poly, 0);
        for (i = 0; i < 5; i++, primary++) {
            setVector(&primary->scale, 4096, 4096, 4096);
            for (j = 0; j < 2; j++) {
                primary->inner[j].r = work->config->inner.r;
                primary->inner[j].g = work->config->inner.g;
                primary->inner[j].b = work->config->inner.b;
                primary->outer[j].r = work->config->outer.r;
                primary->outer[j].g = work->config->outer.g;
                primary->outer[j].b = work->config->outer.b;
                primary->progress[j] = -(i * 2048) - j * 512;
            }
            primary->field_68 = 0;
            primary->field_6C = 0;
            primary->field_6A = 0;
            primary->active = 0;
        }
        for (outer = 0; outer < 1; outer++, fan++) {
            setVector(&fan->point[0], 0, 0, 0);
            for (j = 0, angle = 0; j < 5; j++, angle = j * 1024) {
                setVector(&fan->point[j + 1], (u32)rcos(angle) >> 6,
                          (u32)rsin(angle) >> 8, 0);
                setVector(&fan->point[j + 6], (u32)(rcos(angle) * 3) >> 10,
                          (u32)(rsin(angle) * 5) >> 9, 0);
            }
            setVector(&fan->corner[0], -128, -128, 0);
            setVector(&fan->corner[1], 128, -128, 0);
            setVector(&fan->corner[2], -128, 128, 0);
            setVector(&fan->corner[3], 128, 128, 0);
            fan->inner[0] = 255;
            fan->inner[1] = 192;
            fan->inner[2] = 192;
            fan->outer[0] = 64;
            fan->outer[1] = 64;
            fan->outer[2] = 0;
            fan->size = 0;
            fan->field_84 = 0;
            fan->done = 0;
        }
        for (outer = 0; outer < 3; outer++, screen_ring++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                if (j % 8 == 0) {
                    r = 255;
                    g = 0;
                    b = 0;
                } else if (j % 8 == 1) {
                    r = 255;
                    g = 128;
                    b = 0;
                } else if (j % 8 == 2) {
                    r = 255;
                    g = 255;
                    b = 0;
                } else if (j % 8 == 3) {
                    r = 0;
                    g = 255;
                    b = 0;
                } else if (j % 8 == 4) {
                    r = 0;
                    g = 255;
                    b = 255;
                } else if (j % 8 == 5) {
                    r = 0;
                    g = 128;
                    b = 255;
                } else if (j % 8 == 6) {
                    r = 0;
                    g = 0;
                    b = 255;
                } else {
                    r = 255;
                    g = 0;
                    b = 255;
                }
                setVector(&screen_ring->a[j], (u32)rcos(angle) >> 4,
                          (u32)rsin(angle) >> 4, 0);
                setVector(&screen_ring->b[j], (u32)(rcos(angle) * 3) >> 5,
                          (u32)(rsin(angle) * 3) >> 5, -128);
                setVector(&screen_ring->c[j], (u32)rcos(angle) >> 3,
                          (u32)rsin(angle) >> 3, -256);
                screen_ring->color[j].r = r;
                screen_ring->color[j].g = g;
                screen_ring->color[j].b = b;
            }
            screen_ring->scale = -(outer * 4096 / 3);
            screen_ring->count = 0;
        }
        for (outer = 0; outer < 3; outer++, curtain++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&curtain->a[j], (u32)rcos(angle) >> 4,
                          (u32)rsin(angle) >> 4, 0);
                setVector(&curtain->b[j], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, -64);
            }
            if (outer == 2) {
                curtain->scale = 0;
            } else {
                curtain->scale = -(outer * 2048);
            }
            curtain->count = 0;
        }
        for (outer = 0; outer < 6; outer++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -192, -192, 0);
                    setVector(&sheet->v1[k], 0, -192, 0);
                    setVector(&sheet->v2[k], -192, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 192, -192, 0);
                    setVector(&sheet->v1[k], 192, 0, 0);
                    setVector(&sheet->v2[k], 0, -192, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 192, 192, 0);
                    setVector(&sheet->v1[k], 0, 192, 0);
                    setVector(&sheet->v2[k], 192, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -192, 192, 0);
                    setVector(&sheet->v1[k], -192, 0, 0);
                    setVector(&sheet->v2[k], 0, 192, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->outer[0] = work->config->outer.r;
            sheet->outer[1] = work->config->outer.g;
            sheet->outer[2] = work->config->outer.b;
            sheet->inner[0] = work->config->inner.r;
            sheet->inner[1] = work->config->inner.g;
            sheet->inner[2] = work->config->inner.b;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
            sheet->shown = 0;
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
            MODEL_VARIANT_WORD(web, 0x184) = 0;
            MODEL_VARIANT_WORD(web, 0x188) = 0;
            MODEL_VARIANT_WORD(web, 0x18C) = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = 4096 + outer * 4096 / 3;
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (row = 0, base_angle = 0; row < 6; row++, ring++, base_angle += 256) {
            for (outer = 0, angle = base_angle; outer < 8;
                 outer++, angle = base_angle + outer * 512) {
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
        for (row = 0, base_angle = 0; row < 4; row++, spoke++, base_angle += 200) {
            for (outer = 0, angle = base_angle; outer < 8;
                 outer++, angle = base_angle + outer * 512) {
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
        work->field_1C44 = 0;
        work->field_1C48 = 4096;
        work->field_1C4A = 1024;
        work->field_1C4C = 0;
        work->outer.r = work->config->outer.r;
        work->outer.g = work->config->outer.g;
        work->outer.b = work->config->outer.b;
        work->inner.r = work->config->inner.r;
        work->inner.g = work->config->inner.g;
        work->inner.b = work->config->inner.b;
        work->field_1C5C = 0;
        work->timing_index = 0;
        work->field_1C50 = 0;
        work->field_1C46 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->fade = 0;
        work->tint = 2048;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
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
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        if (work->config->times[work->timing_index] <= work->frame &&
            work->timing_index < work->config->count) {
            for (primary = work->primary, i = 0; i < 5; i++, primary++) {
                for (j = 0; j < 2; j++) {
                    primary->progress[j] = -(i * 1024) - j * 512;
                }
                primary->active = 0;
            }
            work->timing_index++;
            for (outer = 0, sheet = work->sheets; outer < 5; outer++, sheet++) {
                sheet->shown = 0;
            }
        }
        if (work->config->start <= work->frame) {
            SpanishModelVariant464_DrawSheets((u8 *)point);
            FrenchModelVariant464_DrawRings((u8 *)point);
        }
        if (work->config->times[0] <= work->frame) {
            SpanishModelVariant464_DrawAdvancingQuads((u8 *)point);
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
    if (work->phase < 6) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
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
