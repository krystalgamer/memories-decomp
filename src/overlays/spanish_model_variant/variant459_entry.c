#include "../../types.h"
#include "variant459_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT459_CLUT_X
#define MODEL_VARIANT459_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant459EntryState *work = (Variant459EntryState *)point;
    u8 unknown_stack[64];
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant459EntryRing *ring = work->rings;
    Variant459EntryCurtain *curtain = work->curtains;
    Variant459EntryBand *band = work->bands;
    ModelVariantSheet *sheet = work->sheets;
    Variant459EntryWeb *web = work->webs;
    Variant459EntryGlow *glow = work->glows;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *plain = &work->plain;
    POLY_GT4 *ray = &work->ray;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    u16 tpage, clut;
    s32 packed, flat_page, ray_page;
    u16 flat_clut, ray_clut;
    s32 base_angle, n, row, near_y, far_y;
    s32 i, j, angle, ang;
    s32 k;
    s32 near_scale, far_scale;
    s32 near_r, far_r;
    s32 side;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant459EntryConfig *)(D_8013F548 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        work->parts[3] = Model_GetSlotDataEntry(work->slot, work->config->parts[3]);
        work->parts[4] = Model_GetSlotDataEntry(work->slot, work->config->parts[4]);
        work->parts[5] = Model_GetSlotDataEntry(work->slot, work->config->parts[5]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        if (work->config->mode == 1) {
            Model_TransformPartVertex(work->slot, work->config->parts[0], work->config->vertices[0],
                                      &work->points[0]);
            Model_TransformPartVertex(work->slot, work->config->parts[1], work->config->vertices[1],
                                      &work->points[1]);
            Model_TransformPartVertex(work->slot, work->config->parts[2], work->config->vertices[2],
                                      &work->points[2]);
        } else {
            GsGetLwUnit(work->parts[0], &work->matrices[0]);
            setVector(&work->direction,
                      work->target.vx - work->matrices[0].t[0],
                      work->target.vy - work->matrices[0].t[1],
                      work->target.vz - work->matrices[0].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT459_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT459_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F548 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F548 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F548 + 0x1C));
        ray_page = packed >> 16;
        ray_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F548 + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F548 + 0x54));
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
        SetPolyGT4(ray);
        ray->tpage = ray_page;
        setUV4(ray, 128, 208, 255, 208, 128, 128, 255, 128);
        ray->clut = ray_clut;
        SetSemiTrans(ray, 1);
        SetShadeTex(ray, 0);
        for (n = 0; n < 5; n++, curtain++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&curtain->a[j], (u32)(rcos(angle) * 3) >> 7, (u32)(rsin(angle) * 3) >> 7, 0);
                setVector(&curtain->b[j], (u32)rcos(angle) >> 5, (u32)rsin(angle) >> 5, 32);
                setVector(&curtain->c[j], (u32)(rcos(angle) * 7) >> 8, (u32)(rsin(angle) * 7) >> 8, 32);
                curtain->color_a[0] = 255;
                curtain->color_a[1] = 255;
                curtain->color_a[2] = 255;
                curtain->color_b[0] = 255;
                curtain->color_b[1] = 0;
                curtain->color_b[2] = 255;
            }
            curtain->scale = -((n << 11) / 5);
            curtain->growth = 0;
        }
        for (n = 0; n < 4; n++, ring++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&ring->a[j], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&ring->b[j], (u32)(rcos(angle) * 5) >> 6, (u32)(rsin(angle) * 5) >> 6, -64);
            }
            if (n == 3) {
                ring->scale = 0;
            } else {
                ring->scale = -((n << 12) / 3);
            }
            ring->growth = 0;
        }
        for (i = 0; i < 6; i++, band++) {
            band->scale[0] = 4096;
            band->scale[1] = 4096;
            band->scale[2] = 4096;
            for (j = 0; j < 9; j++) {
                band->inner[j].r = work->config->inner.r;
                band->inner[j].g = work->config->inner.g;
                band->inner[j].b = work->config->inner.b;
                band->outer[j].r = work->config->outer.r;
                band->outer[j].g = work->config->outer.g;
                band->outer[j].b = work->config->outer.b;
            }
            band->field_19C = 0;
            band->field_1A0 = 0;
            band->field_19E = 0;
        }
        for (n = 0; n < 13; n++, sheet++) {
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
        far_scale = 192;
        near_scale = 256;
        for (n = 0, base_angle = 0; n < 3; n++, web++, base_angle += 300) {
            for (row = 0, ang = 2048 / 5; row < 4; row++, ang = (row + 1) * 2048 / 5) {
                near_r = rsin(ang) * near_scale >> 12;
                near_y = rcos(ang) * near_scale >> 12;
                far_r = rsin(ang) * far_scale >> 12;
                far_y = rcos(ang) * far_scale >> 12;
                for (angle = base_angle, j = 0; j < 6; j++, angle = base_angle + j * 4096 / 6) {
                    setVector(&web->near[row][j], rcos(angle) * near_r >> 12, near_y, rsin(angle) * near_r >> 12);
                    setVector(&web->far[row][j], rcos(angle) * far_r >> 12, far_y, rsin(angle) * far_r >> 12);
                }
            }
            web->field_184 = 0;
            web->field_188 = 0;
            web->field_18C = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = n * 4096 / 3 + 4096;
            web->field_198 = 0;
            web->field_19C = 0;
        }
        for (i = 0; i < 12; glow++, i++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    glow->outer[j].r = 0;
                    glow->outer[j].g = 0;
                    glow->outer[j].b = 0;
                    glow->inner[j].r = 0;
                    glow->inner[j].g = 0;
                    glow->inner[j].b = 0;
                } else {
                    glow->outer[j].r = 255;
                    glow->outer[j].g = 0;
                    glow->outer[j].b = 255;
                    glow->inner[j].r = 128;
                    glow->inner[j].g = 128;
                    glow->inner[j].b = 128;
                }
            }
        }
        work->color_30C0.r = 192;
        work->color_30C0.g = 192;
        work->color_30C0.b = 192;
        work->field_30CC = 4096;
        work->field_30CE = 1024;
        work->color_30C4.r = 0;
        work->color_30C4.g = 0;
        work->color_30C4.b = 0;
        work->field_30C8 = 0;
        work->field_3094 = 0;
        work->field_3096 = 0;
        work->field_3098 = 0;
        work->field_309A = 0;
        work->field_30BC = 0;
        work->field_30D0 = 0;
        work->field_30D4 = 0;
        work->field_30D8 = 0;
        work->field_30BE = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_30E0 = 0;
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
            side = -1;
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
            side = 1;
        }
        GsGetLwUnit(work->parts[0], &work->matrices[0]);
        setVector(&work->direction,
                  work->target.vx - work->matrices[0].t[0],
                  work->target.vy - work->matrices[0].t[1],
                  work->target.vz - work->matrices[0].t[2]);
        if (work->config->spiral == 1) {
            for (i = 0, angle = -1024; i < work->config->count;
                 i++, angle = (i << 12) / work->config->count - 1024) {
                setVector(&work->points[i], work->matrices[0].t[0] + (rcos(angle) * 24 >> 12),
                          work->matrices[0].t[1] + (rsin(angle) * 24 >> 12) - 64,
                          work->matrices[0].t[2] + side * 16);
                setVector(&work->directions[i], work->target.vx - work->points[i].vx,
                          work->target.vy - work->points[i].vy,
                          work->target.vz - work->points[i].vz);
            }
        } else if (work->config->mode == 1) {
            for (i = 0; i < work->config->count; i++) {
                if (i == 0) {
                    Model_TransformPartVertex(work->slot, work->config->parts[0],
                                              work->config->vertices[0], &work->points[0]);
                } else if (i == 1) {
                    Model_TransformPartVertex(work->slot, work->config->parts[1],
                                              work->config->vertices[1], &work->points[1]);
                } else {
                    Model_TransformPartVertex(work->slot, work->config->parts[2],
                                              work->config->vertices[2], &work->points[i]);
                }
                setVector(&work->directions[i], work->target.vx - work->points[i].vx,
                          work->target.vy - work->points[i].vy,
                          work->target.vz - work->points[i].vz);
            }
        } else {
            GsGetLwUnit(work->parts[1], &work->matrices[1]);
            GsGetLwUnit(work->parts[2], &work->matrices[2]);
            GsGetLwUnit(work->parts[3], &work->matrices[3]);
            GsGetLwUnit(work->parts[4], &work->matrices[4]);
            GsGetLwUnit(work->parts[5], &work->matrices[5]);
            work->points[0].vx = work->matrices[0].t[0];
            work->points[0].vy = work->matrices[0].t[1];
            work->points[0].vz = work->matrices[0].t[2];
            work->points[1].vx = work->matrices[1].t[0];
            work->points[1].vy = work->matrices[1].t[1];
            work->points[1].vz = work->matrices[1].t[2];
            work->points[2].vx = work->matrices[2].t[0];
            work->points[2].vy = work->matrices[2].t[1];
            work->points[2].vz = work->matrices[2].t[2];
            work->points[3].vx = work->matrices[3].t[0];
            work->points[3].vy = work->matrices[3].t[1];
            work->points[3].vz = work->matrices[3].t[2];
            work->points[4].vx = work->matrices[4].t[0];
            work->points[4].vy = work->matrices[4].t[1];
            work->points[4].vz = work->matrices[4].t[2];
            work->points[5].vx = work->matrices[5].t[0];
            work->points[5].vy = work->matrices[5].t[1];
            work->points[5].vz = work->matrices[5].t[2];
            for (i = 0; i < work->config->count; i++) {
                setVector(&work->directions[i], work->target.vx - work->points[i].vx,
                          work->target.vy - work->points[i].vy,
                          work->target.vz - work->points[i].vz);
            }
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrices[0].t[0], work->matrices[0].t[1], work->matrices[0].t[2]);
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
        GsGetLwUnit(work->parts[0], &work->matrices[0]);
        work->direction.vx = work->target.vx - work->matrices[0].t[0];
        work->direction.vy = work->target.vy - work->matrices[0].t[1];
        work->direction.vz = work->target.vz - work->matrices[0].t[2];
        if (work->config->start <= work->frame) {
            func_8013EA24((u8 *)point);
        }
        if (work->config->curtains == 1) {
            func_8013C9AC((u8 *)point);
        }
        if (work->phase > 0) {
            func_8013EFE0((u8 *)point);
            func_8013E224((u8 *)point);
        }
        if (work->phase >= 2) {
            func_8013D604((u8 *)point);
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
