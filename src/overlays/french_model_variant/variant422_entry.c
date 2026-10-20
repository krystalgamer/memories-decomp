#include "../../types.h"
#include "variant422_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant422EntryState *work = (Variant422EntryState *)point;
    Variant422EntryStack stack;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant405Halo *halo;
    Variant405Veil *veil;
    Variant422EntryBand *band;
    ModelVariantSheet *sheet;
    ModelVariantRing *ring;
    ModelVariantSpokeRing *spoke;
    Variant422EntryQuad *quad_record;
    ModelVariantWebNarrow *web;
    POLY_GT4 *textured;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_GT4 *halo_poly;
    POLY_GT4 *extra;
    POLY_FT4 *flat;
    s32 result;
    s32 web_angle, outer, row;
    s32 i, j, k, angle, base_angle, ring_angle;
    s32 near_radius, near_y, far_radius, far_y;
    s32 near_scale, far_scale;
    s32 page, flat_page, halo_page, halo_clut, packed;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;

    band = work->bands;
    sheet = work->sheets;
    ring = work->rings;
    spoke = work->spokes;
    quad_record = work->quads;
    web = work->webs;
    textured = work->textured;
    flat = work->flat_textured;
    halo = &work->halo;
    triangle = &work->triangle;
    quad = &work->quad;
    halo_poly = &work->halo_poly;
    extra = &work->extra;
    veil = work->veils;
    result = 0;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant422EntryConfig *)(D_8013E834 + 0x38) + command;
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
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)D_8013E834);
        page = packed >> 16;
        stack.texture_cluts[1] = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013E834 + 0x1C));
        flat_page = packed >> 16;
        stack.texture_cluts[0] = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)D_8013E834);
        halo_page = packed >> 16;
        halo_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013E834 + 0x1C));
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = page;
        setUV4(textured, 32, 0, 33, 0, 32, 31, 33, 31);
        textured->clut = stack.texture_cluts[1];
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        textured++;
        SetPolyGT4(textured);
        textured->tpage = page;
        setUV4(textured, 0, 0, 31, 0, 0, 31, 31, 31);
        textured->clut = stack.texture_cluts[1];
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 31, 32, 31, 63, 0, 32, 0, 63);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 31, 64, 31, 95, 0, 64, 0, 95);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        SetPolyGT4(extra);
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(halo_poly);
        halo_poly->tpage = halo_page;
        setUV4(halo_poly, 0, 0, 0, 127, 0, 0, 0, 111);
        halo_poly->clut = halo_clut;
        SetSemiTrans(halo_poly, 1);
        SetShadeTex(halo_poly, 0);
        for (outer = 0; outer < 5; outer++, veil++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&veil->a[j], (u32)(rcos(angle) * 7) >> 7, 0,
                          (u32)(rsin(angle) * 7) >> 7);
                setVector(&veil->b[j], (u32)(rcos(angle) * 5) >> 6, 48,
                          (u32)(rsin(angle) * 5) >> 6);
                setVector(&veil->c[j], (u32)(rcos(angle) * 13) >> 7, 48,
                          (u32)(rsin(angle) * 13) >> 7);
            }
            veil->scale = -outer * 2048 / 5;
            veil->count = 0;
        }
        for (row = 0; row < 5; row++) {
            halo->rise[row] = -(row * 256);
            halo->fall[row] = -((row + 1) * 256);
            halo->count[row] = 0;
        }
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
            band->field_1A0 = 0;
            band->field_19C = 0;
            band->field_1A2 = 0;
        }
        for (outer = 0; outer < 1; outer++, quad_record++) {
            setVector(&quad_record->point[0], 0, 0, 0);
            for (j = 0, angle = 0; j < 5; j++, angle = j * 1024) {
                setVector(&quad_record->point[j + 1], (u32)rcos(angle) >> 6,
                          (u32)rsin(angle) >> 8, 0);
                setVector(&quad_record->point[j + 6], (u32)(rcos(angle) * 3) >> 10,
                          (u32)(rsin(angle) * 5) >> 9, 0);
            }
            setVector(&quad_record->corner[0], -128, -128, 0);
            setVector(&quad_record->corner[1], 128, -128, 0);
            setVector(&quad_record->corner[2], -128, 128, 0);
            setVector(&quad_record->corner[3], 128, 128, 0);
            quad_record->inner[0] = 255;
            quad_record->inner[1] = 192;
            quad_record->inner[2] = 192;
            quad_record->outer[0] = 64;
            quad_record->outer[1] = 64;
            quad_record->outer[2] = 0;
            quad_record->size = 0;
            quad_record->field_84 = 0;
            quad_record->done = 0;
        }
        for (outer = 0; outer < 2; outer++, sheet++) {
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
        near_scale = 256;
        far_scale = 192;
        for (outer = 0, web_angle = 0; outer < 3; outer++, web++, web_angle += 300) {
            for (row = 0, base_angle = 2048 / 5; row < 4; row++, base_angle = (row + 1) * 2048 / 5) {
                near_radius = rsin(base_angle) * near_scale >> 12;
                near_y = rcos(base_angle) * near_scale >> 12;
                far_radius = rsin(base_angle) * far_scale >> 12;
                far_y = rcos(base_angle) * far_scale >> 12;
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
            for (outer = 0, ring_angle = base_angle; outer < 8;
                 outer++, ring_angle = base_angle + outer * 512) {
                setVector(&ring->inner[outer], (u32)rcos(ring_angle) >> 4, (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring->outer[outer], (u32)(rcos(ring_angle) * 5) >> 6,
                          (u32)(rsin(ring_angle) * 5) >> 6, 0);
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
            for (outer = 0, ring_angle = base_angle; outer < 8;
                 outer++, ring_angle = base_angle + outer * 512) {
                setVector(&spoke->inner[outer], (u32)(rcos(ring_angle) * 3) >> 5,
                          (u32)(rsin(ring_angle) * 3) >> 5, 0);
                setVector(&spoke->outer[outer], (u32)rcos(ring_angle) >> 3, (u32)rsin(ring_angle) >> 3, 0);
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
        work->field_1DF8 = 0;
        work->field_1DFC = 0;
        work->field_1E00 = 0;
        work->field_1E04 = 0;
        work->field_1E08 = 1024;
        work->field_1DF0 = 0;
        work->field_1DF4 = 0;
        work->field_1E0C = 0;
        work->field_1E10 = 4096;
        work->field_1E12 = 1024;
        work->field_1E14 = 0;
        work->field_1E18 = 0;
        work->field_1E0E = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_1E20 = 0;
        work->field_1E24 = 0;
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
        if (work->config->start <= work->frame) {
            func_8013C620((u8 *)point);
        }
        if (work->phase >= 2) {
            func_8013C128((u8 *)point);
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
    if (work->phase < 5) {
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
