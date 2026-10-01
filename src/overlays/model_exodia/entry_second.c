#include "../../types.h"
#include "entry_second.h"

s32 func_8017B004(SVECTOR *point, s32 command)
{
    ExodiaSecondEntryState *work = (ExodiaSecondEntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ExodiaSecondEntryProjection projection;
    ExodiaSecondEntryBeam *beam = work->beams;
    ExodiaSecondEntryRing *ring = work->rings;
    ExodiaSecondEntryPetal *colors = work->petals;
    POLY_GT4 *textured = work->textured;
    POLY_F4 *flat = &work->flat;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 ring_index, ring_point;
    s32 i, j;
    u16 tpage, clut;
    GsRVIEW2 *view;
    s16 screen_x;
    s32 screen_y;
    u32 frame;
    GsOT *ot;
    s16 dx;
    s32 dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->frame_count = 0;
        work->config = &D_8017CD3C[command];
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        func_80059B90(480, (s16 *)&work->origin);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 416, 256);
        clut = GetClut(640, 225);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        SetPolyF4(flat);
        SetSemiTrans(flat, 1);
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = tpage;
        textured->u0 = 127;
        textured->v0 = 0;
        textured->u1 = 127;
        textured->v1 = 0;
        textured->u2 = 127;
        textured->v2 = 63;
        textured->u3 = 127;
        textured->v3 = 63;
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        textured++;
        SetPolyGT4(textured);
        textured->tpage = tpage;
        textured->u0 = 64;
        textured->v0 = 0;
        textured->u1 = 127;
        textured->v1 = 0;
        textured->u2 = 64;
        textured->v2 = 63;
        textured->u3 = 127;
        textured->v3 = 63;
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        for (i = 0; i < 1; i++, beam++) {
            beam->field_268 = 4096;
            beam->field_26C = 4096;
            beam->field_270 = 4096;
            beam->color.r = 255;
            beam->color.g = 0;
            beam->color.b = 255;
            beam->field_278 = 0;
            beam->field_27C = 0;
            beam->field_27A = 0;
        }
        for (i = 0; i < 12; i++, colors++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    colors->outer[j].r = 0;
                    colors->outer[j].g = 0;
                    colors->outer[j].b = 0;
                    colors->inner[j].r = 0;
                    colors->inner[j].g = 0;
                    colors->inner[j].b = 0;
                } else {
                    colors->outer[j].r = 255;
                    colors->outer[j].g = 224;
                    colors->outer[j].b = 0;
                    colors->inner[j].r = 160;
                    colors->inner[j].g = 160;
                    colors->inner[j].b = 128;
                }
            }
        }
        for (ring_index = 0; ring_index < 2; ring_index++, ring++) {
            for (ring_point = 0; ring_point < 4; ring_point++) {
                if (ring_point == 0) {
                    setVector(&ring->points[ring_point], -1024, -1024, 0);
                    setVector(&ring->points[ring_point + 4], 0, -1024, 0);
                    setVector(&ring->points[ring_point + 8], -1024, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 1) {
                    setVector(&ring->points[ring_point], 1024, -1024, 0);
                    setVector(&ring->points[ring_point + 4], 1024, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, -1024, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 2) {
                    setVector(&ring->points[ring_point], 1024, 1024, 0);
                    setVector(&ring->points[ring_point + 4], 0, 1024, 0);
                    setVector(&ring->points[ring_point + 8], 1024, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 3) {
                    setVector(&ring->points[ring_point], -1024, 1024, 0);
                    setVector(&ring->points[ring_point + 4], -1024, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, 1024, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                }
            }
            ring->inner.r = 192;
            ring->inner.g = 192;
            ring->inner.b = 160;
            ring->outer.r = 255;
            ring->outer.g = 224;
            ring->outer.b = 64;
            ring->scale = 0;
            ring->field_8C = 0;
            ring->field_90 = 0;
        }
        work->width = 1024;
        work->field_C8C = 0;
        work->field_C8E = 0;
        work->field_C90 = 0;
        work->field_C92 = 0;
        work->distance = 0;
        work->field_C9C = 0;
        work->field_CA0 = 0;
        work->field_C94 = 0;
        work->field_C96 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_CA8 = 0;
        work->fade = 0;
        work->field_CB4 = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        work->view_delta.vx = view->vrx - view->vpx;
        work->view_delta.vy = view->vry - view->vpy;
        work->view_delta.vz = view->vrz - view->vpz;
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     (PSXLONG *)&projection.interpolation, (PSXLONG *)&projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     (PSXLONG *)&projection.interpolation, (PSXLONG *)&projection.flag);
        setVector(&work->target, 0, -4000, -6000);
        dx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        if (work->frame < work->config->pulse3) {
            func_80059B90(480, (s16 *)&work->origin);
        } else {
            func_80059B90(0, (s16 *)&work->origin);
        }
        work->direction.vx = work->target.vx - work->origin.vx;
        work->direction.vy = work->target.vy - work->origin.vy;
        work->direction.vz = work->target.vz - work->origin.vz;
        if (work->config->start <= work->frame) {
            func_8017B9B0((u8 *)point);
        }
        if (work->config->petals_start <= work->frame) {
            func_8017BDF0((u8 *)point);
        }
        if (work->config->pulse0 <= work->frame && work->frame < work->config->pulse1) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            func_8017C760((u8 *)point);
        } else if (work->config->pulse1 <= work->frame && work->frame < work->config->pulse2) {
            if (work->phase == 3) {
                work->distance = 0;
                work->phase = 1;
            }
        } else if (work->config->pulse2 <= work->frame && work->frame < work->config->pulse3) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            func_8017C760((u8 *)point);
        } else if (work->config->pulse3 <= work->frame && work->frame < work->config->pulse4) {
            if (work->phase == 3) {
                work->distance = 0;
                work->phase = 1;
            }
        } else if (work->config->pulse4 <= work->frame && work->frame < work->config->pulse5) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            func_8017C760((u8 *)point);
        } else if (work->config->pulse5 <= work->frame && work->frame < work->config->pulse6) {
            if (work->phase == 3) {
                work->distance = 0;
                work->phase = 1;
            }
        } else if (work->config->pulse6 <= work->frame) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            func_8017C760((u8 *)point);
        }
        if (work->config->fade_start <= work->frame) {
            ot = func_80058F10();
            flat->r0 = work->fade;
            flat->g0 = work->fade;
            flat->b0 = work->fade;
            setXY4(flat, 0, 0, 320, 0, 0, 256, 320, 256);
            func_8005B260((u32 *)flat, ot, 0, 1);
            if (work->frame <= work->config->fade_end) {
                work->fade = (work->frame - work->config->fade_start) * 255 /
                             (work->config->fade_end - work->config->fade_start);
                if (work->fade >= 255) {
                    work->fade = 255;
                }
            }
        }
        if (work->config->end <= work->frame) {
            result = 2;
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    return result;
}
