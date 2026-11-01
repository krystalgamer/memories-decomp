#include "../../types.h"
#include "variant458_entry.h"

#ifndef MODEL_VARIANT458_CLUT_X
#define MODEL_VARIANT458_CLUT_X 640
#endif

s32 FrenchModelVariant458_HandleRequest(SVECTOR *point, s32 command)
{
    EntryState458 *work = (EntryState458 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection458 projection;
    EntryRibbon458 *ribbon = work->ribbons;
    EntryStrand458 *strand = work->strands;
    Variant458Sheet *sheet = work->sheets;
    EntryStreamer458 *streamer = work->streamers;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *strand_flat = work->strand_flat;
    s32 result = 0;
    u16 tpage, clut, flat_clut;
    s32 packed;
    s32 flat_page;
    s32 i, j, k, corner, angle, tilt;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        result = 6;
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig458 *)(gFrenchModelVariant458_D230C + 0xFC) + command;
        for (i = 0; i < 7; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
            GsGetLwUnit(work->parts[i], &work->origins[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT458_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT458_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant458_D230C + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant458_D230C + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant458_D230C + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant458_D230C + 0xE0));
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
        SetPolyFT4(strand_flat);
        strand_flat->tpage = packed >> 16;
        setUV4(strand_flat, 0, 224, 31, 224, 0, 255, 31, 255);
        strand_flat->clut = packed;
        SetSemiTrans(strand_flat, 1);
        SetShadeTex(strand_flat, 1);
        strand_flat++;
        SetPolyFT4(strand_flat);
        strand_flat->tpage = packed >> 16;
        setUV4(strand_flat, 32, 224, 63, 224, 32, 255, 63, 255);
        strand_flat->clut = packed;
        SetSemiTrans(strand_flat, 1);
        SetShadeTex(strand_flat, 1);
        for (k = 0; k < 1; k++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            for (j = 0; j < 3; j++) {
                ribbon->inner[j].r = 0;
                ribbon->inner[j].g = 64;
                ribbon->inner[j].b = 255;
                ribbon->outer[j].r = 255;
                ribbon->outer[j].g = 255;
                ribbon->outer[j].b = 255;
                ribbon->offsets[j] = 128 - j * 128;
            }
            ribbon->field_A0 = 0;
            ribbon->field_A4 = 0;
            ribbon->field_A2 = 0;
        }
        for (k = 0; k < 6; k++, strand++) {
            /* The do-while keeps the field stores in their own block, as
             * retail scheduling requires. */
            do {
                setVector(&strand->scale, 4096, 4096, 4096);
                strand->inner.r = 128;
                strand->inner.g = 128;
                strand->inner.b = 128;
                strand->outer.r = 0;
                strand->outer.g = 16;
                strand->outer.b = 128;
                strand->field_138 = 0;
                strand->field_13A = 0;
                strand->field_13C = 0;
                strand->field_1F8 = 0;
                strand->field_1FC = 0;
                strand->delay = k * 16 / 6 + 8;
                strand->count = -(k * 8);
                {
                    /* Retail-faithful dead conversion: its soft-float
                     * libcall is deleted as dead code, but it still makes
                     * loop optimization see a call, which keeps the 16 and
                     * the divide constant inside the loop as in retail. */
                    double unused;

                    unused = k;
                }
            } while (0);
        }
        for (i = 0; i < 7; i++, sheet++) {
            for (corner = 0; corner < 4; corner++) {
                if (corner == 0) {
                    setVector(&sheet->points[0][corner], -128, -128, 0);
                    setVector(&sheet->points[1][corner], 0, -128, 0);
                    setVector(&sheet->points[2][corner], -128, 0, 0);
                    setVector(&sheet->points[3][corner], 0, 0, 0);
                } else if (corner == 1) {
                    setVector(&sheet->points[0][corner], 128, -128, 0);
                    setVector(&sheet->points[1][corner], 128, 0, 0);
                    setVector(&sheet->points[2][corner], 0, -128, 0);
                    setVector(&sheet->points[3][corner], 0, 0, 0);
                } else if (corner == 2) {
                    setVector(&sheet->points[0][corner], 128, 128, 0);
                    setVector(&sheet->points[1][corner], 0, 128, 0);
                    setVector(&sheet->points[2][corner], 128, 0, 0);
                    setVector(&sheet->points[3][corner], 0, 0, 0);
                } else if (corner == 3) {
                    setVector(&sheet->points[0][corner], -128, 128, 0);
                    setVector(&sheet->points[1][corner], -128, 0, 0);
                    setVector(&sheet->points[2][corner], 0, 128, 0);
                    setVector(&sheet->points[3][corner], 0, 0, 0);
                }
            }
            sheet->color[0].r = 255;
            sheet->color[0].g = 255;
            sheet->color[0].b = 255;
            sheet->color[1].r = 0;
            sheet->color[1].g = 64;
            sheet->color[1].b = 255;
        }
        for (k = 0; k < 3; k++, streamer++) {
            for (j = 0; j < 13; j++) {
                streamer->color[j].r = 128;
                streamer->color[j].g = 128;
                streamer->color[j].b = 128;
            }
            streamer->field_208 = 0;
        }
        work->field_1F48 = 4096;
        work->field_1F4A = 1024;
        work->field_1F4C = 0;
        work->field_1F50 = 1024;
        work->field_1F54 = 0;
        work->field_1F58 = 0;
        work->field_1F5C = 0;
        work->factor = 0;
        work->field_1E04 = 0;
        work->scale = 0;
        work->field_1E0C = 0;
        work->field_1F60 = 0;
        work->field_1F62 = 0;
        work->field_1F74 = 0;
        work->field_1F78 = 0;
        work->field_1F64 = 1024;
        work->field_1F68 = 0;
        work->field_1F6C = 0;
        work->field_1F70 = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->duration = work->config->sheet_end - work->config->sheet_start;
        work->fade = 0;
        work->ambient = 2048;
        work->phase = 0;
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
        for (i = 0, angle = 1536, tilt = 0; i < 7; i++, angle = i * 3072 / 7 + 1536, tilt += 1700) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
            GsGetLwUnit(work->parts[i], &work->origins[i]);
            if (i == 4) {
                setVector(&work->destinations[i], work->target.vx, work->target.vy, work->target.vz);
            } else {
                setVector(&work->destinations[i], work->target.vx + (rcos(angle) * 384 >> 12),
                    work->target.vy + (rsin(angle) * 384 >> 12),
                    work->target.vz + (rsin(tilt) * 384 >> 12));
            }
            setVector(&work->deltas[i], work->destinations[i].vx - work->origins[i].t[0],
                work->destinations[i].vy - work->origins[i].t[1],
                work->destinations[i].vz - work->origins[i].t[2]);
        }
        for (k = 0; k < 6; k++) {
            setVector(&work->spans[k], work->destinations[k + 1].vx - work->destinations[k].vx,
                work->destinations[k + 1].vy - work->destinations[k].vy,
                work->destinations[k + 1].vz - work->destinations[k].vz);
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
        if (work->config->draw_start <= work->elapsed) {
            FrenchModelVariant458_DrawStrands((u8 *)point);
            FrenchModelVariant458_DrawStreamers((u8 *)point);
        }
        if (work->elapsed > work->config->sheet_start) {
            FrenchModelVariant458_DrawSheets((u8 *)point);
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
    if (work->phase < 11) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if ((u32)(work->phase - 3) < 4) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 11;
    } else if (work->phase == 11) {
        result = 0;
        work->fade += work->step;
        if (work->fade > 64) {
            work->fade = 64;
            work->phase = 12;
        }
    } else if (work->phase == 12) {
        result = 2;
    }
    return result;
}
