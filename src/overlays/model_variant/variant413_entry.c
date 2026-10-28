#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "variant413_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT413_CLUT_X
#define MODEL_VARIANT413_CLUT_X 512
#endif

/* Header 413's entry: the header-412 entry with three 32-sprite swarm groups
 * in place of the trail and flares, released toward the opposite slot and
 * drawn by the shared swarm helper. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant413EntryState *work = (Variant413EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant413EntryGroup *group;
    Variant412EntryBand *band;
    ModelVariantSheet *sheet;
    ModelVariantRing *ring;
    ModelVariantSpokeRing *spoke;
    Variant412EntryQuad *quad_record;
    ModelVariantWebNarrow *web;
    Variant412EntryRecord *record;
    POLY_GT4 *textured;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    POLY_GT4 *extra;
    POLY_FT4 *flat;
    POLY_FT4 *sprite;
    s32 result;
    s32 web_angle, n, row;
    s32 i, j, k, angle;
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

    group = work->groups;
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
        work->config = (Variant413EntryConfig *)(D_8013C8C0 + 0xFC) + command;
        for (n = 0; n < 3; n++) {
            work->parts[n] = Model_GetSlotDataEntry(work->slot, work->config->parts[n]);
        }
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
        clut = GetClut(MODEL_VARIANT413_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT413_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C0 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C0 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C0 + 0x1C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C0 + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013C8C0 + 0x54));
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
        for (n = 0; n < 3; n++, group++) {
            for (i = 0; i < 32; i++) {
                setVector(&group->v0[i], -256, -256, 0);
                setVector(&group->v1[i], 256, -256, 0);
                setVector(&group->v2[i], -256, 256, 0);
                setVector(&group->v3[i], 256, 256, 0);
                group->color.r = work->config->outer.r;
                group->color.g = work->config->outer.g;
                group->color.b = work->config->outer.b;
                group->scale[i] = -(i * 1024 / 32);
                group->field_594[i] = 0;
                group->field_614[i] = 0;
                group->done[i] = 0;
                group->origin[i].vx = work->matrix.t[0] + work->direction.vx;
                group->origin[i].vy = work->matrix.t[1] + work->direction.vy;
                group->origin[i].vz = work->matrix.t[2] + work->direction.vz;
            }
        }
        for (n = 0; n < 1; n++, band++) {
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
            for (i = 0, angle = j; i < 8; i++, angle = j + i * 512) {
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
        }
        work->field_3AF0 = 0;
        work->field_3AF8 = 4096;
        work->field_3AFA = 1024;
        work->field_3AFC = 0;
        work->field_3B00 = 0;
        work->field_3AF2 = 0;
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
        angle = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[0] = angle;
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        if (work->frame < work->config->origin_end) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        if (work->config->sweep_start <= work->frame && work->frame < work->config->sweep_end) {
            work->sweep = (work->frame - work->config->sweep_start) * 4096 /
                          (work->config->sweep_end - work->config->sweep_start);
        } else {
            work->sweep = 0;
        }
        for (group = work->groups, n = 0; n < 3; n++, group++) {
            if (work->frame < work->config->origin_end) {
                GsGetLwUnit(work->parts[n], &work->origins[n]);
            }
            for (i = 0; i < 32; i++) {
                if (group->scale[i] >= 0 && group->done[i] == 0) {
                    if (work->slot == 0) {
                        setVector(&work->positions[n][i],
                                  work->target.vx + (rsin(work->sweep) * 768 >> 12),
                                  work->target.vy, work->target.vz);
                    } else {
                        setVector(&work->positions[n][i],
                                  work->target.vx - (rsin(work->sweep) * 768 >> 12),
                                  work->target.vy, work->target.vz);
                    }
                    group->done[i] = 1;
                }
                setVector(&work->offsets[n][i], work->positions[n][i].vx - work->origins[n].t[0],
                          work->positions[n][i].vy - work->origins[n].t[1],
                          work->positions[n][i].vz - work->origins[n].t[2]);
            }
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
            func_8013C2F0((u8 *)point);
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

/* The swarm body is the header-430 French helper, compiled here with GCC 2.7.2. */
#define func_8017C2EC func_8013C2F0
#include "../french_model_variant/image132118_swarm.c"
#undef func_8017C2EC
