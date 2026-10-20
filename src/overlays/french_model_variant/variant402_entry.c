#include "../../types.h"
#include "variant402_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant402EntryState *work = (Variant402EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant402EntryProjection projection;
    Variant402EntryRecord *record = work->records;
    Variant402EntryRing *ring = work->rings;
    Family402Band *band = work->bands;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *band_packet = &work->band_packet;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j, angle, scale;
    u16 tpage, clut;
    u16 band_texture, flat_texture;
    s32 flat_page, band_page;
    s32 packed;
    GsRVIEW2 *view;
    s16 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dy, dx;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant402EntryConfig *)(D_8013D060 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D060 + 0xC4));
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D060 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D060 + 0x1C));
        band_page = packed >> 16;
        band_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D060 + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013D060 + 0x54));
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
        SetPolyGT4(band_packet);
        band_packet->tpage = band_page;
        setUV4(band_packet, 128, 207, 255, 207, 128, 128, 255, 128);
        band_packet->clut = band_texture;
        SetSemiTrans(band_packet, 1);
        SetShadeTex(band_packet, 0);
        for (i = 0; i < 1; i++, record++) {
            record->field_48 = 0;
            record->field_4C = 0;
            record->field_4A = 0;
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
            ring->field_80 = 0;
            ring->field_84 = 0;
            ring->field_88 = 0;
        }
        for (ring_index = 0, scale = 0; ring_index < 2; ring_index++, band++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                band->inner[j].vx = (u32)rcos(angle) >> 4;
                band->inner[j].vy = (u32)rsin(angle) >> 4;
                band->inner[j].vz = 0;
                setVector(&band->outer[j], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, -192);
            }
            band->scale = scale;
            band->cycles = 0;
            scale -= 2048;
        }
        work->width = 1024;
        work->field_8A6 = 0;
        work->inner.r = 192;
        work->inner.g = 192;
        work->inner.b = 255;
        work->outer.r = 0;
        work->outer.g = 0;
        work->outer.b = 255;
        work->field_89C = 0;
        work->field_89E = 0;
        work->field_8A0 = 0;
        work->field_8A2 = 0;
        work->field_8A8 = 0;
        work->field_8AA = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_8B0 = 0;
        work->fade = 0;
        work->field_8BC = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        work->view_delta.vx = view->vrx - view->vpx;
        work->view_delta.vy = view->vry - view->vpy;
        work->view_delta.vz = view->vrz - view->vpz;
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        /* Capture projected x before the observed part-index reset. */
        dx = (u16)projection.target.vx;
        work->part_index = 0;
        work->screen_delta.vx = dx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->part_count != 0) {
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
                work->part_index++;
            } while (work->part_index < work->config->part_count);
        }
        if (work->config->start <= work->frame) {
            func_8013CB38((u8 *)point);
            if (work->phase == 0) {
                work->phase = 1;
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
    func_800595C8(2, work->field_8BC, work->field_8BC, work->field_8BC);
    if (work->phase < 3) {
        if (work->field_8BC > 0) {
            work->field_8BC -= 128;
            if (work->field_8BC <= 0) {
                work->field_8BC = 0;
            }
        }
    } else {
        work->field_8BC = work->fade << 5;
    }
    if (work->phase == 1) {
        result = 4;
        work->phase = 2;
    } else if (work->phase == 2) {
        result = 1;
    } else if (work->phase == 3) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 8;
            }
        }
    } else if (work->phase == 8) {
        result = 2;
    }
    return result;
}
