#include "../../types.h"
#include "variant475_entry.h"

s32 func_8013B004(u8 *context, s32 command)
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
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant475EntryConfig *)(D_8013DD38 + 0xFC) + command;
        for (k = 0; k < 8; k++) {
            work->parts[k] = Model_GetSlotDataEntry(work->slot, work->config->parts[k]);
        }
        if (work->slot == 0) {
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
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DD38 + 0xC4));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DD38 + 0x8C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DD38 + 0x1C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DD38 + 0x38));
        func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DD38 + 0x54));
        first_page = packed >> 16;
        first_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DD38 + 0xA8));
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
        work->field_3008 = 1024;
        work->field_2FFC = 0;
        work->orbit = 0;
        work->orbit_progress = 0;
        work->field_2FF4 = 0;
        work->field_2FF6 = 0;
        work->field_2FF8 = 0;
        work->field_3018 = 0;
        work->field_301C = 0;
        work->field_300C = 0;
        work->field_3010 = 0;
        work->field_3014 = 0;
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
        if (work->config->phase4 < work->frame && work->phase == 3) {
            work->phase = 4;
        }
        if (work->config->phase5 < work->frame && work->phase == 4) {
            work->phase = 5;
        }
        if (work->frame >= work->config->orbit_start) {
            if (work->orbit_progress < 1024) {
                work->orbit_progress = ((work->frame - work->config->orbit_start) << 10) /
                                       (work->config->ribbon_start - work->config->orbit_start);
                if (work->orbit_progress >= 1024) {
                    work->orbit_progress = 1024;
                }
            }
            work->orbit += rsin(work->orbit_progress) * 192 >> 12;
        }
        if (work->config->ribbon_start <= work->frame) {
            func_8013BE98(context);
        }
        if (work->config->start <= work->frame && work->phase < 6) {
            func_8013CE34(context);
        }
        if (work->phase >= 1 && work->phase <= 5) {
            func_8013C978(context);
        }
        work->frame_count++;
        animation_frame = Model_GetSlotAnimationFrame(work->slot);
        if (animation_frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = animation_frame;
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
    if (work->phase >= 3 && work->phase <= 5) {
        result = 4;
    } else if (work->phase == 6) {
        result = 1;
        work->phase = 7;
    } else if (work->phase == 7) {
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
