#include "../../types.h"
#include "variant386_entry.h"

#ifdef VERSION_FRENCH
#define MODEL386_CLUT_X 640
#else
#define MODEL386_CLUT_X 512
#endif

/* Header 386's entry: up to five strands fan out from model joints while
 * sheets unfold and three chains swing, then the slot fades back in.
 *
 * The US build is GCC 2.7.2 (the `jr ra; nop` epilogue); the French
 * build of the image is one word shorter under GCC 2.8.1.
 * Matching quirks: the CLUT copy lives in the third halfword of an
 * 8-byte frame slot after 0x38 unused bytes, the orbit angle is written
 * `angle - -(spin + 2048)` so GCC does not reassociate the constant, and
 * the sheet loop keeps its own counter `n`. */
s32 ModelVariant386_HandleRequest(u8 *context, s32 command)
{
    Model386State *work = (Model386State *)context;
    u8 unused[0x38];
    u16 flat_clut[4];
    SVECTOR origin, target;
    VECTOR squared;
    PSXLONG screen0;
    PSXLONG depth, flag;
    PSXLONG screen1;
    Model386Strand *strand;
    Model386Sheet *sheet;
    Model386Chain *chain;
    POLY_GT4 *textured;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_FT4 *flat;
    s32 result;
    GsRVIEW2 *view;
    s32 i;
    s32 j;
    s32 k;
    s32 first;
    s32 angle;
    s16 x0;
    s32 y0;
    s32 frame;
    s32 n;
    u16 tpage;
    u16 clut;
    s32 flat_page;

    strand = work->strands;
    sheet = work->sheets;
    chain = work->chains;
    triangle = &work->triangle;
    quad = &work->quad;
    textured = work->textured;
    flat = work->flat;
    result = 0;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = gModelVariant386_D2C8C + command;
        for (i = 0; i < work->config->count; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
            work->extra = Model_GetSlotDataEntry(work->slot, work->config->extra);
            GsGetLwUnit(work->parts[i], &work->transforms[i]);
            if (i == 0) {
                GsGetLwUnit(work->parts[0], &work->matrix);
                angle = 1024;
            } else if (i == 1) {
                angle = 640;
            } else if (i == 2) {
                angle = 1408;
            } else if (i == 3) {
                angle = 256;
            } else {
                angle = 1792;
            }
            angle += 2048;
            setVector(&work->positions[i], work->matrix.t[0] + ((rcos(angle) * 384) >> 12),
                      work->matrix.t[1] + ((rsin(angle) * 384) >> 12) - 128, work->matrix.t[2]);
            setVector(&work->directions[i],
                      work->positions[i].vx - work->transforms[i].t[0],
                      work->positions[i].vy - work->transforms[i].t[1],
                      work->positions[i].vz - work->transforms[i].t[2]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL386_CLUT_X, 244);
        first = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant386_D2C54[1]);
        flat_clut[2] = first;
        flat_page = first >> 16;
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
        flat->clut = flat_clut[2];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 41, 96, 41, 159, 0, 96, 0, 159);
        flat->clut = flat_clut[2];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 83, 32, 83, 95, 42, 32, 42, 95);
        flat->clut = flat_clut[2];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 83, 96, 83, 159, 42, 96, 42, 159);
        flat->clut = flat_clut[2];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 125, 32, 125, 95, 84, 32, 84, 95);
        flat->clut = flat_clut[2];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 125, 96, 125, 159, 84, 96, 84, 159);
        flat->clut = flat_clut[2];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 0);
        for (i = 0; i < work->config->count; i++, strand++) {
            setVector(&strand->scale, 4096, 4096, 4096);
            strand->color.r = 128;
            strand->color.g = 128;
            strand->color.b = 128;
            strand->tint.r = 0;
            strand->tint.g = 16;
            strand->tint.b = 128;
            setVector(&strand->rotation, 0, 0, 0);
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = (i + 1) * 1024 / (work->config->count + 1) + 1024;
            } else {
                angle = 1024 - i * 1024 / (work->config->count + 1);
            }
            strand->offset.vx = ((s32)((u32)rcos(angle) << 8)) >> 12;
            strand->offset.vy = ((s32)((u32)rsin(angle) << 8)) >> 12;
            strand->offset.vz = 0;
        }
        for (n = 0; n < work->config->count; n++, sheet++) {
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
            sheet->outer.r = 192;
            sheet->outer.g = 192;
            sheet->outer.b = 192;
            sheet->inner.r = 0;
            sheet->inner.g = 64;
            sheet->inner.b = 192;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        for (i = 0; i < 3; i++, chain++) {
            for (k = 0; k < 9; k++) {
                chain->color[k].r = 255;
                chain->color[k].g = 255;
                chain->color[k].b = 128;
            }
            chain->stage = 0;
            chain->start = 0;
            chain->end = i * -16 / 3;
            chain->yaw = (i << 11) / 3;
            chain->pitch = (i << 11) / 3;
            chain->wave = (i << 11) / 3;
            chain->sway = (i << 12) / 3;
        }
        work->width = 1024;
        work->spin = 0;
        for (i = 0; i < work->config->count; i++) {
            work->length[i] = 0;
            work->reach[i] = 0;
            work->mode[i] = 0;
            work->strand_fade[i] = 0;
        }
        work->field_13B0 = 0;
        work->field_13B2 = 0;
        work->sway = 0;
        work->twist = 0;
        work->frame = 0;
        work->time = 0;
        work->last_frame = 0;
        work->step = 0;
        work->fade_span = work->config->shrink_start - work->config->sheet_start;
        work->shrink_span = work->config->shrink_end - work->config->shrink_start;
        work->phase = 0;
        work->fade = 0;
        work->ambient = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        for (angle = 0, i = 0; i < work->config->count; i++, angle = (i << 12) / work->config->count) {
            if (work->phase < 4) {
                GsGetLwUnit(work->parts[i], &work->transforms[i]);
                if (i == 0) {
                    GsGetLwUnit(work->parts[0], &work->matrix);
                    angle = 1024;
                } else if (i == 1) {
                    angle = 640;
                } else if (i == 2) {
                    angle = 1408;
                } else if (i == 3) {
                    angle = 256;
                } else {
                    angle = 1792;
                }
                angle += 2048;
                setVector(&work->positions[i], work->matrix.t[0] + ((rcos(angle) * 384) >> 12),
                          work->matrix.t[1] + ((rsin(angle) * 384) >> 12) - 128, work->matrix.t[2]);
                setVector(&work->directions[i],
                          work->positions[i].vx - work->transforms[i].t[0],
                          work->positions[i].vy - work->transforms[i].t[1],
                          work->positions[i].vz - work->transforms[i].t[2]);
            } else {
                GsGetLwUnit(work->extra, &work->transforms[i]);
                if (i == 0) {
                    GsGetLwUnit(work->extra, &work->matrix);
                }
                setVector(&work->positions[i],
                          work->matrix.t[0] + ((rcos(angle - -(work->spin + 2048)) * 768) >> 12),
                          work->matrix.t[1] + ((rsin(angle - -(work->spin + 2048)) * 768) >> 12),
                          work->matrix.t[2]);
                setVector(&work->directions[i],
                          work->positions[i].vx - work->transforms[i].t[0],
                          work->positions[i].vy - work->transforms[i].t[1],
                          work->positions[i].vz - work->transforms[i].t[2]);
            }
            GsSetLsMatrix(Model_GetLightSourceMatrix());
            setVector(&origin, work->transforms[i].t[0], work->transforms[i].t[1], work->transforms[i].t[2]);
            RotTransPers(&origin, &screen0, &depth, &flag);
            x0 = (s16)screen0;
            y0 = screen0 >> 16;
            setVector(&target, work->positions[i].vx, work->positions[i].vy, work->positions[i].vz);
            RotTransPers(&target, &screen1, &depth, &flag);
            work->dx[i] = (s16)screen1 - x0;
            work->dy[i] = (screen1 >> 16) - y0;
        }
        work->spin += 128;
        if (work->config->shrink_start <= work->time && work->phase == 3) {
            work->phase = 4;
        }
        if (work->phase > 0) {
            FrenchModelVariant403_DrawStrands(context);
        }
        FrenchModelVariant403_DrawSheets(context);
        if (work->phase >= 5) {
            FrenchModelVariant403_DrawChains(context);
        }
        work->frame++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->last_frame) {
            work->time += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->last_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 8) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if (work->phase >= 5 && work->phase < 7) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 8;
    } else if (work->phase == 8) {
        work->fade += work->step;
        if (work->fade >= 64) {
            work->fade = 64;
            work->phase = 9;
        }
    } else if (work->phase == 9) {
        result = 2;
    }
    return result;
}
