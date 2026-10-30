#include "../../types.h"
#include "variant479_entry.h"

#ifndef MODEL_VARIANT479_CLUT_X
#define MODEL_VARIANT479_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant479EntryState *work = (Variant479EntryState *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR unused_position, unused_target;
    VECTOR squared;
    ModelVariantCurtain *curtain = work->curtains;
    Grid379Sheet *grid = work->grids;
    Variant479Rib *rib = work->ribs;
    ModelVariantSheet *sheet = work->sheets;
    Variant458Streamer *streamer = work->streamers;
    Variant479SpiralArm *arm = work->arms;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *frame = &work->frame_poly;
    POLY_GT4 *glow = &work->glow;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_FT4 *flat = work->flat_textured;
    POLY_GT4 *strip = &work->strip;
    s32 result = 0;
    u16 frame_texture, strip_tpage, strip_clut;
    s32 packed, outer, frame_page, y;
    s32 i, j, k, row, angle, radius;
    u16 tpage, clut, flat_texture;
    s32 flat_page;
    GsRVIEW2 *view;
    u32 animation_frame;
    s8 shade;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant479EntryConfig *)(D_8013ED20 + 0xFC) + command;
        for (i = 0; i < 3; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        for (i = 0; i < 3; i++) {
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i],
                      work->target.vx - work->matrices[i].t[0],
                      work->target.vy - work->matrices[i].t[1],
                      work->target.vz - work->matrices[i].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT479_CLUT_X, 244);
        strip_tpage = GetTPage(1, 1, 896, 0);
        strip_clut = GetClut(MODEL_VARIANT479_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013ED20 + 0xC4));
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013ED20 + 0x8C));
        frame_page = packed >> 16;
        frame_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013ED20 + 0x1C));
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
        flat->clut = flat_texture;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 0, 176, 0, 223, 48, 176, 48, 223);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        SetPolyGT4(frame);
        frame->tpage = frame_page;
        setUV4(frame, 192, 64, 255, 64, 192, 127, 255, 127);
        frame->clut = frame_texture;
        SetSemiTrans(frame, 1);
        SetShadeTex(frame, 0);
        SetPolyGT4(strip);
        strip->tpage = strip_tpage;
        setUV4(strip, 0, 128, 0, 255, 31, 128, 31, 255);
        strip->clut = strip_clut;
        SetSemiTrans(strip, 1);
        SetShadeTex(strip, 0);
        SetPolyGT4(glow);
        glow->tpage = packed >> 16;
        setUV4(glow, 128, 207, 255, 207, 128, 128, 255, 128);
        glow->clut = packed;
        SetSemiTrans(glow, 1);
        SetShadeTex(glow, 0);
        for (outer = 0; outer < 5; outer++, grid++) {
            for (row = 0, angle = 0; row < 9; row++, angle = row * 1300 / 8) {
                radius = rsin(angle) * 256 >> 12;
                y = rcos(angle) * 256 >> 12;
                for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                    setVector(&grid->points[row][j], rcos(angle) * radius >> 12, -y,
                              rsin(angle) * radius >> 12);
                }
                grid->colors[row][0] = 0;
                grid->colors[row][1] = 255;
                grid->colors[row][2] = 0;
            }
            grid->size = 0;
            MODEL_VARIANT_WORD(grid, 0x4F0) = 1024;
            grid->done = 0;
        }
        for (k = 0; k < 12; k++, arm++) {
            for (j = 0; j < 3; j++) {
                if (j == 2) {
                    arm->ca[j][0] = 0;
                    arm->ca[j][1] = 0;
                    arm->ca[j][2] = 0;
                    arm->cb[j][0] = 0;
                    arm->cb[j][1] = 0;
                    arm->cb[j][2] = 0;
                } else {
                    arm->ca[j][0] = 0;
                    arm->ca[j][1] = 255;
                    arm->ca[j][2] = 32;
                    arm->cb[j][0] = 128;
                    arm->cb[j][1] = 128;
                    arm->cb[j][2] = 128;
                }
            }
        }
        for (k = 0; k < 5; k++, rib++) {
            rib->scale[0] = 4096;
            rib->scale[1] = 4096;
            rib->scale[2] = 4096;
            for (j = 0, shade = 0; j < 9; shade += 24, j++) {
                rib->ca[j][0] = shade;
                rib->ca[j][1] = shade;
                rib->ca[j][2] = shade;
                rib->cb[j][0] = 0;
                rib->cb[j][1] = shade;
                rib->cb[j][2] = 0;
            }
            rib->field_178 = 0;
            rib->field_17C = 0;
            rib->field_17A = 0;
            if (k == 0) {
                angle = 1024;
            } else if (k / 2 * 2 == k - 1) {
                angle = (k + 1) * 1024 / 6 + 1024;
            } else {
                angle = 1024 - (k << 10) / 6;
            }
            rib->direction[0] = rcos(angle) * 256 >> 12;
            rib->direction[1] = rsin(angle) * 256 >> 12;
            rib->direction[2] = 0;
            rib->size = -(k << 9);
            rib->fade = 0;
            rib->state = 0;
        }
        for (outer = 0; outer < 8; outer++, sheet++) {
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
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[1] = 255;
            sheet->inner[0] = 0;
            sheet->inner[2] = 64;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
        }
        for (outer = 0; outer < 5; outer++, curtain++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&curtain->a[j], (u32)rcos(angle) >> 4, 0, (u32)rsin(angle) >> 4);
                setVector(&curtain->b[j], (u32)(rcos(angle) * 3) >> 5, -64,
                          (u32)(rsin(angle) * 3) >> 5);
            }
            if (outer == 5) {
                curtain->scale = 0;
            } else {
                curtain->scale = -((outer << 12) / 5);
            }
            curtain->count = 0;
        }
        for (k = 0; k < 2; k++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j][0] = 128;
                streamer->color[j][1] = 128;
                streamer->color[j][2] = 128;
            }
            MODEL_VARIANT_WORD(streamer, 0x2A8) = 0;
        }
        work->color_a[0] = work->config->color[0];
        work->color_a[1] = work->config->color[1];
        work->color_a[2] = work->config->color[2];
        work->color_b[1] = 255;
        work->color_b[0] = 0;
        work->color_b[2] = 0;
        work->field_41D8 = 0;
        work->field_4198 = 0;
        work->field_419C = 0;
        work->field_41A0 = 0;
        work->field_41A4 = 0;
        work->field_41A8 = 1024;
        work->field_41AC = 0;
        work->field_41B0 = 0;
        work->field_41B8 = 0;
        work->field_41BA = 0;
        work->field_41BC = 0;
        work->field_41BE = 0;
        work->field_4190 = 1024;
        work->field_41B4 = 0;
        work->field_4194 = 1024;
        work->field_418C = 0;
        work->field_418E = 0;
        work->field_41DC = 0;
        work->field_41E0 = 0;
        work->field_41E4 = 0;
        work->field_41E8 = 0;
        work->field_41C0 = 1024;
        work->field_41C4 = 0;
        work->field_41C8 = 0;
        work->field_41CC = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_41F0 = 0;
        work->field_41F4 = 0;
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
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        work->target.vy = 0;
        for (i = 0; i < 3; i++) {
            GsGetLwUnit(work->parts[i], &work->matrices[i]);
            setVector(&work->directions[i],
                      work->target.vx - work->matrices[i].t[0],
                      work->target.vy - work->matrices[i].t[1],
                      work->target.vz - work->matrices[i].t[2]);
        }
        for (k = 0, angle = -256; k < 5; k++, angle = k * 2560 / 3 - 256) {
            if (k + 1 == 5) {
                setVector(&work->points[k], work->target.vx, work->target.vy, work->target.vz);
            } else if (work->slot == 0) {
                setVector(&work->points[k], work->target.vx + (rcos(angle + 2048) * 512 >> 12),
                          work->target.vy, work->target.vz + (rsin(angle + 2048) * 512 >> 12));
            } else {
                setVector(&work->points[k], work->target.vx + (rcos(angle) * 512 >> 12),
                          work->target.vy, work->target.vz + (rsin(angle) * 512 >> 12));
            }
            setVector(&work->deltas[k],
                      work->points[k].vx - work->matrices[k % 3].t[0],
                      work->points[k].vy - work->matrices[k % 3].t[1],
                      work->points[k].vz - work->matrices[k % 3].t[2]);
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        if (work->phase >= 3 && work->phase <= 5) {
            func_8013E860((u8 *)point);
            func_8013BFD0((u8 *)point);
        }
        if (work->phase >= 2 && work->phase <= 4) {
            func_8013CF68((u8 *)point);
        }
        if (work->phase >= 1 && work->phase <= 4) {
            func_8013C4F8((u8 *)point);
        }
        if (work->config->start <= work->frame && work->phase < 5) {
            func_8013DB90((u8 *)point);
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
    if (work->phase >= 3 && work->phase <= 4) {
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
