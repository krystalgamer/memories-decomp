#include "../../types.h"
#include "variant427_entry.h"

#ifndef MODEL_VARIANT427_CLUT_X
#define MODEL_VARIANT427_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState427 *work = (EntryState427 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection427 projection;
    ModelVariantCurtain *curtain = work->curtains;
    Mesh427 *mesh = work->meshes;
    EntryRibbon427 *ribbon = work->ribbons;
    ModelVariantSheet *sheet = work->sheets;
    Streamer427 *streamer = work->streamers;
    EntrySpiral427 *spiral = work->spirals;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *curtain_packet = &work->curtain_packet;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_GT4 *special = &work->special;
    s32 result = 0;
    u16 extra_clut, other_tpage, other_clut, flat_clut, tpage, clut;
    s32 packed, k, row, extra_page, flat_page;
    s32 i, j, index, angle, radius, height, base;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig427 *)(D_8013EACC + 0xFC) + command;
        for (index = 0; index < 3; index++) {
            work->parts[index] = Model_GetSlotDataEntry(work->slot, work->config->parts[index]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        for (index = 0; index < 3; index++) {
            GsGetLwUnit(work->parts[index], &work->matrices[index]);
            setVector(&work->directions[index],
                work->target.vx - work->matrices[index].t[0],
                work->target.vy - work->matrices[index].t[1],
                work->target.vz - work->matrices[index].t[2]);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT427_CLUT_X, 244);
        other_tpage = GetTPage(1, 1, 896, 0);
        other_clut = GetClut(MODEL_VARIANT427_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013EACC + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013EACC + 0x8C));
        extra_page = packed >> 16;
        extra_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013EACC + 0x1C));
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
        SetPolyGT4(curtain_packet);
        curtain_packet->tpage = packed >> 16;
        setUV4(curtain_packet, 128, 207, 255, 207, 128, 128, 255, 128);
        curtain_packet->clut = packed;
        SetSemiTrans(curtain_packet, 1);
        SetShadeTex(curtain_packet, 0);
        for (k = 0; k < 3; k++, mesh++) {
            for (row = 0, angle = 0, base = 0; row < 9;
                 row++, angle = (base << 2) / 8) {
                radius = rsin(angle) * 384 >> 12;
                height = (rcos(angle) * 384 >> 12) + (rsin(276) * 384 >> 12);
                for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                    setVector(&mesh->rows[row][j], rcos(angle) * radius >> 12,
                        -height, rsin(angle) * radius >> 12);
                }
                base += 325;
                mesh->colors[row][0] = 0;
                mesh->colors[row][1] = 192;
                mesh->colors[row][2] = 0;
            }
            mesh->size = 0;
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
                    spiral->outer[j].g = 255;
                    spiral->outer[j].b = 0;
                    spiral->inner[j].r = 128;
                    spiral->inner[j].g = 128;
                    spiral->inner[j].b = 128;
                }
            }
        }
        for (i = 0; i < 3; i++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->colors[0].r = 128;
            ribbon->colors[0].g = 128;
            ribbon->colors[0].b = 128;
            ribbon->colors[1].r = 0;
            ribbon->colors[1].g = 192;
            ribbon->colors[1].b = 16;
            ribbon->field_238 = 0;
            ribbon->field_23C = 0;
            ribbon->field_23A = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i / 2 * 2 == i - 1) {
                angle = 1024 + (i + 1) * 256;
            } else {
                angle = 1024 - i * 256;
            }
            setVector(&ribbon->position, rcos(angle) * 256 >> 12,
                rsin(angle) * 256 >> 12, 0);
            ribbon->progress = -i * 1024;
        }
        for (k = 0; k < 6; k++, sheet++) {
            for (index = 0; index < 4; index++) {
                if (index == 0) {
                    setVector(&sheet->v0[index], -128, -128, 0);
                    setVector(&sheet->v1[index], 0, -128, 0);
                    setVector(&sheet->v2[index], -128, 0, 0);
                    setVector(&sheet->v3[index], 0, 0, 0);
                } else if (index == 1) {
                    setVector(&sheet->v0[index], 128, -128, 0);
                    setVector(&sheet->v1[index], 128, 0, 0);
                    setVector(&sheet->v2[index], 0, -128, 0);
                    setVector(&sheet->v3[index], 0, 0, 0);
                } else if (index == 2) {
                    setVector(&sheet->v0[index], 128, 128, 0);
                    setVector(&sheet->v1[index], 0, 128, 0);
                    setVector(&sheet->v2[index], 128, 0, 0);
                    setVector(&sheet->v3[index], 0, 0, 0);
                } else if (index == 3) {
                    setVector(&sheet->v0[index], -128, 128, 0);
                    setVector(&sheet->v1[index], -128, 0, 0);
                    setVector(&sheet->v2[index], 0, 128, 0);
                    setVector(&sheet->v3[index], 0, 0, 0);
                }
            }
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 255;
            sheet->inner[2] = 64;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
        }
        for (k = 0; k < 9; k++, curtain++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&curtain->a[j], (u32)(rcos(angle) * 3) >> 5, 0,
                    (u32)(rsin(angle) * 3) >> 5);
                setVector(&curtain->b[j], (u32)rcos(angle) >> 3, -300,
                    (u32)rsin(angle) >> 3);
            }
            if (k < 3) {
                curtain->scale = 0;
            } else {
                curtain->scale = -((k - 3) * 8192 / 6);
            }
            curtain->count = 0;
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j][0] = 128;
                streamer->color[j][1] = 128;
                streamer->color[j][2] = 128;
            }
            MODEL_VARIANT_WORD(streamer, 0x2A8) = 0;
        }
        work->inner.r = 128;
        work->inner.g = 192;
        work->inner.b = 128;
        work->outer.r = 0;
        work->outer.g = 192;
        work->outer.b = 0;
        work->field_37B0 = 0;
        work->size = 0;
        work->field_3774 = 0;
        work->field_3778 = 0;
        work->field_377C = 0;
        work->intensity = 1024;
        work->field_3784 = 0;
        work->field_3788 = 0;
        work->field_3790 = 0;
        work->field_3792 = 0;
        work->field_3794 = 0;
        work->field_3796 = 0;
        work->progress = 0;
        work->width = 1024;
        work->field_376C = 1024;
        work->field_3764 = 0;
        work->field_3766 = 0;
        work->field_37B4 = 0;
        work->field_37B8 = 0;
        work->field_37BC = 0;
        work->field_37C0 = 0;
        work->field_3798 = 1024;
        work->field_379C = 0;
        work->field_37A0 = 0;
        work->field_37A4 = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->field_37C8 = 0;
        work->field_37CC = 0;
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
        for (index = 0; index < 3; index++) {
            GsGetLwUnit(work->parts[index], &work->matrices[index]);
            setVector(&work->directions[index],
                work->target.vx - work->matrices[index].t[0],
                work->target.vy - work->matrices[index].t[1],
                work->target.vz - work->matrices[index].t[2]);
        }
        for (i = 0, angle = 0; i < 3; i++, angle = i * 4096 / 3) {
            if (work->slot == 0) {
                setVector(&work->origins[i],
                    work->target.vx + (rcos(angle + 2048) * 256 >> 12),
                    work->target.vy,
                    work->target.vz + (rsin(angle + 2048) * 256 >> 12));
            } else {
                setVector(&work->origins[i],
                    work->target.vx + (rcos(angle) * 256 >> 12),
                    work->target.vy,
                    work->target.vz + (rsin(angle) * 256 >> 12));
            }
            setVector(&work->ribbon_directions[i],
                work->origins[i].vx - work->matrices[i % 3].t[0],
                work->origins[i].vy - work->matrices[i % 3].t[1],
                work->origins[i].vz - work->matrices[i % 3].t[2]);
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrices[0].t[0],
            work->matrices[0].t[1], work->matrices[0].t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
            &projection.interpolation, &projection.flag);
        screen_x = (u16)projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &projection.target,
            &projection.interpolation, &projection.flag);
        work->screen_delta.vx = (u16)projection.target - screen_x;
        work->screen_delta.vy = (projection.target >> 16) - screen_y;
        if (work->config->draw_start <= work->elapsed && work->phase < 5) {
            func_8013D8A0((u8 *)point);
        }
        if ((u32)(work->phase - 3) < 3) {
            func_8013E5BC((u8 *)point);
            func_8013BFB4((u8 *)point);
        }
        if ((u32)(work->phase - 2) < 3) {
            func_8013CFC4((u8 *)point);
        }
        if ((u32)(work->phase - 1) < 4) {
            func_8013C57C((u8 *)point);
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
