#include "../../types.h"
#include "variant456_entry.h"

/* Header 456's entry: two fans, two trails, two ribbons, sixteen lines,
 * eight sheets and two streamers around the header-337 camera and part
 * tracking. The command row's start frames gate the sheet and line helpers.
 * Both sheet arms clear +0x8C/+0x90 so cross-jumping leaves the loop
 * increments in their own block, as in retail. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant456EntryState *work = (Variant456EntryState *)point;
    Variant456EntryFan *fan = work->fans;
    Variant456EntryTrail *trail = work->trails;
    Variant456EntryRibbon *ribbon = work->ribbons;
    Variant456EntryLine *line = work->lines;
    Variant456EntrySheet *sheet = work->sheets;
    Variant456EntryStreamer *streamer = work->streamers;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *extra = work->flat_extra;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *glow = &work->glow;
    s32 result = 0;
    s32 i, j, k, angle, angle2;
    u16 tpage, clut;
    u16 flat_texture;
    s32 flat_page, packed;
    s32 n;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant456EntryConfig *)(D_8013ED00 + 0xFC) + command;
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
        n = 0;
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ED00 + 0xC4));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ED00 + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ED00 + 0x1C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ED00 + 0x38));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ED00 + 0x54));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013ED00 + 0xE0));
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
        SetPolyFT4(extra);
        setUV4(extra, 0, 224, 31, 224, 0, 255, 31, 255);
        angle = packed >> 16;
        extra->tpage = angle;
        extra->clut = packed;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        extra++;
        SetPolyFT4(extra);
        extra->tpage = angle;
        setUV4(extra, 32, 224, 63, 224, 32, 255, 63, 255);
        extra->clut = packed;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(glow);
        SetSemiTrans(glow, 1);
        SetShadeTex(glow, 0);
        do {
            fan->point[0].vx = 0;
            fan->point[0].vy = 0;
            fan->point[0].vz = 0;
            for (j = 0, angle = 0; j < 5; angle = (j << 12) / 5) {
                setVector(&fan->point[j + 1], rcos(angle) * 375 >> 12, rsin(angle) * 375 >> 12, 0);
                j++;
            }
            n++;
            fan->color.r = 192;
            fan->color.g = 224;
            fan->field_34 = 0;
            fan->field_38 = 0;
            fan->field_3C = 0;
            fan->color.b = 255;
            fan->field_44 = 0;
            fan->field_48 = 0;
            fan->field_4C = 0;
            fan++;
        } while (n < 2);
        for (i = 0, angle = 0, angle2 = 0; i < 2; angle += 1300, angle2 += 800) {
            for (j = 0; j < 3; j++) {
                trail->phase[j] = -(j << 9) / 2 + i * -256;
            }
            trail->scale.vx = 4096;
            trail->scale.vy = 4096;
            trail->scale.vz = 4096;
            trail->color.r = 192;
            trail->color.g = 192;
            trail->color.b = 192;
            trail->secondary.g = 16;
            trail->secondary.r = 0;
            trail->secondary.b = 128;
            trail->field_78 = 0;
            trail->field_7C = 0;
            trail->field_7A = 0;
            trail->completed = 0;
            trail->field_80 = 0;
            trail->target.vx = work->target.vx + ((s32)(rcos(angle) << 9) >> 12);
            trail->target.vy = work->target.vy + ((s32)(rsin(angle) << 9) >> 12);
            trail->target.vz = work->target.vz + ((s32)(rsin(angle2) << 9) >> 12);
            i++;
            trail->delta.vx = trail->target.vx - work->matrix.t[0];
            trail->delta.vy = trail->target.vy - work->matrix.t[1];
            trail->delta.vz = trail->target.vz - work->matrix.t[2];
            trail++;
        }
        for (i = 0; i < 2; i++, ribbon++) {
            k = i << 9;
            for (j = 2; j >= 0; j--) {
                ribbon->phase[j] = k;
            }
            ribbon->scale.vx = 4096;
            ribbon->scale.vy = 4096;
            ribbon->scale.vz = 4096;
            ribbon->color.r = work->config->color.r;
            ribbon->color.g = work->config->color.g;
            ribbon->color.b = work->config->color.b;
            ribbon->secondary.r = work->config->secondary.r;
            ribbon->secondary.g = work->config->secondary.g;
            ribbon->secondary.b = work->config->secondary.b;
            ribbon->field_78 = 0;
            ribbon->field_7C = 0;
            ribbon->field_7A = 0;
            ribbon->field_D8 = 0;
            ribbon->field_DC = 0;
            if (i == 0) {
                angle = 1024;
            } else if ((i / 2) * 2 == i - 1) {
                angle = ((i + 1) << 10) / 3 + 1024;
            } else {
                angle = 1024 - (i << 10) / 3;
            }
            ribbon->heading[0] = (s32)(rcos(angle) << 8) >> 12;
            ribbon->heading[1] = (s32)(rsin(angle) << 8) >> 12;
            ribbon->heading[2] = 0;
        }
        for (i = 0; i < 16; i++, line++) {
            for (j = 0; j < 17; j++) {
                line->phase[j] = -(j << 6) - (i << 9);
            }
            line->scale.vx = 4096;
            line->scale.vy = 4096;
            line->scale.vz = 4096;
            line->color.r = work->config->web.r;
            line->color.g = work->config->web.g;
            line->color.b = work->config->web.b;
            line->secondary.r = 0;
            line->secondary.g = 16;
            line->secondary.b = 128;
            line->field_18 = 0;
            line->field_1C = 0;
            line->field_1A = 0;
            line->field_100 = 0;
            line->field_FC = 0;
            line->field_104 = 0;
            line->heading[0] = (s32)(rcos(angle) << 8) >> 12;
            line->heading[1] = (s32)(rsin(angle) << 8) >> 12;
            line->heading[2] = 0;
        }
        for (n = 0, angle = 0, angle2 = 0; n < 8; n++, sheet++, angle += 1300, angle2 += 800) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -320, -320, 0);
                    setVector(&sheet->v1[k], 0, -320, 0);
                    setVector(&sheet->v2[k], -320, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 320, -320, 0);
                    setVector(&sheet->v1[k], 320, 0, 0);
                    setVector(&sheet->v2[k], 0, -320, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 320, 320, 0);
                    setVector(&sheet->v1[k], 0, 320, 0);
                    setVector(&sheet->v2[k], 320, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -320, 320, 0);
                    setVector(&sheet->v1[k], -320, 0, 0);
                    setVector(&sheet->v2[k], 0, 320, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->target.vx = work->target.vx + ((s32)(rcos(angle) << 9) >> 12);
            sheet->target.vy = work->target.vy + ((s32)(rsin(angle) << 9) >> 12);
            sheet->target.vz = work->target.vz + ((s32)(rsin(angle2) << 9) >> 12);
            sheet->outer[0] = work->config->color.r;
            sheet->outer[1] = work->config->color.g;
            sheet->outer[2] = work->config->color.b;
            sheet->inner[0] = work->config->secondary.r;
            sheet->inner[1] = work->config->secondary.g;
            sheet->inner[2] = work->config->secondary.b;
            sheet->field_B8 = 0;
            sheet->field_BC = 1024;
            if (n == 0) {
                sheet->size = 0;
                sheet->field_8C = 0;
                sheet->field_90 = 0;
            } else {
                sheet->size = -((n - 1) << 15) / 8;
                sheet->field_8C = 0;
                sheet->field_90 = 0;
            }
        }
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->field_2342 = 1024;
        work->field_2344 = 1024;
        work->field_2348 = 1024;
        work->field_2368 = 0;
        work->field_233C = 0;
        work->field_233E = 0;
        work->field_2340 = 0;
        work->field_2358 = 0;
        work->field_235C = 0;
        work->field_2360 = 0;
        work->field_2364 = 0;
        work->field_234C = 0;
        work->field_2350 = 0;
        work->field_2354 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
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
            GsGetLwUnit(work->part, &work->matrix);
            setVector(&work->direction,
                      work->target.vx - work->matrix.t[0],
                      work->target.vy - work->matrix.t[1],
                      work->target.vz - work->matrix.t[2]);
        }
        for (n = 0, sheet = work->sheets; n < 8; n++, sheet++) {
            sheet->delta.vx = sheet->target.vx - work->matrix.t[0];
            sheet->delta.vy = sheet->target.vy - work->matrix.t[1];
            sheet->delta.vz = sheet->target.vz - work->matrix.t[2];
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
        work->screen_x = dx;
        dy = projection.target.vy - screen_y;
        work->screen_y = dy;
        if (work->config->phase_start <= work->frame && work->phase < 2) {
            work->phase = 2;
        }
        if (work->config->sheets_start <= work->frame) {
            func_8013DACC((u8 *)point);
        }
        if (work->config->lines_start <= work->frame) {
            func_8013CF58((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
        work->field_2358 += work->step << 7;
        work->field_235C += work->step << 6;
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
    if (work->phase >= 2 && work->phase <= 4) {
        result = 4;
    } else if (work->phase == 5) {
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
    return result;
}
