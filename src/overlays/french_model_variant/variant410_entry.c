#include "../../types.h"
#include "variant410_entry.h"

/* Header 410's entry: one quad record, nine ribbons and two rings, with the
 * header-337 texture setup and camera tracking. Only the first upload's
 * packed word is kept: its page is shifted out as `flat_page` and its CLUT
 * copied into `flat_texture` before the following uploads reassign `packed`.
 * The ribbon progress rows start from a separate `base` that drops by 512
 * per ribbon, and the ring loop reuses the quad loop's inner counter. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant410EntryState *work = (Variant410EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant410EntryProjection projection;
    Variant410EntryQuads *quads;
    Variant410EntryRibbon *ribbon = work->ribbons;
    Variant410EntryRing *ring = work->rings;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *gradient = &work->gradient;
    POLY_FT4 *quad = &work->quad;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j, angle, base;
    u16 tpage, clut;
    u16 flat_texture;
    s32 flat_page;
    s32 packed;
    GsRVIEW2 *view;
    s16 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant410EntryConfig *)(D_8013CD04 + 0xFC) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CD04 + 0xC4));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CD04 + 0x8C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CD04 + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CD04 + 0x38));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CD04 + 0x54));
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(gradient);
        SetSemiTrans(gradient, 1);
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
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CD04 + 0x54));
        SetPolyFT4(quad);
        quad->tpage = packed >> 16;
        setUV4(quad, 64, 64, 127, 64, 64, 127, 127, 127);
        quad->clut = packed;
        SetSemiTrans(quad, 1);
        SetShadeTex(quad, 0);
        quads = work->quads;
        for (i = 0; i < 1; i++, quads++) {
            for (j = 0; j < 10; j++) {
                setVector(&quads->points[0][j], -256, -256, 0);
                setVector(&quads->points[1][j], 256, -256, 0);
                setVector(&quads->points[2][j], -256, 256, 0);
                setVector(&quads->points[3][j], 256, 256, 0);
                quads->color.r = 255;
                quads->color.g = 64;
                quads->color.b = 0;
                quads->scale[j] = -(j * 4096 / 10);
                quads->angle[j] = 0;
                quads->done[j] = 0;
                quads->field_20C[j] = 0;
                quads->field_234[j] = 0;
                quads->positions[j].vx = work->matrix.t[0] + work->direction.vx;
                quads->positions[j].vy = work->matrix.t[1] + work->direction.vy;
                quads->positions[j].vz = work->matrix.t[2] + work->direction.vz;
            }
        }
        for (i = 0, base = 0; i < 9; i++, ribbon++, base -= 512) {
            for (ring_index = 0; ring_index < 17; ring_index++) {
                ribbon->progress[ring_index] = base - ring_index * 48;
            }
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->inner.r = 255;
            ribbon->inner.g = 0;
            ribbon->inner.b = 0;
            ribbon->outer.r = 128;
            ribbon->outer.g = 0;
            ribbon->outer.b = 0;
            ribbon->field_238 = 0;
            ribbon->field_23C = 0;
            ribbon->field_23A = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = (i + 1) * 1024 / 10 + 1024;
            } else {
                angle = 1024 - i * 1024 / 10;
            }
            ribbon->velocity.vx = (rcos(angle) << 8) >> 12;
            ribbon->velocity.vy = (rsin(angle) << 8) >> 12;
            ribbon->velocity.vz = 0;
        }
        for (j = 0; j < 2; j++, ring++) {
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
            ring->inner.r = 255;
            ring->inner.g = 255;
            ring->inner.b = 255;
            ring->outer.r = 0;
            ring->outer.g = 64;
            ring->outer.b = 255;
            ring->scale = 0;
            ring->field_8C = 0;
            ring->field_90 = 0;
        }
        work->width = 1024;
        work->field_21E4 = 0;
        work->field_21E8 = 0;
        work->field_21EC = 0;
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
        work->view_delta.vx = view->vrx - view->vpx;
        work->view_delta.vy = view->vry - view->vpy;
        work->view_delta.vz = view->vrz - view->vpz;
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->part, &work->matrix);
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
        work->screen_delta.vx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->phase >= 2) {
            func_8013BF58((u8 *)point);
        }
        if (work->phase > 0) {
            func_8013BAAC((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
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
    if (work->config->end <= work->frame && work->phase == 0) {
        work->phase = 1;
    } else if (work->phase >= 1 && work->phase <= 3) {
        result = 4;
    } else if (work->phase == 4) {
        result = 1;
        work->phase = 5;
    } else if (work->phase == 5) {
        work->fade += work->step;
        if (work->fade >= 64) {
            work->fade = 64;
            work->phase = 6;
        }
    } else if (work->phase == 6) {
        result = 2;
    }
    return result;
}
