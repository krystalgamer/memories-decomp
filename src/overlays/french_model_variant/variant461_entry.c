#include "../../types.h"
#include "variant461_entry.h"
#include "../../game/model_graphics_state.h"

#ifndef MODEL_VARIANT461_CLUT_X
#define MODEL_VARIANT461_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState461 *work = (EntryState461 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection461 projection;
    EntryRibbon461 *ribbon = work->ribbons;
    EntryRing461 *ring = &work->ring;
    EntryBand461 *band = work->bands;
    EntryQuad461 *quad_sheet = work->quads;
    EntrySmallRing461 *ring_a = work->rings_a;
    EntrySmallRing461 *ring_b = work->rings_b;
    EntryShape461 *shape = work->shapes;
    ModelVariantWebNarrow *web = work->webs;
    EntryTrail461 *trail = work->trails;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *ring_packet = &work->ring_packet;
    s32 result = 0;
    s32 web_base, n, i;
    s32 packed;
    u16 tpage, clut, flat_clut;
    s32 flat_page;
    s32 j, k, angle, base, delay;
    s32 near_scale, far_scale;
    s32 reset_value;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig461 *)(D_8013D8D4 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT461_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT461_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D8D4 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D8D4 + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D8D4 + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 0, (GsIMAGE *)(D_8013D8D4 + 0x54));
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
        SetPolyFT4(ring_packet);
        ring_packet->tpage = packed >> 16;
        setUV4(ring_packet, 64, 64, 127, 64, 64, 127, 127, 127);
        ring_packet->clut = packed;
        SetSemiTrans(ring_packet, 1);
        SetShadeTex(ring_packet, 0);
        for (n = 0; n < 5; n++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->inner.r = 128;
            ribbon->inner.g = 128;
            ribbon->inner.b = 128;
            ribbon->outer.r = 0;
            ribbon->outer.g = 16;
            ribbon->outer.b = 128;
            ribbon->field_178 = 0;
            ribbon->field_17C = 0;
            ribbon->count = -(n * 16);
            ribbon->field_184 = 0;
            ribbon->state = 0;
            ribbon->length = 1024;
            if (n == 0) {
                angle = 1024;
            } else if (n % 2 == 1) {
                angle = 1024 + (n + 1) * 1024 / 6;
            } else {
                angle = 1024 - n * 1024 / 6;
            }
            setVector(&ribbon->position, rcos(angle) * 256 >> 12,
                rsin(angle) * 256 >> 12, 0);
        }
        for (k = 0; k < 24; k++) {
            setVector(&ring->v0[k], -256, -128, 0);
            setVector(&ring->v1[k], 256, -128, 0);
            setVector(&ring->v2[k], -256, 128, 0);
            setVector(&ring->v3[k], 256, 128, 0);
            ring->color.r = work->config->inner.r;
            ring->color.g = work->config->inner.g;
            ring->color.b = work->config->inner.b;
            ring->field_494[k] = 0;
            ring->field_4F4[k] = 0;
            ring->initialized[k] = 0;
            ring->progress[k] = -(k * 1024 / 24);
            ring->size[k] = -(k * 8192) / 24;
            ring->positions[k].vx = work->matrix.t[0] + work->direction.vx;
            ring->positions[k].vy = work->matrix.t[1] + work->direction.vy;
            ring->positions[k].vz = work->matrix.t[2] + work->direction.vz;
        }
        for (n = 0; n < 1; n++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 2; j++) {
                band->outer[j].r = work->config->outer.r;
                band->outer[j].g = work->config->outer.g;
                band->outer[j].b = work->config->outer.b;
                band->inner[j].r = work->config->inner.r;
                band->inner[j].g = work->config->inner.g;
                band->inner[j].b = work->config->inner.b;
                band->progress[j] = -j * 768;
            }
            band->field_90 = 0;
            band->field_92 = 0;
            band->field_94 = 0;
        }
        for (k = 0; k < 1; k++, shape++) {
            setVector(&shape->origin, 0, 0, 0);
            for (j = 0, angle = 0; j < 5; j++, angle = j * 1024) {
                setVector(&shape->inner[j], (u32)rcos(angle) >> 6,
                    (u32)rsin(angle) >> 8, 0);
                setVector(&shape->outer[j], (u32)(rcos(angle) * 3) >> 10,
                    (u32)(rsin(angle) * 5) >> 9, 0);
            }
            shape->inner_color.r = 255;
            shape->inner_color.g = 192;
            shape->inner_color.b = 192;
            setVector(&shape->corners[0], -128, -128, 0);
            setVector(&shape->corners[1], 128, -128, 0);
            setVector(&shape->corners[2], -128, 128, 0);
            setVector(&shape->corners[3], 128, 128, 0);
            shape->outer_color.r = 64;
            shape->outer_color.g = 64;
            shape->outer_color.b = 0;
            shape->field_80 = 0;
            shape->field_84 = 0;
            shape->field_88 = 0;
        }
        for (k = 0; k < 10; k++, quad_sheet++) {
            for (j = 0; j < 4; j++) {
                if (j == 0) {
                    setVector(&quad_sheet->sheet.v0[j], -128, -128, 0);
                    setVector(&quad_sheet->sheet.v1[j], 0, -128, 0);
                    setVector(&quad_sheet->sheet.v2[j], -128, 0, 0);
                    setVector(&quad_sheet->sheet.v3[j], 0, 0, 0);
                } else if (j == 1) {
                    setVector(&quad_sheet->sheet.v0[j], 128, -128, 0);
                    setVector(&quad_sheet->sheet.v1[j], 128, 0, 0);
                    setVector(&quad_sheet->sheet.v2[j], 0, -128, 0);
                    setVector(&quad_sheet->sheet.v3[j], 0, 0, 0);
                } else if (j == 2) {
                    setVector(&quad_sheet->sheet.v0[j], 128, 128, 0);
                    setVector(&quad_sheet->sheet.v1[j], 0, 128, 0);
                    setVector(&quad_sheet->sheet.v2[j], 128, 0, 0);
                    setVector(&quad_sheet->sheet.v3[j], 0, 0, 0);
                } else if (j == 3) {
                    setVector(&quad_sheet->sheet.v0[j], -128, 128, 0);
                    setVector(&quad_sheet->sheet.v1[j], -128, 0, 0);
                    setVector(&quad_sheet->sheet.v2[j], 0, 128, 0);
                    setVector(&quad_sheet->sheet.v3[j], 0, 0, 0);
                }
            }
            quad_sheet->sheet.outer[0] = 255;
            quad_sheet->sheet.outer[1] = 255;
            quad_sheet->sheet.outer[2] = 255;
            quad_sheet->sheet.inner[0] = 0;
            quad_sheet->sheet.inner[1] = 64;
            quad_sheet->sheet.inner[2] = 255;
            quad_sheet->sheet.size = 0;
            MODEL_VARIANT_WORD(&quad_sheet->sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(&quad_sheet->sheet, 0x90) = 0;
            quad_sheet->field_98 = 0;
        }
        near_scale = 256;
        far_scale = 192;
        for (k = 0, web_base = 0; k < 3; k++, web++, web_base += 300) {
            s32 near_radius, near_y, far_radius, far_y;

            for (i = 0, base = 2048 / 5; i < 4; i++, base = (i + 1) * 2048 / 5) {
                near_radius = rsin(base) * near_scale >> 12;
                near_y = rcos(base) * near_scale >> 12;
                far_radius = rsin(base) * far_scale >> 12;
                far_y = rcos(base) * far_scale >> 12;
                for (j = 0, angle = web_base; j < 6; j++, angle = web_base + j * 4096 / 6) {
                    setVector(&web->near[i][j], rcos(angle) * near_radius >> 12,
                        near_y, rsin(angle) * near_radius >> 12);
                    setVector(&web->far[i][j], rcos(angle) * far_radius >> 12,
                        far_y, rsin(angle) * far_radius >> 12);
                }
            }
            MODEL_VARIANT_WORD(web, 0x184) = 0;
            MODEL_VARIANT_WORD(web, 0x188) = 0;
            MODEL_VARIANT_WORD(web, 0x18C) = 0;
            web->color.r = work->config->web_color.r;
            web->color.g = work->config->web_color.g;
            web->color.b = work->config->web_color.b;
            web->scale = 4096 + k * 4096 / 3;
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (k = 0; k < 16; k++, trail++) {
            for (j = 0, delay = 0; j < 9; delay -= 256, j++) {
                trail->progress[j] = delay;
                trail->colors[j].r = -64 - j * 24;
                trail->colors[j].g = -64 - j * 24;
                trail->colors[j].b = 255;
            }
            trail->field_6C = 0;
            trail->field_70 = 0;
            trail->field_74 = 0;
            trail->field_A0 = 0;
            trail->field_A4 = 0;
        }
        for (i = 0, base = 0; i < 6; i++, ring_a++, base += 256) {
            for (k = 0, j = base; k < 8; k++, j = base + k * 512) {
                setVector(&ring_a->inner[k], (u32)rcos(j) >> 4,
                    (u32)rsin(j) >> 4, 0);
                setVector(&ring_a->outer[k], (u32)(rcos(j) * 5) >> 6,
                    (u32)(rsin(j) * 5) >> 6, 0);
            }
            ring_a->inner_color.r = 64;
            ring_a->inner_color.g = 128;
            ring_a->inner_color.b = 255;
            ring_a->outer_color.r = 0;
            ring_a->outer_color.g = 0;
            ring_a->outer_color.b = 0;
            ring_a->size = i * 1024 / 6;
            ring_a->state = 0;
        }
        for (i = 0, base = 0; i < 4; i++, ring_b++, base += 200) {
            for (k = 0, j = base; k < 8; k++, j = base + k * 512) {
                setVector(&ring_b->inner[k], (u32)(rcos(j) * 3) >> 5,
                    (u32)(rsin(j) * 3) >> 5, 0);
                setVector(&ring_b->outer[k], (u32)rcos(j) >> 3,
                    (u32)rsin(j) >> 3, 0);
            }
            ring_b->inner_color.r = 0;
            ring_b->inner_color.g = 0;
            ring_b->inner_color.b = 0;
            ring_b->outer_color.r = 255;
            ring_b->outer_color.g = 192;
            ring_b->outer_color.b = 64;
            ring_b->size = 1024 - i * 256;
            ring_b->state = 0;
            reset_value = 4096;
        }
        work->intensity = 1024;
        work->angle = 0;
        work->wave = 0;
        work->scale = 1024;
        work->field_3390 = 0;
        work->field_3394 = 0;
        work->field_3398 = 0;
        work->field_33A0 = reset_value;
        work->field_33A2 = 1024;
        work->field_33A4 = 0;
        work->field_33A8 = 0;
        work->field_339A = 0;
        work->field_339C = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->fade = 0;
        work->ambient = 2048;
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
        if (work->elapsed < work->config->matrix_end) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        setVector(&work->origin, work->target.vx, -1150, work->target.vz);
        for (j = 0, angle = 0, base = 0; j < 5; j++, angle += 1700, base = j * 4096 / 5) {
            if (j == 0) {
                setVector(&work->path_targets[j], work->target.vx, 0, work->target.vz);
            } else {
                setVector(&work->path_targets[j], work->target.vx + (rcos(angle) * 256 >> 12),
                    0, work->target.vz + (rsin(angle) * 256 >> 12));
            }
            setVector(&work->path_origins[j], work->origin.vx + (rcos(base) * 256 >> 12),
                work->origin.vy, work->origin.vz + (rsin(base) * 256 >> 12));
            setVector(&work->paths[j], work->path_targets[j].vx - work->path_origins[j].vx,
                work->path_targets[j].vy - work->path_origins[j].vy,
                work->path_targets[j].vz - work->path_origins[j].vz);
        }
        for (j = 0, ring = &work->ring; j < 24; j++) {
            if (ring->progress[j] >= 0 && ring->initialized[j] == 0) {
                if (work->slot == 0) {
                    setVector(&work->ring_positions[j],
                        work->origin.vx + (rsin(work->field_339C) * 768 >> 12),
                        work->origin.vy, work->origin.vz);
                } else {
                    setVector(&work->ring_positions[j],
                        work->origin.vx - (rsin(work->field_339C) * 768 >> 12),
                        work->origin.vy, work->origin.vz);
                }
                ring->initialized[j] = 1;
            }
            setVector(&work->ring_directions[j],
                work->ring_positions[j].vx - work->matrix.t[0],
                work->ring_positions[j].vy - work->matrix.t[1],
                work->ring_positions[j].vz - work->matrix.t[2]);
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
        dx = (u16)projection.target.vx - screen_x;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->phase3_end <= work->elapsed && work->phase == 3) {
            work->phase = 4;
        }
        if (work->config->matrix_end <= work->elapsed && work->phase == 4) {
            work->phase = 5;
        }
        if (work->config->draw_start <= work->elapsed && work->phase < 8) {
            func_8013D498((u8 *)point);
            if (work->phase > 0) {
                func_8013CF54((u8 *)point);
            }
            if (work->phase >= 2) {
                func_8013C508((u8 *)point);
            }
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
    setVector(&work->camera_offset, -512, -384, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->camera_offset, 20);
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 8) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if ((u32)(work->phase - 3) < 4) {
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
