#include "../../types.h"
#include "variant452_entry.h"

/* Header 452's entry: the header-439 skeleton with three webs, a band, a
 * sheet, rings, spoke rings and a fan, plus sixteen streamers and trails
 * for the three per-frame helpers. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant452EntryState *work = (Variant452EntryState *)point;
    /* Preserve the native frame, including two unused matrices. */
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant439EntryBand *band = work->bands;
    ModelVariantSheet *sheet = work->sheets;
    ModelVariantRing *ring = work->rings;
    ModelVariantSpokeRing *spoke = work->spokes;
    Variant439EntryFan *fan = work->fans;
    ModelVariantWeb *web = work->webs;
    Variant452Streamer *streamer = work->streamers;
    Variant452EntryTrail *trail = work->trails;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 outer, web_angle, row;
    s32 i, j, k, angle, base_angle;
    u16 tpage, clut;
    u16 flat_texture;
    s32 flat_page, packed;
    s32 near_radius, near_y, far_radius, far_y;
    s32 near_scale, far_scale;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant452EntryConfig *)(D_8013D3E4 + 0xB2C) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->delta,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D3E4 + 0xAF4));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D3E4 + 0xABC));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D3E4 + 0xA4C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D3E4 + 0xA84));
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
        for (i = 0; i < 1; i++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 9; j++) {
                band->ca[j].r = work->config->inner.r;
                band->ca[j].g = work->config->inner.g;
                band->ca[j].b = work->config->inner.b;
                band->cb[j].r = work->config->outer.r;
                band->cb[j].g = work->config->outer.g;
                band->cb[j].b = work->config->outer.b;
            }
            band->field_19C = 0;
            band->field_1A0 = 0;
            band->field_19E = 0;
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
        for (outer = 0; outer < 1; outer++, sheet++) {
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
        }
        near_scale = 768;
        far_scale = 512;
        for (outer = 0, web_angle = 0; outer < 3; outer++, web++, web_angle += 300) {
            for (row = 0, j = 2048 / 7; row < 6; row++, j = (row + 1) * 2048 / 7) {
                near_radius = rsin(j) * near_scale >> 12;
                near_y = rcos(j) * near_scale >> 12;
                far_radius = rsin(j) * far_scale >> 12;
                far_y = rcos(j) * far_scale >> 12;
                for (j = 0, angle = web_angle; j < 6; j++, angle = web_angle, angle += j * 4096 / 6) {
                    setVector(&web->near[row][j], rcos(angle) * near_radius >> 12,
                              near_y, rsin(angle) * near_radius >> 12);
                    setVector(&web->far[row][j], rcos(angle) * far_radius >> 12,
                              far_y, rsin(angle) * far_radius >> 12);
                }
            }
            MODEL_VARIANT_WORD(web, 0x244) = 0;
            MODEL_VARIANT_WORD(web, 0x248) = 0;
            MODEL_VARIANT_WORD(web, 0x24C) = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = -(outer * 4096 / 3);
            MODEL_VARIANT_WORD(web, 0x258) = 0;
            MODEL_VARIANT_WORD(web, 0x25C) = 0;
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
        for (i = 0; i < 16; i++, streamer++) {
            for (j = 0; j < 5; j++) {
                streamer->outer[j].r = -128 - j * 32;
                streamer->outer[j].g = -128 - j * 32;
                streamer->inner[j].r = -128 - j * 32;
                streamer->inner[j].g = -128 - j * 32;
                streamer->inner[j].b = -128 - j * 32;
                streamer->outer[j].b = 0;
            }
        }
        for (i = 0; i < 16; i++, trail++) {
            for (j = 0; j < 3; j++) {
                if (j == 2) {
                    trail->inner[j].r = 0;
                    trail->inner[j].g = 0;
                    trail->inner[j].b = 0;
                    trail->outer[j].r = 0;
                    trail->outer[j].g = 0;
                    trail->outer[j].b = 0;
                } else {
                    trail->inner[j].r = 255;
                    trail->inner[j].g = 0;
                    trail->inner[j].b = 255;
                    trail->outer[j].r = 128;
                    trail->outer[j].g = 128;
                    trail->outer[j].b = 128;
                }
            }
        }
        for (row = 0, base_angle = 0; row < 4; row++, spoke++, base_angle += 200) {
            for (outer = 0, angle = base_angle; outer < 8; outer++, angle = base_angle + outer * 512) {
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
        work->progress = 0;
        work->field_3398 = 0;
        work->field_339C = 0;
        work->field_3368 = 0;
        work->field_336A = 0;
        work->field_336C = 0;
        work->field_336E = 0;
        work->field_3370 = 0;
        work->field_3388 = 0;
        work->field_338C = 4096;
        work->field_338E = 1024;
        work->field_3390 = 0;
        work->field_33A0 = 0;
        work->field_33A4 = 0;
        work->field_338A = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_33AC = 0;
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
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->delta,
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
        setVector(&work->delta,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        setVector(&work->position,
                  work->matrix.t[0] + work->delta.vx * work->progress / 1536,
                  work->matrix.t[1] + work->delta.vy * work->progress / 1536
                      - (rsin(work->progress) * 256 >> 12),
                  work->matrix.t[2] + work->delta.vz * work->progress / 2048);
        setVector(&work->path,
                  work->target.vx - work->position.vx,
                  work->target.vy - work->position.vy,
                  work->target.vz - work->position.vz);
        if (work->config->start <= work->frame) {
            func_8013C0E0((u8 *)point);
            func_8013C51C((u8 *)point);
            if (work->phase > 0) {
                func_8013D010((u8 *)point);
            }
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
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
