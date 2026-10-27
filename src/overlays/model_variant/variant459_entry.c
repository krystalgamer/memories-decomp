#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "variant459_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifndef MODEL_VARIANT459_CLUT_X
#define MODEL_VARIANT459_CLUT_X 512
#endif

/* Header 459's entry: three webs, sixteen glows, a sheet, the two grids and
 * five curtains, with the header-337 camera and part tracking and the five
 * draw helpers of header 459. The setup is written for the retail register
 * allocation: the grid rows are indexed from `row` inside the loops, the
 * texture words of the veil upload go through a fresh `packed2` and the last
 * halfword of the 64-byte stack pad, and some locals (`ang`, `frame`) hold
 * values that only live a few instructions. */

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant459EntryState *work = (Variant459EntryState *)point;
    u16 unknown_stack[32];
    SVECTOR position, target;
    VECTOR squared;
    Variant337EntryProjection projection;
    Variant476ScreenGrid *grid_b;
    Variant459Curtain *curtain;
    ModelVariantSheet *sheet;
    ModelVariantWeb *web;
    Variant459EntryGlow *glow;
    POLY_G4 *quad;
    POLY_GT4 *plain;
    POLY_GT4 *veil;
    POLY_GT4 *ray;
    POLY_FT4 *flat;
    s32 result;
    POLY_GT4 *textured;
    POLY_G3 *triangle;
    Variant459Grid *grid_a;
    GsRVIEW2 *view;
    s32 angle, base_angle;
    s32 f2;
    u8 *gb_colp;
    s32 f61;
    s32 f64;
    u16 ray_clut, veil_clut;
    s32 f8;
    s32 f56;
    s32 f30;
    s32 f51;
    s32 n;
    s32 web_angle, outer, row;
    s32 f31;
    s32 f16;
    s32 f33;
    s32 f87;
    s32 f79;
    s32 f11;
    s32 veil_page, ray_page;
    s32 veil_tpage;
    s32 packed2;
    s32 f59;
    s32 f90;
    s32 f34;
    s32 gb_base;
    s32 f38;
    s32 f1;
    s32 f23;
    s32 f67;
    s32 f70;
    s32 f10;
    s32 f35;
    s32 f41;
    s32 f42;
    s32 f24;
    s32 f85;
    s32 f89;
    s32 f6;
    s32 f77;
    s32 f86;
    s32 f46;
    s32 f82;
    s32 f66;
    s32 f43;
    s32 by_a, by_b, wy_a, wy_b;
    s32 f76;
    s32 f57;
    s32 f88;
    s32 f81;
    s32 f12;
    s32 f78;
    s32 f73;
    s32 f72;
    s32 f99;
    s32 f71;
    s32 f65;
    s32 f84;
    s32 f68;
    s32 f15;
    s32 f74;
    s32 f49;
    s32 f58;
    s32 f52;
    s32 f25;
    s32 f53;
    s32 f20;
    s32 f7;
    s32 f62;
    s32 f60;
    s32 f26;
    s32 f36;
    s32 f69;
    s32 f91;
    s32 f96;
    s32 f29;
    s32 f80;
    s32 f54;
    s32 near_scale, far_scale;
    s32 i, j, k, m;
    s32 f98;
    s32 f97;
    s32 f95;
    s32 f94;
    s32 f93;
    s32 f92;
    s32 f44;
    s32 f45;
    s32 f21;
    s32 f28;
    SVECTOR *ga_rowp;
    s32 f39;
    s32 f32;
    s32 f50;
    s32 f27;
    s32 f37;
    s32 f48;
    s32 f14;
    s32 f83;
    s32 f19;
    s32 f63;
    s32 f75;
    s32 f18;
    s32 f40;
    s32 f55;
    s32 f13;
    s32 packed, flat_page;
    s32 ang;
    s32 f47;
    s32 f3;
    s32 f17;
    s32 f22;
    s32 f9;
    s32 f5;
    s32 f4;
    s32 gr, gy, br_a, br_b, wr_a, wr_b;
    u8 *ga_colp;
    SVECTOR *gb_rowp;
    u8 *wb_rowp;
    u16 tpage, clut, flat_clut;
    u32 screen_x;
    s32 screen_y;
    s32 dx, dy;
    u32 frame;
    s32 result_tmp;

    curtain = work->curtains;
    sheet = &work->sheet;
    glow = work->glows;
    grid_a = &work->grid;
    grid_b = &work->screen_grid;
    textured = work->textured;
    flat = work->flat_textured;
    triangle = &work->triangle;
    quad = &work->quad;
    plain = &work->plain;
    veil = &work->veil;
    web = work->webs;
    result = 0;
    ray = &work->ray;
    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant459EntryConfig *)(D_8013E338 + 0xFC) + command;
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        row = 0;
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction,
                  work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1],
                  work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT459_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT459_CLUT_X, 244);
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013E338 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013E338 + 0x8C));
        veil_page = packed >> 16;
        veil_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013E338 + 0x1C));
        ray_page = packed >> 16;
        ray_clut = packed;
        packed = func_80059A50(work->slot, 1, (GsIMAGE *)(D_8013E338 + 0xE0));
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
        SetPolyGT4(veil);
        veil->tpage = veil_page;
        setUV4(veil, 192, 64, 255, 64, 192, 127, 255, 127);
        veil->clut = veil_clut;
        SetSemiTrans(veil, 1);
        SetShadeTex(veil, 0);
        SetPolyGT4(plain);
        SetSemiTrans(plain, 1);
        SetShadeTex(plain, 0);
        SetPolyGT4(ray);
        ray->tpage = ray_page;
        setUV4(ray, 128, 207, 255, 207, 128, 128, 255, 128);
        ray->clut = ray_clut;
        SetSemiTrans(ray, 1);
        SetShadeTex(ray, 0);
        for (; row < 9; row++) {
            ga_rowp = (SVECTOR *)grid_a + row * 17;
            f60 = rsin(row * 256) << 8 >> 12;
            f40 = rcos(row * 256) << 8 >> 12;
            for (j = 0, ang = 0; j < 17; j++, ang = j << 8) {
                setVector(&ga_rowp[j], rcos(ang) * f60 >> 12, f40, rsin(ang) * f60 >> 12);
            }
            ((u8 *)grid_a)[row * 4 + 0x4C8] = (row * 256 - row) / 8;
            ((u8 *)grid_a)[row * 4 + 0x4C9] = 64;
            ((u8 *)grid_a)[row * 4 + 0x4CA] = ~((row * 256 - row) / 8);
        }
        gb_base = 0;
        row = 0;
        for (; row < 9; gb_base += 256, row++) {
            gb_rowp = (SVECTOR *)grid_b + row * 9;
            f98 = (rsin(gb_base) * 480 >> 12) * 640 / 1024;
            f99 = rcos(gb_base) * 480 >> 12;
            f93 = (rsin(gb_base) << 9 >> 12) * 640 / 1024;
            f71 = rcos(gb_base) << 9 >> 12;
            for (f28 = 0, angle = 0; f28 < 9; f28++, angle = f28 << 9) {
                setVector(&gb_rowp[f28], rcos(angle) * f98 >> 12, f99, rsin(angle) * f98 >> 12);
                setVector(&gb_rowp[81 + f28], rcos(angle) * f93 >> 12, f71, rsin(angle) * f93 >> 12);
            }
            ((u8 *)grid_b)[row * 4 + 0x510] = (gb_base - row) / 8;
            ((u8 *)grid_b)[row * 4 + 0x511] = 64;
            ((u8 *)grid_b)[row * 4 + 0x512] = ~((gb_base - row) / 8);
        }
        for (n = 0; n < 5; n++, curtain++) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&curtain->a[j], (u32)(rcos(angle) * 3) >> 4, 0, (u32)(rsin(angle) * 3) >> 4);
                setVector(&curtain->b[j], (u32)(rcos(angle) * 9) >> 5, 16, (u32)(rsin(angle) * 9) >> 5);
                setVector((SVECTOR *)((u8 *)curtain + 0x110) + j, (u32)(rcos(angle) * 3) >> 3, 16,
                          (u32)(rsin(angle) * 3) >> 3);
            }
            curtain->rotation.vx = (n << 11) / 5;
            curtain->rotation.vy = 0;
            curtain->rotation.vz = n << 10;
            curtain->scale = n * -6144 / 5;
            MODEL_VARIANT_WORD(curtain, 0x1A4) = n * -1024 / 5;
            MODEL_VARIANT_WORD(curtain, 0x1A8) = 0;
        }
        for (n = 0; n < 1; n++, sheet++) {
            for (f6 = 0; f6 < 4; f6++) {
                if (f6 == 0) {
                    setVector(&sheet->v0[f6], -256, -256, 0);
                    setVector(&sheet->v1[f6], 0, -256, 0);
                    setVector(&sheet->v2[f6], -256, 0, 0);
                    setVector(&sheet->v3[f6], 0, 0, 0);
                } else if (f6 == 1) {
                    setVector(&sheet->v0[f6], 256, -256, 0);
                    setVector(&sheet->v1[f6], 256, 0, 0);
                    setVector(&sheet->v2[f6], 0, -256, 0);
                    setVector(&sheet->v3[f6], 0, 0, 0);
                } else if (f6 == 2) {
                    setVector(&sheet->v0[f6], 256, 256, 0);
                    setVector(&sheet->v1[f6], 0, 256, 0);
                    setVector(&sheet->v2[f6], 256, 0, 0);
                    setVector(&sheet->v3[f6], 0, 0, 0);
                } else if (f6 == 3) {
                    setVector(&sheet->v0[f6], -256, 256, 0);
                    setVector(&sheet->v1[f6], -256, 0, 0);
                    setVector(&sheet->v2[f6], 0, 256, 0);
                    setVector(&sheet->v3[f6], 0, 0, 0);
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
        far_scale = 320;
        near_scale = 512;
        for (n = 0, f30 = 0; n < 3; n++, web++, f30 += 300) {
            for (row = 0, ang = 2048 / 7; row < 6; row++, ang = (row + 1) * 2048 / 7) {
                f94 = rsin(ang) * near_scale >> 12;
                f19 = rcos(ang) * near_scale >> 12;
                f34 = rsin(ang) * far_scale >> 12;
                f18 = rcos(ang) * far_scale >> 12;
                for (angle = f30, f15 = 0; f15 < 6; f15++, angle = f30 + f15 * 4096 / 6) {
                    setVector(&web->near[row][f15], rcos(angle) * f94 >> 12, f19, rsin(angle) * f94 >> 12);
                    setVector(&web->far[row][f15], rcos(angle) * f34 >> 12, f18, rsin(angle) * f34 >> 12);
                }
            }
            MODEL_VARIANT_WORD(web, 0x244) = 0;
            MODEL_VARIANT_WORD(web, 0x248) = 0;
            MODEL_VARIANT_WORD(web, 0x24C) = 0;
            web->color.r = work->config->web.r;
            web->color.g = work->config->web.g;
            web->color.b = work->config->web.b;
            web->scale = -(n * 4096 / 3);
            MODEL_VARIANT_WORD(web, 0x258) = 0;
            MODEL_VARIANT_WORD(web, 0x25C) = 0;
        }
        for (f12 = 0; f12 < 16; glow++, f12++) {
            for (j = 0; j < 2; j++) {
                if (j == 1) {
                    glow->outer[j].r = 0;
                    glow->outer[j].g = 0;
                    glow->outer[j].b = 0;
                    glow->inner[j].r = 0;
                    glow->inner[j].g = 0;
                    glow->inner[j].b = 0;
                } else {
                    glow->outer[j].r = 255;
                    glow->outer[j].g = 0;
                    glow->outer[j].b = 255;
                    glow->inner[j].r = 128;
                    glow->inner[j].g = 128;
                    glow->inner[j].b = 128;
                }
            }
        }
        work->field_27FC = 0;
        work->field_2800 = 0;
        work->field_2804 = 0;
        work->field_2808 = 0;
        work->field_280C = 1024;
        work->field_2810 = 0;
        work->field_2814 = 0;
        work->field_2818 = 0;
        work->field_281C = 0;
        work->field_2820 = 0;
        work->field_2824 = 0;
        work->field_2828 = 1024;
        work->field_282C = 0;
        work->field_2830 = 0;
        for (n = 4; n >= 0; n--) {
            work->zeroes[n] = 0;
        }
        work->field_27EC = 1024;
        work->time = 0;
        work->field_27E4 = 0;
        work->field_27E8 = 0;
        work->field_27F0 = 0;
        work->field_27D4 = 0;
        work->field_27D8 = 4096;
        work->field_27DA = 1024;
        work->field_27DC = 0;
        work->field_27F4 = 0;
        work->field_27F8 = 0;
        work->field_27D6 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_2850 = 0;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
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
        dy = projection.target.vy - screen_y;
        work->screen_delta.vx = dx;
        work->screen_delta.vy = dy;
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        work->position.vx = work->matrix.t[0] + work->direction.vx * work->time / 1536;
        work->position.vy = work->matrix.t[1] + work->direction.vy * work->time / 1536
                            - (rsin(work->time) << 8 >> 12);
        work->position.vz = work->matrix.t[2] + (work->direction.vz * work->time / 2048);
        work->offset.vx = work->target.vx - work->position.vx;
        work->offset.vy = work->target.vy - work->position.vy;
        work->offset.vz = work->target.vz - work->position.vz;
        if (work->config->start <= work->frame && work->frame < work->config->fade_start) {
            func_8013C360((u8 *)point);
        }
        if (work->config->second <= work->frame && work->frame < work->config->fade_start) {
            func_8013C714((u8 *)point);
        }
        if (work->config->fade_start <= work->frame && work->frame <= work->config->fade_end + 4) {
            if (work->frame <= work->config->fade_end - 10) {
                work->facing = ratan2(work->direction.vy, work->direction.vz) + 2048;
            }
            func_8013DD90((u8 *)point);
        }
        if (work->config->fade_end <= work->frame) {
            if (work->phase < 2) {
                work->phase = 2;
            }
            func_8013D430((u8 *)point);
            if (work->phase < 6) {
                func_8013D800((u8 *)point);
                if (work->phase >= 5) {
                    packed2 = func_80059A50(work->slot, 2, (GsIMAGE *)(D_8013E338 + 0));
                    unknown_stack[31] = packed2;
                    frame = packed2 >> 16;
                    SetPolyGT4(veil);
                    veil->tpage = frame;
                    veil->clut = unknown_stack[31];
                    SetSemiTrans(veil, 1);
                    SetShadeTex(veil, 0);
                }
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
