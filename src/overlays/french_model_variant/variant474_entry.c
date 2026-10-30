#include "../../types.h"
#include "variant474_entry.h"

/* Header 474's entry: two funnels, one ribbon, a 9x17 mesh, twelve spirals,
 * two sheets and two streamers, with the header-337 camera and part
 * tracking. As in header 433, the setup keeps the third texture's packed
 * word in a short-lived `packed` (reassigned by the next upload) and copies
 * its CLUT into `flat_texture`. The funnel size store sits in its own
 * `do { } while (0)`: the loop notes stop the scheduler from hoisting the
 * counter increment above it, which reproduces the retail order. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    u32 screen_x;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant474EntryState *work;
    Variant474EntryFunnel *funnel;
    Variant474EntryRibbon *ribbon;
    Variant474EntrySheet *sheet;
    Variant474EntryStreamer *streamer;
    Variant474EntrySpiral *spiral;
    POLY_GT4 *glow;
    POLY_G4 *quad;
    s32 result;
    u16 tpage2, clut2, clut;
    Variant474EntryMesh *mesh;
    POLY_GT4 *textured;
    POLY_FT4 *flat;
    POLY_GT4 *bar;
    POLY_G3 *triangle;
    u16 tpage;
    s32 i, j, k, n, angle, angle2;
    s32 m;
    s32 packed;
    s32 flat_page;
    u16 flat_texture;
    s32 s, c;
    GsRVIEW2 *view;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;

    work = (Variant474EntryState *)point;
    funnel = work->funnels;
    ribbon = &work->ribbon;
    sheet = work->sheets;
    streamer = work->streamers;
    spiral = work->spirals;
    textured = work->textured;
    flat = work->flat;
    bar = work->bars;
    mesh = &work->mesh;
    glow = &work->glow;
    triangle = &work->triangle;
    quad = &work->quad;
    result = 0;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant474EntryConfig *)(D_8013E980 + 0x1C) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)D_8013E980);
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E980 - 0x38));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E980 - 0xA8));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E980 - 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013E980 - 0x70));
        tpage2 = GetTPage(1, 1, 896, 0);
        clut2 = GetClut(640, 244);
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
        setUV4(flat, 128, 128, 255, 128, 128, 223, 255, 223);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 0);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 128, 128, 255, 128, 128, 223, 255, 223);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 0);
        SetShadeTex(flat, 1);
        SetPolyGT4(bar);
        bar->tpage = flat_page;
        setUV4(bar, 128, 223, 223, 223, 128, 128, 223, 128);
        bar->clut = flat_texture;
        SetSemiTrans(bar, 1);
        SetShadeTex(bar, 0);
        bar++;
        SetPolyGT4(bar);
        bar->tpage = flat_page;
        setUV4(bar, 192, 223, 223, 223, 192, 128, 223, 128);
        bar->clut = flat_texture;
        SetSemiTrans(bar, 1);
        SetShadeTex(bar, 0);
        SetPolyGT4(glow);
        glow->tpage = tpage2;
        setUV4(glow, 0, 191, 31, 191, 0, 128, 31, 128);
        glow->clut = clut2;
        SetSemiTrans(glow, 1);
        SetShadeTex(glow, 0);
        for (n = 0, angle = 0; n < 9; n++, angle = n << 8) {
            s = rsin(angle) * 256 >> 12;
            c = rcos(angle) * 256 >> 12;
            for (j = 0, angle2 = 0; j < 17; j++, angle2 = j << 8) {
                setVector(&mesh->row[n][j], rcos(angle2) * s >> 12, -c, rsin(angle2) * s >> 12);
            }
            mesh->color[n][2] = n * 8 + 64;
            mesh->color[n][0] = n * 8;
            mesh->color[n][1] = n * 8;
        }
        for (i = 0; i < 1; i++, ribbon++) {
            for (j = 0; j < 17; j++) {
                ribbon->phase[j] = -(j << 8);
                ribbon->amp[j] = 0;
                if (j < 12) {
                    ribbon->color[j].r = 128;
                    ribbon->color[j].g = 128;
                    ribbon->color[j].b = 128;
                    ribbon->secondary[j].r = 0;
                    ribbon->secondary[j].g = 16;
                    ribbon->secondary[j].b = 128;
                } else {
                    ribbon->color[j].r = (16 - j) << 5;
                    ribbon->color[j].g = (16 - j) << 5;
                    ribbon->color[j].b = (16 - j) << 5;
                    ribbon->secondary[j].r = 0;
                    ribbon->secondary[j].g = (16 - j) << 2;
                    ribbon->secondary[j].b = (16 - j) << 5;
                }
            }
            ribbon->scale[0] = 4096;
            ribbon->scale[1] = 4096;
            ribbon->scale[2] = 4096;
            ribbon->field_340 = 0;
            ribbon->field_344 = 0;
            ribbon->field_342 = 0;
            if (i == 0) {
                angle = 1024;
            } else if ((i / 2) * 2 == i - 1) {
                dx = i << 9;
                angle = dx + 1536;
            } else {
                angle = 1024 - (i << 9);
            }
            ribbon->heading[0] = rcos(angle) * 256 >> 12;
            ribbon->heading[1] = rsin(angle) * 256 >> 12;
            ribbon->heading[2] = 0;
        }
        for (m = 0; m < 2; funnel++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&funnel->inner[j], (u32)rcos(angle) >> 4, 0, (u32)rsin(angle) >> 4);
                setVector(&funnel->outer[j], (u32)(rcos(angle) * 5) >> 6, -64, (u32)(rsin(angle) * 5) >> 6);
            }
            do {
                funnel->size = 0;
            } while (0);
            m++;
            funnel->field_114 = 0;
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
        m = 0;
        do {
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
            m++;
            sheet->outer[0] = 255;
            sheet->outer[1] = 255;
            sheet->outer[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 64;
            sheet->inner[2] = 255;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
            sheet++;
        } while (m < 2);
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->inner_color.r = work->config->secondary[0];
        work->inner_color.g = work->config->secondary[1];
        work->inner_color.b = work->config->secondary[2];
        work->outer_color.r = work->config->color[0];
        work->outer_color.g = work->config->color[1];
        work->outer_color.b = work->config->color[2];
        work->field_1C08 = 0;
        work->field_1BB8 = 0;
        work->field_1BBC = 0;
        work->field_1BC4 = 0;
        work->field_1BC8 = 0;
        work->field_1BCC = 0;
        work->field_1BD0 = 0;
        work->field_1BD4 = 1024;
        work->field_1BD8 = 0;
        work->field_1BDC = 0;
        work->field_1BE8 = 0;
        work->field_1BEA = 0;
        work->field_1BEC = 0;
        work->field_1BEE = 0;
        work->field_1BB4 = 1024;
        work->field_1BC0 = 1024;
        work->field_1BB0 = 0;
        work->field_1BB2 = 0;
        work->field_1C0C = 0;
        work->field_1C10 = 0;
        work->field_1BF0 = 1024;
        work->field_1C14 = 0;
        work->field_1C18 = 0;
        work->field_1BE4 = 0;
        work->field_1BF4 = 0;
        work->field_1BF8 = 0;
        work->field_1BFC = 0;
        work->field_1BE0 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_1C20 = 0;
        work->field_1C24 = 0;
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
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
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
        work->screen_x = dx;
        dy = projection.target.vy - screen_y;
        work->screen_y = dy;
        if (work->config->trigger <= work->frame) {
            func_8013C738((u8 *)point);
            if (work->phase < 6) {
                func_8013BE38((u8 *)point);
            }
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
        if (work->config->fade_begin <= work->frame && work->field_1BE4 < 1024) {
            work->field_1BE4 = ((work->frame - work->config->fade_begin) << 10) /
                               (work->config->fade_end - work->config->fade_begin);
            if (work->field_1BE4 >= 1024) {
                work->field_1BE4 = 1024;
            }
        }
    }
    setVector(&work->camera, -128, -128, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->camera, 20);
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
