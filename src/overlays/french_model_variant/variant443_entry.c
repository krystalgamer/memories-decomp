#include "../../types.h"
#include "variant443_entry.h"

/* The entry's data block.  D_8013F5A8 is where this module's unclassified tail
 * starts, at a helper that the module does not split out yet, 0x554 bytes
 * before the block.  The North American build points this at its own data
 * symbol. */
#ifndef MODEL_VARIANT443_DATA
#define MODEL_VARIANT443_DATA (D_8013F5A8 + 0x554)
#endif

/* The effect palette sits at VRAM x = 640 in the PAL images and at 512 in
 * the North American ones. */
#ifndef MODEL_VARIANT443_CLUT_X
#define MODEL_VARIANT443_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    EntryState443 *work = (EntryState443 *)point;
    MATRIX unused_matrix0, unused_matrix1;
    SVECTOR position, target;
    s16 screen_x[3], screen_y[3];
    VECTOR squared;
    EntryProjection443 projection;
    EntryRibbon443 *ribbon = work->ribbons;
    EntryFramebufferRing443 *ring = work->rings;
    EntryMesh443 *mesh = work->meshes;
    Streamer443 *streamer = work->streamers;
    EntryShortBand443 *band = work->bands;
    EntrySpiral443 *spiral = work->spirals;
    ModelVariantSheet *sheet = work->sheets;
    EntrySphere443 *sphere = work->spheres;
    EntryLongBand443 *long_band = work->long_bands;
    EntryRing443 *ring_a = work->rings_a;
    EntryRing443 *ring_b = work->rings_b;
    EntryShape443 *shape = work->shapes;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_GT4 *extra = &work->extra;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *additional = work->additional;
    s32 result = 0;
    s32 i, j, k, bearing, angle, base;
    s32 row;
    s32 corner;
    s32 ring_base;
    s32 radius;
    u16 tpage, clut, flat_clut;
    s32 flat_page, page, packed;
    s32 height, inner_radius, inner_height;
    s32 outer_size = 256, inner_size = 192;
    s32 red, green, blue;
    GsRVIEW2 *view;
    u32 origin_x;
    s32 origin_y;
    s32 projected_x, projected_y;
    s32 delta_x, delta_y;
    u32 frame;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (EntryConfig443 *)(MODEL_VARIANT443_DATA + 0xFC) + command;
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        Model_TransformPartVertex(work->slot, work->config->parts[0],
                                  work->config->vertices[0], &work->vertices[0]);
        Model_TransformPartVertex(work->slot, work->config->parts[1],
                                  work->config->vertices[1], &work->vertices[1]);
        setVector(&work->positions[0],
            (work->vertices[0].vx + work->vertices[1].vx) / 2,
            (work->vertices[0].vy + work->vertices[1].vy) / 2,
            (work->vertices[0].vz + work->vertices[1].vz) / 2);
        Model_TransformPartVertex(work->slot, work->config->parts[2],
                                  work->config->vertices[2], &work->positions[1]);
        Model_TransformPartVertex(work->slot, work->config->parts[3],
                                  work->config->vertices[3], &work->positions[2]);
        setVector(&work->origin,
            (work->positions[0].vx + work->positions[1].vx + work->positions[2].vx) / 3,
            (work->positions[0].vy + work->positions[1].vy + work->positions[2].vy) / 3,
            (work->positions[0].vz + work->positions[1].vz + work->positions[2].vz) / 3);
        setVector(&work->direction, work->target.vx - work->origin.vx,
            work->target.vy - work->origin.vy, work->target.vz - work->origin.vz);
        for (i = 0; i < 3; i++) {
            if (i + 1 == 3) {
                setVector(&work->paths[i],
                    work->positions[0].vx - work->positions[i].vx,
                    work->positions[0].vy - work->positions[i].vy,
                    work->positions[0].vz - work->positions[i].vz);
            } else {
                setVector(&work->paths[i],
                    work->positions[i + 1].vx - work->positions[i].vx,
                    work->positions[i + 1].vy - work->positions[i].vy,
                    work->positions[i + 1].vz - work->positions[i].vz);
            }
        }
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT443_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT443_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(MODEL_VARIANT443_DATA + 0xC4));
        flat_page = packed >> 16;
        flat_clut = packed;
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(MODEL_VARIANT443_DATA + 0x8C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(MODEL_VARIANT443_DATA + 0x1C));
        ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(MODEL_VARIANT443_DATA + 0x54));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(MODEL_VARIANT443_DATA + 0xE0));
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
        page = packed >> 16;
        SetPolyFT4(additional);
        additional->tpage = page;
        setUV4(additional, 0, 224, 31, 224, 0, 255, 31, 255);
        additional->clut = packed;
        SetSemiTrans(additional, 1);
        SetShadeTex(additional, 1);
        additional++;
        SetPolyFT4(additional);
        additional->tpage = page;
        setUV4(additional, 32, 224, 63, 224, 32, 255, 63, 255);
        additional->clut = packed;
        SetSemiTrans(additional, 1);
        SetShadeTex(additional, 1);
        SetPolyGT4(extra);
        SetSemiTrans(extra, 1);
        SetShadeTex(extra, 0);
        for (j = 0; j < 3; j++, ribbon++) {
            setVector(&ribbon->scale, 4096, 4096, 4096);
            ribbon->inner.r = work->config->inner[0][j];
            ribbon->inner.g = work->config->inner[1][j];
            ribbon->inner.b = work->config->inner[2][j];
            ribbon->outer.r = work->config->inner[0][j];
            ribbon->outer.g = work->config->inner[1][j];
            ribbon->outer.b = work->config->inner[2][j];
            ribbon->field_238 = 0;
            ribbon->field_23A = 0;
            ribbon->field_23C = 0;
            if (j == 0) {
                bearing = 1024;
            } else if (j / 2 * 2 == j - 1) {
                bearing = 1024 + (j + 1) * 512;
            } else {
                bearing = 1024 - j * 512;
            }
            ribbon->position.vx = work->stored_target[0] - (rcos(bearing) * 384 >> 12);
            ribbon->position.vy = work->stored_target[1] - (rsin(bearing) * 384 >> 12);
            ribbon->position.vz = work->stored_target[2];
        }
        for (i = 0, ring_base = 0; i < 5; i++, ring++, ring_base = i * 273) {
            for (k = 0, angle = ring_base; k < 4; k++, angle = ring_base + k * 4096 / 3) {
                setVector(&ring->points[0][k], rcos(angle) * 384 >> 12,
                    rsin(angle) * 384 >> 12, 0);
                setVector(&ring->points[1][k], rcos(angle) * 512 >> 12,
                    rsin(angle) * 512 >> 12, -96);
                setVector(&ring->points[2][k], rcos(angle) * 768 >> 12,
                    rsin(angle) * 768 >> 12, -128);
                ring->inner.r = 255;
                ring->inner.g = 255;
                ring->inner.b = 255;
                ring->outer.r = 255;
                ring->outer.g = 0;
                ring->outer.b = 255;
            }
            ring->initial = -(i * 2048 / 5);
            ring->hidden = 0;
        }
        for (j = 0; j < 1; j++, mesh++) {
            setVector(&mesh->scale, 4096, 4096, 4096);
            for (k = 0; k < 9; k++) {
                if (k == 0 || k == 8) {
                    red = 128; green = 0; blue = 0;
                } else if (k == 1) {
                    red = 128; green = 64; blue = 0;
                } else if (k == 2) {
                    red = 128; green = 128; blue = 0;
                } else if (k == 3) {
                    red = 0; green = 128; blue = 0;
                } else if (k == 4) {
                    red = 0; green = 128; blue = 128;
                } else if (k == 5) {
                    red = 0; green = 0; blue = 128;
                } else if (k == 6) {
                    red = 128; green = 0; blue = 128;
                } else {
                    red = 128; green = 0; blue = 64;
                }
                mesh->colors[k].r = red;
                mesh->colors[k].g = green;
                mesh->colors[k].b = blue;
            }
            mesh->field_544 = 0;
            mesh->field_548 = 0;
            mesh->field_546 = 0;
        }
        for (j = 0; j < 3; j++, streamer++) {
            for (k = 0; k < 13; k++) {
                streamer->color[k].r = 128;
                streamer->color[k].g = 128;
                streamer->color[k].b = 128;
            }
            *(s32 *)streamer->unknown_0208 = 0;
        }
        for (j = 0; j < 1; j++, long_band++) {
            setVector(&long_band->scale, 4096, 4096, 4096);
            for (k = 0; k < 9; k++) {
                long_band->inner[k].r = 0;
                long_band->inner[k].g = 0;
                long_band->inner[k].b = 192;
                long_band->outer[k].r = 192;
                long_band->outer[k].g = 192;
                long_band->outer[k].b = 192;
            }
            long_band->field_19C = 0;
            long_band->field_1A0 = 0;
            long_band->field_19E = 0;
        }
        for (j = 0; j < 3; j++, band++) {
            setVector(&band->scale, 4096, 4096, 4096);
            for (k = 0; k < 2; k++) {
                if (j + 1 == 3 && k == 1) {
                    band->inner[k].r = work->config->inner[0][0];
                    band->inner[k].g = work->config->inner[1][0];
                    band->inner[k].b = work->config->inner[2][0];
                    band->outer[k].r = work->config->outer[0][0];
                    band->outer[k].g = work->config->outer[1][0];
                    band->outer[k].b = work->config->outer[2][0];
                } else {
                    band->inner[k].r = work->config->inner[0][j + k];
                    band->inner[k].g = work->config->inner[1][j + k];
                    band->inner[k].b = work->config->inner[2][j + k];
                    band->outer[k].r = work->config->outer[0][j + k];
                    band->outer[k].g = work->config->outer[1][j + k];
                    band->outer[k].b = work->config->outer[2][j + k];
                }
            }
            band->field_68 = 0;
            band->field_6C = 0;
            band->field_6A = 0;
        }
        for (i = 0; i < 1; i++, shape++) {
            setVector(&shape->inner[0], 0, 0, 0);
            for (k = 0, angle = 0; k < 5; k++, angle = k * 1024) {
                setVector(&shape->inner[k + 1], rcos(angle) * 64 >> 12,
                    rsin(angle) * 16 >> 12, 0);
                setVector(&shape->outer[k], rcos(angle) * 12 >> 12,
                    rsin(angle) * 40 >> 12, 0);
            }
            setVector(&shape->corners[0], -128, -128, 0);
            setVector(&shape->corners[1], 128, -128, 0);
            setVector(&shape->corners[2], -128, 128, 0);
            setVector(&shape->corners[3], 128, 128, 0);
            shape->inner_color.r = 255;
            shape->inner_color.g = 192;
            shape->inner_color.b = 192;
            shape->outer_color.r = 64;
            shape->outer_color.g = 64;
            shape->outer_color.b = 0;
            shape->field_80 = 0;
            shape->field_84 = 0;
            shape->field_88 = 0;
        }
        for (i = 0; i < 5; i++, sheet++) {
            for (corner = 0; corner < 4; corner++) {
                if (corner == 0) {
                    setVector(&sheet->v0[corner], -192, -192, 0);
                    setVector(&sheet->v1[corner], 0, -192, 0);
                    setVector(&sheet->v2[corner], -192, 0, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                } else if (corner == 1) {
                    setVector(&sheet->v0[corner], 192, -192, 0);
                    setVector(&sheet->v1[corner], 192, 0, 0);
                    setVector(&sheet->v2[corner], 0, -192, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                } else if (corner == 2) {
                    setVector(&sheet->v0[corner], 192, 192, 0);
                    setVector(&sheet->v1[corner], 0, 192, 0);
                    setVector(&sheet->v2[corner], 192, 0, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                } else if (corner == 3) {
                    setVector(&sheet->v0[corner], -192, 192, 0);
                    setVector(&sheet->v1[corner], -192, 0, 0);
                    setVector(&sheet->v2[corner], 0, 192, 0);
                    setVector(&sheet->v3[corner], 0, 0, 0);
                }
            }
            if (i < 3) {
                sheet->outer[0] = work->config->outer[0][i];
                sheet->outer[1] = work->config->outer[1][i];
                sheet->outer[2] = work->config->outer[2][i];
                sheet->inner[0] = work->config->inner[0][i];
                sheet->inner[1] = work->config->inner[1][i];
                sheet->inner[2] = work->config->inner[2][i];
            } else {
                sheet->outer[0] = 255; sheet->outer[1] = 255; sheet->outer[2] = 255;
                sheet->inner[0] = 0; sheet->inner[1] = 0; sheet->inner[2] = 255;
            }
            sheet->size = 0;
            *(s32 *)(sheet->pad8C + 0) = 0;
            *(s32 *)(sheet->pad8C + 4) = 0;
        }
        for (i = 0, base = 0; i < 3; i++, sphere++, base += 300) {
            for (row = 0, ring_base = 2048 / 5; row < 4; row++, ring_base = (row + 1) * 2048 / 5) {
                radius = rsin(ring_base) * outer_size >> 12;
                height = rcos(ring_base) * outer_size >> 12;
                inner_radius = rsin(ring_base) * inner_size >> 12;
                inner_height = rcos(ring_base) * inner_size >> 12;
                for (k = 0, bearing = base; k < 6; k++, bearing = base + k * 4096 / 6) {
                    setVector(&sphere->near[row][k], rcos(bearing) * radius >> 12,
                        height, rsin(bearing) * radius >> 12);
                    setVector(&sphere->far[row][k], rcos(bearing) * inner_radius >> 12,
                        inner_height, rsin(bearing) * inner_radius >> 12);
                }
            }
            sphere->field_184 = 0;
            sphere->field_188 = 0;
            sphere->field_18C = 0;
            sphere->color.r = work->config->sphere_color.r;
            sphere->color.g = work->config->sphere_color.g;
            sphere->color.b = work->config->sphere_color.b;
            sphere->scale = 4096 + i * 4096 / 3;
            sphere->done = 0;
            sphere->field_19C = 0;
        }
        for (row = 0, ring_base = 0; row < 6; row++, ring_a++, ring_base += 256) {
            for (i = 0, angle = ring_base; i < 8; angle += 512, i++) {
                setVector(&ring_a->inner[i], rcos(angle) * 256 >> 12,
                    rsin(angle) * 256 >> 12, 0);
                setVector(&ring_a->outer[i], rcos(angle) * 320 >> 12,
                    rsin(angle) * 320 >> 12, 0);
            }
            ring_a->inner_color.r = 64;
            ring_a->inner_color.g = 128;
            ring_a->inner_color.b = 255;
            ring_a->outer_color.r = 0;
            ring_a->outer_color.g = 0;
            ring_a->outer_color.b = 0;
            ring_a->initial = row * 1024 / 6;
            ring_a->hidden = 0;
        }
        for (j = 0; j < 12; j++, spiral++) {
            for (k = 0; k < 2; k++) {
                if (k == 1) {
                    spiral->outer[k].r = 0; spiral->outer[k].g = 0; spiral->outer[k].b = 0;
                    spiral->inner[k].r = 0; spiral->inner[k].g = 0; spiral->inner[k].b = 0;
                } else {
                    spiral->outer[k].r = 255; spiral->outer[k].g = 0; spiral->outer[k].b = 255;
                    spiral->inner[k].r = 128; spiral->inner[k].g = 128; spiral->inner[k].b = 128;
                }
            }
        }
        for (row = 0, ring_base = 0; row < 4; row++, ring_b++, ring_base += 200) {
            for (i = 0, angle = ring_base; i < 8; angle += 512, i++) {
                setVector(&ring_b->inner[i], rcos(angle) * 384 >> 12,
                    rsin(angle) * 384 >> 12, 0);
                setVector(&ring_b->outer[i], rcos(angle) * 512 >> 12,
                    rsin(angle) * 512 >> 12, 0);
            }
            ring_b->outer_color.r = 255;
            ring_b->outer_color.g = 192;
            ring_b->inner_color.r = 0;
            ring_b->inner_color.g = 0;
            ring_b->inner_color.b = 0;
            ring_b->outer_color.b = 64;
            ring_b->initial = 1024 - row * 256;
            ring_b->hidden = 0;
        }
        work->field_3668 = 16;
        work->field_366C = 16;
        work->spin = 0;
        work->field_3670 = 0;
        work->field_3674 = 0;
        work->factor = 1024;
        work->field_3658 = 0;
        work->progress = 0;
        work->length = 1024;
        work->rotation_x = 0;
        work->rotation_y = 0;
        work->rotation_z = 0;
        work->field_3660 = 0;
        work->field_3662 = 0;
        work->field_3664 = 0;
        work->field_3666 = 0;
        work->field_364C = 0;
        work->size = 0;
        work->field_367A = 1024;
        work->travel = 0;
        work->field_3690 = 0;
        work->field_3694 = 0;
        work->field_364E = 0;
        work->frame = 0;
        work->elapsed = 0;
        work->animation = 0;
        work->step = 0;
        work->phase = 0;
        work->field_369C = 0;
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
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        Model_TransformPartVertex(work->slot, work->config->parts[0],
                                  work->config->vertices[0], &work->vertices[0]);
        Model_TransformPartVertex(work->slot, work->config->parts[1],
                                  work->config->vertices[1], &work->vertices[1]);
        setVector(&work->positions[0],
            (work->vertices[0].vx + work->vertices[1].vx) / 2,
            (work->vertices[0].vy + work->vertices[1].vy) / 2,
            (work->vertices[0].vz + work->vertices[1].vz) / 2);
        Model_TransformPartVertex(work->slot, work->config->parts[2],
                                  work->config->vertices[2], &work->positions[1]);
        Model_TransformPartVertex(work->slot, work->config->parts[3],
                                  work->config->vertices[3], &work->positions[2]);
        setVector(&work->origin,
            (work->positions[0].vx + work->positions[1].vx + work->positions[2].vx) / 3,
            (work->positions[0].vy + work->positions[1].vy + work->positions[2].vy) / 3,
            (work->positions[0].vz + work->positions[1].vz + work->positions[2].vz) / 3);
        setVector(&work->direction, work->target.vx - work->origin.vx,
            work->target.vy - work->origin.vy, work->target.vz - work->origin.vz);
        for (i = 0; i < 3; i++) {
            if (i + 1 == 3) {
                setVector(&work->paths[i],
                    work->positions[0].vx - work->positions[i].vx,
                    work->positions[0].vy - work->positions[i].vy,
                    work->positions[0].vz - work->positions[i].vz);
            } else {
                setVector(&work->paths[i],
                    work->positions[i + 1].vx - work->positions[i].vx,
                    work->positions[i + 1].vy - work->positions[i].vy,
                    work->positions[i + 1].vz - work->positions[i].vz);
            }
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
            &projection.interpolation, &projection.flag);
        origin_x = (u16)projection.projected.vx;
        origin_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
            &projection.interpolation, &projection.flag);
        delta_x = (u16)projection.target.vx - origin_x;
        delta_y = projection.target.vy - origin_y;
        work->projected.vx = delta_x;
        work->projected.vy = delta_y;
        for (i = 0; i < 3; i++) {
            setVector(&position, work->positions[i].vx, work->positions[i].vy, work->positions[i].vz);
            RotTransPers(&position, (PSXLONG *)&projection.projected,
                &projection.interpolation, &projection.flag);
#ifdef MODEL_VARIANT426_ENTRY_PAIRED_SCREEN
            /* The North American gcc 2.7.2 build reads both coordinates
             * before the two stores. */
            projected_x = (u16)projection.projected.vx;
            projected_y = projection.projected.vy;
            screen_x[i] = projected_x;
            screen_y[i] = projected_y;
#else
            projected_x = (u16)projection.projected.vx;
            screen_x[i] = projected_x;
            projected_y = projection.projected.vy;
            screen_y[i] = projected_y;
#endif
        }
        for (i = 0; i < 3; i++) {
            if (i + 1 == 3) {
                work->screen_x[i] = screen_x[0] - screen_x[i];
                work->screen_y[i] = screen_y[0] - screen_y[i];
            } else {
                work->screen_x[i] = screen_x[i + 1] - screen_x[i];
                work->screen_y[i] = screen_y[i + 1] - screen_y[i];
            }
        }
        if (work->config->start <= work->elapsed) {
            func_8013DF2C((u8 *)point);
            func_8013E668((u8 *)point);
            if (work->phase > 0) {
                func_8013EEF0((u8 *)point);
            }
            if (work->phase >= 2) {
                FrenchModelVariant443_DrawRibbons((u8 *)point);
            }
        }
        if (work->phase >= 4) {
            func_8013D98C((u8 *)point);
        }
        if (work->phase >= 5) {
            func_8013D3AC((u8 *)point);
        }
        work->frame++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation) {
            work->elapsed += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation = frame;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 8) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade << 5;
    }
    if (work->phase == 5 || work->phase == 6) {
        result = 4;
    } else if (work->phase == 7) {
        result = 1;
        work->phase = 8;
    } else if (work->phase == 8) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 9;
            }
        }
    } else if (work->phase == 9) {
        result = 2;
    }
    return result;
}
