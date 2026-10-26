#include "../../types.h"
#include "variant448_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT448_CLUT_X
#define MODEL_VARIANT448_CLUT_X 512
#endif

/* Header 448's entry: the header-404 entry shape with a 0x34-byte config, a
 * radial record, twelve spiral arms, three fan units and an orbit in place of
 * the glow and band records, five image uploads and three per-frame helpers.
 * The loop variables are shared between the record loops the way the
 * allocation shows: i for the three-record loops, m for the call-free inner
 * counter, q and r for the web and ring counters. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant448EntryState *work = (Variant448EntryState *)point;
    u8 unknown_stack[64];
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    POLY_G3 *triangle;
    Variant448Radial *radial;
    Variant448SpiralArm *arm;
    GsRVIEW2 *view;
    Variant448FanUnit *fan;
    POLY_GT4 *textured;
    Variant448Orbit *orbit;
    s32 near_scale, far_scale;
    ModelVariantRing *ring;
    s32 r;
    ModelVariantSpokeRing *spoke;
    u32 screen_x;
    Variant448EntryQuad *quad_record;
    u16 tpage, clut, flat_clut;
    ModelVariantWebNarrow *web;
    s32 fan_base;
    POLY_G4 *quad;
    POLY_GT4 *extra;
    POLY_FT4 *flat;
    s32 result;
    s32 q;
    s32 far_radius;
    s32 screen_y;
    s32 j;
    s32 ring_angle;
    s32 near_radius;
    s32 k, angle, m;
    s32 web_angle, outer;
    s32 one;
    s32 spoke_angle;
    s32 quad_angle;
    s32 i;
    s32 spoke_base;
    s32 packed, flat_page;
    s32 near_y;
    s32 far_y;
    s32 dx, dy;
    u32 frame;

    arm = work->spiral;
    fan = work->fans;
    orbit = work->orbit;
    ring = work->rings;
    spoke = work->spokes;
    quad_record = work->quads;
    web = work->webs;
    textured = work->textured;
    triangle = &work->triangle;
    flat = work->flat_textured;
    quad = &work->quad;
    radial = work->radial;
    result = 0;
    extra = &work->extra;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant448EntryConfig *)(D_8013F3E4 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT448_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT448_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F3E4 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F3E4 + 0x8C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F3E4 + 0x1C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F3E4 + 0x54));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013F3E4 + 0));
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = packed >> 16;
        setUV4(textured, 96, 128, 127, 128, 96, 255, 127, 255);
        textured->clut = packed;
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
        for (outer = 0; outer < 1; outer++, radial++) {
            for (m = 0, angle = 0; m < 17; m++, angle = m << 8) {
                setVector(&radial->point[0], 0, 0, 0);
                setVector(&radial->point[m + 1], (u32)(rcos(angle) * 3) >> 5, 0, (u32)(rsin(angle) * 3) >> 5);
                setVector(&radial->point[m + 18], (u32)rcos(angle) >> 3, 0, (u32)rsin(angle) >> 3);
            }
            radial->center.r = 255;
            radial->center.g = 255;
            radial->center.b = 255;
            radial->middle.r = 0;
            radial->middle.g = 128;
            radial->middle.b = 255;
            radial->edge.r = 0;
            radial->edge.g = 0;
            radial->edge.b = 0;
            radial->size = 0;
        }
        for (i = 0, one = 1, fan_base = 0; i < 3; fan_base -= 512, i++, fan++) {
            for (j = 0; j < 2; j++) {
                for (k = 0; k < 2; k++) {
                    fan->amount[j][k] = fan_base - j * 256 - k * 512;
                }
                fan->stage[j] = 0;
            }
            for (k = 0; k < 2; k++) {
                fan->cb[k][0] = work->config->outer.r * (one - k);
                fan->cb[k][1] = work->config->outer.g * (one - k);
                fan->cb[k][2] = work->config->outer.b * (one - k);
                fan->ca[k][0] = work->config->inner.r * (one - k);
                fan->ca[k][1] = work->config->inner.g * (one - k);
                fan->ca[k][2] = work->config->inner.b * (one - k);
            }
        }
        for (outer = 0; outer < 1; outer++, orbit++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&orbit->points[0][k], -256, -256, 0);
                    setVector(&orbit->points[1][k], 0, -256, 0);
                    setVector(&orbit->points[2][k], -256, 0, 0);
                    setVector(&orbit->points[3][k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&orbit->points[0][k], 256, -256, 0);
                    setVector(&orbit->points[1][k], 256, 0, 0);
                    setVector(&orbit->points[2][k], 0, -256, 0);
                    setVector(&orbit->points[3][k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&orbit->points[0][k], 256, 256, 0);
                    setVector(&orbit->points[1][k], 0, 256, 0);
                    setVector(&orbit->points[2][k], 256, 0, 0);
                    setVector(&orbit->points[3][k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&orbit->points[0][k], -256, 256, 0);
                    setVector(&orbit->points[1][k], -256, 0, 0);
                    setVector(&orbit->points[2][k], 0, 256, 0);
                    setVector(&orbit->points[3][k], 0, 0, 0);
                }
            }
            orbit->outer.r = work->config->outer.r;
            orbit->outer.g = work->config->outer.g;
            orbit->outer.b = work->config->outer.b;
            orbit->inner.r = work->config->inner.r;
            orbit->inner.g = work->config->inner.g;
            orbit->inner.b = work->config->inner.b;
            MODEL_VARIANT_WORD(orbit, 0x88) = 0;
            MODEL_VARIANT_WORD(orbit, 0x8C) = 0;
            MODEL_VARIANT_WORD(orbit, 0x90) = 0;
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 2; j++) {
                    for (m = 0; m < 3; m++) {
                        orbit->size[i][j][m] = 0;
                        orbit->done[i][j][m] = 0;
                    }
                }
            }
        }
        for (i = 0; i < 12; i++, arm++) {
            for (m = 0; m < 2; m++) {
                if (m == 1) {
                    arm->ca[m][0] = 0;
                    arm->ca[m][1] = 0;
                    arm->ca[m][2] = 0;
                    arm->cb[m][0] = 0;
                    arm->cb[m][1] = 0;
                    arm->cb[m][2] = 0;
                } else {
                    arm->ca[m][0] = work->config->inner.r;
                    arm->ca[m][1] = work->config->inner.g;
                    arm->ca[m][2] = work->config->inner.b;
                    arm->cb[m][0] = work->config->outer.r;
                    arm->cb[m][1] = work->config->outer.g;
                    arm->cb[m][2] = work->config->outer.b;
                }
            }
        }
        for (outer = 0; outer < 1; outer++, quad_record++) {
            setVector(&quad_record->point[0], 0, 0, 0);
            for (m = 0, quad_angle = 0; m < 5; m++, quad_angle = m * 1024) {
                setVector(&quad_record->point[m + 1], (u32)rcos(quad_angle) >> 6,
                          (u32)rsin(quad_angle) >> 8, 0);
                setVector(&quad_record->point[m + 6], (u32)(rcos(quad_angle) * 3) >> 10,
                          (u32)(rsin(quad_angle) * 5) >> 9, 0);
            }
            setVector(&quad_record->corner[0], -128, -128, 0);
            setVector(&quad_record->corner[1], 128, -128, 0);
            setVector(&quad_record->corner[2], -128, 128, 0);
            setVector(&quad_record->corner[3], 128, 128, 0);
            quad_record->inner[0] = 255;
            quad_record->inner[1] = 192;
            quad_record->inner[2] = 192;
            quad_record->outer[0] = 64;
            quad_record->outer[1] = 64;
            quad_record->outer[2] = 0;
            quad_record->size = 0;
            quad_record->field_84 = 0;
            quad_record->done = 0;
        }
        near_scale = 256;
        far_scale = 192;
        for (outer = 0, web_angle = 0; outer < 3; outer++, web++, web_angle += 300) {
            for (j = 0, m = 2048 / 5; j < 4; j++, m = (j + 1) * 2048 / 5) {
                near_radius = rsin(m) * near_scale >> 12;
                near_y = rcos(m) * near_scale >> 12;
                far_radius = rsin(m) * far_scale >> 12;
                far_y = rcos(m) * far_scale >> 12;
                for (q = 0, angle = web_angle; q < 6; q++, angle = web_angle + q * 4096 / 6) {
                    setVector(&web->near[j][q], rcos(angle) * near_radius >> 12,
                              near_y, rsin(angle) * near_radius >> 12);
                    setVector(&web->far[j][q], rcos(angle) * far_radius >> 12,
                              far_y, rsin(angle) * far_radius >> 12);
                }
            }
            MODEL_VARIANT_WORD(web, 0x184) = 0;
            MODEL_VARIANT_WORD(web, 0x188) = 0;
            MODEL_VARIANT_WORD(web, 0x18C) = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = 4096 + outer * 4096 / 3;
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (r = 0, q = 0; r < 6; r++, ring++, q += 256) {
            for (outer = 0, ring_angle = q; outer < 8;
                 outer++, ring_angle = q + outer * 512) {
                setVector(&ring->inner[outer], (u32)rcos(ring_angle) >> 4, (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring->outer[outer], (u32)(rcos(ring_angle) * 5) >> 6,
                          (u32)(rsin(ring_angle) * 5) >> 6, 0);
            }
            ring->color[0] = 64;
            ring->color[1] = 128;
            ring->color[2] = 255;
            ring->pad84[0] = 0;
            ring->pad84[1] = 0;
            ring->pad84[2] = 0;
            ring->angle = r * 1024 / 6;
            ring->count = 0;
        }
        for (r = 0, spoke_base = 0; r < 4; r++, spoke++, spoke_base += 200) {
            for (outer = 0, spoke_angle = spoke_base; outer < 8;
                 outer++, spoke_angle = spoke_base + outer * 512) {
                setVector(&spoke->inner[outer], (u32)(rcos(spoke_angle) * 3) >> 5,
                          (u32)(rsin(spoke_angle) * 3) >> 5, 0);
                setVector(&spoke->outer[outer], (u32)rcos(spoke_angle) >> 3, (u32)rsin(spoke_angle) >> 3, 0);
            }
            spoke->pad80[0] = 0;
            spoke->pad80[1] = 0;
            spoke->pad80[2] = 0;
            spoke->color[0] = 255;
            spoke->color[1] = 192;
            spoke->color[2] = 64;
            spoke->angle = 1024 - r * 256;
            spoke->count = 0;
        }
        work->field_1B24 = 0;
        work->field_1B26 = 0;
        work->field_1B28 = 0;
        work->field_1B2A = 0;
        work->field_1B2C = 0;
        work->field_1B30 = 4096;
        work->field_1B32 = 1024;
        work->field_1B34 = 0;
        work->field_1B38 = 0;
        work->field_1B2E = 0;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->position.vy = 0;
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        work->position.vx = work->target.vx;
        work->position.vz = work->target.vz;
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        dx = (u16)projection.target.vx - screen_x;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->start <= work->frame) {
            func_8013C254((u8 *)point);
        }
        if (work->phase >= 5) {
            func_8013D638((u8 *)point);
        }
        if (work->phase > 0) {
            func_8013C678((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    setVector(&work->offset, -512, -256, 0);
    func_8005F5C8(work->slot, 3, &work->offset, 20);
    func_800595C8(2, work->tint, work->tint, work->tint);
    if (work->phase < 11) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->phase >= 5 && work->phase <= 6) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 11;
    } else if (work->phase == 11) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 12;
            }
        }
    } else if (work->phase == 12) {
        result = 2;
    }
    return result;
}
