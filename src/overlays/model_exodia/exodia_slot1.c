#include "../../types.h"
#include "exodia_slot1.h"

s32 ModelExodia_HandleSecondRequest(SVECTOR *point, s32 command)
{
    ExodiaSecondEntryState *work = (ExodiaSecondEntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    ModelVariantProjection projection;
    ExodiaSecondEntryBeam *beam = work->beams;
    ModelVariantPointSheet *ring = work->rings;
    ModelVariantFlaggedSpiralArm *colors = work->petals;
    POLY_GT4 *textured = work->packets.textured;
    POLY_F4 *flat = &work->flat;
    POLY_G3 *triangle = &work->packets.triangle;
    POLY_G4 *quad = &work->packets.quad;
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
        work->config = &gModelExodia_aSecondConfigs[command];
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        Model_GetSlot0OffsetWorldPosition(480, (s16 *)&work->origin);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 416, 256);
        clut = GetClut(EXODIA_SECOND_CLUT_X, EXODIA_SECOND_CLUT_Y);
        GetTPage(1, 1, 896, 0);
        GetClut(EXODIA_SECOND_GLOW_CLUT_X, 244);
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
            ring->outer.r = 192;
            ring->outer.g = 192;
            ring->outer.b = 160;
            ring->inner.r = 255;
            ring->inner.g = 224;
            ring->inner.b = 64;
            ring->size = 0;
            ring->field_8C = 0;
            ring->field_90 = 0;
        }
        work->width = 1024;
        work->petal_angle = 0;
        work->field_C8E = 0;
        work->petal_radius = 0;
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
        RotTransPers(&position, (long *)&projection.projected,
                     (&projection.interpolation), (&projection.flag));
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (long *)&projection.target,
                     (&projection.interpolation), (&projection.flag));
        setVector(&work->target, 0, -4000, -6000);
        dx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        if (work->frame < work->config->pulse3) {
            Model_GetSlot0OffsetWorldPosition(480, (s16 *)&work->origin);
        } else {
            Model_GetSlot0OffsetWorldPosition(0, (s16 *)&work->origin);
        }
        work->direction.vx = work->target.vx - work->origin.vx;
        work->direction.vy = work->target.vy - work->origin.vy;
        work->direction.vz = work->target.vz - work->origin.vz;
        if (work->config->start <= work->frame) {
            ModelExodia_DrawRings((u8 *)point);
        }
        if (work->config->petals_start <= work->frame) {
            ModelExodia_DrawPetals((u8 *)point);
        }
        if (work->config->pulse0 <= work->frame && work->frame < work->config->pulse1) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            ModelExodia_DrawBeam((u8 *)point);
        } else if (work->config->pulse1 <= work->frame && work->frame < work->config->pulse2) {
            if (work->phase == 3) {
                work->distance = 0;
                work->phase = 1;
            }
        } else if (work->config->pulse2 <= work->frame && work->frame < work->config->pulse3) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            ModelExodia_DrawBeam((u8 *)point);
        } else if (work->config->pulse3 <= work->frame && work->frame < work->config->pulse4) {
            if (work->phase == 3) {
                work->distance = 0;
                work->phase = 1;
            }
        } else if (work->config->pulse4 <= work->frame && work->frame < work->config->pulse5) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            ModelExodia_DrawBeam((u8 *)point);
        } else if (work->config->pulse5 <= work->frame && work->frame < work->config->pulse6) {
            if (work->phase == 3) {
                work->distance = 0;
                work->phase = 1;
            }
        } else if (work->config->pulse6 <= work->frame) {
            if (work->phase == 1) {
                work->phase = 2;
            }
            ModelExodia_DrawBeam((u8 *)point);
        }
        if (work->config->fade_start <= work->frame) {
            ot = Graphics_GetDefaultOrderingTable();
            flat->r0 = work->fade;
            flat->g0 = work->fade;
            flat->b0 = work->fade;
            setXY4(flat, 0, 0, 320, 0, 0, EXODIA_SECOND_SCREEN_HEIGHT, 320,
                   EXODIA_SECOND_SCREEN_HEIGHT);
            Graphics_SubmitDrawModePacket((u32 *)flat, ot, 0, 1);
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

void ModelExodia_DrawRings(u8 *context)
{
    ExodiaSecondRingState *work;
    SVECTOR rotation;
    VECTOR position;
    VECTOR scale;
    MATRIX local;
    MATRIX screen;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    GsOT *ot;
    s32 i, j;
    s32 flicker;
    ExodiaRing *ring;
    POLY_GT4 *quad;

    work = (ExodiaSecondRingState *)context;
    ring = work->rings;
    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->quad;
    for (i = 0; i < 2; i++, ring++) {
        flicker = 0;
        if (work->frame_count & 1) {
            flicker = ring->scale / 8;
        }
        if (i == 0) {
            if (work->phase < 2) {
                if (ring->scale < 4096) {
                    ring->scale = ((work->frame - work->timing->start) << 12) /
                                  (work->timing->ramp_end - work->timing->start);
                    if (ring->scale >= 4096) {
                        ring->scale = 4096;
                        work->phase = 1;
                    }
                } else {
                    ring->scale = 4096;
                }
            } else if (work->timing->second_start <= work->frame) {
                ring->scale = 6144;
            }
        } else {
            if (work->timing->second_start <= work->frame && work->phase >= 2) {
                ring->scale = 4096;
            } else {
                ring->scale = 0;
            }
        }
        setVector(&rotation, 0, 0, 0);
        if (i == 0) {
            local.t[0] = work->origin.vx;
            local.t[1] = work->origin.vy;
            local.t[2] = work->origin.vz;
        } else {
            local.t[0] = work->origin.vx + work->direction.vx * work->distance / 1024;
            local.t[1] = work->origin.vy + work->direction.vy * work->distance / 1024;
            local.t[2] = work->origin.vz + work->direction.vz * work->distance / 1024;
        }
        scale.vx = ring->scale + flicker;
        scale.vy = ring->scale + flicker;
        scale.vz = ring->scale + flicker;
        RotMatrix(&rotation, &local);
        coordinate.coord = local;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &screen);
        GsSetLsMatrix(&screen);
        ReadRotMatrix(&screen);
        RotMatrix(&rotation, &screen);
        ScaleMatrix(&screen, &scale);
        SetRotMatrix(&screen);
        for (j = 0; j < 4; j++) {
            RotTransPers4(&ring->points[j], &ring->points[j + 4],
                          &ring->points[j + 8], &ring->points[j + 12],
                          (long *)&quad->x0, (long *)&quad->x1,
                          (long *)&quad->x2, (long *)&quad->x3,
                          (long *)&interpolation, (long *)&flag);
#ifdef MODEL_EXODIA_USA
            /* The US build stores the projected corners back in place. */
            setXY4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                   quad->x2, quad->y2, quad->x3, quad->y3);
#endif
            setRGB0(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB1(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB2(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB3(quad, ring->inner.r, ring->inner.g, ring->inner.b);
            flag = 0;
            GsSortPoly(quad, ot, 0);
        }
    }
}

void ModelExodia_DrawPetals(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    ExodiaSecondEntryState *work;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 radius;
    s32 eighth;
    s32 scaled_width;
    s32 distance;
    s32 spread;
    s32 turn;
    ModelVariantFlaggedSpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;

    work = (ExodiaSecondEntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    arm = work->petals;
    poly = work->packets.textured;
    radius = work->petal_radius * 4;
    eighth = work->petal_radius / 8;
    /* The template tests retain both products, but their assignments are dead. */
    if (radius * work->field_C8E < 0) {
        length = 0;
    }
    if (eighth * work->field_C8E < 0) {
        length = 0;
    }
    spread = 0x400 - work->field_C92;
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + work->petal_angle * 2;
        theta = (i << 13) / 12 + work->petal_angle;
        for (k = 0; k < 2; k++) {
            if (!(work->frame_count & 1)) {
                length = (k * 128 + 0x30) * spread / 1024;
            } else {
                length = (k * 64 + 0x18) * spread / 1024;
            }
            if (k == 0) {
                setVector(&arm->a[0], 0, 0, 0);
            } else {
                distance = radius * k;
                setVector(&arm->a[k], rcos(phi) * (rsin(theta) * distance >> 12) >> 12,
                          rcos(theta) * distance >> 12,
                          rsin(phi) * (rsin(theta) * distance >> 12) >> 12);
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(work->frame_count & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->origin.vx;
    m.t[1] = work->origin.vy;
    m.t[2] = work->origin.vz;
    scale.vx = size;
    scale.vy = size;
    scale.vz = size;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    arm = work->petals;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->otz[k] = RotTransPers4(&arm->a[0], &arm->a[k], &arm->a[0], &arm->a[k],
                                            &arm->sa[0], &arm->sa[k], &arm->sa[0], &arm->sa[k], &p, &arm->flag[k]);
                RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                dx = (s16)arm->sa[k] - (s16)arm->sa[0];
                dy = (arm->sa[k] >> 16) - (arm->sa[0] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            } else {
                arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                            &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1], &p, &arm->flag[k]);
                RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                dx = (s16)arm->sa[k + 1] - (s16)arm->sa[k];
                dy = (arm->sa[k + 1] >> 16) - (arm->sa[k] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            }
        }
    }
    arm = work->petals;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 1; k++) {
            poly->x0 = arm->sa[k] + arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) + arm->oy[k];
            poly->x1 = arm->sa[k + 1] + arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) + arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            poly->r0 = arm->outer[k].r;
            poly->g0 = arm->outer[k].g;
            poly->b0 = arm->outer[k].b;
            poly->r1 = arm->outer[k + 1].r;
            poly->g1 = arm->outer[k + 1].g;
            poly->b1 = arm->outer[k + 1].b;
            poly->r2 = arm->inner[k].r;
            poly->g2 = arm->inner[k].g;
            poly->b2 = arm->inner[k].b;
            poly->r3 = arm->inner[k + 1].r;
            poly->g3 = arm->inner[k + 1].g;
            poly->b3 = arm->inner[k + 1].b;
            arm->otz[k] = 0;
            arm->flag[k] = 0;
            if (arm->otz[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);
            }
            poly->x0 = arm->sa[k] - arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) - arm->oy[k];
            poly->x1 = arm->sa[k + 1] - arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) - arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            poly->r0 = arm->outer[k].r;
            poly->g0 = arm->outer[k].g;
            poly->b0 = arm->outer[k].b;
            poly->r1 = arm->outer[k + 1].r;
            poly->g1 = arm->outer[k + 1].g;
            poly->b1 = arm->outer[k + 1].b;
            poly->r2 = arm->inner[k].r;
            poly->g2 = arm->inner[k].g;
            poly->b2 = arm->inner[k].b;
            poly->r3 = arm->inner[k + 1].r;
            poly->g3 = arm->inner[k + 1].g;
            poly->b3 = arm->inner[k + 1].b;
            arm->otz[k] = 0;
            arm->flag[k] = 0;
            if (arm->otz[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);
            }
        }
    }
    work->petal_angle += work->step * 16;
    if (work->petal_radius < 0x400) {
        work->petal_radius += work->step * 64;
        if (work->petal_radius >= 0x400) {
            work->petal_radius = 0x400;
        }
    }
}

