#include "../../types.h"
#include "variant373_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState373 *work = (EntryState373 *)point;
    EntryTextureStack373 texture_stack;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection373 projection;
    Family373Grid *grid = &work->grid;
    Family373Layer *layer = work->layers;
    EntryPointGroup373 *group = work->groups;
    EntryStrip373 *strip = work->strips;
    Family373QuadGroup *quadrant = work->quadrants;
    EntryRing373 *ring = work->rings;
    Family373Ray *ray = work->rays;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *shared = work->shared;
    POLY_GT4 *layer_packet = work->layer_packets;
    POLY_GT4 *grid_packet = work->packets.grid;
    POLY_FT4 *flat = work->packets.flat.pair;
    POLY_FT4 *tile = work->packets.flat.tiles;
    POLY_FT4 *point_packet = &work->point_packet;
    s32 result = 0;
    u16 tpage0, tpage1, tpage2;
    s32 i, j, k, n, q, offset, packed, latitude, angle, radius, height, red, base;
    s32 group_scale;
    u32 screen_x, frame;
    s32 screen_y;
    GsRVIEW2 *view;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->frame_count = 0;
        work->config = (EntryConfig373 *)(D_8013EEF0 + 0x70) + command;
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
        packed = ModelTexture_PackPageClutCopy(work->slot, 0, (GsIMAGE *)D_8013EEF0);
        tpage0 = packed >> 16;
        texture_stack.clut0 = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013EEF0 + 0x1C));
        tpage1 = packed >> 16;
        texture_stack.clut1 = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013EEF0 + 0x38));
        tpage2 = packed >> 16;
        texture_stack.clut2 = packed;
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(shared);
        shared->tpage = tpage0;
        setUV4(shared, 32, 0, 33, 0, 32, 31, 33, 31);
        shared->clut = texture_stack.clut0;
        SetSemiTrans(shared, 1);
        SetShadeTex(shared, 0);
        shared++;
        SetPolyGT4(shared);
        shared->tpage = tpage0;
        setUV4(shared, 0, 0, 31, 0, 0, 31, 31, 31);
        shared->clut = texture_stack.clut0;
        SetSemiTrans(shared, 1);
        SetShadeTex(shared, 0);
        SetPolyFT4(flat);
        flat->tpage = tpage1;
        setUV4(flat, 31, 32, 31, 63, 0, 32, 0, 63);
        flat->clut = texture_stack.clut1;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = tpage1;
        setUV4(flat, 31, 64, 31, 95, 0, 64, 0, 95);
        flat->clut = texture_stack.clut1;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        for (j = 0, offset = 0; j < 16; j++, layer_packet++, offset = -(j * 256) / 16) {
            SetPolyGT4(layer_packet);
            layer_packet->tpage = tpage1;
            setUV4(layer_packet, 0, offset - 1, 0, offset - 16,
                31, offset - 1, 31, offset - 16);
            layer_packet->clut = texture_stack.clut1;
            SetSemiTrans(layer_packet, 1);
            SetShadeTex(layer_packet, 0);
        }
        for (i = 0, offset = 0; i < 8; i++, offset = -(i * 128) / 8, tile++, grid_packet++) {
            SetPolyFT4(tile);
            tile->tpage = tpage0;
            setUV4(tile, 32, offset + 127, 95, offset + 127,
                32, offset + 111, 95, offset + 111);
            tile->clut = texture_stack.clut0;
            SetSemiTrans(tile, 1);
            SetShadeTex(tile, 0);
            SetPolyGT4(grid_packet);
            grid_packet->tpage = tpage0;
            setUV4(grid_packet, 32, offset + 127, 95, offset + 127,
                32, offset + 111, 95, offset + 111);
            grid_packet->clut = texture_stack.clut0;
            SetSemiTrans(grid_packet, 1);
            SetShadeTex(grid_packet, 0);
        }
        SetPolyFT4(point_packet);
        point_packet->tpage = tpage2;
        setUV4(point_packet, 96, 0, 159, 0, 96, 63, 159, 63);
        point_packet->clut = texture_stack.clut2;
        SetSemiTrans(point_packet, 1);
        SetShadeTex(point_packet, 0);
        for (i = 0, latitude = 0; i < 9; i++) {
            radius = rsin(latitude) * 128 >> 12;
            height = rcos(latitude) * 128 >> 12;
            for (j = 0, k = 0; j < 17; j++, k = j * 256) {
                setVector(&grid->points[i][j], rcos(k) * radius >> 12,
                    height, rsin(k) * radius >> 12);
            }
            grid->colors[i].r = (latitude - i) / 8;
            grid->colors[i].g = 64;
            grid->colors[i].b = ~((latitude - i) / 8);
            latitude += 256;
        }
        for (q = 0; q < 1; q++, strip++) {
            setVector(&strip->scale, 4096, 4096, 4096);
            strip->color.r = 0;
            strip->color.g = 128;
            strip->color.b = 255;
            strip->field_5C = 0;
            strip->field_60 = 0;
            strip->field_5E = 0;
        }
        for (q = 0; q < 16; q++, ray++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    ray->outer[j].r = 0;
                    ray->outer[j].g = 0;
                    ray->outer[j].b = 0;
                    ray->inner[j].r = 0;
                    ray->inner[j].g = 0;
                    ray->inner[j].b = 0;
                } else {
                    ray->outer[j].r = 0;
                    ray->outer[j].g = 128;
                    ray->outer[j].b = 255;
                    ray->inner[j].r = 96;
                    ray->inner[j].g = 112;
                    ray->inner[j].b = 128;
                }
            }
        }
        for (n = 0; n < 1; n++, quadrant++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&quadrant->points[0][k], -192, -192, 0);
                    setVector(&quadrant->points[1][k], 0, -192, 0);
                    setVector(&quadrant->points[2][k], -192, 0, 0);
                    setVector(&quadrant->points[3][k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&quadrant->points[0][k], 192, -192, 0);
                    setVector(&quadrant->points[1][k], 192, 0, 0);
                    setVector(&quadrant->points[2][k], 0, -192, 0);
                    setVector(&quadrant->points[3][k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&quadrant->points[0][k], 192, 192, 0);
                    setVector(&quadrant->points[1][k], 0, 192, 0);
                    setVector(&quadrant->points[2][k], 192, 0, 0);
                    setVector(&quadrant->points[3][k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&quadrant->points[0][k], -192, 192, 0);
                    setVector(&quadrant->points[1][k], -192, 0, 0);
                    setVector(&quadrant->points[2][k], 0, 192, 0);
                    setVector(&quadrant->points[3][k], 0, 0, 0);
                }
            }
            quadrant->color[0].r = 255;
            quadrant->color[0].g = 255;
            quadrant->color[0].b = 255;
            quadrant->color[1].r = 0;
            quadrant->color[1].g = 128;
            quadrant->color[1].b = 255;
        }
        for (n = 0; n < 5; n++, layer++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&layer->points[j], (u32)rcos(angle) >> 5, 0,
                    (u32)rsin(angle) >> 5);
                setVector(&layer->points[j + 17], (u32)(rcos(angle) * 3) >> 6,
                    16, (u32)(rsin(angle) * 3) >> 6);
                setVector(&layer->points[j + 34], (u32)rcos(angle) >> 4,
                    16, (u32)rsin(angle) >> 4);
            }
            setVector(&layer->rotation, n * 2048 / 5, 0, n * 1024);
            layer->scale = -n * 6144 / 5;
            layer->field_1A4 = -n * 1024 / 5;
            layer->done = 0;
        }
        for (n = 0; n < 3; n++, group++) {
            for (j = 0, angle = 0; j < 16; j++, angle = j * 256) {
                setVector(&group->points[j], (u32)rcos(angle) >> 3, 0,
                    (u32)rsin(angle) >> 3);
            }
            for (k = 0, angle = 0; k < 4; k++, angle = k * 1024) {
                setVector(&group->corners[k], (u32)rcos(angle - 1536) >> 5,
                    (u32)rsin(angle - 1536) >> 5, 0);
            }
            setVector(&group->rotation, n * 2048 / 3, 0, n * 5120 / 3);
            group_scale = -n * 6144;
            group->scale = group_scale / 3;
        }
        for (i = 0, base = 0; i < 4; i++, ring++, base += 256) {
            for (n = 0, angle = base; n < 8; angle += 512, n++) {
                setVector(&ring->inner[n], (u32)rcos(angle) >> 4,
                    (u32)rsin(angle) >> 4, 0);
                setVector(&ring->outer[n], (u32)(rcos(angle) * 5) >> 6,
                    (u32)(rsin(angle) * 5) >> 6, 0);
            }
            ring->inner_color.r = 64;
            ring->inner_color.g = 128;
            ring->inner_color.b = 255;
            ring->outer_color.r = 0;
            ring->outer_color.g = 0;
            ring->outer_color.b = 0;
            ring->size = i * 256;
            ring->field_8C = 0;
        }
        work->grid_scale = 0;
        work->translation = 0;
        work->rotation = 0;
        work->oscillation = 0;
        work->grid_brightness = 1024;
        for (n = 4; n >= 0; n--) {
            work->fields_244C[n] = 0;
        }
        work->brightness = 255;
        work->width = 1024;
        work->scale = 0;
        work->field_2460 = 0;
        work->field_2462 = 0;
        work->field_2464 = 0;
        work->field_2468 = 0;
        work->field_2466 = 0;
        work->field_246A = 0;
        work->factor = 0;
        work->angle = 0;
        work->field_247C = 0;
        work->texture_offset = 0;
        work->field_246C = 0;
        work->field_246E = 0;
        work->field_2470 = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->fade = 0;
        work->phase = 0;
        work->field_2488 = 0;
        work->field_248C = 0;
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
        if (work->phase < 4) {
            GsGetLwUnit(work->parts[0], &work->matrix);
            setVector(&work->direction, work->target.vx - work->matrix.t[0],
                work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        }
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
        if (work->config->layers_start <= work->elapsed && work->phase < 8) {
            func_8013C0B0((u8 *)point);
        }
        if (work->config->grid_start <= work->elapsed && work->phase < 8) {
            if (work->phase >= 6) {
                packed = ModelTexture_PackPageClutCopy(work->slot, 2, (GsIMAGE *)D_8013EEF0);
                tpage0 = packed >> 16;
                texture_stack.clut0 = packed;
                for (i = 0, grid_packet = work->packets.grid; i < 8; i++, grid_packet++) {
                    SetPolyGT4(grid_packet);
                    grid_packet->tpage = tpage0;
                    grid_packet->clut = texture_stack.clut0;
                    SetSemiTrans(grid_packet, 1);
                    SetShadeTex(grid_packet, 0);
                }
            }
            func_8013C76C((u8 *)point);
        }
        if (work->config->points_start <= work->elapsed) {
            func_8013CEE4((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
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
    if (work->phase >= 3 && work->phase <= 6) {
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
