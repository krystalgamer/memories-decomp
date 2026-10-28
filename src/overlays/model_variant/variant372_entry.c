#include "../../types.h"
#include "variant372_entry.h"

/* Header 372's entry: eight ribbons fan out from a model joint while
 * sixteen sheets unfold, then the slot fades to its target ambient.
 * Retail copies two path strings into locals it never reads.
 *
 * The US build is GCC 2.7.2 (the `jr ra; nop` epilogue and the
 * local-allocated struct-copy scratches); the French header-389 build
 * of the same source is GCC 2.8.1 and only moves the CLUT column.
 * Matching quirks: the orbit loop reuses `angle` for its first phase,
 * the odd-ribbon angle is written from `n` so loop strength reduction
 * creates the spilled 1024 step, and the phase test is inverted so
 * `result = 4` comes first. */

#ifdef VERSION_FRENCH
#define MODEL372_CLUT_X 640
#else
#define MODEL372_CLUT_X 512
#endif

#define setCV(p, R, G, B) ((p)->r = (R), (p)->g = (G), (p)->b = (B))

s32 func_8013B004(u8 *context, s32 command)
{
    Model372State *work = (Model372State *)context;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    Model372Path23 test;
    Model372Path25 beamer;
    SVECTOR origin, target;
    VECTOR squared;
    PSXLONG screen0;
    PSXLONG depth, flag;
    PSXLONG screen1;
    Model372Ribbon *ribbon;
    Model372Sheet *sheet;
    POLY_GT4 *textured;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_GT4 *extra;
    POLY_FT4 *flat;
    s32 result;
    GsRVIEW2 *view;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 start;
    u16 flat_clut;
    s32 first;
    s32 n;
    s32 delay;
    s32 angle;
    s32 b;
    s16 x0;
    s32 y0;
    s32 frame;
    u16 tpage;
    u16 clut;
    s32 flat_page;

    ribbon = work->ribbons;
    sheet = work->sheets;
    textured = work->textured;
    flat = work->flat;
    triangle = &work->triangle;
    quad = &work->quad;
    extra = &work->extra;
    result = 0;
    test = D_8013CE10;
    beamer = D_8013CE28;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = D_8013CF40 + command;
        for (i = 0; i < 8; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->view,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        for (i = 0; i < 8; i++) {
            setVector(&work->positions[i], work->target.vx, work->target.vy, work->target.vz);
            GsGetLwUnit(work->parts[i], &work->transforms[i]);
            setVector(&work->directions[i],
                      work->positions[i].vx - work->transforms[i].t[0],
                      work->positions[i].vy - work->transforms[i].t[1],
                      work->positions[i].vz - work->transforms[i].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL372_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL372_CLUT_X, 244);
        first = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CE44[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CE44[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CE44[1]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CE44[2]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CE44[3]);
        flat_page = first >> 16;
        flat_clut = first;
        first = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CE44[0]);
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
        SetPolyGT4(extra);
        extra->tpage = first >> 16;
        setUV4(extra, 96, 128, 96, 255, 127, 128, 127, 255);
        extra->clut = first;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
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
        for (n = 0, delay = 0; n < 8; n++, ribbon++, delay -= 512) {
            start = delay;
            for (k = 0; k < 17; k++) {
                ribbon->progress[k] = start;
                if (k == 16) {
                    ribbon->tint[16].r = 0;
                    ribbon->tint[16].g = 0;
                    ribbon->tint[16].b = 0;
                    ribbon->color[16].r = 0;
                    ribbon->color[16].g = 0;
                    ribbon->color[16].b = 0;
                } else {
                    ribbon->tint[k].r = 224;
                    ribbon->tint[k].g = 240;
                    ribbon->tint[k].b = 255;
                    ribbon->color[k].r = 128;
                    ribbon->color[k].g = 128;
                    ribbon->color[k].b = 128;
                }
                start -= 48;
            }
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->rotation_x = 0;
            ribbon->rotation_z = 0;
            ribbon->rotation_y = 0;
            ribbon->done = 0;
            if (n == 0) {
                angle = 1024;
            } else if (n % 2 == 1) {
                angle = (n + 1) * 1024 / 9 + 1024;
            } else {
                angle = 1024 - n * 1024 / 9;
            }
            ribbon->offset.vx = (rcos(angle) << 8) >> 12;
            ribbon->offset.vy = (rsin(angle) << 8) >> 12;
            ribbon->offset.vz = 0;
        }
        for (j = 0; j < 16; j++, sheet++) {
            for (i = 0; i < 4; i++) {
                if (i == 0) {
                    setVector(&sheet->v0[i], -128, -128, 0);
                    setVector(&sheet->v1[i], 0, -128, 0);
                    setVector(&sheet->v2[i], -128, 0, 0);
                    setVector(&sheet->v3[i], 0, 0, 0);
                } else if (i == 1) {
                    setVector(&sheet->v0[i], 128, -128, 0);
                    setVector(&sheet->v1[i], 128, 0, 0);
                    setVector(&sheet->v2[i], 0, -128, 0);
                    setVector(&sheet->v3[i], 0, 0, 0);
                } else if (i == 2) {
                    setVector(&sheet->v0[i], 128, 128, 0);
                    setVector(&sheet->v1[i], 0, 128, 0);
                    setVector(&sheet->v2[i], 128, 0, 0);
                    setVector(&sheet->v3[i], 0, 0, 0);
                } else if (i == 3) {
                    setVector(&sheet->v0[i], -128, 128, 0);
                    setVector(&sheet->v1[i], -128, 0, 0);
                    setVector(&sheet->v2[i], 0, 128, 0);
                    setVector(&sheet->v3[i], 0, 0, 0);
                }
            }
            sheet->outer.r = 255;
            sheet->outer.g = 255;
            sheet->outer.b = 255;
            sheet->inner.r = 0;
            sheet->inner.g = 64;
            sheet->inner.b = 255;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
            sheet->shown = 0;
        }
        work->spin.pad = 1024;
        setVector(&work->spin, 0, 0, 0);
        work->angle_a = 0;
        work->angle_b = 0;
        work->frame = 0;
        work->time = 0;
        work->last_frame = 0;
        work->step = 0;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
        if (work->slot == 0) {
            work->matrix.t[0] += work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] += work->config->offset[2];
        } else {
            work->matrix.t[0] -= work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] -= work->config->offset[2];
        }
        setVector(&work->view,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        for (i = 0, angle = 0, b = 0; i < 8; i++, angle += 800, b += 1300) {
            if (work->config->paired == 1 && i != 0) {
                setVector(&work->positions[i], work->target.vx + ((rcos(angle + b) << 7) >> 12),
                          work->target.vy + ((rsin(angle) << 7) >> 12), work->target.vz + ((rsin(b) << 7) >> 12));
            } else {
                setVector(&work->positions[i], work->target.vx, work->target.vy, work->target.vz);
            }
            GsGetLwUnit(work->parts[i], &work->transforms[i]);
            if (work->slot == 0) {
                work->transforms[i].t[0] += work->config->offset[0];
                work->transforms[i].t[1] += work->config->offset[1];
                work->transforms[i].t[2] += work->config->offset[2];
            } else {
                work->transforms[i].t[0] -= work->config->offset[0];
                work->transforms[i].t[1] += work->config->offset[1];
                work->transforms[i].t[2] -= work->config->offset[2];
            }
            setVector(&work->directions[i],
                      work->positions[i].vx - work->transforms[i].t[0],
                      work->positions[i].vy - work->transforms[i].t[1],
                      work->positions[i].vz - work->transforms[i].t[2]);
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&origin, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&origin, &screen0, &depth, &flag);
        x0 = (s16)screen0;
        y0 = screen0 >> 16;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &screen1, &depth, &flag);
        work->screen_delta.vx = (s16)screen1 - x0;
        work->screen_delta.vy = (screen1 >> 16) - y0;
        if (work->config->ribbon_start <= work->time) {
            func_8013BE68(context);
        }
        if (work->config->sheet_start <= work->time) {
            func_8013C828(context);
        }
        work->frame++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->last_frame) {
            work->time += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->last_frame = frame;
        }
    }
    setVector(&work->camera, -512, -256, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->camera, 20);
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
    if (work->phase >= 2 && work->phase < 5) {
        result = 4;
    } else {
        if (work->phase == 5) {
            result = 1;
            work->phase = 6;
        } else if (work->phase == 6) {
            if (work->fade < 64) {
                work->fade += work->step;
                if (work->fade >= 64) {
                    work->fade = 64;
                    work->phase = 7;
                }
            }
        } else if (work->phase == 7) {
            result = 2;
        }
    }
    return result;
}
