#include "../../types.h"
#include "entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    ExodiaEntryState *work = (ExodiaEntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ExodiaEntryProjection projection;
    ExodiaEntryRing *ring = work->rings;
    ExodiaColorRecord *colors = work->colors;
    POLY_GT4 *textured = work->textured;
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
    s16 dx;
    s32 dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = &D_8013C7AC[command];
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->delta.vx = work->target.vx - work->matrix.t[0];
        work->delta.vy = work->target.vy - work->matrix.t[1];
        work->delta.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 416, 256);
        clut = GetClut(640, 225);
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
        for (ring_index = 0; ring_index < 1; ring_index++, ring++) {
            for (ring_point = 0; ring_point < 4; ring_point++) {
                if (ring_point == 0) {
                    setVector(&ring->points[ring_point], -1536, -1536, 0);
                    setVector(&ring->points[ring_point + 4], 0, -1536, 0);
                    setVector(&ring->points[ring_point + 8], -1536, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 1) {
                    setVector(&ring->points[ring_point], 1536, -1536, 0);
                    setVector(&ring->points[ring_point + 4], 1536, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, -1536, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 2) {
                    setVector(&ring->points[ring_point], 1536, 1536, 0);
                    setVector(&ring->points[ring_point + 4], 0, 1536, 0);
                    setVector(&ring->points[ring_point + 8], 1536, 0, 0);
                    setVector(&ring->points[ring_point + 12], 0, 0, 0);
                } else if (ring_point == 3) {
                    setVector(&ring->points[ring_point], -1536, 1536, 0);
                    setVector(&ring->points[ring_point + 4], -1536, 0, 0);
                    setVector(&ring->points[ring_point + 8], 0, 1536, 0);
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
        for (i = 0; i < 16; i++, colors++) {
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
                    colors->outer[j].g = 0;
                    colors->outer[j].b = 255;
                    colors->inner[j].r = 128;
                    colors->inner[j].g = 128;
                    colors->inner[j].b = 128;
                }
            }
        }
        work->field_4B0 = 1024;
        work->field_4CC = 1024;
        work->field_4A0 = 0;
        work->field_4A4 = 0;
        work->field_4A8 = 0;
        work->field_4AC = 0;
        work->field_4B4 = 0;
        work->field_4B8 = 0;
        work->field_4BC = 0;
        work->field_4C0 = 0;
        work->field_4C4 = 0;
        work->field_4C8 = 0;
        work->field_4D0 = 0;
        work->field_4D4 = 0;
        work->progress = 0;
        work->field_488 = 0;
        work->field_48C = 0;
        work->field_490 = 1024;
        work->field_494 = 0;
        work->field_478 = 0;
        work->field_47C = 4096;
        work->field_47E = 1024;
        work->field_480 = 0;
        work->field_498 = 0;
        work->field_49C = 0;
        work->field_47A = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->grown = 0;
        work->field_4E0 = 0;
        work->field_460 = 0;
        work->field_4EC = 2048;
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
        work->matrix.t[1] += 256;
        work->delta.vx = work->target.vx - work->matrix.t[0];
        work->delta.vy = work->target.vy - work->matrix.t[1];
        work->delta.vz = work->target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     (PSXLONG *)&projection.interpolation, (PSXLONG *)&projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     (PSXLONG *)&projection.interpolation, (PSXLONG *)&projection.flag);
        dx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        work->position.vx = work->matrix.t[0] + work->delta.vx * work->progress / 1536;
        work->position.vy = work->matrix.t[1] + work->delta.vy * work->progress / 1536 -
                            (rsin(work->progress) * 256 >> 12);
        work->position.vz = work->matrix.t[2] + work->delta.vz * work->progress / 2048;
        work->remaining.vx = work->target.vx - work->position.vx;
        work->remaining.vy = work->target.vy - work->position.vy;
        work->remaining.vz = work->target.vz - work->position.vz;
        if (work->config->start <= work->frame && work->frame < work->config->end) {
            func_8013B860((u8 *)point);
        }
        if (work->config->ramp_end <= work->frame && work->frame < work->config->end) {
            func_8013BC0C((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    if (work->config->shrink_end < work->frame) {
        result = 2;
    }
    return result;
}
