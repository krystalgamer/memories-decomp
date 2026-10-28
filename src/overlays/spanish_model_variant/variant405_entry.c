#include "../../types.h"
#include "variant405_entry.h"

#ifndef MODEL_VARIANT405_CLUT_X
#define MODEL_VARIANT405_CLUT_X 640
#endif

/* Header 405's loader entry: three rings, a trail angle per three trail
 * records, six 512-unit sheets, two sets of three 17-point curtains, four
 * spokes and two ribbons. The run path re-reads the opponent slot position
 * and part 0 until config word 0x1C, freezing the aim origin after word
 * 0x10, then draws through the outer-band, 0x12CC and pulse helpers. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant405EntryState *work = (Variant405EntryState *)point;
    Variant405EntryStack stack;
    Variant405EntryRing *ring = work->rings;
    Variant405EntrySheet *sheet = work->sheets;
    Variant405EntryCurtain *outer = work->outer_curtains;
    Variant405EntryCurtain *inner = work->inner_curtains;
    Variant405EntrySpoke *spoke = work->spokes;
    Variant405EntryRibbon *ribbon = work->ribbons;
    Variant405EntryTrail *trail;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *glyph;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    u16 flat_page;
    u16 tpage, clut;
    s32 packed;
    s32 n, i, j, k, v, angle, base, q;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant405EntryConfig *)(D_8013CFBC + 0x38) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        k = j = 0;
        glyph = work->glyphs;
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT405_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013CFBC + 0x1C));
        flat_page = packed >> 16;
        stack.texture_cluts[0] = packed;
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
        setUV4(flat, 31, 32, 31, 63, 0, 32, 0, 63);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 31, 64, 31, 95, 0, 64, 0, 95);
        flat->clut = stack.texture_cluts[0];
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        do {
            SetPolyGT4(glyph);
            glyph->tpage = flat_page;
            setUV4(glyph, 31, k, 31, k + 15, 0, k, 0, k + 15);
            glyph->clut = stack.texture_cluts[0];
            SetSemiTrans(glyph, 1);
            SetShadeTex(glyph, 0);
            j++;
            k = j * 16;
            glyph++;
        } while (j < 16);
        for (i = 0; i < 3; i++, ring++) {
            for (j = 0; j < 2; j++) {
                setVector(&ring->a[j], (u32)rcos(1024) >> 7, (u32)rsin(1024) >> 7, 0);
                setVector(&ring->b[j], 0, 0, 0);
                setVector(&ring->c[j], (u32)rcos(3072) >> 7, (u32)rsin(3072) >> 7, 0);
            }
            ring->field_49 = 32;
            ring->field_4A = 128;
            ring->field_4C = 4096;
            ring->field_50 = 4096;
            ring->field_54 = 4096;
            ring->field_48 = 0;
            ring->field_5C = 0;
            ring->field_5E = 0;
            ring->field_60 = 0;
            ring->field_62 = 0;
            ring->field_64 = 0;
            ring->field_68 = -512;
        }
        trail = work->trails;
        for (n = 0; n < 3; n++, trail++) {
            trail->angle = -(n * 4096 / 3);
        }
        for (n = 0; n < 6; n++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -512, -512, 0);
                    setVector(&sheet->v1[k], 0, -512, 0);
                    setVector(&sheet->v2[k], -512, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 512, -512, 0);
                    setVector(&sheet->v1[k], 512, 0, 0);
                    setVector(&sheet->v2[k], 0, -512, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 512, 512, 0);
                    setVector(&sheet->v1[k], 0, 512, 0);
                    setVector(&sheet->v2[k], 512, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -512, 512, 0);
                    setVector(&sheet->v1[k], -512, 0, 0);
                    setVector(&sheet->v2[k], 0, 512, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 64;
            sheet->inner[2] = 255;
            sheet->field_98 = 0;
            sheet->field_88 = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        for (n = 0; n < 3; n++, outer++) {
            for (j = 0, v = 0; j < 17; j++, v = j * 256) {
                setVector(&outer->inner[j], (u32)rcos(v) >> 4, (u32)rsin(v) >> 4, 0);
                setVector(&outer->outer[j], (u32)rcos(v) * 3 >> 5, (u32)rsin(v) * 3 >> 5, 0);
                outer->outer_color[0] = 255;
                outer->outer_color[1] = 255;
                outer->outer_color[2] = 255;
                outer->inner_color[0] = 128;
                outer->inner_color[1] = 192;
                outer->inner_color[2] = 255;
            }
            outer->angle = n * -12288 / 3;
            outer->field_11C = 0;
        }
        for (n = 0; n < 3; n++, inner++) {
            for (j = 0, v = 0; j < 17; j++, v = j * 256) {
                setVector(&inner->inner[j], (u32)rcos(v) >> 3, (u32)rsin(v) >> 3, 0);
                setVector(&inner->outer[j], (u32)rcos(v) >> 2, (u32)rsin(v) >> 2, 0);
                inner->outer_color[0] = 255;
                inner->outer_color[1] = 255;
                inner->outer_color[2] = 255;
                inner->inner_color[0] = 128;
                inner->inner_color[1] = 192;
                inner->inner_color[2] = 255;
            }
            inner->angle = 0;
            inner->field_11C = 0;
        }
        for (q = 0, base = 0; q < 4; q++, spoke++, base += 256) {
            for (n = 0, angle = base; n < 8; angle += 512, n++) {
                setVector(&spoke->inner[n], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&spoke->outer[n], (u32)rcos(angle) * 5 >> 6, (u32)rsin(angle) * 5 >> 6, 0);
            }
            spoke->outer_color[0] = 64;
            spoke->outer_color[1] = 128;
            spoke->outer_color[2] = 255;
            spoke->inner_color[0] = 0;
            spoke->inner_color[1] = 0;
            spoke->inner_color[2] = 0;
            spoke->angle = q * 256;
            spoke->field_8C = 0;
        }
        for (i = 0; i < 2; i++, ribbon++) {
            ribbon->field_50 = 4096;
            ribbon->field_54 = 4096;
            ribbon->field_58 = 4096;
            ribbon->outer[0] = 255;
            ribbon->outer[1] = 255;
            ribbon->outer[2] = 255;
            ribbon->inner[0] = 0;
            ribbon->inner[1] = 16;
            ribbon->inner[2] = 128;
        }
        work->field_3244 = 1024;
        work->field_323C = 0;
        work->field_3240 = 0;
        work->field_322C = 0;
        work->field_3248 = 0;
        work->field_324C = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_3258 = 0;
        work->fade = 0;
        work->tint = 2048;
        work->command = command;
    } else {
        SVECTOR position, target;
        VECTOR squared;
        Variant337EntryProjection projection;

        view = Model_GetCameraViewBuffer();
        setVector(&work->view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->frame < work->config->field_1C) {
            if (work->slot == 0) {
                Model_CopySlotPosition(1, (u16 *)&work->target);
            } else {
                Model_CopySlotPosition(0, (u16 *)&work->target);
            }
            GsGetLwUnit(work->parts[0], &work->matrix);
            if (work->frame <= work->config->field_10) {
                setVector(&work->origin, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
            }
            setVector(&work->direction,
                      work->target.vx - work->origin.vx,
                      work->target.vy - work->origin.vy,
                      work->target.vz - work->origin.vz);
        }
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
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        func_8013CB64((u8 *)point);
        func_8013C2CC((u8 *)point);
        func_8013BD00((u8 *)point);
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 5) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->config->field_10 <= work->frame) {
        if (work->phase == 0) {
            work->phase = 1;
        }
        work->field_322C = 1;
    }
    if (work->config->field_14 <= work->frame) {
        work->field_322C = 2;
    }
    if (work->config->field_18 <= work->frame) {
        work->field_322C = work->config->field_0C;
    }
    if (work->phase >= 2 && work->phase <= 3) {
        result = 4;
    } else if (work->phase == 4) {
        result = 1;
        work->phase = 5;
    } else if (work->phase == 5) {
        work->fade += work->step;
        if (work->fade >= 64) {
            work->fade = 64;
            work->phase = 6;
        }
    } else if (work->phase == 6) {
        result = 2;
    }
    return result;
}
