#include "../../types.h"
#include "variant385_entry.h"

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant385EntryState *work = (Variant385EntryState *)point;
    u8 unknown_stack[64];
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant385EntryWeb *web = work->webs;
    Variant385EntryTube *tube = work->tubes;
    Variant385EntrySheet *sheet = work->sheets;
    Variant385EntryCurtain *curtain = work->curtains;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *plain = &work->plain;
    POLY_GT4 *paged = &work->paged;
    POLY_FT4 *flat = work->flat_textured;
    s32 result = 0;
    u16 tpage, clut;
    s32 packed, flat_page, paged_page;
    u16 flat_clut, paged_clut;
    s32 size;
    s32 base_angle, n, row, near_y, far_y;
    s32 i, j, angle, ang;
    s32 k;
    s32 near_scale, far_scale;
    s32 near_r, far_r;
    s32 r, g, b;
    s32 tr, tg, tb;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant385EntryConfig *)(D_8013DEB4 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        GsGetLwUnit(work->parts[0], &work->matrix);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
            work->matrix.t[0] += work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] -= work->config->offset[2];
        } else {
            setVector(&work->target, 0, -300, 450);
            work->matrix.t[0] += work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] += work->config->offset[2];
        }
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DEB4 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DEB4 + 0x8C));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DEB4 + 0x1C));
        paged_page = packed >> 16;
        paged_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DEB4 + 0x38));
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013DEB4 + 0x54));
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
        SetPolyGT4(plain);
        SetSemiTrans(plain, 1);
        SetShadeTex(plain, 0);
        SetPolyGT4(paged);
        paged->tpage = paged_page;
        setUV4(paged, 128, 207, 255, 207, 128, 128, 255, 128);
        paged->clut = paged_clut;
        SetSemiTrans(paged, 1);
        SetShadeTex(paged, 0);
        far_scale = 192;
        near_scale = 256;
        for (n = 0, base_angle = 0; n < 3; n++, web++, base_angle += 300) {
            for (row = 0, ang = 2048 / 5; row < 4; row++, ang = (row + 1) * 2048 / 5) {
                near_r = rsin(ang) * near_scale >> 12;
                near_y = rcos(ang) * near_scale >> 12;
                far_r = rsin(ang) * far_scale >> 12;
                far_y = rcos(ang) * far_scale >> 12;
                for (angle = base_angle, j = 0; j < 6; j++, angle = base_angle + j * 4096 / 6) {
                    setVector(&web->near[row][j], rcos(angle) * near_r >> 12, near_y, rsin(angle) * near_r >> 12);
                    setVector(&web->far[row][j], rcos(angle) * far_r >> 12, far_y, rsin(angle) * far_r >> 12);
                }
            }
            web->field_184 = 0;
            web->field_188 = 0;
            web->field_18C = 0;
            web->color.r = 128;
            web->color.g = 128;
            web->color.b = 128;
            web->scale = n * 4096 / 3 + 4096;
            web->field_198 = 0;
            web->field_19C = 0;
        }
        for (i = 0; i < 1; i++, tube++) {
            tube->field_6C = 0;
            tube->field_70 = 0;
            tube->field_6E = 0;
        }
        for (n = 0; n < 2; n++, sheet++) {
            for (k = 0; k < 4; k++) {
                size = work->config->size;
                if (k == 0) {
                    setVector(&sheet->v0[k], -size, -size, 0);
                    setVector(&sheet->v1[k], 0, -size, 0);
                    setVector(&sheet->v2[k], -size, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 1) {
                    setVector(&sheet->v0[k], size, -size, 0);
                    setVector(&sheet->v1[k], size, 0, 0);
                    setVector(&sheet->v2[k], 0, -size, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 2) {
                    setVector(&sheet->v0[k], size, size, 0);
                    setVector(&sheet->v1[k], 0, size, 0);
                    setVector(&sheet->v2[k], size, 0, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                } else if (k == 3) {
                    setVector(&sheet->v0[k], -size, size, 0);
                    setVector(&sheet->v1[k], -size, 0, 0);
                    setVector(&sheet->v2[k], 0, size, 0);
                    setVector(&sheet->v3[k], 0, 0, 0);
                }
            }
            sheet->field_80 = 0;
            sheet->field_84 = 0;
            sheet->field_88 = 0;
        }
        for (n = 0; n < 4; n++, curtain++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&curtain->inner[j], rcos(angle) * work->config->inner_radius >> 12,
                          rsin(angle) * work->config->inner_radius >> 12, 0);
                setVector(&curtain->outer[j], rcos(angle) * work->config->outer_radius >> 12,
                          rsin(angle) * work->config->outer_radius >> 12, -64);
            }
            if (n == 3) {
                curtain->angle = 0;
            } else {
                curtain->angle = -(n * 4096 / 3);
            }
            curtain->field_114 = 0;
        }
        work->field_1028 = 1024;
        work->color[0] = 192;
        work->color[1] = 192;
        work->color[2] = 192;
        work->field_101E = 255;
        work->field_102A = 0;
        work->field_101C = 0;
        work->field_101D = 0;
        work->field_1020 = 0;
        work->field_1022 = 0;
        work->field_1024 = 0;
        work->field_1026 = 0;
        work->field_102C = 0;
        work->field_102E = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_1034 = 0;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
            work->matrix.t[0] -= work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] -= work->config->offset[2];
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
            work->matrix.t[0] += work->config->offset[0];
            work->matrix.t[1] += work->config->offset[1];
            work->matrix.t[2] += work->config->offset[2];
        }
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
        work->index = 0;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->count != 0) {
            do {
                if (work->index == 0) {
                    GsGetLwUnit(work->parts[0], &work->matrix);
                    if (work->slot == 0) {
                        work->matrix.t[0] -= work->config->offset[0];
                        work->matrix.t[1] += work->config->offset[1];
                        work->matrix.t[2] -= work->config->offset[2];
                    } else {
                        work->matrix.t[0] += work->config->offset[0];
                        work->matrix.t[1] += work->config->offset[1];
                        work->matrix.t[2] += work->config->offset[2];
                    }
                } else if (work->index == 1) {
                    GsGetLwUnit(work->parts[1], &work->matrix);
                } else if (work->index == 2) {
                    GsGetLwUnit(work->parts[2], &work->matrix);
                }
                work->direction.vx = work->target.vx - work->matrix.t[0];
                work->direction.vy = work->target.vy - work->matrix.t[1];
                work->direction.vz = work->target.vz - work->matrix.t[2];
                if (work->config->start <= work->frame) {
                    func_8013D1B8((u8 *)point);
                }
                if (work->phase > 0) {
                    func_8013CB00((u8 *)point);
                    func_8013C478((u8 *)point);
                }
            } while (++work->index < work->config->count);
        }
        if (work->config->start <= work->frame) {
            func_8013D6B4((u8 *)point);
            if (work->phase <= 0) {
                func_8013BF5C((u8 *)point);
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
    if (work->phase >= 2 && work->phase <= 5) {
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
