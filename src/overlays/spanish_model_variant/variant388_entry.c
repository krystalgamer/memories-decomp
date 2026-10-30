#include "../../types.h"
#include "variant388_entry.h"

#ifndef MODEL_VARIANT388_CLUT_X
#define MODEL_VARIANT388_CLUT_X 640
#endif

#define SETC(c, R, G, B) ((c).r = (R), (c).g = (G), (c).b = (B))

/* Header-388 loader entry. A non-negative command builds the tube, arms,
 * sheets, feedback rings and spokes; a negative command tracks the slot
 * parts and runs the strips, ribbons, sheets, petals and feedback helpers.
 * The screen words are read as scalar longs so the index store can be
 * scheduled ahead of their loads. */
s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant388EntryState *work = (Variant388EntryState *)point;
    u8 unknown_stack[64];
    SVECTOR position, target;
    VECTOR squared;
    PSXLONG projected, interpolation, flag, target_sxy;
    Variant388EntryTube *tube = work->tubes;
    Variant388EntrySheet *sheet = work->sheets;
    Variant388EntryFeedback *feedback = work->feedback;
    Variant388EntrySpoke *spoke = work->spokes;
    Variant388EntryArm *arm = work->arms;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *plain = &work->plain;
    POLY_GT4 *paged = &work->paged;
    s32 result = 0;
    u16 tpage, clut;
    u16 flat_clut, paged_clut;
    s32 packed, flat_page, paged_page;
    s32 i, j, k, n, angle, base;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->frame_count = 0;
        work->config = (Variant388EntryConfig *)(D_8013DE7C + 0xFC) + command;
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
        clut = GetClut(MODEL_VARIANT388_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT388_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DE7C + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DE7C + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DE7C + 0x1C));
        paged_page = packed >> 16;
        paged_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DE7C + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DE7C + 0x54));
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
        SetPolyGT4(plain);
        SetSemiTrans(plain, 1);
        SetShadeTex(plain, 0);
        SetPolyGT4(paged);
        paged->tpage = paged_page;
        setUV4(paged, 128, 207, 255, 207, 128, 128, 255, 128);
        paged->clut = paged_clut;
        SetSemiTrans(paged, 1);
        SetShadeTex(paged, 0);
        for (i = 0; i < 1; i++, tube++) {
            tube->scale[0] = 4096;
            tube->scale[1] = 4096;
            tube->scale[2] = 4096;
            tube->color.r = 255;
            tube->color.g = 0;
            tube->color.b = 255;
            tube->field_C8 = 0;
            tube->field_CC = 0;
            tube->field_CA = 0;
        }
        for (i = 0; i < 12; i++, arm++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    SETC(arm->ca[j], 0, 0, 0);
                    SETC(arm->cb[j], 0, 0, 0);
                } else {
                    SETC(arm->ca[j], 255, 0, 255);
                    SETC(arm->cb[j], 128, 128, 128);
                }
            }
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
            SETC(sheet->inner, 255, 255, 255);
            SETC(sheet->outer, 255, 0, 255);
            sheet->field_88 = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        for (n = 0; n < 5; n++, feedback++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&feedback->points[0][j], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&feedback->points[1][j], (u32)(rcos(angle) * 3) >> 5,
                          (u32)(rsin(angle) * 3) >> 5, -64);
                setVector(&feedback->points[2][j], (u32)(rcos(angle) * 7) >> 6,
                          (u32)(rsin(angle) * 7) >> 6, -64);
                SETC(feedback->inner, 255, 255, 255);
                SETC(feedback->outer, 255, 0, 255);
            }
            feedback->progress = -(n * 2048 / 5);
            feedback->field_1A4 = 0;
        }
        for (j = 0, base = 0; j < 4; j++, spoke++, base += 256) {
            for (n = 0, angle = base; n < 8; angle += 512, n++) {
                setVector(&spoke->inner[n], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&spoke->outer[n], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
            }
            SETC(spoke->color, 64, 128, 255);
            SETC(spoke->shade, 0, 0, 0);
            spoke->angle = j << 8;
            spoke->field_8C = 0;
        }
        work->field_1892 = 1024;
        work->field_1884 = 0;
        work->field_1886 = 0;
        work->field_1888 = 0;
        work->field_188A = 0;
        work->field_1890 = 0;
        work->field_1894 = 0;
        work->field_188C = 0;
        work->field_188E = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_189C = 0;
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
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        if (work->frame < work->config->track_end) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, &projected, &interpolation, &flag);
        screen_x = (u16)projected;
        screen_y = (s16)(projected >> 16);
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &target_sxy, &interpolation, &flag);
        dx = (u16)target_sxy - screen_x;
        dy = (s16)(target_sxy >> 16) - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        for (work->index = 0; work->index < work->config->count; work->index++) {
            if (work->frame < work->config->track_end) {
                if (work->index == 0) {
                    GsGetLwUnit(work->parts[0], &work->matrix);
                } else if (work->index == 1) {
                    GsGetLwUnit(work->parts[1], &work->matrix);
                } else if (work->index == 2) {
                    GsGetLwUnit(work->parts[2], &work->matrix);
                }
            }
            work->direction.vx = work->target.vx - work->matrix.t[0];
            work->direction.vy = work->target.vy - work->matrix.t[1];
            work->direction.vz = work->target.vz - work->matrix.t[2];
            func_8013D410((u8 *)point);
            if (work->config->sheet_start <= work->frame) {
                func_8013C9A4((u8 *)point);
            }
            if (work->phase > 0) {
                func_8013C344((u8 *)point);
                func_8013BCD4((u8 *)point);
            }
        }
        if (work->phase >= 2) {
            func_8013CE9C((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
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
    if (work->phase >= 2 && work->phase <= 5) {
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
