#include "../../types.h"
#include "variant425_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState425 *work = (EntryState425 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection425 projection;
    Mesh425Rows *mesh = &work->mesh;
    Funnel423 *funnel = work->funnels;
    EntryRibbon425 *ribbon = work->ribbons;
    ModelVariantSheet *sheet = work->sheets;
    Variant425Streamer *streamer = work->streamers;
    EntrySpiral425 *spiral = work->spirals;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *funnel_packet = &work->funnel_packet;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_GT4 *special = &work->special;
    s32 result = 0;
    u16 extra_clut, flat_clut, other_tpage, other_clut, tpage, clut;
    s32 packed, row, flat_page, extra_page, base, red;
    s32 i, j, k, sheet_point, angle, longitude, radius, height;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y;
    s16 target_z;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig425 *)(D_8013E908 + 0xFC) + command;
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
        clut = GetClut(640, 244);
        other_tpage = GetTPage(1, 1, 896, 0);
        other_clut = GetClut(640, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E908 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E908 + 0x8C));
        extra_page = packed >> 16;
        extra_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E908 + 0x1C));
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
        SetPolyGT4(extra);
        extra->tpage = extra_page;
        setUV4(extra, 192, 64, 255, 64, 192, 127, 255, 127);
        extra->clut = extra_clut;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(special);
        special->tpage = other_tpage;
        setUV4(special, 0, 128, 0, 255, 31, 128, 31, 255);
        special->clut = other_clut;
        SetSemiTrans(special, 1);
        SetShadeTex(special, 0);
        SetPolyGT4(funnel_packet);
        funnel_packet->tpage = packed >> 16;
        setUV4(funnel_packet, 128, 207, 255, 207, 128, 128, 255, 128);
        funnel_packet->clut = packed;
        SetSemiTrans(funnel_packet, 1);
        SetShadeTex(funnel_packet, 0);
        for (row = 0, angle = 0, base = 0, red = 0; row < 9;
             row++, angle = (base << 2) / 8) {
            radius = rsin(angle) * 512 >> 12;
            height = (rcos(angle) * 512 >> 12) + (rsin(276) * 512 >> 12);
            for (j = 0, longitude = 0; j < 17; j++, longitude = j * 256) {
                setVector(&mesh->rows[row][j], rcos(longitude) * radius >> 12,
                    -height, rsin(longitude) * radius >> 12);
            }
            base += 325;
            mesh->colors[row][0] = red;
            mesh->colors[row][1] = red;
            red += 20;
            mesh->colors[row][2] = 192;
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
                    spiral->outer[j].r = 0;
                    spiral->outer[j].g = 0;
                    spiral->outer[j].b = 128;
                    spiral->inner[j].r = 128;
                    spiral->inner[j].g = 128;
                    spiral->inner[j].b = 192;
                }
            }
        }
        for (i = 0; i < 1; i++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->colors[0].r = 128;
            ribbon->colors[0].g = 128;
            ribbon->colors[0].b = 128;
            ribbon->colors[1].r = 0;
            ribbon->colors[1].g = 16;
            ribbon->colors[1].b = 128;
            ribbon->field_238 = 0;
            ribbon->field_23C = 0;
            ribbon->field_23A = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i / 2 * 2 == i - 1) {
                angle = 1024 + (i + 1) * 512;
            } else {
                angle = 1024 - i * 512;
            }
            setVector(&ribbon->position, rcos(angle) * 256 >> 12,
                rsin(angle) * 256 >> 12, 0);
        }
        for (k = 0; k < 2; k++, sheet++) {
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
                setVector(&funnel->outer[j], (u32)(rcos(angle) * 9) >> 7, -512,
                    (u32)(rsin(angle) * 9) >> 7);
            }
            if (k == 4) {
                funnel->size = 0;
            } else {
                funnel->size = -(k * 8192 / 3);
            }
            MODEL_VARIANT_WORD(funnel, 0x114) = 0;
        }
        for (i = 0; i < 1; i++, streamer++) {
            for (j = 0; j < 2; j++) {
                streamer->color[j][0] = 128;
                streamer->color[j][1] = 128;
                streamer->color[j][2] = 128;
            }
            MODEL_VARIANT_WORD(streamer, 0x50) = 0;
        }
        work->inner.r = work->config->inner.r;
        work->inner.g = work->config->inner.g;
        work->inner.b = work->config->inner.b;
        work->outer.r = work->config->outer.r;
        work->outer.g = work->config->outer.g;
        work->outer.b = work->config->outer.b;
        work->field_17F8 = 0;
        work->size = 0;
        work->field_17B8 = 0;
        work->field_17BC = 0;
        work->field_17C0 = 0;
        work->intensity = 1024;
        work->field_17C8 = 0;
        work->field_17CC = 0;
        work->field_17D8 = 0;
        work->field_17DA = 0;
        work->field_17DC = 0;
        work->field_17DE = 0;
        work->width = 1024;
        work->field_17B0 = 1024;
        work->field_17A8 = 0;
        work->field_17AA = 0;
        work->field_17FC = 0;
        work->field_1800 = 0;
        work->field_1804 = 0;
        work->field_1808 = 0;
        work->field_17E0 = 1024;
        work->travel = 0;
        work->field_17E4 = 0;
        work->field_17E8 = 0;
        work->field_17EC = 0;
        work->progress = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->field_1810 = 0;
        work->field_1814 = 0;
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
            target_z = work->target.vz + 1400;
            work->target.vz = target_z - work->travel * 1400 / 1024;
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
            work->target.vy = 0;
            target_z = work->target.vz - 1400;
            work->target.vz = target_z + work->travel * 1400 / 1024;
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
            &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &projection.target,
            &projection.interpolation, &projection.flag);
        work->screen_delta.vx = (u16)projection.target - screen_x;
        work->screen_delta.vy = (projection.target >> 16) - screen_y;
        if (work->config->draw_start <= work->elapsed) {
            func_8013D6DC((u8 *)point);
        }
        if (work->phase >= 3) {
            func_8013E360((u8 *)point);
            func_8013BEE4((u8 *)point);
        }
        if (work->phase >= 2) {
            func_8013CDA4((u8 *)point);
        }
        if (work->phase > 0) {
            func_8013C370((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
        if (work->config->travel_start <= work->elapsed) {
            if (work->travel < 1024) {
                work->travel = ((work->elapsed - work->config->travel_start) << 10) /
                    (work->config->travel_end - work->config->travel_start);
                if (work->travel >= 1024) {
                    work->travel = 1024;
                }
            }
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
