#include "../../types.h"
#include "variant475_entry.h"

#ifndef MODEL_VARIANT475_CLUT_X
#define MODEL_VARIANT475_CLUT_X 640
#endif

s32 FrenchModelVariant475_HandleRequest(u8 *context, s32 command)
{
    /* Retail projection inputs follow 64 bytes of unrecovered locals. */
    u8 unknown_stack[64];
    SVECTOR origin;
    SVECTOR target;
    VECTOR squared;
    Variant475EntryProjection projection;
    Variant475EntryState *work;
    Variant475EntryQuads *quads;
    Variant475EntryRibbon *ribbon;
    ModelVariantSheetSet *sheet;
    Variant458Streamer *streamer;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_GT4 *poly;
    POLY_GT4 *extra;
    POLY_FT4 *flat;
    POLY_FT4 *extra_flat;
    s32 result;
    s32 packed;
    s32 angle;
    s32 quad_angle;
    u16 clut;
    u16 first_clut;
    u16 first_page;
    u16 tpage;
    s32 i, j, k;
    u32 animation_frame;
    GsRVIEW2 *view;
    u16 origin_x;
    s32 origin_y;

    work = (Variant475EntryState *)context;
    quads = work->quads;
    sheet = work->sheets;
    streamer = work->streamers;
    poly = work->textured;
    flat = work->flat;
    ribbon = work->ribbons;
    triangle = &work->triangle;
    quad = &work->quad;
    extra = &work->extra;
    extra_flat = &work->extra_flat;
    result = 0;
    if (command >= 0) {
        work->tail.slot = Model_GetActiveSlotIndex();
        work->config = gFrenchModelVariant475_aConfigs + command;
        for (k = 0; k < 8; k++) {
            work->parts[k] = Model_GetSlotDataEntry(work->tail.slot, work->config->parts[k]);
        }
        if (work->tail.slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        for (k = 0, angle = 0; k < 8; k++, angle += 1536) {
            setVector(&work->starts[k], work->matrix.t[0] + (rcos(angle) * 320 >> 12),
                      work->matrix.t[1] + (rsin(angle) * 320 >> 12), work->matrix.t[2]);
            setVector(&work->ends[k], work->target.vx + (rcos(angle) * 256 >> 12),
                      work->target.vy + (rsin(angle) * 256 >> 12), work->target.vz);
            setVector(&work->deltas[k], work->ends[k].vx - work->starts[k].vx,
                      work->ends[k].vy - work->starts[k].vy, work->ends[k].vz - work->starts[k].vz);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT475_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT475_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant475_aImages[7]);
        ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant475_aImages[5]);
        ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant475_aImages[1]);
        ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant475_aImages[2]);
        ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant475_aImages[3]);
        first_page = packed >> 16;
        first_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->tail.slot, 1, &gFrenchModelVariant475_aImages[6]);
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(poly);
        poly->tpage = tpage;
        setUV4(poly, 126, 0, 127, 0, 126, 63, 127, 63);
        poly->clut = clut;
        SetSemiTrans(poly, 1);
        SetShadeTex(poly, 0);
        poly++;
        SetPolyGT4(poly);
        poly->tpage = tpage;
        setUV4(poly, 64, 0, 127, 0, 64, 63, 127, 63);
        poly->clut = clut;
        SetSemiTrans(poly, 1);
        SetShadeTex(poly, 0);
        SetPolyFT4(flat);
        flat->tpage = first_page;
        setUV4(flat, 0, 128, 0, 175, 48, 128, 48, 175);
        flat->clut = first_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = first_page;
        setUV4(flat, 0, 176, 0, 223, 48, 176, 48, 223);
        flat->clut = first_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        SetPolyGT4(extra);
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyFT4(extra_flat);
        extra_flat->tpage = packed >> 16;
        setUV4(extra_flat, 0, 64, 63, 64, 0, 127, 63, 127);
        extra_flat->clut = packed;
        SetSemiTrans(extra_flat, 1);
        SetShadeTex(extra_flat, 0);
        for (i = 0, quad_angle = 0; i < 1; i++, quad_angle = i << 12, quads++) {
            for (j = 0; j < 8; j++) {
                setVector(&quads->a[j], -128, -128, 0);
                setVector(&quads->b[j], 128, -128, 0);
                setVector(&quads->c[j], -128, 128, 0);
                setVector(&quads->d[j], 128, 128, 0);
                quads->color.r = 128;
                quads->color.g = 128;
                quads->color.b = 128;
                quads->level[j] = -j * 256;
                quads->hidden[j] = 0;
                quads->field_194[j] = 0;
                setVector(&quads->position, work->matrix.t[0] + (rcos(quad_angle) * 512 >> 12),
                          work->matrix.t[1], work->matrix.t[2] + (rsin(quad_angle) * 512 >> 12));
            }
        }
        for (i = 0; i < 8; i++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->inner.r = 128;
            ribbon->inner.g = 128;
            ribbon->inner.b = 128;
            ribbon->outer.r = 0;
            ribbon->outer.g = 16;
            ribbon->outer.b = 128;
            ribbon->field_1B8 = 0;
            ribbon->field_1BC = 0;
            ribbon->count = -(i * 16);
            ribbon->field_1C4 = 0;
            ribbon->state = 0;
            ribbon->len = 1024;
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = 1024 + (i + 1) * 1024 / 9;
            } else {
                angle = 1024 - i * 1024 / 9;
            }
            setVector(&ribbon->velocity, rcos(angle) * 256 >> 12, rsin(angle) * 256 >> 12, 0);
        }
        for (j = 0; j < 16; j++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -192, -192, 0);
                    setVector(&sheet->v1[k], 0, -192, 0);
                    setVector(&sheet->v2[k], -192, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 192, -192, 0);
                    setVector(&sheet->v1[k], 192, 0, 0);
                    setVector(&sheet->v2[k], 0, -192, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 192, 192, 0);
                    setVector(&sheet->v1[k], 0, 192, 0);
                    setVector(&sheet->v2[k], 192, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -192, 192, 0);
                    setVector(&sheet->v1[k], -192, 0, 0);
                    setVector(&sheet->v2[k], 0, 192, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 64;
            sheet->inner[2] = 255;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
            sheet->shown = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            s32 point;
            for (point = 0; point < 17; point++) {
                streamer->color[point][0] = 128;
                streamer->color[point][1] = 128;
                streamer->color[point][2] = 128;
            }
            MODEL_VARIANT_WORD(streamer, 0x2A8) = 0;
        }
        work->field_2FFA = 1024;
        work->streamer_length = 1024;
        work->field_2FFC = 0;
        work->orbit = 0;
        work->orbit_progress = 0;
        work->field_2FF4 = 0;
        work->field_2FF6 = 0;
        work->field_2FF8 = 0;
        work->ribbon_wave = 0;
        work->ribbon_phase = 0;
        work->streamer_rotation_x = 0;
        work->streamer_rotation_y = 0;
        work->streamer_rotation_z = 0;
        work->clock.frame_count = 0;
        work->clock.frame = 0;
        work->clock.animation_frame = 0;
        work->clock.step = 0;
        work->tail.phase = 0;
        work->clock.fade = 0;
        work->tail.tint = 2048;
        work->tail.command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx, view->vry - view->vpy,
                  view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->clock.angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->tail.slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        for (k = 0, angle = 0; k < 8; k++, angle += 1536) {
            setVector(&work->starts[k], work->matrix.t[0] + (rcos(angle + work->orbit) * 320 >> 12),
                      work->matrix.t[1] + (rsin(angle + work->orbit) * 320 >> 12), work->matrix.t[2]);
            setVector(&work->ends[k], work->target.vx + (rcos(angle) * 256 >> 12),
                      work->target.vy + (rsin(angle) * 256 >> 12), work->target.vz);
            setVector(&work->deltas[k], work->ends[k].vx - work->starts[k].vx,
                      work->ends[k].vy - work->starts[k].vy, work->ends[k].vz - work->starts[k].vz);
        }
        setVector(&origin, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&origin, &projection.origin, &projection.p, &projection.flag);
        origin_x = projection.origin;
        origin_y = projection.origin >> 16;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &projection.target, &projection.p, &projection.flag);
        work->screen_delta.vx = (u16)projection.target - origin_x;
        work->screen_delta.vy = (projection.target >> 16) - origin_y;
        if (work->config->phase4 < work->clock.frame && work->tail.phase == 3) {
            work->tail.phase = 4;
        }
        if (work->config->phase5 < work->clock.frame && work->tail.phase == 4) {
            work->tail.phase = 5;
        }
        if (work->clock.frame >= work->config->orbit_start) {
            if (work->orbit_progress < 1024) {
                work->orbit_progress = ((work->clock.frame - work->config->orbit_start) << 10) /
                                       (work->config->ribbon_start - work->config->orbit_start);
                if (work->orbit_progress >= 1024) {
                    work->orbit_progress = 1024;
                }
            }
            work->orbit += rsin(work->orbit_progress) * 192 >> 12;
        }
        if (work->config->ribbon_start <= work->clock.frame) {
            ModelVariant458_DrawRibbons(context);
        }
        if (work->config->start <= work->clock.frame && work->tail.phase < 6) {
            ModelVariant458_DrawQuads(context);
        }
        if (work->tail.phase >= 1 && work->tail.phase <= 5) {
            ModelVariant458_DrawSheets(context);
        }
        work->clock.frame_count++;
        animation_frame = Model_GetSlotAnimationFrame(work->tail.slot);
        if (animation_frame != work->clock.animation_frame) {
            work->clock.frame += Model_GetFrameStep();
            work->clock.step = Model_GetFrameStep();
            work->clock.animation_frame = animation_frame;
        }
    }
    Model_SetSlotAmbient(2, work->tail.tint, work->tail.tint, work->tail.tint);
    if (work->tail.phase < 6) {
        if (work->tail.tint > 0) {
            work->tail.tint -= 128;
            if (work->tail.tint <= 0) {
                work->tail.tint = 0;
            }
        }
    } else {
        work->tail.tint = work->clock.fade << 5;
    }
    if (work->tail.phase >= 3 && work->tail.phase <= 5) {
        result = 4;
    } else if (work->tail.phase == 6) {
        result = 1;
        work->tail.phase = 7;
    } else if (work->tail.phase == 7) {
        if (work->clock.fade < 64) {
            work->clock.fade += work->clock.step;
            if (work->clock.fade >= 64) {
                work->clock.fade = 64;
                work->tail.phase = 8;
            }
        }
    } else if (work->tail.phase == 8) {
        result = 2;
    }
    return result;
}
