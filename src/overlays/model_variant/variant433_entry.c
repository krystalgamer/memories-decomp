#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant433_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT433_CLUT_X
#define MODEL_VARIANT433_CLUT_X 512
#endif

/* Header 433's entry: six fans, six ribbons, three bands, sixteen rings, seven
 * sheets and two streamers, with the header-337 camera and part tracking and
 * the draw helpers of header 433. The setup borrows some locals for values
 * that only live a few instructions (the extra packets' tpage in `angle`, the
 * odd bands' heading in `dx`); the retail register allocation depends on it. */

s32 ModelVariant433_HandleRequest(SVECTOR *point, s32 command)
{
    u32 screen_x;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant433EntryFan *fan;
    Variant433EntryState *work;
    Variant433EntryRibbon *ribbon;
    Variant433EntryBand *band;
    Variant433EntryBand *ring;
    Variant433EntrySheet *sheet;
    Variant433EntryStreamer *streamer;
    POLY_G4 *quad;
    s32 result;
    POLY_GT4 *textured;
    u16 tpage, clut;
    POLY_FT4 *extra;
    POLY_G3 *triangle;
    s32 i, j, k, angle, angle2;
    POLY_FT4 *flat;
    u16 flat_texture;
    s32 packed;
    s32 n;
    s32 outer;
    GsRVIEW2 *view;
    s32 flat_page;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;

    work = (Variant433EntryState *)point;
    fan = (Variant433EntryFan *)work;
    ribbon = (Variant433EntryRibbon *)((u8 *)work + 0x270);
    band = (Variant433EntryBand *)((u8 *)work + 0xF00);
    ring = (Variant433EntryBand *)((u8 *)work + 0x1518);
    sheet = (Variant433EntrySheet *)((u8 *)work + 0x3598);
    streamer = (Variant433EntryStreamer *)((u8 *)work + 0x39F8);
    textured = work->textured;
    flat = work->flat;
    extra = work->flat_extra;
    triangle = &work->triangle;
    quad = &work->quad;
    result = 0;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant433EntryConfig *)(gModelVariant433_D39A0 + 0x38) + command;
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
        clut = GetClut(MODEL_VARIANT433_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT433_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)gModelVariant433_D39A0);
        func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant433_D39A0 - 0x38));
        func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant433_D39A0 - 0xA8));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(gModelVariant433_D39A0 + 0x1C));
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
        SetShadeTex(extra, 1);
        extra++;
        SetPolyFT4(extra);
        extra->tpage = angle;
        setUV4(extra, 32, 224, 63, 224, 32, 255, 63, 255);
        extra->clut = packed;
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 1);
        n = 0;
        do {
            fan->point[0].vx = 0;
            fan->point[0].vy = 0;
            fan->point[0].vz = 0;
            for (j = 0, angle = 0; j < 8; angle = j << 9) {
                setVector(&fan->point[j + 1], (u32)(rcos(angle) * 75) >> 11, (u32)(rsin(angle) * 75) >> 11, 0);
                j++;
            }
            n++;
            fan->color.r = 192;
            fan->color.g = 224;
            fan->field_4C = 0;
            fan->field_50 = 0;
            fan->field_54 = 0;
            fan->color.b = 255;
            fan->field_5C = 0;
            fan->field_60 = 0;
            fan->field_64 = 0;
            fan++;
        } while (n < 6);
        for (i = 0, angle = 0, angle2 = 0; i < 6; angle += 1300, angle2 += 800) {
            for (j = 0; j < 9; j++) {
                ribbon->phase[j] = -(j << 9) / 8 + i * -512;
            }
            ribbon->scale.vx = 4096;
            ribbon->scale.vy = 4096;
            ribbon->scale.vz = 4096;
            ribbon->color.r = 128;
            ribbon->color.g = 128;
            ribbon->color.b = 192;
            ribbon->secondary.r = 0;
            ribbon->secondary.g = 0;
            ribbon->secondary.b = 128;
            ribbon->field_138 = 0;
            ribbon->field_13C = 0;
            ribbon->field_13A = 0;
            ribbon->completed = 0;
            ribbon->target.vx = work->target.vx + ((s32)(rcos(angle) << 8) >> 12);
            ribbon->target.vy = work->target.vy + ((s32)(rsin(angle) << 8) >> 12);
            ribbon->target.vz = work->target.vz + ((s32)(rsin(angle2) << 8) >> 12);
            ribbon->delta.vx = ribbon->target.vx - work->matrix.t[0];
            ribbon->delta.vy = ribbon->target.vy - work->matrix.t[1];
            ribbon->delta.vz = ribbon->target.vz - work->matrix.t[2];
            i++;
            ribbon++;
        }
        for (i = 0; i < 3; i++, band++) {
            outer = i << 9;
            for (j = 8; j >= 0; j--) {
                band->amount[j] = outer;
            }
            band->secondary.g = 16;
            band->scale.vx = 4096;
            band->scale.vy = 4096;
            band->scale.vz = 4096;
            band->color.r = 192;
            band->color.g = 192;
            band->color.b = 192;
            band->secondary.r = 0;
            band->secondary.b = 128;
            band->field_138 = 0;
            band->field_13C = 0;
            band->field_13A = 0;
            band->field_204 = 0;
            if (i == 0) {
                angle = 1024;
            } else if ((i / 2) * 2 == i - 1) {
                dx = i << 8;
                angle = dx + 1280;
            } else {
                angle = 1024 - (i << 8);
            }
            band->heading[0] = (s32)(rcos(angle) << 8) >> 12;
            band->heading[1] = (s32)(rsin(angle) << 8) >> 12;
            band->heading[2] = 0;
        }
        for (i = 0; i < 16; i++, ring++) {
            for (j = 0; j < 9; j++) {
                ring->amount[j] = -(j << 7) - (i << 9);
            }
            ring->secondary.g = 16;
            ring->scale.vx = 4096;
            ring->scale.vy = 4096;
            ring->scale.vz = 4096;
            ring->color.r = 192;
            ring->color.g = 192;
            ring->color.b = 192;
            ring->secondary.r = 0;
            ring->secondary.b = 128;
            ring->field_138 = 0;
            ring->field_13C = 0;
            ring->field_13A = 0;
            ring->field_204 = 0;
            ring->heading[0] = (s32)(rcos(angle) << 8) >> 12;
            ring->heading[1] = (s32)(rsin(angle) << 8) >> 12;
            ring->heading[2] = 0;
        }
        n = 0;
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
            n++;
            sheet->outer[0] = 224;
            sheet->outer[1] = 240;
            sheet->outer[2] = 255;
            sheet->inner[2] = 255;
            sheet->inner[0] = 0;
            sheet->inner[1] = 128;
            sheet->field_98 = 0;
            sheet->field_9C = 1024;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
            sheet++;
        } while (n < 7);
        for (i = 0; i < 2; i++, streamer++) {
            for (j = 0; j < 17; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_2A8 = 0;
        }
        work->field_42CE = 1024;
        work->field_42D0 = 1024;
        work->field_42D4 = 1024;
        work->field_42F4 = 0;
        work->field_42C8 = 0;
        work->field_42CA = 0;
        work->field_42CC = 0;
        work->field_42E4 = 0;
        work->field_42E8 = 0;
        work->field_42EC = 0;
        work->field_42F0 = 0;
        work->field_42D8 = 0;
        work->field_42DC = 0;
        work->field_42E0 = 0;
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
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->part, &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        ribbon = work->ribbons;
        for (i = 0; i < 6; i++, ribbon++) {
            ribbon->delta.vx = ribbon->target.vx - work->matrix.t[0];
            ribbon->delta.vy = ribbon->target.vy - work->matrix.t[1];
            ribbon->delta.vz = ribbon->target.vz - work->matrix.t[2];
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
        if (work->config->trigger <= work->frame) {
            SpanishModelVariant450_DrawLines((u8 *)point);
            SpanishModelVariant450_DrawQuadGroups((u8 *)point);
            if (work->phase < 2) {
                ModelVariant433_DrawRings((u8 *)point);
            }
            if (work->phase > 0) {
                ModelVariant433_DrawRibbons((u8 *)point);
            }
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
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
