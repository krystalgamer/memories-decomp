#include "../../types.h"
#include "variant380_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT380_CLUT_X
#define MODEL_VARIANT380_CLUT_X 512
#endif

/* Header 380's entry: up to five beams run from model parts to targets on a
 * 192-unit ring at height -2048, with one sheet per beam and two streamers.
 * From phase 3 the beam origins sit on that ring and their targets circle
 * the opposing slot's position. GCC 2.7.2.
 *
 * Matching quirks: the two `do { } while (0)` blocks only add loop depth.
 * The first splits the part-index offset into its own spilled counter and
 * stops the matrix address from being strength-reduced; the second raises
 * the weight of `angle` so it gets its register before the first loop's
 * induction pointers. The -2048 height is a variable set inside the first
 * loop so it is hoisted and rematerialized, the sheet loop has its own
 * counter, the screen delta Y is computed before X, and the span store sits
 * before the fade reset. */

s32 ModelVariant380_HandleRequest(SVECTOR *point, s32 command)
{
    Variant380EntryState *work = (Variant380EntryState *)point;
    Variant380EntryStack stack;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant380EntryBeam *beam = work->beams;
    ModelVariantSheetSet *sheet = work->sheets;
    Variant337EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result = 0;
    s32 i, j;
    s32 angle, angle2;
    u16 tpage, clut;
    s32 packed, flat_page;
    GsRVIEW2 *view;
    s16 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dy;
    s32 height, sheet_index;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant380EntryConfig *)(gModelVariant380_D2970 + 0x38) + command;
        for (i = 0, angle = 0; i < work->config->count;) {
            height = -2048;
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
            /* Single-pass block; see the matching notes above. */
            do {
                setVector(&work->targets[i], (rsin(angle) * 192) >> 12, height,
                          (rcos(angle) * 192) >> 12);
            } while (0);
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i], work->targets[i].vx - work->matrices[i].t[0],
                      work->targets[i].vy - work->matrices[i].t[1],
                      work->targets[i].vz - work->matrices[i].t[2]);
            i++;
            angle = (i << 12) / work->config->count;
        }
        stack.texture_cluts[1] = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)gModelVariant380_D2970);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT380_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gModelVariant380_D2970 + 0x1C));
        stack.texture_cluts[0] = packed;
        flat_page = packed >> 16;
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
        setUV4(flat, 41, 32, 41, 95, 0, 32, 0, 95);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 41, 96, 41, 159, 0, 96, 0, 159);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 83, 32, 83, 95, 42, 32, 42, 95);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 83, 96, 83, 159, 42, 96, 42, 159);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 125, 32, 125, 95, 84, 32, 84, 95);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 125, 96, 125, 159, 84, 96, 84, 159);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        for (i = 0; i < work->config->count; i++, beam++) {
            setVector(&beam->scale, 4096, 4096, 4096);
            beam->inner.r = 128;
            beam->inner.g = 128;
            beam->inner.b = 128;
            beam->outer.r = 0;
            beam->outer.g = 16;
            beam->outer.b = 128;
            beam->field_1B8 = 0;
            beam->field_1BA = 0;
            beam->field_1BC = 0;
            /* Single-pass block; see the matching notes above. */
            do {
                if (i == 0) {
                    angle = 1024;
                } else if (i % 2 == 1) {
                    angle = 1024 + (i + 1) * 1024 / (work->config->count + 1);
                } else {
                    angle = 1024 - i * 1024 / (work->config->count + 1);
                }
            } while (0);
            beam->velocity.vx = (rcos(angle) * 256) >> 12;
            beam->velocity.vy = (rsin(angle) * 256) >> 12;
            beam->velocity.vz = 0;
        }
        for (sheet_index = 0; sheet_index < work->config->count; sheet_index++, sheet++) {
            for (j = 0; j < 4; j++) {
                if (j == 0) {
                    setVector(&sheet->v0[j], -128, -128, 0);
                    setVector(&sheet->v1[j], 0, -128, 0);
                    setVector(&sheet->v2[j], -128, 0, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                } else if (j == 1) {
                    setVector(&sheet->v0[j], 128, -128, 0);
                    setVector(&sheet->v1[j], 128, 0, 0);
                    setVector(&sheet->v2[j], 0, -128, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                } else if (j == 2) {
                    setVector(&sheet->v0[j], 128, 128, 0);
                    setVector(&sheet->v1[j], 0, 128, 0);
                    setVector(&sheet->v2[j], 128, 0, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                } else if (j == 3) {
                    setVector(&sheet->v0[j], -128, 128, 0);
                    setVector(&sheet->v1[j], -128, 0, 0);
                    setVector(&sheet->v2[j], 0, 128, 0);
                    setVector(&sheet->v3[j], 0, 0, 0);
                }
            }
            sheet->outer[0] = 192;
            sheet->outer[1] = 192;
            sheet->outer[2] = 192;
            sheet->inner[0] = 0;
            sheet->inner[1] = 128;
            sheet->inner[2] = 255;
            sheet->shown = 0;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 255;
                streamer->color[j].g = 255;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->width = 1024;
        for (i = 0; i < work->config->count; i++) {
            work->field_1B70[i] = -(i * 8);
            work->field_1B7A[i] = 0;
            work->field_1B84[i] = 0;
            work->field_1B8E[i] = 0;
            work->field_1BB4[i] = 0;
            work->field_1BC8[i] = 0;
            work->field_1BDC[i] = 0;
            work->field_1BA0[i] = 1024;
        }
        work->field_1B98 = 0;
        work->field_1B9A = 0;
        work->field_1BF0 = 0;
        work->field_1BF4 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->span = work->config->end - work->config->start;
        work->fade = 0;
        work->ambient = 2048;
        work->phase = 0;
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
        for (i = 0, angle = 0, angle2 = 0; i < work->config->count;) {
            if (work->phase < 3) {
                GsGetLwUnit(work->parts[i], &work->matrices[i]);
            } else {
                work->matrices[i].t[0] = (rsin(angle) * 192) >> 12;
                work->matrices[i].t[1] = -2048;
                work->matrices[i].t[2] = (rcos(angle) * 192) >> 12;
                setVector(&work->targets[i], work->target.vx + ((rsin(angle) * 192) >> 12),
                          work->target.vy + ((rsin(angle2) * 128) >> 12),
                          work->target.vz + ((rcos(angle) * 192) >> 12));
            }
            setVector(&work->directions[i], work->targets[i].vx - work->matrices[i].t[0],
                      work->targets[i].vy - work->matrices[i].t[1],
                      work->targets[i].vz - work->matrices[i].t[2]);
            GsSetLsMatrix(Model_GetLightSourceMatrix());
            setVector(&position, work->matrices[i].t[0], work->matrices[i].t[1], work->matrices[i].t[2]);
            RotTransPers(&position, (PSXLONG *)&projection.projected,
                         &projection.interpolation, &projection.flag);
            screen_x = projection.projected.vx;
            screen_y = projection.projected.vy;
            setVector(&target, work->targets[i].vx, work->targets[i].vy, work->targets[i].vz);
            RotTransPers(&target, (PSXLONG *)&projection.target,
                         &projection.interpolation, &projection.flag);
            dy = projection.target.vy - screen_y;
            work->screen_dx[i] = projection.target.vx - screen_x;
            work->screen_dy[i] = dy;
            i++;
            angle = (i << 12) / work->config->count;
            angle2 = (i * 7000) / work->config->count;
        }
        if (work->config->start <= work->frame) {
            if (work->phase > 0) {
                FrenchModelVariant397_DrawBeams(point);
            }
            FrenchModelVariant397_DrawSheets(point);
            if (work->phase >= 4) {
                FrenchModelVariant397_DrawStreamers(point);
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
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 6) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if (work->phase >= 3 && work->phase <= 6) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 9;
    } else if (work->phase == 9) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 10;
            }
        }
    } else if (work->phase == 10) {
        result = 2;
    }
    return result;
}
