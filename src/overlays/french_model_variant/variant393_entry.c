#include "../../types.h"
#include "variant393_entry.h"

#ifndef MODEL_VARIANT393_CLUT_X
#define MODEL_VARIANT393_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant393EntryState *work = (Variant393EntryState *)point;
    /* Retail projection inputs follow 64 bytes of unrecovered locals. */
    u8 unknown_stack[64];
    SVECTOR position, target;
    VECTOR squared;
    Variant393EntryProjection projection;
    Variant337EntryRecord *record = work->records;
    Variant337EntryRing *ring = work->rings;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_FT4 *extra_flat = work->extra_flat;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j, angle;
    u16 tpage, clut, flat_texture;
    s32 packed;
    s32 flat_page, extra_page;
    GsRVIEW2 *view;
    Variant393EntryConfig *config;
    s32 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant393EntryConfig *)(D_8013D3B0 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        if (work->config->mode == 1) {
            func_800593D0(work->slot, work->config->parts[0], work->config->vertices[0],
                          &work->positions[0]);
            func_800593D0(work->slot, work->config->parts[1], work->config->vertices[1],
                          &work->positions[1]);
            func_800593D0(work->slot, work->config->parts[2], work->config->vertices[2],
                          &work->positions[2]);
        } else {
            GsGetLwUnit(work->parts[0], &work->matrix);
            if (work->slot == 0) {
                work->matrix.t[0] += work->config->offset[0];
                work->matrix.t[1] += work->config->offset[1];
                work->matrix.t[2] -= work->config->offset[2];
            } else {
                work->matrix.t[0] += work->config->offset[0];
                work->matrix.t[1] += work->config->offset[1];
                work->matrix.t[2] += work->config->offset[2];
            }
        }
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT393_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT393_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D3B0 + 0xC4));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D3B0 + 0x8C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D3B0 + 0x1C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D3B0 + 0x38));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D3B0 + 0x54));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D3B0 + 0xE0));
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
        SetPolyFT4(extra_flat);
        extra_page = packed >> 16;
        extra_flat->tpage = extra_page;
        setUV4(extra_flat, 0, 224, 31, 224, 0, 255, 31, 255);
        extra_flat->clut = packed;
        SetSemiTrans(extra_flat, 1);
        SetShadeTex(extra_flat, 1);
        extra_flat++;
        SetPolyFT4(extra_flat);
        extra_flat->tpage = extra_page;
        setUV4(extra_flat, 32, 224, 63, 224, 32, 255, 63, 255);
        extra_flat->clut = packed;
        SetSemiTrans(extra_flat, 1);
        SetShadeTex(extra_flat, 1);
        for (i = 0; i < 1; i++, record++) {
            setVector(&record->scale, 4096, 4096, 4096);
            record->inner.r = work->config->ribbon.r;
            record->inner.g = work->config->ribbon.g;
            record->inner.b = work->config->ribbon.b;
            record->outer.r = 0;
            record->outer.g = 16;
            record->outer.b = 128;
            record->field_238 = 0;
            record->field_23C = 0;
            record->field_23A = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = 1024 + (i + 1) * 512;
            } else {
                angle = 1024 - i * 512;
            }
            record->velocity.vx = (rcos(angle) << 8) >> 12;
            record->velocity.vy = (rsin(angle) << 8) >> 12;
            record->velocity.vz = 0;
        }
        for (ring_index = 0; ring_index < 2; ring_index++, ring++) {
            for (ring_point = 0; ring_point < 4; ring_point++) {
                if (ring_point == 0) {
                    setVector(&ring->points[ring_point], -128, -128, 0);
                    setVector(&ring->points[ring_point + 4], 0, -128, 0);
                    setVector(&ring->points[ring_point + 8], -128, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 1) {
                    setVector(&ring->points[ring_point], 128, -128, 0);
                    setVector(&ring->points[ring_point + 4], 128, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, -128, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 2) {
                    setVector(&ring->points[ring_point], 128, 128, 0);
                    setVector(&ring->points[ring_point + 4], 0, 128, 0);
                    setVector(&ring->points[ring_point + 8], 128, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 3) {
                    setVector(&ring->points[ring_point], -128, 128, 0);
                    setVector(&ring->points[ring_point + 4], -128, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, 128, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                }
            }
            ring->inner.r = work->config->inner.r;
            ring->inner.g = work->config->inner.g;
            ring->inner.b = work->config->inner.b;
            ring->outer.r = work->config->outer.r;
            ring->outer.g = work->config->outer.g;
            ring->outer.b = work->config->outer.b;
            ring->scale = 0;
            ring->field_8C = 0;
            ring->field_90 = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->width = 1024;
        work->field_DD4 = 1024;
        work->field_DD8 = 1024;
        work->field_DCC = 0;
        work->field_DCE = 0;
        work->field_DD0 = 0;
        work->field_DE8 = 0;
        work->field_DEC = 0;
        work->field_DF0 = 0;
        work->field_DF4 = 0;
        work->field_DDC = 0;
        work->field_DE0 = 0;
        work->field_DE4 = 0;
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
        setVector(&work->view_delta, view->vrx - view->vpx, view->vry - view->vpy,
                  view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
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
        config = work->config;
        work->part_index = 0;
        dx = (u16)projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        if (config->count != 0) {
            do {
                if (config->mode == 0) {
                    if (work->part_index == 0) {
                        GsGetLwUnit(work->parts[0], &work->matrix);
                    } else if (work->part_index == 1) {
                        GsGetLwUnit(work->parts[1], &work->matrix);
                    } else {
                        GsGetLwUnit(work->parts[2], &work->matrix);
                    }
                    if (work->slot == 0) {
                        work->matrix.t[0] -= work->config->offset[0];
                        work->matrix.t[1] += work->config->offset[1];
                        work->matrix.t[2] -= work->config->offset[2];
                    } else {
                        work->matrix.t[0] += work->config->offset[0];
                        work->matrix.t[1] += work->config->offset[1];
                        work->matrix.t[2] += work->config->offset[2];
                    }
                } else {
                    if (work->part_index == 0) {
                        func_800593D0(work->slot, config->parts[0], config->vertices[0],
                                      &work->positions[0]);
                        work->matrix.t[0] = work->positions[0].vx;
                        work->matrix.t[1] = work->positions[0].vy;
                        work->matrix.t[2] = work->positions[0].vz;
                    } else if (work->part_index == 1) {
                        func_800593D0(work->slot, config->parts[1], config->vertices[1],
                                      &work->positions[1]);
                        work->matrix.t[0] = work->positions[1].vx;
                        work->matrix.t[1] = work->positions[1].vy;
                        work->matrix.t[2] = work->positions[1].vz;
                    } else {
                        func_800593D0(work->slot, config->parts[2], config->vertices[2],
                                      &work->positions[2]);
                        work->matrix.t[0] = work->positions[2].vx;
                        work->matrix.t[1] = work->positions[2].vy;
                        work->matrix.t[2] = work->positions[2].vz;
                    }
                }
                work->direction.vx = work->target.vx - work->matrix.t[0];
                work->direction.vy = work->target.vy - work->matrix.t[1];
                work->direction.vz = work->target.vz - work->matrix.t[2];
                if (work->config->start <= work->frame) {
                    func_8013BDDC((u8 *)point);
                    func_8013C704((u8 *)point);
                }
                work->part_index++;
                config = work->config;
            } while (work->part_index < config->count);
        }
        if (work->phase >= 3) {
            func_8013CB8C((u8 *)point);
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