void ModelExodia_DrawBeam(u8 *context)
{
    ExodiaBeamState *work;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX local;
    MATRIX screen;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    GsOT *ot;
    s32 angle;
    s16 i, j;
    s16 width;
    ExodiaBeam *beam;
    POLY_GT4 *quad;

    work = (ExodiaBeamState *)context;
    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->quad;
    angle = ratan2(work->angles.vy, work->angles.vx) + 2048;
    if (work->phase > 0) {
        if (!(work->frame_count & 1)) {
            width = work->width / 2;
        } else {
            width = work->width * 640 / 1024;
        }
        beam = work->beams;
        for (i = 0; i < 1; i++, beam++) {
            for (j = 0; j < 17; j++) {
                (j + beam->outer)->vx = rcos(1024) * width >> 12;
                (j + beam->outer)->vy = rsin(1024) * width >> 12;
                (j + beam->outer)->vz = 0;
                setVector(&beam->center[j], 0, 0, 0);
                (j + beam->inner)->vx = rcos(3072) * width >> 12;
                (j + beam->inner)->vy = rsin(3072) * width >> 12;
                (j + beam->inner)->vz = 0;
                setVector(&rotation, 0, 0, angle);
                if (j == 0) {
                    local.t[0] = work->origin.vx;
                    local.t[1] = work->origin.vy;
                    local.t[2] = work->origin.vz;
                } else {
                    local.t[0] = work->origin.vx + work->direction.vx * j / 16 * work->distance / 1024;
                    local.t[1] = work->origin.vy + work->direction.vy * j / 16 * work->distance / 1024;
                    local.t[2] = work->origin.vz + work->direction.vz * j / 16 * work->distance / 1024;
                }
                setVector(&scale, 4096, 4096, 4096);
                RotMatrix(&rotation, &local);
                coordinate.coord = local;
                coordinate.super = 0;
                coordinate.flg = 0;
                GsGetLs(&coordinate, &screen);
                GsSetLsMatrix(&screen);
                ReadRotMatrix(&screen);
                RotMatrix(&rotation, &screen);
                ScaleMatrix(&screen, &scale);
                SetRotMatrix(&screen);
                beam->depth[j] = RotTransPers3(&beam->outer[j],
                    &beam->center[j], &beam->inner[j],
                    (long *)&beam->projected[0][j], (long *)&beam->projected[1][j],
                    (long *)&beam->projected[2][j], (long *)&interpolation,
                    (long *)&beam->flags[j]);
            }
        }
        beam = work->beams;
        for (i = 0; i < 1; i++, beam++) {
            setRGB0(quad, 255, 255, 64);
            setRGB1(quad, 255, 255, 64);
            setRGB2(quad, 160, 160, 160);
            setRGB3(quad, 160, 160, 160);
            for (j = 0; j < 16; j++) {
                quad->x0 = beam->projected[0][j];
                quad->y0 = beam->projected[0][j] >> 16;
                quad->x1 = beam->projected[0][j + 1];
                quad->y1 = beam->projected[0][j + 1] >> 16;
                quad->x2 = beam->projected[1][j];
                quad->y2 = beam->projected[1][j] >> 16;
                quad->x3 = beam->projected[1][j + 1];
                quad->y3 = beam->projected[1][j + 1] >> 16;
                if (beam->depth[j] >= 0 && beam->flags[j] >= 0) {
                    GsSortPoly(quad, ot, beam->depth[j]);
                }
                quad->x0 = beam->projected[2][j];
                quad->y0 = beam->projected[2][j] >> 16;
                quad->x1 = beam->projected[2][j + 1];
                quad->y1 = beam->projected[2][j + 1] >> 16;
                quad->x2 = beam->projected[1][j];
                quad->y2 = beam->projected[1][j] >> 16;
                quad->x3 = beam->projected[1][j + 1];
                quad->y3 = beam->projected[1][j + 1] >> 16;
                if (beam->depth[j] >= 0 && beam->flags[j] >= 0) {
                    GsSortPoly(quad, ot, beam->depth[j]);
                }
            }
        }
        if (work->distance <= 1024) {
            work->distance += work->step * 128;
            if (work->distance >= 1024) {
                work->distance = 1024;
                if (work->phase == 2) {
                    work->phase = 3;
                }
            }
        }
    }
}
