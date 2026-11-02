#include "../../types.h"
#include "variant408_entry.h"

/* The CLUT column the entry uploads its palettes to: 640 in the European
 * build, 512 in the North American one (variant391_entry.c). */
#ifndef MODEL_VARIANT408_CLUT_X
#define MODEL_VARIANT408_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    ModelVariant408EntryState *work = (ModelVariant408EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position;
    SVECTOR target;
    VECTOR squared;
    ModelVariant408Projection projection;
    ModelVariant408Mesh *mesh = work->meshes;
    ModelVariant408Ring *ring = work->rings;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *other = &work->other;
    POLY_GT4 *opaque = &work->opaque;
    POLY_FT4 *sprite = work->sprites;
    s32 result = 0;
    s32 phase;
    s32 mesh_index;
    s32 i;
    s32 radius;
    s32 angle;
    s32 longitude;
    s32 j;
    s32 k;
    s32 x;
    s32 y;
    u16 tpage;
    u16 clut;
    s32 texture;
    s32 texture_page;
    s32 packed_texture;
    GsRVIEW2 *view;
    SVECTOR *other_point;
    s16 screen_x;
    s32 screen_y;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->frame = 0;
        work->config = &D_8013C550[command];
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            work->target.vx = 0;
            work->target.vy = -300;
            work->target.vz = -450;
        } else {
            work->target.vx = 0;
            work->target.vy = -300;
            work->target.vz = 450;
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->delta.vx = work->target.vx - work->matrix.t[0];
        work->delta.vy = work->target.vy - work->matrix.t[1];
        work->delta.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT408_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT408_CLUT_X, 244);
        packed_texture = func_80059A50(work->slot, 1, &D_8013C470[6]);
        texture = packed_texture;
        texture_page = packed_texture >> 16;
        packed_texture = func_80059A50(work->slot, 1, &D_8013C470[4]);
        packed_texture = func_80059A50(work->slot, 1, &D_8013C470[0]);
        packed_texture = func_80059A50(work->slot, 1, &D_8013C470[7]);
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = tpage;
        textured->u0 = 126;
        textured->v0 = 0;
        textured->u1 = 127;
        textured->v1 = 0;
        textured->u2 = 126;
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
        SetPolyFT4(sprite);
        sprite->tpage = texture_page;
        sprite->u0 = 0;
        sprite->v0 = 128;
        sprite->u1 = 0;
        sprite->v1 = 175;
        sprite->u2 = 48;
        sprite->v2 = 128;
        sprite->u3 = 48;
        sprite->v3 = 175;
        sprite->clut = texture;
        SetSemiTrans(sprite, 1);
        SetShadeTex(sprite, 1);
        sprite++;
        SetPolyFT4(sprite);
        sprite->tpage = texture_page;
        sprite->u0 = 0;
        sprite->v0 = 176;
        sprite->u1 = 0;
        sprite->v1 = 223;
        sprite->u2 = 48;
        sprite->v2 = 176;
        sprite->u3 = 48;
        sprite->v3 = 223;
        sprite->clut = texture;
        SetSemiTrans(sprite, 1);
        SetShadeTex(sprite, 1);
        SetPolyGT4(other);
        SetSemiTrans(other, 1);
        SetShadeTex(other, 0);
        SetPolyGT4(opaque);
        SetSemiTrans(opaque, 0);
        SetShadeTex(opaque, 0);
        for (mesh_index = 0; mesh_index < 1; mesh_index++, mesh++) {
            mesh->scale.vx = 4096;
            mesh->scale.vy = 4096;
            mesh->scale.vz = 4096;
            for (j = 0; j < 17; j++) {
                mesh->phases[j] = -12000 + j * 750;
                mesh->repeats[j] = 0;
            }
            for (k = 0; k < 9; k++) {
                s32 r, g, b;
                if (k % 2 == 0) {
                    r = 128;
                    g = 0;
                    b = 0;
                } else {
                    r = 128;
                    g = 64;
                    b = 0;
                }
                mesh->colors[k].r = r;
                mesh->colors[k].g = g;
                mesh->colors[k].b = b;
            }
        }
        for (i = 0, phase = 0; i < 4; i++, ring++, phase += 300) {
            radius = 256 - i * 32;
            for (j = 0, angle = 2048 / 5; j < 4; j++, angle = (j + 1) * 2048 / 5) {
                x = (rsin(angle) * radius) >> 12;
                y = (rcos(angle) * radius) >> 12;
                for (k = 0, longitude = phase;
                     k < 6; k++, longitude = phase + k * 4096 / 6) {
                    point = (SVECTOR *)(k * sizeof(*point)
                        + (j * sizeof(ring->points[0][0]) + (u32)ring));
                    point->vx = (rcos(longitude) * x) >> 12;
                    point->vy = y;
                    point->vz = (rsin(longitude) * x) >> 12;
                    point[24].vx = (rcos(longitude) * x) >> 12;
                    other_point = (SVECTOR *)((u32)point + 192);
                    other_point->vy = y - 32;
                    other_point->vz = (rsin(longitude) * x) >> 12;
                }
            }
            ring->position.vx = 0;
            ring->position.vy = 0;
            ring->position.vz = 0;
            ring->color.r = 255;
            ring->color.g = 0;
            ring->color.b = 0;
            ring->phase = -(i << 8) / 4;
            ring->repeats = 0;
        }
        work->extension = 0;
        work->angle = 0;
        work->repeat_limit = 1024;
        work->field_DF8 = 0;
        work->field_DFC = 0;
        work->field_DF4 = 0;
        work->field_DF6 = 0;
        work->frame = 0;
        work->elapsed = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->state = 0;
        work->fade = 0;
        work->brightness = 2048;
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
        GsGetLwUnit(work->part, &work->matrix);
        work->delta.vx = work->target.vx - work->matrix.t[0];
        work->delta.vy = work->target.vy - work->matrix.t[1];
        work->delta.vz = work->target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        position.vx = work->matrix.t[0];
        position.vy = work->matrix.t[1];
        position.vz = work->matrix.t[2];
        RotTransPers(&position, (long *)&projection.projected,
                     (long *)&projection.interpolation, (long *)&projection.flag);
        screen_x = projection.projected;
        screen_y = projection.projected >> 16;
        target.vx = work->target.vx;
        target.vy = work->target.vy;
        target.vz = work->target.vz;
        RotTransPers(&target, (long *)&projection.target_projected,
                     (long *)&projection.interpolation, (long *)&projection.flag);
        work->screen_delta.vx = projection.target_projected - screen_x;
        work->screen_delta.vy = (projection.target_projected >> 16) - screen_y;
        if (work->state > 0) {
            func_8013C02C((u8 *)point);
        }
        if (work->elapsed > work->config->start) {
            func_8013B93C((u8 *)point);
        }
        work->frame++;
        angle = Model_GetSlotAnimationFrame(work->slot);
        if (angle != work->animation_frame) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = angle;
        }
    }
    func_800595C8(2, work->brightness, work->brightness, work->brightness);
    if (work->state < 5) {
        if (work->brightness > 0) {
            work->brightness -= 128;
            if (work->brightness <= 0) {
                work->brightness = 0;
            }
        }
    } else {
        work->brightness = work->fade * 32;
    }
    if (work->state >= 1 && work->state <= 3) {
        result = 4;
    } else if (work->state == 4) {
        result = 1;
        work->state = 5;
    } else if (work->state == 5) {
        work->fade += work->step;
        if (work->fade >= 64) {
            work->fade = 64;
            work->state = 6;
        }
    } else if (work->state == 6) {
        result = 2;
    }
    return result;
}
