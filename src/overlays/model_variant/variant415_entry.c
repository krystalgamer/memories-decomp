#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant415_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT415_CLUT_X
#define MODEL_VARIANT415_CLUT_X 512
#endif

/* Header 415's entry: three ribbons, two sheets, two sets of three strands,
 * four rings, two lines and three sweeps, with the header-431 camera and
 * part tracking and the draw helpers of header 415. The French header-432
 * entry (VERSION_FRENCH) advances the bar index at the end of the bar loop
 * instead of at its top. */

s32 ModelVariant415_HandleRequest(SVECTOR *point, s32 command)
{
    Variant415EntryState *work;
    Variant415EntryStack stack;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    u32 frame;
    Variant415EntryRibbon *ribbon;
    GsRVIEW2 *view;
    ModelVariantSheet *sheet;
    s32 i, j, k, ring_base, ring_angle;
    s32 texture_word;
    s32 ring_index;
    s32 value;
    Variant415EntryStrand *strand_a;
    Variant415EntryStrand *strand_b;
    ModelVariantRing *ring;
    s32 index;
    Variant415EntryLine *line;
    s32 offset;
    POLY_GT4 *textured;
    s16 dx;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    u16 tpage, clut;
    POLY_GT4 *bar;
    Variant415EntrySweep *sweep;
    s32 n;
    s32 strand_angle;
    POLY_FT4 *flat;
    s32 result;
    s32 packed;
    u16 screen_x, screen_y;
    u16 flat_page, unset_page, unset_clut;
    s32 dy;

    work = (Variant415EntryState *)point;
    ribbon = work->ribbons;
    sheet = work->sheets;
    strand_a = work->strands_a;
    strand_b = work->strands_b;
    ring = work->rings;
    line = work->lines;
    triangle = &work->triangle;
    quad = &work->quad;
    textured = work->textured;
    flat = work->flat_textured;
    result = 0;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant415EntryConfig *)(gModelVariant415_D1F7C + 0x38) + command;
        for (i = 0; i < 5; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        for (i = 0; i < 5; i++) {
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i],
                      work->target.vx - work->matrices[i].t[0],
                      work->target.vy - work->matrices[i].t[1],
                      work->target.vz - work->matrices[i].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT415_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT415_CLUT_X, 244);
        texture_word = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant415_D1F7C + 0x1C));
        bar = work->bars;
        stack.texture_cluts[0] = texture_word;
        flat_page = texture_word >> 16;
        func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant415_D1F7C + 0x8C));
        func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant415_D1F7C + 0x1C));
        i = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant415_D1F7C + 0xE0));
        j = 0;
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        k = 0;
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
        flat->tpage = unset_page;
        setUV4(flat, 0, 128, 0, 175, 48, 128, 48, 175);
        flat->clut = unset_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = unset_page;
        setUV4(flat, 0, 176, 0, 223, 48, 176, 48, 223);
        flat->clut = unset_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        do {
#ifndef VERSION_FRENCH
            j++;
#endif
            SetPolyGT4(bar);
            bar->tpage = flat_page;
            setUV4(bar, 31, k, 31, k + 15, 0, k, 0, k + 15);
            bar->clut = stack.texture_cluts[0];
            SetSemiTrans(bar, 1);
            SetShadeTex(bar, 0);
#ifdef VERSION_FRENCH
            j++;
#endif
            k = j * 16;
            bar++;
        } while (j < 16);
        for (i = 0; i < 3; i++, ribbon++) {
            for (j = 0, offset = 0; j < 2; j++) {
                setVector(&ribbon->point[0][j], (u32)rcos(1024) >> 8, (u32)rsin(1024) >> 8, 0);
                setVector(&ribbon->point[2][j], (u32)rcos(3072) >> 8, (u32)rsin(3072) >> 8, 0);
                value = offset - (i << 9);
                offset -= 256;
                ribbon->amount[j] = value;
            }
            ribbon->color.g = 32;
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->color.r = 0;
            ribbon->color.b = 128;
            ribbon->field_5C = 0;
            ribbon->field_60 = 0;
            ribbon->field_5E = 0;
        }
        for (sweep = work->sweeps, index = 0; index < 3; index++, sweep++) {
            sweep->scale = -((index << 12) / 3);
        }
        for (n = 0; n < 2; n++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -128, -128, 0);
                    setVector(&sheet->v1[k], 0, -128, 0);
                    setVector(&sheet->v2[k], -128, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 128, -128, 0);
                    setVector(&sheet->v1[k], 128, 0, 0);
                    setVector(&sheet->v2[k], 0, -128, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 128, 128, 0);
                    setVector(&sheet->v1[k], 0, 128, 0);
                    setVector(&sheet->v2[k], 128, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -128, 128, 0);
                    setVector(&sheet->v1[k], -128, 0, 0);
                    setVector(&sheet->v2[k], 0, 128, 0);
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
        }
        for (index = 0; index < 3; index++, strand_a++) {
            for (j = 0, strand_angle = 0; j < 17; strand_angle = j << 8) {
                setVector(&strand_a->inner[j], (u32)rcos(strand_angle) >> 4, (u32)rsin(strand_angle) >> 4, 0);
                setVector(&strand_a->outer[j], (u32)(rcos(strand_angle) * 3) >> 5,
                          (u32)(rsin(strand_angle) * 3) >> 5, 0);
                j++;
                strand_a->color[0].r = 255;
                strand_a->color[0].g = 255;
                strand_a->color[0].b = 255;
                strand_a->color[1].r = 128;
                strand_a->color[1].g = 192;
                strand_a->color[1].b = 255;
            }
            strand_a->scale = 0;
            strand_a->done = 0;
        }
        for (n = 0; n < 3; n++, strand_b++) {
            for (j = 0, strand_angle = 0; j < 17; strand_angle = j << 8) {
                setVector(&strand_b->inner[j], (u32)rcos(strand_angle) >> 3, (u32)rsin(strand_angle) >> 3, 0);
                setVector(&strand_b->outer[j], (u32)(rcos(strand_angle) * 3) >> 4,
                          (u32)(rsin(strand_angle) * 3) >> 4, 0);
                j++;
                strand_b->color[0].r = 255;
                strand_b->color[0].g = 255;
                strand_b->color[0].b = 255;
                strand_b->color[1].r = 128;
                strand_b->color[1].g = 192;
                strand_b->color[1].b = 255;
            }
            strand_b->scale = -(n * 4096) / 3;
            strand_b->done = 0;
        }
        for (ring_index = 0, ring_base = 0; ring_index < 4; ring_index++, ring++, ring_base += 256) {
            for (n = 0, ring_angle = ring_base; n < 8; ring_angle += 512, n++) {
                setVector(&ring->inner[n], (u32)rcos(ring_angle) >> 4, (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring->outer[n], (u32)(rcos(ring_angle) * 5) >> 6,
                          (u32)(rsin(ring_angle) * 5) >> 6, 0);
            }
            ring->color[0] = 64;
            ring->color[1] = 128;
            ring->color[2] = 255;
            ring->pad84[0] = 0;
            ring->pad84[1] = 0;
            ring->pad84[2] = 0;
            ring->angle = ring_index << 8;
            ring->count = 0;
        }
        for (i = 0; i < 2; i++, line++) {
            setVector(&line->scale, 4096, 4096, 4096);
            line->color[0].r = 255;
            line->color[0].g = 255;
            line->color[0].b = 255;
            line->color[1].r = 0;
            line->color[1].g = 16;
            line->color[1].b = 128;
        }
        work->field_30D0 = 1024;
        work->field_30CC = 0;
        work->field_30D4 = 0;
        work->field_30E0 = 0;
        work->start_index = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->fade = 0;
        work->tint = 2048;
        work->phase = 0;
        work->field_30DC = 0;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        if (work->slot == 0) {
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        for (i = 0; i < 5; i++) {
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i],
                      work->target.vx - work->matrices[i].t[0],
                      work->target.vy - work->matrices[i].t[1],
                      work->target.vz - work->matrices[i].t[2]);
            setVector(&position, work->matrices[i].t[0], work->matrices[i].t[1], work->matrices[i].t[2]);
            RotTransPers(&position, (PSXLONG *)&projection.target,
                         &projection.interpolation, &projection.flag);
            dx = screen_x - projection.target.vx;
            dy = screen_y - projection.target.vy;
            work->screen_x[i] = dx;
            work->screen_y[i] = dy;
        }
        work->field_2F7C = dx;
        work->field_2F7E = dy;
        if (work->phase >= 2) {
            if (work->phase < 4) {
                SpanishModelVariant432_DrawSheets((u8 *)point, work->phase);
            }
            SpanishModelVariant432_DrawBands((u8 *)point);
        }
        if (work->config->timings[work->start_index] <= work->frame) {
            ModelVariant415_DrawRibbons((Variant415EntryRibbon *)point);
        }
        if (work->config->timings[0] <= work->frame) {
            SpanishModelVariant432_DrawBandLayers((u8 *)point);
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
    if (work->phase < 7) {
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
