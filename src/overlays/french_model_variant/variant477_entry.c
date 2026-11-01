#include "../../types.h"
#include "variant477_entry.h"

s32 FrenchModelVariant477_HandleRequest(SVECTOR *point, s32 command)
{
    EntryState477 *work = (EntryState477 *)point;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection477 projection;
    EntryPanel477 *panel;
    EntryGroup477 *group;
    EntryBand477 *band = work->bands;
    EntrySheet477 *sheet = work->sheets;
    EntrySmallRing477 *ring_a = work->rings_a;
    EntrySmallRing477 *ring_b = work->rings_b;
    EntryShape477 *shape = work->shapes;
    ModelVariantWebNarrow *web = work->webs;
    EntryTrail477 *trail = work->trails;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *extra = &work->extra;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *quads_packet = &work->quads_packet;
    POLY_FT4 *panels_packet = &work->panels_packet;
    s32 result = 0;
    u16 flat_clut, panels_clut;
    s32 web_base, n, i;
    s32 packed, panels_page;
    u16 tpage, clut;
    s32 flat_page;
    s32 j, k, angle, base, delay, sheet_point;
    s32 ring_angle;
    s32 near_scale, far_scale, reset_value;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig477 *)(gFrenchModelVariant477_D1DC0 + 0xFC) + command;
        for (n = 0; n < 2; n++) {
            work->parts[n] = Model_GetSlotDataEntry(work->slot, work->config->parts[n]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        panel = work->panels;
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(640, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(640, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant477_D1DC0 + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant477_D1DC0 + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant477_D1DC0 + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant477_D1DC0 + 0x38));
        panels_page = packed >> 16;
        panels_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(gFrenchModelVariant477_D1DC0 + 0x54));
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
        SetPolyFT4(panels_packet);
        panels_packet->tpage = panels_page;
        setUV4(panels_packet, 128, 0, 255, 0, 128, 63, 255, 63);
        panels_packet->clut = panels_clut;
        SetSemiTrans(panels_packet, 0);
        SetShadeTex(panels_packet, 1);
        SetPolyFT4(quads_packet);
        quads_packet->tpage = packed >> 16;
        setUV4(quads_packet, 64, 64, 127, 64, 64, 127, 127, 127);
        quads_packet->clut = packed;
        SetSemiTrans(quads_packet, 1);
        SetShadeTex(quads_packet, 0);
        for (n = 0; n < 2; n++, panel++) {
            for (k = 0; k < 1; k++) {
                setVector(&panel->a[k], -256, -256, 0);
                setVector(&panel->b[k], 256, -256, 0);
                setVector(&panel->c[k], -256, 256, 0);
                setVector(&panel->d[k], 256, 256, 0);
                panel->color.r = work->config->inner.r;
                panel->color.g = work->config->inner.g;
                panel->color.b = work->config->inner.b;
                panel->size[k] = -1;
                panel->done[k] = 0;
                panel->field_44[k] = 0;
                panel->initialized[k] = 0;
                panel->positions[k].vx = work->matrix.t[0] + work->direction.vx;
                panel->positions[k].vy = work->matrix.t[1] + work->direction.vy;
                panel->positions[k].vz = work->matrix.t[2] + work->direction.vz;
            }
            work->enabled[n] = 0;
        }
        group = work->groups;
        for (n = 0; n < 2; n++, group++) {
            for (k = 0; k < 10; k++) {
                setVector(&group->a[k], -256, -256, 0);
                setVector(&group->b[k], 256, -256, 0);
                setVector(&group->c[k], -256, 256, 0);
                setVector(&group->d[k], 256, 256, 0);
                group->color.r = work->config->inner.r;
                group->color.g = work->config->inner.g;
                group->color.b = work->config->inner.b;
                group->angle[k] = 0;
                group->done[k] = 0;
                group->field_20C[k] = 0;
                group->initialized[k] = 0;
                group->size[k] = -(k * 4096 / 10);
                group->positions[k].vx = work->matrix.t[0] + work->direction.vx;
                group->positions[k].vy = work->matrix.t[1] + work->direction.vy;
                group->positions[k].vz = work->matrix.t[2] + work->direction.vz;
            }
        }
        for (n = 0; n < 1; n++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (j = 0; j < 2; j++) {
                band->outer[j].r = work->config->outer.r;
                band->outer[j].g = work->config->outer.g;
                band->outer[j].b = work->config->outer.b;
                band->inner[j].r = work->config->inner.r;
                band->inner[j].g = work->config->inner.g;
                band->inner[j].b = work->config->inner.b;
                band->progress[j] = -j * 768;
            }
            band->field_90 = 0;
            band->field_92 = 0;
            band->field_94 = 0;
        }
        for (k = 0; k < 1; k++, shape++) {
            setVector(&shape->origin, 0, 0, 0);
            for (j = 0, angle = 0; j < 5; j++, angle = j * 1024) {
                setVector(&shape->inner[j], (u32)rcos(angle) >> 6,
                    (u32)rsin(angle) >> 8, 0);
                setVector(&shape->outer[j], (u32)(rcos(angle) * 3) >> 10,
                    (u32)(rsin(angle) * 5) >> 9, 0);
            }
            shape->inner_color.r = 255;
            shape->inner_color.g = 192;
            shape->inner_color.b = 192;
            setVector(&shape->corners[0], -128, -128, 0);
            setVector(&shape->corners[1], 128, -128, 0);
            setVector(&shape->corners[2], -128, 128, 0);
            setVector(&shape->corners[3], 128, 128, 0);
            shape->outer_color.r = 64;
            shape->outer_color.g = 64;
            shape->outer_color.b = 0;
            shape->field_80 = 0;
            shape->field_84 = 0;
            shape->field_88 = 0;
        }
        for (k = 0; k < 2; k++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->points[0][sheet_point], -64, -64, 0);
                    setVector(&sheet->points[1][sheet_point], 0, -64, 0);
                    setVector(&sheet->points[2][sheet_point], -64, 0, 0);
                    setVector(&sheet->points[3][sheet_point], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->points[0][sheet_point], 64, -64, 0);
                    setVector(&sheet->points[1][sheet_point], 64, 0, 0);
                    setVector(&sheet->points[2][sheet_point], 0, -64, 0);
                    setVector(&sheet->points[3][sheet_point], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->points[0][sheet_point], 64, 64, 0);
                    setVector(&sheet->points[1][sheet_point], 0, 64, 0);
                    setVector(&sheet->points[2][sheet_point], 64, 0, 0);
                    setVector(&sheet->points[3][sheet_point], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->points[0][sheet_point], -64, 64, 0);
                    setVector(&sheet->points[1][sheet_point], -64, 0, 0);
                    setVector(&sheet->points[2][sheet_point], 0, 64, 0);
                    setVector(&sheet->points[3][sheet_point], 0, 0, 0);
                }
            }
            sheet->inner.r = work->config->inner.r;
            sheet->inner.g = work->config->inner.g;
            sheet->inner.b = work->config->inner.b;
            sheet->outer.r = work->config->outer.r;
            sheet->outer.g = work->config->outer.g;
            sheet->outer.b = work->config->outer.b;
            setVector(&sheet->scale, 0, 0, 0);
        }
        near_scale = 256;
        far_scale = 192;
        for (k = 0, web_base = 0; k < 3; k++, web++, web_base += 300) {
            s32 near_radius, near_y, far_radius, far_y;

            for (i = 0, base = 2048 / 5; i < 4; i++, base = (i + 1) * 2048 / 5) {
                near_radius = rsin(base) * near_scale >> 12;
                near_y = rcos(base) * near_scale >> 12;
                far_radius = rsin(base) * far_scale >> 12;
                far_y = rcos(base) * far_scale >> 12;
                for (j = 0, angle = web_base; j < 6; j++, angle = web_base + j * 4096 / 6) {
                    setVector(&web->near[i][j], rcos(angle) * near_radius >> 12,
                        near_y, rsin(angle) * near_radius >> 12);
                    setVector(&web->far[i][j], rcos(angle) * far_radius >> 12,
                        far_y, rsin(angle) * far_radius >> 12);
                }
            }
            MODEL_VARIANT_WORD(web, 0x184) = 0;
            MODEL_VARIANT_WORD(web, 0x188) = 0;
            MODEL_VARIANT_WORD(web, 0x18C) = 0;
            web->color.r = work->config->web_color.r;
            web->color.g = work->config->web_color.g;
            web->color.b = work->config->web_color.b;
            web->scale = 4096 + k * 4096 / 3;
            web->done = 0;
            MODEL_VARIANT_WORD(web, 0x19C) = 0;
        }
        for (k = 0; k < 16; k++, trail++) {
            for (j = 0, delay = 0; j < 9; delay -= 256, j++) {
                trail->progress[j] = delay;
                trail->colors[j].r = -64 - j * 24;
                trail->colors[j].g = -64 - j * 24;
                trail->colors[j].b = 255;
            }
            trail->field_6C = 0;
            trail->field_70 = 0;
            trail->field_74 = 0;
            trail->field_A0 = 0;
            trail->field_A4 = 0;
        }
        for (i = 0, base = 0; i < 6; i++, ring_a++, base += 256) {
            for (k = 0, ring_angle = base; k < 8; k++, ring_angle = base + k * 512) {
                setVector(&ring_a->inner[k], (u32)rcos(ring_angle) >> 4,
                    (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring_a->outer[k], (u32)(rcos(ring_angle) * 5) >> 6,
                    (u32)(rsin(ring_angle) * 5) >> 6, 0);
            }
            ring_a->inner_color.r = 64;
            ring_a->inner_color.g = 128;
            ring_a->inner_color.b = 255;
            ring_a->outer_color.r = 0;
            ring_a->outer_color.g = 0;
            ring_a->outer_color.b = 0;
            ring_a->size = i * 1024 / 6;
            ring_a->state = 0;
        }
        for (i = 0, base = 0; i < 4; i++, ring_b++, base += 200) {
            for (k = 0, ring_angle = base; k < 8; k++, ring_angle = base + k * 512) {
                setVector(&ring_b->inner[k], (u32)(rcos(ring_angle) * 3) >> 5,
                    (u32)(rsin(ring_angle) * 3) >> 5, 0);
                setVector(&ring_b->outer[k], (u32)rcos(ring_angle) >> 3,
                    (u32)rsin(ring_angle) >> 3, 0);
            }
            ring_b->inner_color.r = 0;
            ring_b->inner_color.g = 0;
            ring_b->inner_color.b = 0;
            ring_b->outer_color.r = 255;
            ring_b->outer_color.g = 192;
            ring_b->outer_color.b = 64;
            ring_b->size = 1024 - i * 256;
            ring_b->state = 0;
            reset_value = 4096;
        }
        work->field_208C = 0;
        work->field_2094 = reset_value;
        work->field_2096 = 1024;
        work->field_2098 = 0;
        work->field_20A4 = 0;
        work->field_208E = 0;
        work->field_2090 = 0;
        work->frame_count = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
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
        } else {
            Model_CopySlotU16Values(0, (u16 *)&work->target);
        }
        if (work->elapsed < work->config->matrix_end) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        panel = work->panels;
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        for (n = 0; n < 2; n++, panel++) {
            for (k = 0, angle = 0; k < 1; k++, angle += 1300) {
                if (panel->size[k] >= 0 && panel->initialized[k] == 0) {
                    if (work->slot == 0) {
                        if (k % 4 == 0) {
                            setVector(&panel->captured[k],
                                work->target.vx, work->target.vy, work->target.vz);
                        } else {
                            setVector(&panel->captured[k],
                                work->target.vx + (rcos(angle) * 192 >> 12),
                                work->target.vy + (rsin(angle) * 192 >> 12),
                                work->target.vz);
                        }
                    } else {
                        if (k % 4 == 0) {
                            setVector(&panel->captured[k],
                                work->target.vx, work->target.vy, work->target.vz);
                        } else {
                            setVector(&panel->captured[k],
                                work->target.vx + (rcos(angle) * 192 >> 12),
                                work->target.vy + (rsin(angle) * 192 >> 12),
                                work->target.vz);
                        }
                    }
                    GsGetLwUnit(work->parts[n], &panel->matrices[k]);
                    panel->initialized[k] = 1;
                }
                setVector(&panel->directions[k],
                    panel->captured[k].vx - panel->matrices[k].t[0],
                    panel->captured[k].vy - panel->matrices[k].t[1],
                    panel->captured[k].vz - panel->matrices[k].t[2]);
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
        dx = (u16)projection.target.vx - screen_x;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->draw_start <= work->elapsed) {
            SpanishModelVariant477_DrawPanels((u8 *)point);
        }
        if (work->phase >= 2) {
            SpanishModelVariant477_DrawQuadGroups((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
    setVector(&work->camera_offset, -512, -256, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->camera_offset, 20);
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
    if ((u32)(work->phase - 2) < 3) {
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
