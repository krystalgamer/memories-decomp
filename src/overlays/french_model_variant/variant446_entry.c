#include "../../types.h"
#include "variant446_entry.h"

#ifndef MODEL_VARIANT446_CLUT_X
#define MODEL_VARIANT446_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState446 *work = (EntryState446 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant402EntryProjection projection;
    EntryRay446 *ray = work->rays;
    EntryLines446 *lines = work->lines;
    EntryStrip446 *strip = work->strips;
    EntryPulse446 *pulse = work->pulses;
    EntryRing446 *ring = work->rings;
    POLY_GT4 *textured = work->textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *ring_packet = &work->ring_packet;
    POLY_FT4 *flat = work->flat;
    s32 result = 0;
    u16 tpage, clut, flat_page, ring_page, ring_clut;
    s32 packed, flat_clut;
    s32 i, j;
    s32 azimuth, row, k, latitude, longitude;
    s32 pulse_point;
    s32 outer_radius, outer_y, inner_radius, inner_y;
    s32 outer_size, inner_size;
    s32 extent;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig446 *)(D_8013E6D4 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        GsGetLwUnit(work->parts[1], &work->second_matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT446_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT446_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E6D4 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E6D4 + 0x8C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E6D4 + 0x1C));
        ring_page = packed >> 16;
        ring_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E6D4 + 0x38));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E6D4 + 0x54));
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
        SetPolyGT4(extra);
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(ring_packet);
        ring_packet->tpage = ring_page;
        setUV4(ring_packet, 128, 207, 255, 207, 128, 128, 255, 128);
        ring_packet->clut = ring_clut;
        SetSemiTrans(ring_packet, 1);
        SetShadeTex(ring_packet, 0);
        for (i = 0; i < 16; i++, ray++) {
            for (j = 0; j < 3; j++) {
                if (j == 2) {
                    ray->outer[j].r = 0;
                    ray->outer[j].g = 0;
                    ray->outer[j].b = 0;
                    ray->inner[j].r = 0;
                    ray->inner[j].g = 0;
                    ray->inner[j].b = 0;
                } else {
                    ray->outer[j].r = 255;
                    ray->outer[j].g = 0;
                    ray->outer[j].b = 255;
                    ray->inner[j].r = 128;
                    ray->inner[j].g = 128;
                    ray->inner[j].b = 128;
                }
            }
        }
        outer_size = 256;
        inner_size = 192;
        for (row = 0, azimuth = 0; row < 3; row++, lines++, azimuth += 300) {
            for (k = 0, latitude = 2048 / 5; k < 4;
                 k++, latitude = (k + 1) * 2048 / 5) {
                outer_radius = rsin(latitude) * outer_size >> 12;
                outer_y = rcos(latitude) * outer_size >> 12;
                inner_radius = rsin(latitude) * inner_size >> 12;
                inner_y = rcos(latitude) * inner_size >> 12;
                for (j = 0, longitude = azimuth; j < 6;
                     j++, longitude = azimuth + j * 4096 / 6) {
                    setVector(&lines->points[0][k][j],
                        rcos(longitude) * outer_radius >> 12,
                        outer_y, rsin(longitude) * outer_radius >> 12);
                    setVector(&lines->points[1][k][j],
                        rcos(longitude) * inner_radius >> 12,
                        inner_y, rsin(longitude) * inner_radius >> 12);
                }
            }
            lines->direction.vx = 0;
            lines->direction.vy = 0;
            lines->direction.vz = 0;
            lines->color.r = 128;
            lines->color.g = 128;
            lines->color.b = 128;
            lines->size = 4096 + row * 4096 / 3;
            lines->finished = 0;
            lines->field_19C = 0;
        }
        for (i = 0; i < 1; i++, strip++) {
            strip->field_B4 = 0;
            strip->field_B8 = 0;
            strip->field_B6 = 0;
        }
        for (row = 0; row < 2; row++, pulse++) {
            for (pulse_point = 0; pulse_point < 4; pulse_point++) {
                extent = work->config->extent;
                if (pulse_point == 0) {
                    setVector(&pulse->points[0][pulse_point], -extent, -extent, 0);
                    setVector(&pulse->points[1][pulse_point], 0, -extent, 0);
                    setVector(&pulse->points[2][pulse_point], -extent, 0, 0);
                    setVector(&pulse->points[3][pulse_point], 0, 0, 0);
                } else if (pulse_point == 1) {
                    setVector(&pulse->points[0][pulse_point], extent, -extent, 0);
                    setVector(&pulse->points[1][pulse_point], extent, 0, 0);
                    setVector(&pulse->points[2][pulse_point], 0, -extent, 0);
                    setVector(&pulse->points[3][pulse_point], 0, 0, 0);
                } else if (pulse_point == 2) {
                    setVector(&pulse->points[0][pulse_point], extent, extent, 0);
                    setVector(&pulse->points[1][pulse_point], 0, extent, 0);
                    setVector(&pulse->points[2][pulse_point], extent, 0, 0);
                    setVector(&pulse->points[3][pulse_point], 0, 0, 0);
                } else if (pulse_point == 3) {
                    setVector(&pulse->points[0][pulse_point], -extent, extent, 0);
                    setVector(&pulse->points[1][pulse_point], -extent, 0, 0);
                    setVector(&pulse->points[2][pulse_point], 0, extent, 0);
                    setVector(&pulse->points[3][pulse_point], 0, 0, 0);
                }
            }
            setVector(&pulse->scale, 0, 0, 0);
        }
        for (row = 0; row < 4; row++, ring++) {
            for (j = 0, longitude = 0; j < 17; j++, longitude = j * 256) {
                setVector(&ring->inner[j], rcos(longitude) * work->config->inner_radius >> 12,
                    rsin(longitude) * work->config->inner_radius >> 12, 0);
                setVector(&ring->outer[j], rcos(longitude) * work->config->outer_radius >> 12,
                    rsin(longitude) * work->config->outer_radius >> 12, -64);
            }
            if (row == 3) {
                ring->progress = 0;
            } else {
                ring->progress = -(row * 4096 / 3);
            }
            ring->state = 0;
        }
        work->width = 1024;
        work->radius = 0;
        work->inner.r = 192;
        work->inner.g = 192;
        work->inner.b = 255;
        work->outer.r = 0;
        work->outer.g = 0;
        work->outer.b = 255;
        work->ray_length = 1024;
        work->ray_size = 0;
        work->spin = 0;
        work->field_1BC6 = 0;
        work->field_1BC8 = 0;
        work->field_1BCA = 0;
        work->progress = 0;
        work->field_1BD2 = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->field_1BE0 = 0;
        work->fade = 0;
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
        GsGetLwUnit(work->parts[1], &work->second_matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
            &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
            &projection.interpolation, &projection.flag);
        dx = (u16)projection.target.vx;
        work->part_index = 0;
        work->screen_delta.vx = dx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->count != 0) {
            do {
                if (work->part_index == 0) {
                    GsGetLwUnit(work->parts[0], &work->matrix);
                } else if (work->part_index == 1) {
                    GsGetLwUnit(work->parts[1], &work->matrix);
                } else if (work->part_index == 2) {
                    GsGetLwUnit(work->parts[2], &work->matrix);
                }
                work->direction.vx = work->target.vx - work->matrix.t[0];
                work->direction.vy = work->target.vy - work->matrix.t[1];
                work->direction.vz = work->target.vz - work->matrix.t[2];
                if (work->config->draw_start <= work->elapsed) {
                    func_8013CFE8((u8 *)point);
                }
                if (work->config->rays_start <= work->elapsed) {
                    func_8013DCAC((u8 *)point);
                }
                if (work->phase > 0) {
                    func_8013C95C((u8 *)point);
                    func_8013C2F8((u8 *)point);
                }
                work->part_index++;
            } while (work->part_index < work->config->count);
        }
        if (work->config->draw_start <= work->elapsed) {
            func_8013D4BC((u8 *)point);
            if (work->phase <= 0) {
                func_8013BE00((u8 *)point);
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
    if (work->phase >= 2 && work->phase <= 5) {
        result = 4;
    } else if (work->phase == 6) {
        result = 1;
        work->phase = 7;
    } else if (work->phase == 7) {
        if (work->fade < 64) {
            work->fade += work->step >> 1;
            if (work->fade >= 64) {
                work->fade = 0;
                work->phase = 8;
            }
        }
    } else if (work->phase == 8) {
        result = 2;
    }
    return result;
}
