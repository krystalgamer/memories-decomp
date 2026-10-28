#include "../../types.h"
#include "variant437_entry.h"

#ifndef MODEL_VARIANT437_CLUT_X
#define MODEL_VARIANT437_CLUT_X 512
#endif

/* Header 437's loader entry. Setup resolves the command record and three
 * slot parts, uploads the textures and initialises the bloom groups and the
 * ribbons, cones, beam, tiles, discs and star this image never draws. Each
 * update keeps the eight bloom origins around the model, aims them at the
 * opponent, draws the blooms through the helper below and fades the
 * ambient light. */
s32 func_8013B004(u8 *context, s32 command)
{
    Variant437State *work = (Variant437State *)context;
    Variant437Group *group = work->groups;
    Variant437Beam *beam = &work->beam;
    Variant437Tile *tile = work->tiles;
    Variant437Disc *disc = work->discs;
    Variant437Disc *ring = work->rings;
    Variant437Star *star = &work->star;
    Variant437Cone *cone = work->cones;
    Variant437Ribbon *ribbon = work->ribbons;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *bloom = &work->bloom;
    s32 result = 0;
    /* Declared ahead of the other counters so their spill slots follow the
     * target's frame order. */
    s32 phase, base, i, m;
    s32 packed;
    s32 height, outer_height;
    /* Unused; retains the target's 64-byte stack gap below the points. */
    MATRIX unused[2];
    SVECTOR point, target;
    VECTOR squared;
    PSXLONG screen, interpolation, flag, aim_screen;
    Variant437Config *config;
    u16 tpage, clut;
    u16 flat_tpage, flat_clut;
    s32 j, k, n, spin, radius, outer;
    s32 angle;
    s16 origin_x, origin_y;
    s32 frame;
    s32 inner_size, outer_size;
    /* Set inside the last setup loop and stored after it, as the target
     * loads 4096 inside that loop. */
    s32 unit;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = config = &D_8013C988[command];
        work->parts[0] = Model_GetSlotDataEntry(work->slot, config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            work->target.vx = 0;
            work->target.vy = -300;
            work->target.vz = -450;
        } else {
            work->target.vx = 0;
            work->target.vy = -300;
            work->target.vz = 450;
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        work->delta.vx = work->target.vx - work->matrix.t[0];
        work->delta.vy = work->target.vy - work->matrix.t[1];
        work->delta.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 0x380, 0);
        clut = GetClut(MODEL_VARIANT437_CLUT_X, 244);
        GetTPage(1, 1, 0x380, 0);
        GetClut(MODEL_VARIANT437_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C88C[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C88C[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C88C[1]);
        /* Read before the last upload reuses packed; the scheduler sinks the
         * two reads below that call. */
        flat_tpage = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C88C[3]);
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = tpage;
        textured->u0 = 126;
        textured->v0 = 0;
        textured->u1 = 127;
        textured->v1 = 0;
        textured->u2 = 126;
        textured->v2 = 63;
        textured->u3 = 127;
        textured->v3 = 63;
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        textured++;
        SetPolyGT4(textured);
        textured->tpage = tpage;
        textured->u0 = 64;
        textured->v0 = 0;
        textured->u1 = 127;
        textured->v1 = 0;
        textured->u2 = 64;
        textured->v2 = 63;
        textured->u3 = 127;
        textured->v3 = 63;
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        SetPolyFT4(flat);
        flat->tpage = flat_tpage;
        flat->u0 = 0;
        flat->v0 = 128;
        flat->u1 = 0;
        flat->v1 = 175;
        flat->u2 = 48;
        flat->v2 = 128;
        flat->u3 = 48;
        flat->v3 = 175;
        flat->clut = flat_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_tpage;
        flat->u0 = 0;
        flat->v0 = 176;
        flat->u1 = 0;
        flat->v1 = 223;
        flat->u2 = 48;
        flat->v2 = 176;
        flat->u3 = 48;
        flat->v3 = 223;
        flat->clut = flat_clut;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        SetPolyFT4(bloom);
        bloom->tpage = packed >> 16;
        bloom->u0 = 64;
        bloom->v0 = 64;
        bloom->u1 = 127;
        bloom->v1 = 64;
        bloom->u2 = 64;
        bloom->v2 = 127;
        bloom->u3 = 127;
        bloom->v3 = 127;
        bloom->clut = packed;
        SetSemiTrans(bloom, 1);
        SetShadeTex(bloom, 0);

        for (i = 0, phase = 0, base = 0; i < 8; i++, phase = i * 512, base += 1300, group++) {
            for (j = 0; j < 12; j++) {
                setVector(&group->a[j], -256, -256, 0);
                setVector(&group->b[j], 256, -256, 0);
                setVector(&group->c[j], -256, 256, 0);
                setVector(&group->d[j], 256, 256, 0);
                group->color[0] = 64;
                group->color[1] = 64;
                group->color[2] = 64;
                group->size[j] = -(j * 2048 / 12) - (64 + (rsin(base) * 64 >> 12));
                group->done[j] = 0;
                group->extra[j] = 0;
                group->origin.vx = work->matrix.t[0] + (rcos(phase) * 512 >> 12);
                group->origin.vy = work->matrix.t[1];
                group->origin.vz = work->matrix.t[2] + (rsin(phase) * 512 >> 12);
            }
        }

        for (i = 0; i < 1; i++, beam++) {
            beam->scale.vx = 4096;
            beam->scale.vy = 4096;
            beam->scale.vz = 4096;
            for (k = 0; k < 2; k++) {
                beam->color[k][0] = work->config->color[0][0];
                beam->color[k][1] = work->config->color[0][1];
                beam->color[k][2] = work->config->color[0][2];
                beam->glow[k][0] = work->config->color[1][0];
                beam->glow[k][1] = work->config->color[1][1];
                beam->glow[k][2] = work->config->color[1][2];
                beam->offset[k] = k * -768;
            }
            beam->rx = 0;
            beam->ry = 0;
            beam->unknown94 = 0;
        }

        for (j = 0; j < 1; j++, star++) {
            setVector(&star->points[0], 0, 0, 0);
            /* The unsigned shifts give the target's srl. */
            for (k = 0, angle = 0; k < 5; k++, angle = k * 1024) {
                setVector(&star->points[k + 1], (u32)rcos(angle) >> 6, (u32)rsin(angle) >> 8, 0);
                setVector(&star->points[k + 6], (u32)(rcos(angle) * 3) >> 10, (u32)(rsin(angle) * 5) >> 9, 0);
            }
            star->color[0] = 255;
            star->color[1] = 192;
            star->color[2] = 192;
            setVector(&star->quad[0], -128, -128, 0);
            setVector(&star->quad[1], 128, -128, 0);
            setVector(&star->quad[2], -128, 128, 0);
            setVector(&star->quad[3], 128, 128, 0);
            star->glow[0] = 64;
            star->glow[1] = 64;
            star->glow[2] = 0;
            star->unknown80[0] = 0;
            star->unknown80[1] = 0;
            star->unknown80[2] = 0;
        }

        for (j = 0; j < 2; j++, tile++) {
            for (n = 0; n < 4; n++) {
                if (n == 0) {
                    setVector(&tile->a[n], -64, -64, 0);
                    setVector(&tile->b[n], 0, -64, 0);
                    setVector(&tile->c[n], -64, 0, 0);
                    setVector(&tile->d[n], 0, 0, 0);
                } else if (n == 1) {
                    setVector(&tile->a[n], 64, -64, 0);
                    setVector(&tile->b[n], 64, 0, 0);
                    setVector(&tile->c[n], 0, -64, 0);
                    setVector(&tile->d[n], 0, 0, 0);
                } else if (n == 2) {
                    setVector(&tile->a[n], 64, 64, 0);
                    setVector(&tile->b[n], 0, 64, 0);
                    setVector(&tile->c[n], 64, 0, 0);
                    setVector(&tile->d[n], 0, 0, 0);
                } else if (n == 3) {
                    setVector(&tile->a[n], -64, 64, 0);
                    setVector(&tile->b[n], -64, 0, 0);
                    setVector(&tile->c[n], 0, 64, 0);
                    setVector(&tile->d[n], 0, 0, 0);
                }
            }
            tile->color[0] = work->config->color[1][0];
            tile->color[1] = work->config->color[1][1];
            tile->color[2] = work->config->color[1][2];
            tile->glow[0] = work->config->color[0][0];
            tile->glow[1] = work->config->color[0][1];
            tile->glow[2] = work->config->color[0][2];
            tile->unknown88[0] = 0;
            tile->unknown88[1] = 0;
            tile->unknown88[2] = 0;
        }

        inner_size = 256;
        outer_size = 192;
        for (j = 0, base = 0; j < 3; j++, cone++, base += 300) {
            for (m = 0, phase = 2048 / 5; m < 4; m++, phase = (m + 1) * 2048 / 5) {
                radius = rsin(phase) * inner_size >> 12;
                height = rcos(phase) * inner_size >> 12;
                outer = rsin(phase) * outer_size >> 12;
                outer_height = rcos(phase) * outer_size >> 12;
                for (k = 0, angle = base; k < 6; k++, angle = base + k * 4096 / 6) {
                    setVector(&cone->inner[m][k], rcos(angle) * radius >> 12, height,
                              rsin(angle) * radius >> 12);
                    setVector(&cone->outer[m][k], rcos(angle) * outer >> 12, outer_height,
                              rsin(angle) * outer >> 12);
                }
            }
            cone->unknown184[0] = 0;
            cone->unknown184[1] = 0;
            cone->unknown184[2] = 0;
            cone->color[0] = work->config->color[2][0];
            cone->color[1] = work->config->color[2][1];
            cone->color[2] = work->config->color[2][2];
            cone->angle = j * 4096 / 3 + 4096;
            cone->unknown198 = 0;
            cone->unknown19C = 0;
        }

        for (j = 0; j < 16; j++, ribbon++) {
            for (k = 0; k < 9; k++) {
                ribbon->color[k][0] = -64 - k * 24;
                ribbon->color[k][1] = -64 - k * 24;
                ribbon->color[k][2] = 255;
                ribbon->offset[k] = k * -256;
            }
            ribbon->unknown6C[0] = 0;
            ribbon->unknown6C[1] = 0;
            ribbon->unknown6C[2] = 0;
            ribbon->unknownA0 = 0;
            ribbon->unknownA4 = 0;
        }

        for (m = 0, phase = 0; m < 6; m++, disc++, phase += 256) {
            for (j = 0, angle = phase; j < 8; angle += 512, j++) {
                setVector(&disc->inner[j], (u32)rcos(angle) >> 4,
                          (u32)rsin(angle) >> 4, 0);
                setVector(&disc->outer[j], (u32)(rcos(angle) * 5) >> 6,
                          (u32)(rsin(angle) * 5) >> 6, 0);
            }
            disc->color[0] = 64;
            disc->color[1] = 128;
            disc->color[2] = 255;
            disc->glow[0] = 0;
            disc->glow[1] = 0;
            disc->glow[2] = 0;
            disc->angle = m * 1024 / 6;
            disc->unknown8C = 0;
        }

        for (m = 0, phase = 0; m < 4; m++, ring++, phase += 200) {
            for (j = 0, angle = phase; j < 8; angle += 512, j++) {
                setVector(&ring->inner[j], (u32)(rcos(angle) * 3) >> 5,
                          (u32)(rsin(angle) * 3) >> 5, 0);
                setVector(&ring->outer[j], (u32)rcos(angle) >> 3,
                          (u32)rsin(angle) >> 3, 0);
            }
            ring->glow[0] = 255;
            ring->glow[1] = 192;
            ring->color[0] = 0;
            ring->color[1] = 0;
            ring->color[2] = 0;
            ring->glow[2] = 64;
            ring->angle = 1024 - m * 256;
            ring->unknown8C = 0;
            unit = 4096;
        }

        work->unknown2E94 = 0;
        work->unknown2E98 = unit;
        work->unknown2E9A = 1024;
        work->unknown2E9C = 0;
        work->unknown2EA0 = 0;
        work->unknown2E96 = 0;
        work->frame = 0;
        work->time = 0;
        work->last_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->fade = 0;
        work->ambient = 2048;
        work->command = command;
    } else {
        GsRVIEW2 *view = Model_GetCameraViewBuffer();

        work->view.vx = view->vrx - view->vpx;
        work->view.vy = view->vry - view->vpy;
        work->view.vz = view->vrz - view->vpz;
        Square0(&work->view, &squared);
        work->pitch = ratan2(work->view.vy, SquareRoot0(squared.vx + squared.vz));
        work->yaw = -ratan2(work->view.vz, work->view.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
            spin = 2048;
            work->aim.vx = work->target.vx;
            work->aim.vy = 0;
            work->aim.vz = work->target.vz;
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
            spin = 0;
            work->aim.vx = work->target.vx;
            work->aim.vy = 0;
            work->aim.vz = work->target.vz;
        }
        if (work->config->follow_end >= work->time) {
            GsGetLwUnit(work->parts[0], &work->matrix);
        }
        work->delta.vx = work->target.vx - work->matrix.t[0];
        work->delta.vy = work->target.vy - work->matrix.t[1];
        work->delta.vz = work->target.vz - work->matrix.t[2];
        group = work->groups;
        for (i = 0; i < 8; i++, group++) {
            group->origin.vx = work->matrix.t[0] + (rcos(spin) * 256 >> 12);
            group->origin.vy = work->matrix.t[1];
            group->origin.vz = work->matrix.t[2] + (rsin(spin) * 256 >> 12);
            group->direction.vx = work->aim.vx - group->origin.vx;
            group->direction.vy = work->aim.vy - group->origin.vy;
            group->direction.vz = work->aim.vz - group->origin.vz;
            spin += 512;
        }
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        point.vx = work->matrix.t[0];
        point.vy = work->matrix.t[1];
        point.vz = work->matrix.t[2];
        RotTransPers(&point, &screen, &interpolation, &flag);
        origin_x = screen;
        origin_y = screen >> 16;
        target.vx = work->target.vx;
        target.vy = work->target.vy;
        target.vz = work->target.vz;
        RotTransPers(&target, &aim_screen, &interpolation, &flag);
        work->projected.vx = ((DVECTOR *)&aim_screen)->vx - origin_x;
        work->projected.vy = (*(PSXLONG *)&aim_screen >> 16) - origin_y;
        if (work->config->bloom_start <= work->time) {
            func_8013C230(context);
        }
        work->frame++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->last_frame) {
            work->time += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->last_frame = frame;
        }
    }

    work->orbit.vx = -512;
    work->orbit.vy = -256;
    work->orbit.vz = 0;
    Model_QueueCameraOrbitFromPreset(work->slot, 3, &work->orbit, 20);
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->phase < 6) {
        if (work->ambient > 0) {
            work->ambient -= 128;
            if (work->ambient <= 0) {
                work->ambient = 0;
            }
        }
    } else {
        work->ambient = work->fade * 32;
    }
    if (work->phase >= 1 && work->phase <= 4) {
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

#ifndef VERSION_FRENCH
#define func_8017C230 func_8013C230
#include "../french_model_variant/image22822_blooms.c"
#endif
