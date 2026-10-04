#include "../../types.h"
#include "../../game/func_80058E1C.h"
#include "variant407_entry.h"

/* Header 407's entry: forty-eight petals at the start of the work area,
 * sixteen 0xA8-byte records, and a single per-frame helper (the petals
 * drawer). */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant407EntryState *work = (Variant407EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant407EntryPetals *petals;
    Variant407EntryBand *band;
    ModelVariantSheet *sheet;
    ModelVariantRing *ring;
    ModelVariantSpokeRing *spoke;
    Variant407EntryQuad *quad_record;
    ModelVariantWebNarrow *web;
    Variant407EntryRecord *record;
    POLY_GT4 *textured;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_GT4 *extra;
    POLY_FT4 *flat;
    POLY_FT4 *sprite;
    s32 result;
    s32 web_angle, row;
    s32 i, j, k, angle;
    s32 default_scale;
    s32 band_index;
    s32 packed;
    s32 near_radius, near_y, far_radius, far_y;
    s32 near_scale, far_scale;
    u16 tpage, clut;
    s32 flat_page;
    u16 flat_clut;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    s32 dy;
    u32 frame;

    petals = &work->petals;
    band = work->bands;
    sheet = work->sheets;
    ring = work->rings;
    spoke = work->spokes;
    quad_record = work->quads;
    web = work->webs;
    record = work->records;
    textured = work->textured;
    flat = work->flat_textured;
    triangle = &work->triangle;
    quad = &work->quad;
    extra = &work->extra;
    sprite = &work->sprite;
    result = 0;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant407EntryConfig *)(D_8013C8C8 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(512, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(512, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C8 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C8 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C8 + 0x1C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C8 + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C8 + 0x54));
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
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        SetPolyFT4(sprite);
        sprite->tpage = packed >> 16;
        setUV4(sprite, 64, 64, 127, 64, 64, 127, 127, 127);
        sprite->clut = packed;
        SetSemiTrans(sprite, 1);
        SetShadeTex(sprite, 0);
        for (i = 0; i < 48; i++) {
            setVector(&petals->v0[i], -work->config->petal_size, -work->config->petal_size, 0);
            setVector(&petals->v1[i], work->config->petal_size, -work->config->petal_size, 0);
            setVector(&petals->v2[i], -work->config->petal_size, work->config->petal_size, 0);
            setVector(&petals->v3[i], work->config->petal_size, work->config->petal_size, 0);
            petals->color.r = work->config->outer.r;
            petals->color.g = work->config->outer.g;
            petals->color.b = work->config->outer.b;
            petals->scale[i] = -(i * 1024 / 48);
            petals->field_854[i] = 0;
            petals->field_914[i] = 0;
            petals->done[i] = 0;
            petals->origin[i].vx = work->matrix.t[0] + work->direction.vx;
            petals->origin[i].vy = work->matrix.t[1] + work->direction.vy;
            petals->origin[i].vz = work->matrix.t[2] + work->direction.vz;
        }
        for (band_index = 0; band_index < 1; band_index++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 2; j++) {
                band->ca[j].r = work->config->inner.r;
                band->ca[j].g = work->config->inner.g;
                band->ca[j].b = work->config->inner.b;
                band->cb[j].r = work->config->outer.r;
                band->cb[j].g = work->config->outer.g;
                band->cb[j].b = work->config->outer.b;
                band->offset[j] = -(j * 768);
            }
            band->field_90 = 0;
            band->field_92 = 0;
            band->field_94 = 0;
        }
        for (i = 0; i < 1; i++, quad_record++) {
            setVector(&quad_record->point[0], 0, 0, 0);
            for (j = 0, angle = 0; j < 5; j++, angle = j * 1024) {
                setVector(&quad_record->point[j + 1], (u32)rcos(angle) >> 6,
                          (u32)rsin(angle) >> 8, 0);
                setVector(&quad_record->point[j + 6], (u32)(rcos(angle) * 3) >> 10,
                          (u32)(rsin(angle) * 5) >> 9, 0);
            }
            setVector(&quad_record->corner[0], -128, -128, 0);
            setVector(&quad_record->corner[1], 128, -128, 0);
            setVector(&quad_record->corner[2], -128, 128, 0);
            setVector(&quad_record->corner[3], 128, 128, 0);
            quad_record->inner[0] = 255;
            quad_record->inner[1] = 192;
            quad_record->inner[2] = 192;
            quad_record->outer[0] = 64;
            quad_record->outer[1] = 64;
            quad_record->outer[2] = 0;
            quad_record->size = 0;
            quad_record->field_84 = 0;
            quad_record->done = 0;
        }
        for (i = 0; i < 2; i++, sheet++) {
            for (k = 0; k < 4; k++) {
                if (k == 0) {
                    setVector(&sheet->v0[k], -64, -64, 0);
                    setVector(&sheet->v1[k], 0, -64, 0);
                    setVector(&sheet->v2[k], -64, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], 64, -64, 0);
                    setVector(&sheet->v1[k], 64, 0, 0);
                    setVector(&sheet->v2[k], 0, -64, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], 64, 64, 0);
                    setVector(&sheet->v1[k], 0, 64, 0);
                    setVector(&sheet->v2[k], 64, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -64, 64, 0);
                    setVector(&sheet->v1[k], -64, 0, 0);
                    setVector(&sheet->v2[k], 0, 64, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->outer[0] = work->config->outer.r;
            sheet->outer[1] = work->config->outer.g;
            sheet->outer[2] = work->config->outer.b;
            sheet->inner[0] = work->config->inner.r;
            sheet->inner[1] = work->config->inner.g;
            sheet->inner[2] = work->config->inner.b;
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
        }
        near_scale = 256;
        far_scale = 192;
        for (i = 0, web_angle = 0; i < 3; i++, web++, web_angle += 300) {
            for (row = 0, j = 2048 / 5; row < 4; row++, j = (row + 1) * 2048 / 5) {
                near_radius = rsin(j) * near_scale >> 12;
                near_y = rcos(j) * near_scale >> 12;
                far_radius = rsin(j) * far_scale >> 12;
                far_y = rcos(j) * far_scale >> 12;
                for (j = 0, angle = web_angle; j < 6; j++, angle = web_angle + j * 4096 / 6) {
                    setVector(&web->near[row][j], rcos(angle) * near_radius >> 12,
                              near_y, rsin(angle) * near_radius >> 12);
                    setVector(&web->far[row][j], rcos(angle) * far_radius >> 12,
                              far_y, rsin(angle) * far_radius >> 12);
                }
            }
            MODEL_VARIANT_WORD(web, 0x184) = 0;
            MODEL_VARIANT_WORD(web, 0x188) = 0;
            MODEL_VARIANT_WORD(web, 0x18C) = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = 4096 + i * 4096 / 3;
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (i = 0; i < 16; i++, record++) {
            for (j = 0; j < 9; j++) {
                record->color[j].r = 192 - j * 24;
                record->color[j].g = 192 - j * 24;
                record->color[j].b = 255;
                record->offset[j] = -(j * 256);
            }
            record->field_6C = 0;
            record->field_70 = 0;
            record->field_74 = 0;
            record->field_A0 = 0;
            record->field_A4 = 0;
        }
        for (row = 0, j = 0; row < 6; row++, ring++, j += 256) {
            for (i = 0, angle = j; i < 8; angle += 512, i++) {
                setVector(&ring->inner[i], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&ring->outer[i], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
            }
            ring->color[0] = 64;
            ring->color[1] = 128;
            ring->color[2] = 255;
            ring->pad84[0] = 0;
            ring->pad84[1] = 0;
            ring->pad84[2] = 0;
            ring->angle = row * 1024 / 6;
            ring->count = 0;
        }
        for (row = 0, j = 0; row < 4; row++, spoke++, j += 200) {
            for (i = 0, angle = j; i < 8; angle += 512, i++) {
                setVector(&spoke->inner[i], (u32)(rcos(angle) * 3) >> 5,
                          (u32)(rsin(angle) * 3) >> 5, 0);
                setVector(&spoke->outer[i], (u32)rcos(angle) >> 3, (u32)rsin(angle) >> 3, 0);
            }
            spoke->pad80[0] = 0;
            spoke->pad80[1] = 0;
            spoke->pad80[2] = 0;
            spoke->color[0] = 255;
            spoke->color[1] = 192;
            spoke->color[2] = 64;
            spoke->angle = 1024 - row * 256;
            spoke->count = 0;
            default_scale = 4096;
        }
        work->field_29E8 = 0;
        work->field_29F0 = default_scale;
        work->field_29F2 = 1024;
        work->field_29F4 = 0;
        work->field_29F8 = 0;
        work->field_29EA = 0;
        work->sweep = 0;
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
        if (work->frame < work->config->track_end) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        if (work->config->sweep_start <= work->frame && work->frame < work->config->sweep_end) {
            work->sweep = work->config->sweep_angle * ((work->frame - work->config->sweep_start) << 12) /
                          (work->config->sweep_end - work->config->sweep_start);
        } else {
            work->sweep = 0;
        }
        petals = &work->petals;
        for (i = 0; i < 48; i++) {
            if (petals->scale[i] >= 0 && petals->done[i] == 0) {
                if (work->slot == 0) {
                    setVector(&work->positions[i], work->target.vx + (rsin(work->sweep) * 768 >> 12),
                              work->target.vy, work->target.vz);
                } else {
                    setVector(&work->positions[i], work->target.vx - (rsin(work->sweep) * 768 >> 12),
                              work->target.vy, work->target.vz);
                }
                petals->done[i] = 1;
            }
            setVector(&work->offsets[i], work->positions[i].vx - work->matrix.t[0],
                      work->positions[i].vy - work->matrix.t[1],
                      work->positions[i].vz - work->matrix.t[2]);
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
        work->screen_delta.vx = (u16)projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->start <= work->frame) {
            func_8013C2DC((u8 *)point);
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
