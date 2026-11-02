#include "../../types.h"
#include "variant478_entry.h"

#ifndef MODEL_VARIANT478_CLUT_X
#define MODEL_VARIANT478_CLUT_X 640
#endif

s32 FrenchModelVariant478_HandleRequest(SVECTOR *point, s32 command)
{
    EntryState478 *work = (EntryState478 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection478 projection;
    EntryQuads478 *quad_group;
    EntryPetals478 *petal;
    EntrySwarm478 *swarm = &work->swarm;
    EntryPair478 *pair = work->pairs;
    ModelVariantSheet *sheet = work->sheets;
    EntryRing478 *ring = work->rings;
    EntryRing478 *large_ring = work->large_rings;
    EntrySmall478 *small = work->small;
    ModelVariantWebNarrow *grid = work->grids;
    EntryExtra478 *extra = work->extras;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *swarm_packet = &work->swarm_packet;
    s32 result = 0;
    u16 tpage, clut, flat_clut;
    s32 bearing, row, packed, height, outer_height;
    s32 flat_page;
    s32 i, j, k, angle, radius, outer_radius;
    s32 inner_scale, outer_scale, red, palette, sheet_point, progress;
    s32 reset_value;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig478 *)(gFrenchModelVariant478_D2220 + 0xFC) + command;
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
        clut = GetClut(MODEL_VARIANT478_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT478_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant478_D2220 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant478_D2220 + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant478_D2220 + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant478_D2220 + 0x54));
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
        SetPolyFT4(swarm_packet);
        swarm_packet->tpage = packed >> 16;
        setUV4(swarm_packet, 64, 64, 127, 64, 64, 127, 127, 127);
        swarm_packet->clut = packed;
        SetSemiTrans(swarm_packet, 1);
        SetShadeTex(swarm_packet, 0);
        for (i = 0; i < 32; i++) {
            setVector(&swarm->a[i], -256, -256, 0);
            setVector(&swarm->b[i], 256, -256, 0);
            setVector(&swarm->c[i], -256, 256, 0);
            setVector(&swarm->d[i], 256, 256, 0);
            swarm->color[0] = work->config->outer.r;
            swarm->color[1] = work->config->outer.g;
            swarm->color[2] = work->config->outer.b;
            swarm->size[i] = -i * 32;
            swarm->done[i] = 0;
            swarm->reset[i] = 0;
            swarm->positions[i].vx = work->matrix.t[0] + work->direction.vx;
            swarm->positions[i].vy = work->matrix.t[1] + work->direction.vy;
            swarm->positions[i].vz = work->matrix.t[2] + work->direction.vz;
        }
        quad_group = work->quads;
        for (j = 0; j < 1; j++, quad_group++) {
            for (i = 0; i < 10; i++) {
                setVector(&quad_group->a[i], -384, -384, 0);
                setVector(&quad_group->b[i], 384, -384, 0);
                setVector(&quad_group->c[i], -384, 384, 0);
                setVector(&quad_group->d[i], 384, 384, 0);
                quad_group->color[0] = work->config->outer.r;
                quad_group->color[1] = work->config->outer.g;
                quad_group->color[2] = work->config->outer.b;
                quad_group->size[i] = -(i * 4096 / 10);
                quad_group->angle[i] = 0;
                quad_group->done[i] = 0;
                quad_group->field_20C[i] = 0;
                quad_group->reset[i] = 0;
                quad_group->positions[i].vx = work->matrix.t[0] + work->direction.vx;
                quad_group->positions[i].vy = work->matrix.t[1] + work->direction.vy;
                quad_group->positions[i].vz = work->matrix.t[2] + work->direction.vz;
            }
        }
        petal = work->petals;
        for (j = 0; j < 3; j++, petal++) {
            for (i = 0; i < 8; i++) {
                setVector(&petal->v0[i], -128, -128, 0);
                setVector(&petal->v1[i], 128, -128, 0);
                setVector(&petal->v2[i], -128, 128, 0);
                setVector(&petal->v3[i], 128, 128, 0);
                petal->color[0] = work->config->outer.r;
                petal->color[1] = work->config->outer.g;
                petal->color[2] = work->config->outer.b;
                petal->size[i] = -i * 512 - j * 256;
                petal->field_164[i] = 0;
                petal->hidden[i] = 0;
                petal->field_1A4[i] = 0;
                petal->field_1C4[i] = 0;
                petal->positions[i].vx = work->matrix.t[0] + work->direction.vx;
                petal->positions[i].vy = work->matrix.t[1] + work->direction.vy;
                petal->positions[i].vz = work->matrix.t[2] + work->direction.vz;
            }
        }
        for (j = 0; j < 1; j++, pair++) {
            setVector(&pair->scale, 4096, 4096, 4096);
            for (k = 0; k < 2; k++) {
                pair->inner[k].r = work->config->inner.r;
                pair->inner[k].g = work->config->inner.g;
                pair->inner[k].b = work->config->inner.b;
                pair->outer[k].r = work->config->outer.r;
                pair->outer[k].g = work->config->outer.g;
                pair->outer[k].b = work->config->outer.b;
                pair->progress[k] = -k * 768;
            }
            pair->field_90 = 0;
            pair->field_92 = 0;
            pair->field_94 = 0;
        }
        for (i = 0; i < 1; i++, small++) {
            setVector(&small->center, 0, 0, 0);
            for (k = 0, angle = 0; k < 5; k++, angle = k * 1024) {
                setVector(&small->first[k], (u32)rcos(angle) >> 6,
                    (u32)rsin(angle) >> 8, 0);
                setVector(&small->second[k], (u32)(rcos(angle) * 3) >> 10,
                    (u32)(rsin(angle) * 5) >> 9, 0);
            }
            setVector(&small->corners[0], -128, -128, 0);
            setVector(&small->corners[1], 128, -128, 0);
            setVector(&small->corners[2], -128, 128, 0);
            setVector(&small->corners[3], 128, 128, 0);
            small->inner.r = 255;
            small->inner.g = 192;
            small->inner.b = 192;
            small->outer.r = 64;
            small->outer.g = 64;
            small->outer.b = 0;
            small->field_80 = 0;
            small->field_84 = 0;
            small->field_88 = 0;
        }
        for (i = 0; i < 2; i++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->v0[sheet_point], -64, -64, 0);
                    setVector(&sheet->v1[sheet_point], 0, -64, 0);
                    setVector(&sheet->v2[sheet_point], -64, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->v0[sheet_point], 64, -64, 0);
                    setVector(&sheet->v1[sheet_point], 64, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, -64, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->v0[sheet_point], 64, 64, 0);
                    setVector(&sheet->v1[sheet_point], 0, 64, 0);
                    setVector(&sheet->v2[sheet_point], 64, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->v0[sheet_point], -64, 64, 0);
                    setVector(&sheet->v1[sheet_point], -64, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, 64, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
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
        inner_scale = 256;
        outer_scale = 192;
        for (i = 0, bearing = 0; i < 3; i++, grid++, bearing += 300) {
            for (row = 0, k = 2048 / 5; row < 4; row++, k = (row + 1) * 2048 / 5) {
                radius = rsin(k) * inner_scale >> 12;
                height = rcos(k) * inner_scale >> 12;
                outer_radius = rsin(k) * outer_scale >> 12;
                outer_height = rcos(k) * outer_scale >> 12;
                for (k = 0, angle = bearing; k < 6; k++, angle = bearing + k * 4096 / 6) {
                    setVector(&grid->near[row][k], rcos(angle) * radius >> 12,
                        height, rsin(angle) * radius >> 12);
                    setVector(&grid->far[row][k], rcos(angle) * outer_radius >> 12,
                        outer_height, rsin(angle) * outer_radius >> 12);
                }
            }
            MODEL_VARIANT_WORD(grid, 0x184) = 0;
            MODEL_VARIANT_WORD(grid, 0x188) = 0;
            MODEL_VARIANT_WORD(grid, 0x18C) = 0;
            grid->color.r = work->config->grid.r;
            grid->color.g = work->config->grid.g;
            grid->color.b = work->config->grid.b;
            grid->scale = 4096 + i * 4096 / 3;
            grid->done = 0;
            MODEL_VARIANT_WORD(grid, 0x19C) = 0;
        }
        for (i = 0, palette = -64; i < 16; i++, extra++) {
            for (k = 0, progress = 0, red = palette; k < 9; k++) {
                extra->progress[k] = progress;
                progress -= 256;
                extra->color[k][0] = red;
                extra->color[k][1] = red;
                red -= 24;
                extra->color[k][2] = 255;
            }
            extra->field_6C = 0;
            extra->field_70 = 0;
            extra->field_74 = 0;
            extra->field_A0 = 0;
            extra->field_A4 = 0;
        }
        for (row = 0, k = 0; row < 6; row++, k += 256, ring++) {
            for (i = 0, angle = k; i < 8; angle += 512, i++) {
                setVector(&ring->inner[i], (u32)rcos(angle) >> 4,
                    (u32)rsin(angle) >> 4, 0);
                setVector(&ring->outer[i], (u32)(rcos(angle) * 5) >> 6,
                    (u32)(rsin(angle) * 5) >> 6, 0);
            }
            ring->inner_color.r = 64;
            ring->inner_color.g = 128;
            ring->inner_color.b = 255;
            ring->outer_color.r = 0;
            ring->outer_color.g = 0;
            ring->outer_color.b = 0;
            ring->size = row * 1024 / 6;
            ring->field_8C = 0;
        }
        for (row = 0, k = 0; row < 4; row++, large_ring++, k += 200) {
            for (i = 0, angle = k; i < 8; angle += 512, i++) {
                setVector(&large_ring->inner[i], (u32)(rcos(angle) * 3) >> 5,
                    (u32)(rsin(angle) * 3) >> 5, 0);
                setVector(&large_ring->outer[i], (u32)rcos(angle) >> 3,
                    (u32)rsin(angle) >> 3, 0);
            }
            large_ring->inner_color.r = 0;
            large_ring->inner_color.g = 0;
            large_ring->inner_color.b = 0;
            large_ring->outer_color.r = 255;
            large_ring->outer_color.g = 192;
            large_ring->outer_color.b = 64;
            large_ring->size = 1024 - row * 256;
            large_ring->field_8C = 0;
            reset_value = 4096;
        }
        work->field_2A10 = 0;
        work->field_2A14 = reset_value;
        work->field_2A16 = 1024;
        work->field_2A18 = 0;
        work->wave = 0;
        work->field_2A20 = 0;
        work->field_2A12 = 0;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
            work->matrix.t[2] += 64;
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
            work->matrix.t[2] -= 64;
        }
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
        if (work->config->start <= work->elapsed) {
            SpanishModelVariant478_DrawPetalRings((u8 *)point);
        }
        if (work->config->swarm_start <= work->elapsed) {
            FrenchModelVariant478_DrawSwarm((u8 *)point);
        }
        if (work->phase >= 2) {
            SpanishModelVariant478_DrawOrbitingQuads((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
    setVector(&work->camera_offset, -512, -256, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->camera_offset, 20);
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
