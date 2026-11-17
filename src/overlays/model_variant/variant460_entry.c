#include "../../types.h"
#include "variant460_entry.h"

/* Header 460's loader entry. Setup resolves the command record and two
 * slot parts, uploads the textures and initialises two panels, two
 * ten-quad groups and a set of rings, domes and sheets this image never
 * draws. Each update tracks the target, draws the panels and quad groups
 * through the two helpers below and fades the ambient light. */
s32 ModelVariant460_HandleRequest(u8 *context, s32 command)
{
    Variant460State *work = (Variant460State *)context;
    SVECTOR position, target;
    VECTOR squared;
    long origin, interpolation, flag, projected;
    ModelVariantPanel *panel;
    ModelVariantBandPair *pair = &work->pair;
    ModelVariantSheet *sheet = work->sheets;
    ModelVariantColorRing *ring = work->rings;
    ModelVariantColorRing *halo = work->halos;
    ModelVariantShape *star = &work->star;
    ModelVariantWebNarrow *dome = work->domes;
    POLY_G3 *triangle = &work->packets.triangle;
    POLY_G4 *quad = &work->packets.quad;
    ModelVariantTrail *column = work->columns;
    POLY_GT4 *textured = work->packets.textured;
    POLY_GT4 *glow = &work->glow;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *frame = &work->quads_packet;
    POLY_FT4 *panel_poly = &work->panels_packet;
    s32 result = 0;
    u16 flat_texture, panel_texture;
    s32 base, i, row, packed, panel_page, y, inner_y;
    ModelVariantQuadGroup *group;
    s32 flat_page;
    s32 elevation, outer, j, angle, halo_angle, k, corner, radius, inner, ring_angle, ring_base, scale;
    u16 tpage, clut;
    s8 shade;
    s16 origin_x;
    s32 origin_y;
    s32 outer_size, inner_size;
    GsRVIEW2 *view;
    u32 animation_frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = gModelVariant460_aConfigs + command;
        for (i = 0; i < 2; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->aim.target, 0, -300, -450);
        } else {
            setVector(&work->aim.target, 0, -300, 450);
        }
        panel = work->panels;
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->aim.direction, work->aim.target.vx - work->matrix.t[0],
                  work->aim.target.vy - work->matrix.t[1], work->aim.target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(512, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(512, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant460_aImages[7]);
        flat_page = packed >> 16;
        flat_texture = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant460_aImages[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant460_aImages[1]);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant460_aImages[2]);
        panel_page = packed >> 16;
        panel_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant460_aImages[3]);
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
        SetPolyGT4(glow);
        SetSemiTrans(glow, 1);
        SetShadeTex(glow, 0);
        SetPolyFT4(panel_poly);
        panel_poly->tpage = panel_page;
        setUV4(panel_poly, 128, 0, 255, 0, 128, 63, 255, 63);
        panel_poly->clut = panel_texture;
        SetSemiTrans(panel_poly, 0);
        SetShadeTex(panel_poly, 1);
        SetPolyFT4(frame);
        frame->tpage = packed >> 16;
        setUV4(frame, 64, 64, 127, 64, 64, 127, 127, 127);
        frame->clut = packed;
        SetSemiTrans(frame, 1);
        SetShadeTex(frame, 0);
        for (i = 0; i < 2; i++, panel++) {
            for (j = 0; j < 1; j++) {
                setVector(&panel->a[j], -256, -256, 0);
                setVector(&panel->b[j], 256, -256, 0);
                setVector(&panel->c[j], -256, 256, 0);
                setVector(&panel->d[j], 256, 256, 0);
                panel->color.r = work->config->inner.r;
                panel->color.g = work->config->inner.g;
                panel->color.b = work->config->inner.b;
                panel->size[j] = -1;
                panel->done[j] = 0;
                panel->field_44[j] = 0;
                panel->initialized[j] = 0;
                panel->positions[j].vx = work->matrix.t[0] + work->aim.direction.vx;
                panel->positions[j].vy = work->matrix.t[1] + work->aim.direction.vy;
                panel->positions[j].vz = work->matrix.t[2] + work->aim.direction.vz;
            }
            work->enabled[i] = 0;
        }
        for (group = work->groups, i = 0; i < 2; i++, group++) {
            for (j = 0; j < 10; j++) {
                setVector(&group->a[j], -256, -256, 0);
                setVector(&group->b[j], 256, -256, 0);
                setVector(&group->c[j], -256, 256, 0);
                setVector(&group->d[j], 256, 256, 0);
                group->color.r = work->config->inner.r;
                group->color.g = work->config->inner.g;
                group->color.b = work->config->inner.b;
                group->angle[j] = 0;
                group->done[j] = 0;
                group->field_20C[j] = 0;
                group->initialized[j] = 0;
                group->size[j] = -((j << 12) / 10);
                group->positions[j].vx = work->matrix.t[0] + work->aim.direction.vx;
                group->positions[j].vy = work->matrix.t[1] + work->aim.direction.vy;
                group->positions[j].vz = work->matrix.t[2] + work->aim.direction.vz;
            }
        }
        for (i = 0; i < 1; i++, pair++) {
            setVector(&pair->scale, 4096, 4096, 4096);
            for (k = 0; k < 2; k++) {
                pair->ca[k].r = work->config->outer.r;
                pair->ca[k].g = work->config->outer.g;
                pair->ca[k].b = work->config->outer.b;
                pair->cb[k].r = work->config->inner.r;
                pair->cb[k].g = work->config->inner.g;
                pair->cb[k].b = work->config->inner.b;
                pair->offset[k] = -(k * 768);
            }
            pair->field_90 = 0;
            pair->field_92 = 0;
            pair->field_94 = 0;
        }
        for (outer = 0; outer < 1; outer++, star++) {
            setVector(&star->origin, 0, 0, 0);
            for (k = 0, angle = 0; k < 5; k++, angle = k << 10) {
                setVector(&star->inner[k], (u32)rcos(angle) >> 6, (u32)rsin(angle) >> 8, 0);
                setVector(&star->outer[k], (u32)(rcos(angle) * 3) >> 10, (u32)(rsin(angle) * 5) >> 9, 0);
            }
            star->inner_color.r = 255;
            star->inner_color.g = 192;
            star->inner_color.b = 192;
            setVector(&star->corners[0], -128, -128, 0);
            setVector(&star->corners[1], 128, -128, 0);
            setVector(&star->corners[2], -128, 128, 0);
            setVector(&star->corners[3], 128, 128, 0);
            star->outer_color.r = 64;
            star->outer_color.g = 64;
            star->outer_color.b = 0;
            star->field_80 = 0;
            star->field_84 = 0;
            star->field_88 = 0;
        }
        for (outer = 0; outer < 2; outer++, sheet++) {
            for (corner = 0; corner < 4; corner++) {
                if (corner == 0) {
                    setVector(&sheet->v0[corner], -64, -64, 0);
                    setVector(&sheet->v1[corner], 0, -64, 0);
                    setVector(&sheet->v2[corner], -64, 0, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                } else if (corner == 1) {
                    setVector(&sheet->v0[corner], 64, -64, 0);
                    setVector(&sheet->v1[corner], 64, 0, 0);
                    setVector(&sheet->v2[corner], 0, -64, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                } else if (corner == 2) {
                    setVector(&sheet->v0[corner], 64, 64, 0);
                    setVector(&sheet->v1[corner], 0, 64, 0);
                    setVector(&sheet->v2[corner], 64, 0, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                } else if (corner == 3) {
                    setVector(&sheet->v0[corner], -64, 64, 0);
                    setVector(&sheet->v1[corner], -64, 0, 0);
                    setVector(&sheet->v2[corner], 0, 64, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                }
            }
            sheet->outer.r = work->config->inner.r;
            sheet->outer.g = work->config->inner.g;
            sheet->outer.b = work->config->inner.b;
            sheet->inner.r = work->config->outer.r;
            sheet->inner.g = work->config->outer.g;
            sheet->inner.b = work->config->outer.b;
            sheet->size = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        outer_size = 256;
        inner_size = 192;
        for (j = 0, base = 0; j < 3; j++, dome++, base += 300) {
            for (row = 0, elevation = 2048 / 5; row < 4; row++, elevation = (row + 1) * 2048 / 5) {
                radius = rsin(elevation) * outer_size >> 12;
                y = rcos(elevation) * outer_size >> 12;
                scale = rsin(elevation) * inner_size >> 12;
                inner_y = rcos(elevation) * inner_size >> 12;
                for (k = 0, angle = base; k < 6; k++, angle = base + (k << 12) / 6) {
                    setVector(&dome->near[row][k], rcos(angle) * radius >> 12, y,
                              rsin(angle) * radius >> 12);
                    setVector(&dome->far[row][k], rcos(angle) * scale >> 12, inner_y,
                              rsin(angle) * scale >> 12);
                }
            }
            dome->field_184 = 0;
            dome->field_188 = 0;
            dome->field_18C = 0;
            dome->color.r = work->config->web_color.r;
            dome->color.g = work->config->web_color.g;
            dome->color.b = work->config->web_color.b;
            dome->scale = (j << 12) / 3 + 4096;
            dome->done = 0;
            dome->field_19C = 0;
        }
        for (j = 0; j < 16; j++, column++) {
            for (k = 0; k < 9; k++) {
                column->color[k].r = shade = -64 - k * 24;
                column->color[k].g = shade;
                column->color[k].b = 255;
                column->offset[k] = -(k << 8);
            }
            column->field_6C = 0;
            column->field_70 = 0;
            column->field_74 = 0;
            column->field_A0 = 0;
            column->field_A4 = 0;
        }
        for (row = 0, ring_base = 0; row < 6; row++, ring++, ring_base += 256) {
            for (outer = 0, ring_angle = ring_base; outer < 8; ring_angle += 512, outer++) {
                setVector(&ring->inner[outer], (u32)rcos(ring_angle) >> 4, (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring->outer[outer], (u32)(rcos(ring_angle) * 5) >> 6, (u32)(rsin(ring_angle) * 5) >> 6, 0);
            }
            ring->inner_color.r = 64;
            ring->inner_color.g = 128;
            ring->inner_color.b = 255;
            ring->outer_color.r = 0;
            ring->outer_color.g = 0;
            ring->outer_color.b = 0;
            ring->angle = (row << 10) / 6;
            ring->count = 0;
        }
        for (row = 0, elevation = 0; row < 4; row++, halo++, elevation += 200) {
            for (j = 0, halo_angle = elevation; j < 8; halo_angle += 512, j++) {
                setVector(&halo->inner[j], (u32)(rcos(halo_angle) * 3) >> 5, (u32)(rsin(halo_angle) * 3) >> 5, 0);
                setVector(&halo->outer[j], (u32)rcos(halo_angle) >> 3, (u32)rsin(halo_angle) >> 3, 0);
            }
            halo->outer_color.r = 255;
            halo->outer_color.g = 192;
            halo->inner_color.r = 0;
            halo->inner_color.g = 0;
            halo->inner_color.b = 0;
            halo->outer_color.b = 64;
            halo->angle = 1024 - (row << 8);
            halo->count = 0;
            inner = 4096;
        }
        work->field_208C = 0;
        work->field_2094 = inner;
        work->field_2096 = 1024;
        work->field_2098 = 0;
        work->field_20A4 = 0;
        work->field_208E = 0;
        work->field_2090 = 0;
        work->clock.frame_count = 0;
        work->clock.frame = 0;
        work->clock.animation_frame = 0;
        work->clock.step = 0;
        work->phase = 0;
        work->clock.fade = 0;
        work->tint = 2048;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        setVector(&work->aim.view_delta, view->vrx - view->vpx,
                  view->vry - view->vpy, view->vrz - view->vpz);
        Square0(&work->aim.view_delta, &squared);
        work->clock.angles[0] = ratan2(work->aim.view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->clock.angles[1] = -ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->aim.target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->aim.target);
        }
        if (work->clock.frame < work->config->matrix_end) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        panel = work->panels;
        setVector(&work->aim.direction, work->aim.target.vx - work->matrix.t[0],
                  work->aim.target.vy - work->matrix.t[1], work->aim.target.vz - work->matrix.t[2]);
        for (i = 0; i < 2; i++, panel++) {
            for (j = 0, angle = 0; j < 1; j++, angle += 1300) {
                if (panel->size[j] >= 0 && panel->initialized[j] == 0) {
                    if (work->slot == 0) {
                        if (j % 4 == 0) {
                            setVector(&panel->captured[j],
                                  work->aim.target.vx, work->aim.target.vy, work->aim.target.vz);
                        } else {
                            setVector(&panel->captured[j],
                                  work->aim.target.vx + (rcos(angle) * 192 >> 12),
                                  work->aim.target.vy + (rsin(angle) * 192 >> 12), work->aim.target.vz);
                        }
                    } else if ((j & 3) == 0) {
                        setVector(&panel->captured[j],
                                  work->aim.target.vx, work->aim.target.vy, work->aim.target.vz);
                    } else {
                        setVector(&panel->captured[j],
                                  work->aim.target.vx + (rcos(angle) * 192 >> 12),
                                  work->aim.target.vy + (rsin(angle) * 192 >> 12), work->aim.target.vz);
                    }
                    GsGetLwUnit(work->parts[i], &panel->matrices[j]);
                    panel->initialized[j] = 1;
                }
                setVector(&panel->directions[j],
                          panel->captured[j].vx - panel->matrices[j].t[0],
                          panel->captured[j].vy - panel->matrices[j].t[1],
                          panel->captured[j].vz - panel->matrices[j].t[2]);
            }
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, &origin, &interpolation, &flag);
        origin_x = origin;
        origin_y = origin >> 16;
        setVector(&target, work->aim.target.vx, work->aim.target.vy, work->aim.target.vz);
        RotTransPers(&target, &projected, &interpolation, &flag);
        work->aim.screen_delta.vx = projected - origin_x;
        work->aim.screen_delta.vy = (projected >> 16) - origin_y;
        if (work->config->draw_start <= work->clock.frame) {
            SpanishModelVariant477_DrawPanels((Variant460State *)context);
        }
        if (work->phase >= 2) {
            SpanishModelVariant477_DrawQuadGroups((Variant460State *)context);
        }
        work->clock.frame_count++;
        animation_frame = Model_GetSlotAnimationFrame(work->slot);
        if (animation_frame != work->clock.animation_frame) {
            work->clock.frame += Model_GetFrameStep();
            work->clock.step = Model_GetFrameStep();
            work->clock.animation_frame = animation_frame;
        }
    }
    setVector(&work->orbit, -512, -256, 0);
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->orbit, 20);
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 6) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->clock.fade << 5;
    }
    if (work->phase >= 2 && work->phase <= 4) {
        result = 4;
    } else if (work->phase == 5) {
        result = 1;
        work->phase = 6;
    } else if (work->phase == 6) {
        if (work->clock.fade < 64) {
            work->clock.fade += work->clock.step;
            if (work->clock.fade >= 64) {
                work->clock.fade = 64;
                work->phase = 7;
            }
        }
    } else if (work->phase == 7) {
        result = 2;
    }
    return result;
}

/* The helpers are the Spanish header-477 panels and quads, rebuilt at the
 * North American addresses on header 460's record, which names the members
 * they read identically; the header-477 entry header is skipped. */
#define FRENCH_MODEL_VARIANT477_ENTRY_H
#define EntryState477 Variant460State
#include "../spanish_model_variant/variant477_panels.c"
#include "../spanish_model_variant/variant477_quads.c"
