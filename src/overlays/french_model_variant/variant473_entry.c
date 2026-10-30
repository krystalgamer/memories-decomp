#include "../../types.h"
#include "variant473_entry.h"

#ifndef MODEL_VARIANT473_CLUT_X
#define MODEL_VARIANT473_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState473 *work = (EntryState473 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection473 projection;
    Funnel473 *funnel = work->funnels;
    Grid385Sheet *mesh = work->meshes;
    EntryRibbon473 *ribbon = work->ribbons;
    ModelVariantSheet *sheet = work->sheets;
    Variant458Streamer *streamer = work->streamers;
    EntrySpiral473 *spiral = work->spirals;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *funnel_packet = &work->funnel_packet;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *additional = work->additional;
    s32 result = 0;
    u16 funnel_clut, extra_clut, flat_clut, tpage, clut;
    s32 packed, k, row, extra_page, funnel_page, flat_page;
    s32 i, j, sheet_point, angle, radius, height, base;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig473 *)(D_8013ECE4 + 0xFC) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT473_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT473_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ECE4 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ECE4 + 0x8C));
        extra_page = packed >> 16;
        extra_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ECE4 + 0x1C));
        funnel_page = packed >> 16;
        funnel_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ECE4 + 0xE0));
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
        SetPolyFT4(additional);
        additional->tpage = packed >> 16;
        setUV4(additional, 0, 224, 31, 224, 0, 255, 31, 255);
        additional->clut = packed;
        SetSemiTrans(additional, 1);
        SetShadeTex(additional, 1);
        additional++;
        SetPolyFT4(additional);
        additional->tpage = packed >> 16;
        setUV4(additional, 32, 224, 63, 224, 32, 255, 63, 255);
        additional->clut = packed;
        SetSemiTrans(additional, 1);
        SetShadeTex(additional, 1);
        SetPolyGT4(extra);
        extra->tpage = extra_page;
        setUV4(extra, 192, 64, 255, 64, 192, 127, 255, 127);
        extra->clut = extra_clut;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(funnel_packet);
        funnel_packet->tpage = funnel_page;
        setUV4(funnel_packet, 128, 207, 255, 207, 128, 128, 255, 128);
        funnel_packet->clut = funnel_clut;
        SetSemiTrans(funnel_packet, 1);
        SetShadeTex(funnel_packet, 0);
        for (k = 0; k < 5; k++, mesh++) {
            for (row = 0, angle = 0, base = 0; row < 9;
                 row++, angle = (base << 2) / 8) {
                radius = rsin(angle) * 256 >> 12;
                height = rcos(angle) * 256 >> 12;
                for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                    setVector(&mesh->points[row][j], rcos(angle) * radius >> 12,
                        -height, rsin(angle) * radius >> 12);
                }
                base += 325;
                mesh->colors[row][0] = row * 20;
                mesh->colors[row][1] = row * 20;
                mesh->colors[row][2] = 192;
            }
            mesh->size = 0;
            MODEL_VARIANT_WORD(mesh, 0x4F0) = 1024;
            mesh->done = 0;
        }
        for (i = 0; i < 12; i++, spiral++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    spiral->outer[j].r = 0;
                    spiral->outer[j].g = 0;
                    spiral->outer[j].b = 0;
                    spiral->inner[j].r = 0;
                    spiral->inner[j].g = 0;
                    spiral->inner[j].b = 0;
                } else {
                    spiral->outer[j].r = 255;
                    spiral->outer[j].g = 0;
                    spiral->outer[j].b = 255;
                    spiral->inner[j].r = 128;
                    spiral->inner[j].g = 128;
                    spiral->inner[j].b = 128;
                }
            }
        }
        for (i = 0; i < 5; i++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            for (j = 0; j < 9; j++) {
                ribbon->inner[j].r = j * 24;
                ribbon->inner[j].g = j * 24;
                ribbon->inner[j].b = j * 24;
                ribbon->outer[j].r = 0;
                ribbon->outer[j].g = 0;
                ribbon->outer[j].b = j * 16;
            }
            ribbon->field_178 = 0;
            ribbon->field_17C = 0;
            ribbon->field_17A = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i / 2 * 2 == i - 1) {
                angle = 1024 + (i + 1) * 1024 / 6;
            } else {
                angle = 1024 - i * 1024 / 6;
            }
            setVector(&ribbon->position, rcos(angle) * 256 >> 12,
                rsin(angle) * 256 >> 12, 0);
            ribbon->progress = -(i * 512);
            ribbon->field_184 = 0;
            ribbon->field_188 = 0;
        }
        for (k = 0; k < 6; k++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->v0[sheet_point], -128, -128, 0);
                    setVector(&sheet->v1[sheet_point], 0, -128, 0);
                    setVector(&sheet->v2[sheet_point], -128, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->v0[sheet_point], 128, -128, 0);
                    setVector(&sheet->v1[sheet_point], 128, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, -128, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->v0[sheet_point], 128, 128, 0);
                    setVector(&sheet->v1[sheet_point], 0, 128, 0);
                    setVector(&sheet->v2[sheet_point], 128, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->v0[sheet_point], -128, 128, 0);
                    setVector(&sheet->v1[sheet_point], -128, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, 128, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
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
        for (k = 0; k < 5; k++, funnel++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&funnel->inner[j], (u32)rcos(angle) >> 4, 0,
                    (u32)rsin(angle) >> 4);
                setVector(&funnel->outer[j], (u32)(rcos(angle) * 5) >> 6, -64,
                    (u32)(rsin(angle) * 5) >> 6);
            }
            if (k == 5) {
                funnel->size = 0;
            } else {
                funnel->size = -(k * 4096 / 5);
            }
            MODEL_VARIANT_WORD(funnel, 0x114) = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j][0] = 128;
                streamer->color[j][1] = 128;
                streamer->color[j][2] = 128;
            }
            MODEL_VARIANT_WORD(streamer, 0x2A8) = 0;
        }
        work->inner.r = work->config->inner.r;
        work->inner.g = work->config->inner.g;
        work->inner.b = work->config->inner.b;
        work->outer.r = work->config->outer.r;
        work->outer.g = work->config->outer.g;
        work->outer.b = work->config->outer.b;
        work->field_3C48 = 0;
        work->size = 0;
        work->field_3C0C = 0;
        work->field_3C10 = 0;
        work->field_3C14 = 0;
        work->intensity = 1024;
        work->field_3C1C = 0;
        work->field_3C20 = 0;
        work->field_3C28 = 0;
        work->field_3C2A = 0;
        work->field_3C2C = 0;
        work->field_3C2E = 0;
        work->width = 1024;
        work->progress = 0;
        work->field_3C04 = 1024;
        work->field_3BFC = 0;
        work->field_3BFE = 0;
        work->field_3C4C = 0;
        work->field_3C50 = 0;
        work->field_3C54 = 0;
        work->field_3C58 = 0;
        work->field_3C30 = 1024;
        work->field_3C34 = 0;
        work->field_3C38 = 0;
        work->field_3C3C = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->field_3C60 = 0;
        work->field_3C64 = 0;
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
            Model_CopySlotU16Values(1, (u16 *)&work->target);
            work->target.vy = 0;
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
            work->target.vy = 0;
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        for (i = 0, angle = -256; i < 5; i++, angle = i * 2560 / 3 - 256) {
            if (i + 1 == 5) {
                setVector(&work->origins[i], work->target.vx, work->target.vy, work->target.vz);
            } else if (work->slot == 0) {
                setVector(&work->origins[i],
                    work->target.vx + (rcos(angle + 2048) * 512 >> 12),
                    work->target.vy,
                    work->target.vz + (rsin(angle + 2048) * 512 >> 12));
            } else {
                setVector(&work->origins[i],
                    work->target.vx + (rcos(angle) * 512 >> 12),
                    work->target.vy,
                    work->target.vz + (rsin(angle) * 512 >> 12));
            }
            setVector(&work->ribbon_directions[i],
                work->origins[i].vx - work->matrix.t[0],
                work->origins[i].vy - work->matrix.t[1],
                work->origins[i].vz - work->matrix.t[2]);
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
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if ((u32)(work->phase - 3) < 3) {
            func_8013E86C((u8 *)point);
            func_8013C04C((u8 *)point);
        }
        if ((u32)(work->phase - 2) < 3) {
            func_8013CFB0((u8 *)point);
        }
        if ((u32)(work->phase - 1) < 4) {
            func_8013C5AC((u8 *)point);
        }
        if (work->config->draw_start <= work->elapsed && work->phase < 5) {
            func_8013DBE8((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 7) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if ((u32)(work->phase - 3) < 2) {
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
