#include "../../types.h"
#include "variant411_entry.h"

#ifndef MODEL_VARIANT411_CLUT_X
#define MODEL_VARIANT411_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState411 *work = (EntryState411 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant402EntryProjection projection;
    Tube411 *tube = work->tubes;
    Beam411 *beam = work->beams;
    ModelVariantSheet *sheet = work->sheets;
    Funnel411 *funnel = work->funnels;
    EntryRing411 *ring = work->rings;
    EntryExtra411 *extra_record = work->extras;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_GT4 *extra = &work->extra;
    POLY_GT4 *secondary = &work->secondary;
    POLY_GT4 *ring_packet = &work->ring_packet;
    s32 result = 0;
    u16 tpage, clut, ring_texture, flat_texture;
    s32 ring_page, packed, flat_page;
    s32 i, j, k, row, angle, bearing;
    s32 red, green, blue;
    s32 sheet_point;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->frame_count = 0;
        work->config = (EntryConfig411 *)(D_8013D9B4 + 0xFC) + command;
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
        clut = GetClut(MODEL_VARIANT411_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT411_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D9B4 + 0xC4));
        flat_page = packed >> 16;
        flat_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D9B4 + 0x8C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D9B4 + 0x1C));
        ring_page = packed >> 16;
        ring_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D9B4 + 0x38));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D9B4 + 0x54));
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
        SetPolyGT4(extra);
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyGT4(secondary);
        SetSemiTrans(secondary, 0);
        SetShadeTex(secondary, 0);
        SetPolyGT4(ring_packet);
        ring_packet->tpage = ring_page;
        setUV4(ring_packet, 128, 208, 255, 208, 128, 128, 255, 128);
        ring_packet->clut = ring_texture;
        SetSemiTrans(ring_packet, 1);
        SetShadeTex(ring_packet, 0);
        for (k = 0; k < 2; k++, funnel++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                funnel->inner[j].vx = (u32)rcos(angle) >> 4;
                funnel->inner[j].vy = (u32)rsin(angle) >> 4;
                funnel->inner[j].vz = 0;
                setVector(&funnel->outer[j], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, -64);
            }
            if (k == 1) {
                funnel->size = 0;
            } else {
                funnel->size = k * -4096;
            }
            MODEL_VARIANT_WORD(funnel, 0x114) = 0;
        }
        for (i = 0; i < 1; i++, tube++) {
            MODEL_VARIANT_WORD(tube, 0x534) = 4096;
            MODEL_VARIANT_WORD(tube, 0x538) = 4096;
            MODEL_VARIANT_WORD(tube, 0x53C) = 4096;
            for (j = 0; j < 9; j++) {
                if (j % 2 == 0) {
                    red = 96;
                    green = 96;
                    blue = 96;
                } else {
                    red = 0;
                    green = 0;
                    blue = 128;
                }
                tube->colors[j].r = red;
                tube->colors[j].g = green;
                tube->colors[j].b = blue;
            }
            MODEL_VARIANT_HALF(tube, 0x544) = 0;
            MODEL_VARIANT_WORD(tube, 0x548) = 0;
            MODEL_VARIANT_HALF(tube, 0x546) = 0;
        }
        for (i = 0; i < 16; i++, beam++) {
            if (i % 8 == 0) {
                red = 128; green = 0; blue = 0; beam->age = 0;
            } else if (i % 8 == 1) {
                red = 128; green = 64; blue = 0; beam->age = 2;
            } else if (i % 8 == 2) {
                red = 128; green = 128; blue = 0; beam->age = 3;
            } else if (i % 8 == 3) {
                red = 0; green = 128; blue = 0; beam->age = 1;
            } else if (i % 8 == 4) {
                red = 0; green = 128; blue = 128; beam->age = 4;
            } else if (i % 8 == 5) {
                red = 0; green = 0; blue = 128; beam->age = 7;
            } else if (i % 8 == 6) {
                red = 128; green = 0; blue = 128; beam->age = 5;
            } else {
                red = 128; green = 0; blue = 64; beam->age = 6;
            }
            beam->outer[0] = red + 127;
            beam->outer[1] = green + 127;
            beam->inner[0] = red;
            beam->inner[1] = green;
            beam->inner[2] = blue;
            beam->outer[2] = blue + 127;
        }
        for (k = 0; k < 2; k++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->v0[sheet_point], -256, -256, 0);
                    setVector(&sheet->v1[sheet_point], 0, -256, 0);
                    setVector(&sheet->v2[sheet_point], -256, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->v0[sheet_point], 256, -256, 0);
                    setVector(&sheet->v1[sheet_point], 256, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, -256, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->v0[sheet_point], 256, 256, 0);
                    setVector(&sheet->v1[sheet_point], 0, 256, 0);
                    setVector(&sheet->v2[sheet_point], 256, 0, 0);
                    setVector(&sheet->v3[sheet_point], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->v0[sheet_point], -256, 256, 0);
                    setVector(&sheet->v1[sheet_point], -256, 0, 0);
                    setVector(&sheet->v2[sheet_point], 0, 256, 0);
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
        for (row = 0, bearing = 0, j = 0; row < 4; row++, ring++, bearing += 256) {
            for (k = 0, angle = bearing; k < 8; k++, j++) {
                ring->inner[k].vx = (u32)rcos(angle) >> 4;
                ring->inner[k].vy = (u32)rsin(angle) >> 4;
                ring->inner[k].vz = 0;
                setVector(&ring->outer[k], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
                if (j % 8 == 0) {
                    red = 255; green = 0; blue = 0;
                } else if (j % 8 == 1) {
                    red = 255; green = 128; blue = 0;
                } else if (j % 8 == 2) {
                    red = 255; green = 255; blue = 0;
                } else if (j % 8 == 3) {
                    red = 0; green = 255; blue = 0;
                } else if (j % 8 == 4) {
                    red = 0; green = 255; blue = 255;
                } else if (j % 8 == 5) {
                    red = 0; green = 0; blue = 255;
                } else if (j % 8 == 6) {
                    red = 255; green = 0; blue = 255;
                } else {
                    red = 255; green = 0; blue = 128;
                }
                angle += 512;
                ring->colors[k].r = red;
                ring->colors[k].g = green;
                ring->colors[k].b = blue;
            }
            ring->inner_color.r = 64;
            ring->inner_color.g = 128;
            ring->inner_color.b = 255;
            ring->outer_color.r = 0;
            ring->outer_color.g = 0;
            ring->outer_color.b = 0;
            ring->size = 1024 - row * 256;
            ring->field_AC = 0;
        }
        for (i = 0; i < 2; i++, extra_record++) {
            setVector(&extra_record->scale, 4096, 4096, 4096);
            extra_record->inner.r = 255;
            extra_record->inner.g = 255;
            extra_record->inner.b = 255;
            extra_record->outer.r = 0;
            extra_record->outer.g = 16;
            extra_record->outer.b = 128;
        }
        work->inner.r = 128;
        work->inner.g = 128;
        work->inner.b = 128;
        work->outer.r = 0;
        work->outer.g = 0;
        work->outer.b = 128;
        work->width = 1024;
        work->spin = 0;
        work->angle = 0;
        work->field_1674 = 0;
        work->field_1678 = 0;
        work->progress = 0;
        work->field_165C = 0;
        work->field_165E = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
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
            Model_CopySlotU16Values(1, (u16 *)&work->target);
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->part, &work->matrix);
        if (work->slot == 0) {
            work->matrix.t[0] += work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] += work->config->offset[2];
        } else {
            work->matrix.t[0] -= work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] -= work->config->offset[2];
        }
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
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
        if (work->config->sheet_start < work->elapsed) {
            func_8013CE58((u8 *)point);
        }
        if (work->config->expansion_start < work->elapsed) {
            func_8013BE60((u8 *)point);
            func_8013D4A4((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
    func_800595C8(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 5) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if ((u32)(work->phase - 2) < 2) {
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
