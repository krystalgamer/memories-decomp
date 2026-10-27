#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant402_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT402_CLUT_X
#define MODEL_VARIANT402_CLUT_X 512
#endif

/* Header 402's entry: the header-398 skeleton with a quad group and swarm
 * launched toward the other slot, sixteen streaks, and two per-frame
 * helpers. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant402EntryState *work = (Variant402EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant402EntrySwarm *swarm = &work->swarm;
    Variant402EntryBand *band = work->bands;
    ModelVariantSheet *sheet = work->sheets;
    ModelVariantRing *ring = work->rings;
    ModelVariantSpokeRing *spoke = work->spokes;
    Variant439EntryFan *fan = work->fans;
    ModelVariantWebNarrow *web = work->webs;
    Variant402EntryStreak *streak = work->streaks;
    POLY_GT4 *textured = work->textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_FT4 *flat = work->flat_textured;
    POLY_FT4 *glow = &work->glow;
    Variant402EntryQuads *quads;
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
        work->config = (Variant402EntryConfig *)D_8013CD44 + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        outer = 0;
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT402_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT402_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)D_8013CD0C);
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CD0C - 0x38));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CD0C - 0xA8));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CD0C - 0x70));
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
        SetPolyFT4(glow);
        glow->tpage = packed >> 16;
        setUV4(glow, 64, 64, 127, 64, 64, 127, 127, 127);
        glow->clut = packed;
        SetSemiTrans(glow, 1);
        SetShadeTex(glow, 0);
        for (; outer < 32; outer++) {
            setVector(&swarm->a[outer], -256, -256, 0);
            setVector(&swarm->b[outer], 256, -256, 0);
            setVector(&swarm->c[outer], -256, 256, 0);
            setVector(&swarm->d[outer], 256, 256, 0);
            swarm->color[0] = work->config->outer.r;
            swarm->color[1] = work->config->outer.g;
            swarm->color[2] = work->config->outer.b;
            swarm->size[outer] = -(outer * 32);
            swarm->done[outer] = 0;
            swarm->reset[outer] = 0;
            swarm->position[outer].vx = work->matrix.t[0] + work->direction.vx;
            swarm->position[outer].vy = work->matrix.t[1] + work->direction.vy;
            swarm->position[outer].vz = work->matrix.t[2] + work->direction.vz;
        }
        for (quads = work->quads, i = 0; i < 1; i++, quads++) {
            for (outer = 0; outer < 10; outer++) {
                setVector(&quads->a[outer], -256, -256, 0);
                setVector(&quads->b[outer], 256, -256, 0);
                setVector(&quads->c[outer], -256, 256, 0);
                setVector(&quads->d[outer], 256, 256, 0);
                quads->color[0] = work->config->outer.r;
                quads->color[1] = work->config->outer.g;
                quads->color[2] = work->config->outer.b;
                quads->angle[outer] = 0;
                quads->done[outer] = 0;
                MODEL_VARIANT_WORD(quads, 0x20C + outer * 4) = 0;
                quads->reset[outer] = 0;
                quads->size[outer] = -((outer << 12) / 10);
                quads->position[outer].vx = work->matrix.t[0] + work->direction.vx;
                quads->position[outer].vy = work->matrix.t[1] + work->direction.vy;
                quads->position[outer].vz = work->matrix.t[2] + work->direction.vz;
            }
        }
        for (i = 0; i < 1; i++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 2; j++) {
                band->inner[j].r = work->config->inner.r;
                band->inner[j].g = work->config->inner.g;
                band->inner[j].b = work->config->inner.b;
                band->outer[j].r = work->config->outer.r;
                band->outer[j].g = work->config->outer.g;
                band->outer[j].b = work->config->outer.b;
                band->depth[j] = -(j * 0x300);
            }
            band->field_90 = 0;
            band->field_92 = 0;
            band->field_94 = 0;
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
        for (outer = 0; outer < 2; outer++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -64, -64, 0);
                    setVector(&sheet->v1[k], 0, -64, 0);
                    setVector(&sheet->v2[k], -64, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 64, -64, 0);
                    setVector(&sheet->v1[k], 64, 0, 0);
                    setVector(&sheet->v2[k], 0, -64, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 64, 64, 0);
                    setVector(&sheet->v1[k], 0, 64, 0);
                    setVector(&sheet->v2[k], 64, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -64, 64, 0);
                    setVector(&sheet->v1[k], -64, 0, 0);
                    setVector(&sheet->v2[k], 0, 64, 0);
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
            for (row = 0, j = 2048 / 5; row < 4; row++, j = (row + 1) * 2048 / 5) {
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
        for (outer = 0; outer < 16; outer++, streak++) {
            for (j = 0; j < 9; j++) {
                streak->color[j].r = -64 - j * 24;
                streak->color[j].g = -64 - j * 24;
                streak->color[j].b = 255;
                streak->depth[j] = -(j * 256);
            }
            streak->field_6C = 0;
            streak->field_70 = 0;
            streak->field_74 = 0;
            streak->field_A0 = 0;
            streak->field_A4 = 0;
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
        work->field_22E4 = 0;
        work->field_22E8 = 4096;
        work->field_22EA = 1024;
        work->field_22EC = 0;
        work->field_22F0 = 0;
        work->field_22E6 = 0;
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
        if (work->config->follow_limit >= work->frame) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
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
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->start <= work->frame) {
            func_8013C1E0((u8 *)point);
        }
        if (work->phase >= 2) {
            func_8013C778((u8 *)point);
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
