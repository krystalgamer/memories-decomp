#include "../../types.h"
#include "variant460_entry.h"

/* Header 460's loader entry. Setup resolves the command record and two
 * slot parts, uploads the textures and initialises two panels, two
 * ten-quad groups and a set of rings, domes and sheets this image never
 * draws. Each update tracks the target, draws the panels and quad groups
 * through the two helpers below and fades the ambient light. */
s32 func_8013B004(u8 *context, s32 command)
{
    Variant460State *work = (Variant460State *)context;
    SVECTOR position, target;
    VECTOR squared;
    PSXLONG origin, interpolation, flag, projected;
    Panel477 *panel;
    Variant460Pair *pair = &work->pair;
    ModelVariantSheet *sheet = work->sheets;
    Variant460Ring *ring = work->rings;
    Variant460Ring *halo = work->halos;
    Variant460Star *star = &work->star;
    Variant460Dome *dome = work->domes;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    Variant460Column *column = work->columns;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *glow = &work->glow;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *frame = &work->frame_poly;
    POLY_FT4 *panel_poly = &work->panel_poly;
    s32 result = 0;
    u16 flat_texture, panel_texture;
    s32 base, i, row, packed, panel_page, y, inner_y;
    QuadGroup477 *group;
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
        work->config = (Variant460Config *)(D_8013CDCC + 0xFC) + command;
        for (i = 0; i < 2; i++) {
            work->parts[i] = Model_GetSlotDataEntry(work->slot, work->config->parts[i]);
        }
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        panel = work->panels;
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->delta, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(512, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(512, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CDCC + 0xC4));
        flat_page = packed >> 16;
        flat_texture = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CDCC + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CDCC + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CDCC + 0x38));
        panel_page = packed >> 16;
        panel_texture = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013CDCC + 0x54));
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
                panel->color[0] = work->config->color_b[0];
                panel->color[1] = work->config->color_b[1];
                panel->color[2] = work->config->color_b[2];
                panel->size[j] = -1;
                panel->done[j] = 0;
                MODEL_VARIANT_WORD(panel, 0x44 + j * 4) = 0;
                MODEL_VARIANT_WORD(panel, 0x48 + j * 4) = 0;
                *(s16 *)(panel->unknown_20 + j * 8) = work->matrix.t[0] + work->delta.vx;
                *(s16 *)(panel->unknown_20 + j * 8 + 2) = work->matrix.t[1] + work->delta.vy;
                *(s16 *)(panel->unknown_20 + j * 8 + 4) = work->matrix.t[2] + work->delta.vz;
            }
            work->enabled[i] = 0;
        }
        for (group = work->groups, i = 0; i < 2; i++, group++) {
            for (j = 0; j < 10; j++) {
                setVector(&group->a[j], -256, -256, 0);
                setVector(&group->b[j], 256, -256, 0);
                setVector(&group->c[j], -256, 256, 0);
                setVector(&group->d[j], 256, 256, 0);
                group->color[0] = work->config->color_b[0];
                group->color[1] = work->config->color_b[1];
                group->color[2] = work->config->color_b[2];
                group->angle[j] = 0;
                group->done[j] = 0;
                MODEL_VARIANT_WORD(group, 0x20C + j * 4) = 0;
                MODEL_VARIANT_WORD(group, 0x234 + j * 4) = 0;
                group->size[j] = -((j << 12) / 10);
                *(s16 *)(group->unknown_140 + j * 8) = work->matrix.t[0] + work->delta.vx;
                *(s16 *)(group->unknown_140 + j * 8 + 2) = work->matrix.t[1] + work->delta.vy;
                *(s16 *)(group->unknown_140 + j * 8 + 4) = work->matrix.t[2] + work->delta.vz;
            }
        }
        for (i = 0; i < 1; i++, pair++) {
            setVector(&pair->scale, 4096, 4096, 4096);
            for (k = 0; k < 2; k++) {
                pair->inner[k][0] = work->config->color_a[0];
                pair->inner[k][1] = work->config->color_a[1];
                pair->inner[k][2] = work->config->color_a[2];
                pair->outer[k][0] = work->config->color_b[0];
                pair->outer[k][1] = work->config->color_b[1];
                pair->outer[k][2] = work->config->color_b[2];
                pair->size[k] = -(k * 768);
            }
            pair->field_90 = 0;
            pair->field_92 = 0;
            pair->field_94 = 0;
        }
        for (outer = 0; outer < 1; outer++, star++) {
            setVector(&star->a[0], 0, 0, 0);
            for (k = 0, angle = 0; k < 5; k++, angle = k << 10) {
                setVector(&star->a[k + 1], (u32)rcos(angle) >> 6, (u32)rsin(angle) >> 8, 0);
                setVector(&star->b[k], (u32)(rcos(angle) * 3) >> 10, (u32)(rsin(angle) * 5) >> 9, 0);
            }
            star->color_a[0] = 255;
            star->color_a[1] = 192;
            star->color_a[2] = 192;
            setVector(&star->square[0], -128, -128, 0);
            setVector(&star->square[1], 128, -128, 0);
            setVector(&star->square[2], -128, 128, 0);
            setVector(&star->square[3], 128, 128, 0);
            star->color_b[0] = 64;
            star->color_b[1] = 64;
            star->color_b[2] = 0;
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
            sheet->outer[0] = work->config->color_b[0];
            sheet->outer[1] = work->config->color_b[1];
            sheet->outer[2] = work->config->color_b[2];
            sheet->inner[0] = work->config->color_a[0];
            sheet->inner[1] = work->config->color_a[1];
            sheet->inner[2] = work->config->color_a[2];
            sheet->size = 0;
            MODEL_VARIANT_WORD(sheet, 0x8C) = 0;
            MODEL_VARIANT_WORD(sheet, 0x90) = 0;
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
                    setVector(&dome->outer[row][k], rcos(angle) * radius >> 12, y,
                              rsin(angle) * radius >> 12);
                    setVector(&dome->inner[row][k], rcos(angle) * scale >> 12, inner_y,
                              rsin(angle) * scale >> 12);
                }
            }
            dome->field_184 = 0;
            dome->field_188 = 0;
            dome->field_18C = 0;
            dome->color[0] = work->config->color_c[0];
            dome->color[1] = work->config->color_c[1];
            dome->color[2] = work->config->color_c[2];
            dome->size = (j << 12) / 3 + 4096;
            dome->field_198 = 0;
            dome->field_19C = 0;
        }
        for (j = 0; j < 16; j++, column++) {
            for (k = 0; k < 9; k++) {
                column->color[k][0] = shade = -64 - k * 24;
                column->color[k][1] = shade;
                column->color[k][2] = 255;
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
                setVector(&ring->a[outer], (u32)rcos(ring_angle) >> 4, (u32)rsin(ring_angle) >> 4, 0);
                setVector(&ring->b[outer], (u32)(rcos(ring_angle) * 5) >> 6, (u32)(rsin(ring_angle) * 5) >> 6, 0);
            }
            ring->color_a[0] = 64;
            ring->color_a[1] = 128;
            ring->color_a[2] = 255;
            ring->color_b[0] = 0;
            ring->color_b[1] = 0;
            ring->color_b[2] = 0;
            ring->size = (row << 10) / 6;
            ring->field_8C = 0;
        }
        for (row = 0, elevation = 0; row < 4; row++, halo++, elevation += 200) {
            for (j = 0, halo_angle = elevation; j < 8; halo_angle += 512, j++) {
                setVector(&halo->a[j], (u32)(rcos(halo_angle) * 3) >> 5, (u32)(rsin(halo_angle) * 3) >> 5, 0);
                setVector(&halo->b[j], (u32)rcos(halo_angle) >> 3, (u32)rsin(halo_angle) >> 3, 0);
            }
            halo->color_b[0] = 255;
            halo->color_b[1] = 192;
            halo->color_a[0] = 0;
            halo->color_a[1] = 0;
            halo->color_a[2] = 0;
            halo->color_b[2] = 64;
            halo->size = 1024 - (row << 8);
            halo->field_8C = 0;
            inner = 4096;
        }
        work->field_208C = 0;
        work->field_2094 = inner;
        work->field_2096 = 1024;
        work->field_2098 = 0;
        work->field_20A4 = 0;
        work->field_208E = 0;
        work->field_2090 = 0;
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
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        panel = work->panels;
        setVector(&work->delta, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        for (i = 0; i < 2; i++, panel++) {
            for (j = 0, angle = 0; j < 1; j++, angle += 1300) {
                if (panel->size[j] >= 0 && MODEL_VARIANT_WORD(panel, 0x48 + j * 4) == 0) {
                    if (work->slot == 0) {
                        if (j % 4 == 0) {
                            setVector((VECTOR *)(panel->unknown_6C + j * 16),
                                  work->target.vx, work->target.vy, work->target.vz);
                        } else {
                            setVector((VECTOR *)(panel->unknown_6C + j * 16),
                                  work->target.vx + (rcos(angle) * 192 >> 12),
                                  work->target.vy + (rsin(angle) * 192 >> 12), work->target.vz);
                        }
                    } else if ((j & 3) == 0) {
                        setVector((VECTOR *)(panel->unknown_6C + j * 16),
                                  work->target.vx, work->target.vy, work->target.vz);
                    } else {
                        setVector((VECTOR *)(panel->unknown_6C + j * 16),
                                  work->target.vx + (rcos(angle) * 192 >> 12),
                                  work->target.vy + (rsin(angle) * 192 >> 12), work->target.vz);
                    }
                    GsGetLwUnit(work->parts[i], &panel->matrix[j]);
                    MODEL_VARIANT_WORD(panel, 0x48 + j * 4) = 1;
                }
                setVector(&panel->direction[j],
                          *(s32 *)(panel->unknown_6C + j * 16) - panel->matrix[j].t[0],
                          *(s32 *)(panel->unknown_6C + j * 16 + 4) - panel->matrix[j].t[1],
                          *(s32 *)(panel->unknown_6C + j * 16 + 8) - panel->matrix[j].t[2]);
            }
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, &origin, &interpolation, &flag);
        origin_x = origin;
        origin_y = origin >> 16;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, &projected, &interpolation, &flag);
        work->projected.vx = projected - origin_x;
        work->projected.vy = (projected >> 16) - origin_y;
        if (work->config->panel_start <= work->frame) {
            func_8013C488(context);
        }
        if (work->phase >= 2) {
            func_8013C960(context);
        }
        work->frame_count++;
        animation_frame = Model_GetSlotAnimationFrame(work->slot);
        if (animation_frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = animation_frame;
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

/* The helpers are the Spanish header-477 panels and quads, rebuilt at the
 * North American addresses. */
#define func_8013C484 func_8013C488
#include "../spanish_model_variant/variant477_panels.c"
#define func_8013C958 func_8013C960
#include "../spanish_model_variant/variant477_quads.c"
