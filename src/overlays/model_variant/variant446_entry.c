#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "variant446_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT446_CLUT_X
#define MODEL_VARIANT446_CLUT_X 512
#endif

/* Header 446's entry: the header-412 entry shape with five ribbons, five
 * trails, two sets of ten sheets and twenty-four sprites, five tracked parts
 * and four per-frame helpers.  Matching quirks: the unused MATRIX locals keep
 * retail's frame; the web radii are block locals so their spill slots follow
 * the ribbon divisor; the update loops count with k, not j; the ribbon parity
 * test is n % 2 == 1, which keeps the ribbon progress unreduced; and the
 * spoke angles are recomputed from the base while the ring angles step. */

s32 ModelVariant446_HandleRequest(SVECTOR *point, s32 command)
{
    Variant446EntryState *work = (Variant446EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    ModelVariantSheetSet *sheet;
    Variant446EntryRibbon *trail;
    ModelVariantSheetSet *panel;
    Variant446EntrySprites *sprites;
    Variant446EntryBand *band;
    ModelVariantRing *ring;
    ModelVariantSpokeRing *spoke;
    Variant446EntryQuad *quad_record;
    ModelVariantWebNarrow *web;
    Variant446EntryRecord *record;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_FT4 *flat;
    s32 result;
    s32 web_angle;
    s32 n;
    s32 row;
    s32 near_scale, far_scale;
    Variant446EntryRibbon *ribbon;
    POLY_GT4 *textured;
    s32 i, j, k, angle;
    s32 packed;
    u16 tpage, clut;
    s32 flat_page;
    u16 flat_clut;
    s32 base_angle, ring_angle;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    s32 dy;
    u32 frame;

    sheet = work->sheets;
    panel = work->panels;
    band = work->bands;
    ring = work->rings;
    spoke = work->spokes;
    quad_record = work->quads;
    web = work->webs;
    record = work->records;
    textured = work->textured;
    flat = work->flat_textured;
    ribbon = work->ribbons;
    trail = work->trails;
    sprites = &work->sprites;
    triangle = &work->triangle;
    quad = &work->quad;
    result = 0;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant446EntryConfig *)(gModelVariant446_D3BF4 + 0xFC) + command;
        for (j = 0; j < 5; j++) {
            work->parts[j] = Model_GetSlotDataEntry(work->slot, work->config->parts[j]);
        }
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
        clut = GetClut(MODEL_VARIANT446_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT446_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant446_D3BF4 + 0xC4));
        /* Reassigning packed keeps the page shift next to the first upload. */
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant446_D3BF4 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant446_D3BF4 + 0x1C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant446_D3BF4 + 0x54));
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
        for (n = 0; n < 5; n++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->ca[0] = 128;
            ribbon->ca[1] = 128;
            ribbon->ca[2] = 128;
            ribbon->cb[0] = 0;
            ribbon->cb[1] = 16;
            ribbon->cb[2] = 128;
            ribbon->field_1B8 = 0;
            ribbon->field_1BC = 0;
            ribbon->progress = -(n * 16);
            ribbon->field_1C4 = 0;
            ribbon->field_1C8 = 0;
            ribbon->field_1CC = 1024;
            if (n == 0) {
                angle = 1024;
            } else if (n % 2 == 1) {
                angle = (n + 1) * 1024 / 6 + 1024;
            } else {
                angle = 1024 - (n << 10) / 6;
            }
            ribbon->offset.vx = (rcos(angle) * 256) >> 12;
            ribbon->offset.vy = (rsin(angle) * 256) >> 12;
            ribbon->offset.vz = 0;
        }
        work->trail_delay = 128;
        for (n = 0; n < 5; n++, trail++) {
            setVector(&trail->scale, 4096, 4096, 4096);
            trail->ca[0] = 128;
            trail->ca[1] = 128;
            trail->ca[2] = 128;
            trail->cb[0] = 0;
            trail->cb[1] = 16;
            trail->cb[2] = 128;
            trail->field_1B8 = 0;
            trail->field_1BC = 0;
            trail->progress = -work->trail_delay - n * 16;
            trail->field_1C4 = 0;
            trail->field_1C8 = 0;
            trail->field_1CC = 1024;
            if (n == 0) {
                angle = 1024;
            } else if (n % 2 == 1) {
                angle = (n + 1) * 1024 / 6 + 1024;
            } else {
                angle = 1024 - (n << 10) / 6;
            }
            trail->offset.vx = (rcos(angle) * 256) >> 12;
            trail->offset.vy = (rsin(angle) * 256) >> 12;
            trail->offset.vz = 0;
        }
        for (i = 0; i < 24; i++) {
            setVector(&sprites->v0[i], -256, -128, 0);
            setVector(&sprites->v1[i], 256, -128, 0);
            setVector(&sprites->v2[i], -256, 128, 0);
            setVector(&sprites->v3[i], 256, 128, 0);
            sprites->color.r = work->config->outer.r;
            sprites->color.g = work->config->outer.g;
            sprites->color.b = work->config->outer.b;
            sprites->scale[i] = -(i * 1024 / 24);
            sprites->delay[i] = i * -8192 / 24;
            sprites->field_494[i] = 0;
            sprites->field_4F4[i] = 0;
            sprites->done[i] = 0;
            sprites->origin[i].vx = work->matrix.t[0] + work->direction.vx;
            sprites->origin[i].vy = work->matrix.t[1] + work->direction.vy;
            sprites->origin[i].vz = work->matrix.t[2] + work->direction.vz;
        }
        for (n = 0; n < 1; n++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 2; j++) {
                band->ca[j].r = work->config->inner.r;
                band->ca[j].g = work->config->inner.g;
                band->ca[j].b = work->config->inner.b;
                band->cb[j].r = work->config->outer.r;
                band->cb[j].g = work->config->outer.g;
                band->cb[j].b = work->config->outer.b;
                band->offset[j] = -(j * 768);
            }
            band->field_90 = 0;
            band->field_92 = 0;
            band->field_94 = 0;
        }
        for (i = 0; i < 1; i++, quad_record++) {
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
        for (i = 0; i < 10; i++, sheet++) {
            for (j = 0; j < 4; j++) {
                if (j == 0) {
                    setVector(&sheet->v0[j], -128, -128, 0);
                    setVector(&sheet->v1[j], 0, -128, 0);
                    setVector(&sheet->v2[j], -128, 0, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                } else if (j == 1) {
                    setVector(&sheet->v0[j], 128, -128, 0);
                    setVector(&sheet->v1[j], 128, 0, 0);
                    setVector(&sheet->v2[j], 0, -128, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                } else if (j == 2) {
                    setVector(&sheet->v0[j], 128, 128, 0);
                    setVector(&sheet->v1[j], 0, 128, 0);
                    setVector(&sheet->v2[j], 128, 0, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                } else if (j == 3) {
                    setVector(&sheet->v0[j], -128, 128, 0);
                    setVector(&sheet->v1[j], -128, 0, 0);
                    setVector(&sheet->v2[j], 0, 128, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
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
        for (i = 0; i < 10; i++, panel++) {
            for (j = 0; j < 4; j++) {
                if (j == 0) {
                    setVector(&panel->v0[j], -128, -128, 0);
                    setVector(&panel->v1[j], 0, -128, 0);
                    setVector(&panel->v2[j], -128, 0, 0);
                    setVector(&panel->v3[j], 0, 0, 0);
                } else if (j == 1) {
                    setVector(&panel->v0[j], 128, -128, 0);
                    setVector(&panel->v1[j], 128, 0, 0);
                    setVector(&panel->v2[j], 0, -128, 0);
                    setVector(&panel->v3[j], 0, 0, 0);
                } else if (j == 2) {
                    setVector(&panel->v0[j], 128, 128, 0);
                    setVector(&panel->v1[j], 0, 128, 0);
                    setVector(&panel->v2[j], 128, 0, 0);
                    setVector(&panel->v3[j], 0, 0, 0);
                } else if (j == 3) {
                    setVector(&panel->v0[j], -128, 128, 0);
                    setVector(&panel->v1[j], -128, 0, 0);
                    setVector(&panel->v2[j], 0, 128, 0);
                    setVector(&panel->v3[j], 0, 0, 0);
                }
            }
            panel->outer[0] = work->config->outer.r;
            panel->outer[1] = work->config->outer.g;
            panel->outer[2] = work->config->outer.b;
            panel->inner[0] = work->config->inner.r;
            panel->inner[1] = work->config->inner.g;
            panel->inner[2] = work->config->inner.b;
            panel->size = 0;
            MODEL_VARIANT_WORD(panel, 0x8C) = 0;
            MODEL_VARIANT_WORD(panel, 0x90) = 0;
            panel->shown = 0;
        }
        near_scale = 256;
        far_scale = 192;
        for (i = 0, web_angle = 0; i < 3; i++, web++, web_angle += 300) {
            s32 near_radius, near_y, far_radius, far_y;

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
            web->scale = 4096 + i * 4096 / 3;
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (i = 0; i < 16; i++, record++) {
            for (j = 0; j < 9; j++) {
                record->color[j].r = 192 - j * 24;
                record->color[j].g = 192 - j * 24;
                record->color[j].b = 255;
                record->offset[j] = -(j * 256);
            }
            record->field_6C = 0;
            record->field_70 = 0;
            record->field_74 = 0;
            record->field_A0 = 0;
            record->field_A4 = 0;
        }
        for (row = 0, base_angle = 0; row < 6; row++, ring++, base_angle += 256) {
            for (i = 0, ring_angle = base_angle; i < 8; ring_angle += 512, i++) {
                setVector(&ring->inner[i], (u32)rcos(ring_angle) >> 4, (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring->outer[i], (u32)(rcos(ring_angle) * 5) >> 6,
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
            for (i = 0, ring_angle = base_angle; i < 8; i++, ring_angle = base_angle + i * 512) {
                setVector(&spoke->inner[i], (u32)(rcos(ring_angle) * 3) >> 5,
                          (u32)(rsin(ring_angle) * 3) >> 5, 0);
                setVector(&spoke->outer[i], (u32)rcos(ring_angle) >> 3, (u32)rsin(ring_angle) >> 3, 0);
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
        work->field_4A90 = 1024;
        work->field_4A80 = 0;
        work->field_4A88 = 0;
        work->field_4A84 = 1024;
        work->field_4A94 = 0;
        work->field_4A98 = 0;
        work->field_4A9C = 0;
        work->field_4AA4 = 4096;
        work->field_4AA6 = 1024;
        work->field_4AA8 = 0;
        work->field_4AAC = 0;
        work->field_4A9E = 0;
        work->sweep = 0;
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
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        if (work->frame < work->config->part_frame) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        setVector(&work->position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        for (k = 0, angle = 0, base_angle = 0; k < 5; k++, angle += 1700,
             base_angle = (k << 12) / 5) {
            if (k == 0) {
                setVector(&work->tips[k], work->position.vx, work->position.vy - 640,
                          work->position.vz);
            } else {
                setVector(&work->tips[k], work->position.vx + ((rcos(angle) * 256) >> 12),
                          work->position.vy - 640,
                          work->position.vz + ((rsin(angle) * 256) >> 12));
            }
            GsGetLwUnit(work->parts[k], &work->part_matrices[k]);
            if (work->config->use_part_points == 0) {
                setVector(&work->bases[k], work->position.vx + ((rcos(base_angle) * 256) >> 12),
                          work->position.vy,
                          work->position.vz + ((rsin(base_angle) * 256) >> 12));
            } else {
                setVector(&work->bases[k], work->part_matrices[k].t[0],
                          work->part_matrices[k].t[1], work->part_matrices[k].t[2]);
            }
            setVector(&work->spans[k], work->tips[k].vx - work->bases[k].vx,
                      work->tips[k].vy - work->bases[k].vy,
                      work->tips[k].vz - work->bases[k].vz);
        }
        for (k = 0, angle = 0, base_angle = 0; k < 5; k++, angle += 1700,
             base_angle = (k << 12) / 5) {
            if (k == 0) {
                setVector(&work->targets[k], work->target.vx, 0, work->target.vz);
            } else {
                setVector(&work->targets[k], work->target.vx + ((rcos(angle) * 256) >> 12), 0,
                          work->target.vz + ((rsin(angle) * 256) >> 12));
            }
            setVector(&work->ground[k], work->target.vx + ((rcos(base_angle) * 256) >> 12), -1024,
                      work->target.vz + ((rsin(base_angle) * 256) >> 12));
            setVector(&work->target_spans[k], work->targets[k].vx - work->ground[k].vx,
                      work->targets[k].vy - work->ground[k].vy,
                      work->targets[k].vz - work->ground[k].vz);
        }
        sprites = &work->sprites;
        for (i = 0; i < 24; i++) {
            if (sprites->scale[i] >= 0 && sprites->done[i] == 0) {
                if (work->slot == 0) {
                    setVector(&work->positions[i], rsin(work->sweep) * 768 >> 12, -300, -450);
                } else {
                    setVector(&work->positions[i], -(rsin(work->sweep) * 768 >> 12), -300, 450);
                }
                sprites->done[i] = 1;
            }
            setVector(&work->offsets[i], work->positions[i].vx - work->matrix.t[0],
                      work->positions[i].vy - work->matrix.t[1],
                      work->positions[i].vz - work->matrix.t[2]);
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        work->screen_delta.vx = (u16)projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->phase3_frame <= work->frame && work->phase == 2) {
            work->phase = 3;
        }
        if (work->config->phase5_frame <= work->frame && work->phase == 4) {
            work->phase = 5;
        }
        if (work->config->start <= work->frame && work->phase < 8) {
            if (work->phase < 6) {
                FrenchModelVariant463_DrawPairedSheets((u8 *)point);
            }
            FrenchModelVariant463_DrawSheets((u8 *)point);
            if (work->phase > 0) {
                FrenchModelVariant463_DrawRibbons((u8 *)point);
                FrenchModelVariant463_DrawTrailRibbons((u8 *)point);
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
    setVector(&work->offset, -512, -256, 0);
    func_8005F5C8(work->slot, 3, &work->offset, 20);
    func_800595C8(2, work->tint, work->tint, work->tint);
    if (work->phase < 8) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->phase >= 2 && work->phase <= 6) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 8;
    } else if (work->phase == 8) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 9;
            }
        }
    } else if (work->phase == 9) {
        result = 2;
    }
    return result;
}
