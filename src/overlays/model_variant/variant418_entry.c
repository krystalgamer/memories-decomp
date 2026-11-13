#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "variant418_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT418_CLUT_X
#define MODEL_VARIANT418_CLUT_X 512
#endif

/* Header 418's entry: the French header-439 entry shape with twelve spiral
 * arms, 6x6 webs, one sheet, the header-338 matrix setup when the config
 * mode is 1, and no screen rings or curtains. */

s32 ModelVariant418_HandleRequest(SVECTOR *point, s32 command)
{
    Variant418EntryState *work = (Variant418EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ModelVariantProjection projection;
    ModelVariantColorBandPadded *band = work->bands;
    ModelVariantSheet *sheet = work->sheets;
    ModelVariantRing *ring = work->rings;
    ModelVariantRing *spoke = work->spokes;
    ModelVariantFan *fan = work->fans;
    ModelVariantWeb *web = work->webs;
    ModelVariantFlaggedSpiralArm *arm = work->arms;
    POLY_GT4 *textured = work->packets.textured;
    POLY_G3 *triangle = &work->packets.triangle;
    POLY_G4 *quad = &work->packets.quad;
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
    u32 screen_x;
    s32 screen_y;
    s32 dy;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = gModelVariant418_aConfigs + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->aim.target, 0, -300, -450);
        } else {
            setVector(&work->aim.target, 0, -300, 450);
        }
        if (work->config->mode == 1) {
            Model_TransformPartVertex(work->slot, work->config->parts[0], work->config->vertex,
                          &work->vertex_position);
            work->matrix.t[0] = work->vertex_position.vx;
            work->matrix.t[1] = work->vertex_position.vy;
            work->matrix.t[2] = work->vertex_position.vz;
        } else {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        setVector(&work->aim.direction,
                  work->aim.target.vx - work->matrix.t[0],
                  work->aim.target.vy - work->matrix.t[1],
                  work->aim.target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT418_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT418_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant418_aImages[7]);
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant418_aImages[5]);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant418_aImages[1]);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant418_aImages[2]);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant418_aImages[3]);
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
        for (i = 0; i < 1; i++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
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
            sheet->outer[0] = work->config->outer.r;
            sheet->outer[1] = work->config->outer.g;
            sheet->outer[2] = work->config->outer.b;
            sheet->inner[0] = work->config->inner.r;
            sheet->inner[1] = work->config->inner.g;
            sheet->inner[2] = work->config->inner.b;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        near_scale = 512;
        far_scale = 320;
        for (outer = 0, web_angle = 0; outer < 3; outer++, web++, web_angle += 300) {
            for (row = 0, j = 2048 / 7; row < 6; row++, j = (row + 1) * 2048 / 7) {
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
            MODEL_VARIANT_WORD(web, 0x244) = 0;
            MODEL_VARIANT_WORD(web, 0x248) = 0;
            MODEL_VARIANT_WORD(web, 0x24C) = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = -(outer * 8192 / 3);
            MODEL_VARIANT_WORD(web, 0x258) = 0;
            MODEL_VARIANT_WORD(web, 0x25C) = 0;
        }
        for (row = 0, base_angle = 0; row < 6; row++, ring++, base_angle += 256) {
            for (outer = 0, angle = base_angle; outer < 8; angle += 512, outer++) {
                setVector(&ring->inner[outer], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&ring->outer[outer], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
            }
            ring->inner_color[0] = 64;
            ring->inner_color[1] = 128;
            ring->inner_color[2] = 255;
            ring->outer_color[0] = 0;
            ring->outer_color[1] = 0;
            ring->outer_color[2] = 0;
            ring->angle = row * 1024 / 6;
            ring->count = 0;
        }
        for (i = 0; i < 12; i++, arm++) {
            for (j = 0; j < 2; j++) {
                arm->ca[j][0] = work->config->inner.r * (1 - j);
                arm->ca[j][1] = work->config->inner.g * (1 - j);
                arm->ca[j][2] = work->config->inner.b * (1 - j);
                arm->cb[j][0] = work->config->outer.r * (1 - j);
                arm->cb[j][1] = work->config->outer.g * (1 - j);
                arm->cb[j][2] = work->config->outer.b * (1 - j);
            }
        }
        for (row = 0, base_angle = 0; row < 4; row++, spoke++, base_angle += 200) {
            for (outer = 0, angle = base_angle; outer < 8; outer++, angle = base_angle + outer * 512) {
                setVector(&spoke->inner[outer], (u32)(rcos(angle) * 3) >> 5,
                          (u32)(rsin(angle) * 3) >> 5, 0);
                setVector(&spoke->outer[outer], (u32)rcos(angle) >> 3, (u32)rsin(angle) >> 3, 0);
            }
            spoke->inner_color[0] = 0;
            spoke->inner_color[1] = 0;
            spoke->inner_color[2] = 0;
            spoke->outer_color[0] = 255;
            spoke->outer_color[1] = 192;
            spoke->outer_color[2] = 64;
            spoke->angle = 1024 - row * 256;
            spoke->count = 0;
        }
        work->spiral_angle = 0;
        work->field_1B6E = 0;
        work->spiral_size = 0;
        work->spiral_shrink = 0;
        work->field_1B88 = 0;
        work->field_1B8C = 4096;
        work->field_1B8E = 1024;
        work->field_1B90 = 0;
        work->field_1B94 = 0;
        work->field_1B98 = 0;
        work->field_1B8A = 0;
        work->clock.frame_count = 0;
        work->clock.frame = 0;
        work->clock.animation_frame = 0;
        work->clock.step = 0;
        work->phase = 0;
        work->field_1BA0 = 0;
        work->clock.fade = 0;
        work->tint = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->aim.view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->aim.view_delta, &squared);
        angle = ratan2(work->aim.view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[0] = angle;
        work->clock.angles[1] = -ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->aim.target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->aim.target);
        }
        if (work->clock.frame < work->config->end) {
            if (work->config->mode == 1) {
                Model_TransformPartVertex(work->slot, work->config->parts[0], work->config->vertex,
                              &work->vertex_position);
                work->matrix.t[0] = work->vertex_position.vx;
                work->matrix.t[1] = work->vertex_position.vy;
                work->matrix.t[2] = work->vertex_position.vz;
            } else {
                GsGetLwUnit(work->parts[0], &work->matrix);
            }
        }
        setVector(&work->aim.direction,
                  work->aim.target.vx - work->matrix.t[0],
                  work->aim.target.vy - work->matrix.t[1],
                  work->aim.target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (long *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->aim.target.vx, work->aim.target.vy, work->aim.target.vz);
        RotTransPers(&target, (long *)&projection.target,
                     &projection.interpolation, &projection.flag);
        work->aim.screen_delta.vx = (u16)projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->aim.screen_delta.vy = dy;
        if (work->config->start <= work->clock.frame) {
            ModelVariant418_DrawSheet((u8 *)point);
            if (work->phase > 0) {
                ModelVariant418_DrawWebs((u8 *)point);
                ModelVariant418_DrawSpiral((u8 *)point);
            }
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
    if (work->phase < 6) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->clock.fade << 5;
    }
    if (work->phase >= 2 && work->phase <= 4) {
        result = 4;
    } else if (work->phase == 5) {
        result = 1;
        work->phase = 6;
    } else if (work->phase == 6) {
        if (work->clock.fade < 64) {
            work->clock.fade += work->clock.step;
            if (work->clock.fade >= 64) {
                work->clock.fade = 64;
                work->phase = 7;
            }
        }
    } else if (work->phase == 7) {
        result = 2;
    }
    return result;
}
