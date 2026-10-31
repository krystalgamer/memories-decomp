#include "../../types.h"
#include "variant366_entry.h"

/* The French header-383 images load the effect palette from VRAM x = 640. */
#ifdef VERSION_FRENCH
#define MODEL_VARIANT366_CLUT_X 640
#else
#define MODEL_VARIANT366_CLUT_X 512
#endif

/* Header 366's loader entry. Setup resolves the command record and three
 * slot parts, uploads the textures and initialises the strip, quad group,
 * ribbons, rings and rays. Each update aims the effect from every listed
 * part at the opponent, draws the rays, ribbons and quads, then the ribbon
 * cones once the phase reaches 4, and fades the ambient light. */
s32 func_8013B004(u8 *context, s32 command)
{
    Variant366State *work = (Variant366State *)context;
    Variant366Strip *strip = work->strips;
    Variant366Quads *quads = work->quads;
    Variant366Ribbon *ribbon = work->ribbons;
    Variant366Ring *ring = work->rings;
    Variant366Ray *ray = work->rays;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *plain = &work->plain;
    POLY_GT4 *paged = &work->paged;
    POLY_F4 *mask = &work->mask;
    s32 result = 0;
    /* Unused; retains the target's 64-byte stack gap below the points. */
    u8 unknown_stack[64];
    SVECTOR position, target;
    VECTOR squared;
    Variant366Projection projection;
    u16 tpage, clut, flat_clut;
    s32 packed, flat_page;
    s32 i, j, k, n, m, angle, base;
    GsRVIEW2 *view;
    u32 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->frame_count = 0;
        work->config = &D_8013E558[command];
        work->parts[0] = Model_GetSlotDataEntry(work->slot, work->config->parts[0]);
        work->parts[1] = Model_GetSlotDataEntry(work->slot, work->config->parts[1]);
        work->parts[2] = Model_GetSlotDataEntry(work->slot, work->config->parts[2]);
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
                  work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT366_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT366_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E45C[7]);
        flat_page = packed >> 16;
        flat_clut = packed;
        /* The later results are dead, but storing them keeps `packed` from
         * standing in for flat_clut, so flat_page is computed here. */
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E45C[5]);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E45C[1]);
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyF4(mask);
        SetSemiTrans(mask, 1);
        SetPolyGT4(paged);
        paged->tpage = tpage;
        setUV4(paged, 127, 63, 127, 63, 127, 63, 127, 63);
        paged->clut = clut;
        SetSemiTrans(paged, 1);
        SetShadeTex(paged, 0);
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
        for (n = 0; n < 1; n++, strip++) {
            setVector(&strip->scale, 4096, 4096, 4096);
            strip->color.r = 0;
            strip->color.g = 128;
            strip->color.b = 255;
            strip->field_5C = 0;
            strip->field_60 = 0;
            strip->field_5E = 0;
        }
        for (n = 0; n < 16; n++, ray++) {
            for (i = 0; i < 2; i++) {
                if (i == 1) {
                    ray->outer[i].r = 0;
                    ray->outer[i].g = 0;
                    ray->outer[i].b = 0;
                    ray->inner[i].r = 0;
                    ray->inner[i].g = 0;
                    ray->inner[i].b = 0;
                } else {
                    ray->outer[i].r = 0;
                    ray->outer[i].g = 128;
                    ray->outer[i].b = 255;
                    ray->inner[i].r = 96;
                    ray->inner[i].g = 112;
                    ray->inner[i].b = 128;
                }
            }
        }
        for (k = 0; k < 1; k++, quads++) {
            for (j = 0; j < 4; j++) {
                if (j == 0) {
                    setVector(&quads->v0[j], -192, -192, 0);
                    setVector(&quads->v1[j], 0, -192, 0);
                    setVector(&quads->v2[j], -192, 0, 0);
                    setVector(&quads->v3[j], 0, 0, 0);
                } else if (j == 1) {
                    setVector(&quads->v0[j], 192, -192, 0);
                    setVector(&quads->v1[j], 192, 0, 0);
                    setVector(&quads->v2[j], 0, -192, 0);
                    setVector(&quads->v3[j], 0, 0, 0);
                } else if (j == 2) {
                    setVector(&quads->v0[j], 192, 192, 0);
                    setVector(&quads->v1[j], 0, 192, 0);
                    setVector(&quads->v2[j], 192, 0, 0);
                    setVector(&quads->v3[j], 0, 0, 0);
                } else if (j == 3) {
                    setVector(&quads->v0[j], -192, 192, 0);
                    setVector(&quads->v1[j], -192, 0, 0);
                    setVector(&quads->v2[j], 0, 192, 0);
                    setVector(&quads->v3[j], 0, 0, 0);
                }
            }
            quads->outer.r = 255;
            quads->outer.g = 255;
            quads->outer.b = 255;
            quads->inner.r = 0;
            quads->inner.g = 128;
            quads->inner.b = 255;
        }
        for (k = 0; k < 3; k++, ribbon++) {
            for (i = 0, angle = 0; i < 17; i++, angle = i * 256) {
                setVector(&ribbon->inner[i], (u32)(rcos(angle) * 5) >> 6, 0,
                          (u32)(rsin(angle) * 5) >> 6);
                setVector(&ribbon->middle[i], (u32)(rcos(angle) * 13) >> 7, 48,
                          (u32)(rsin(angle) * 13) >> 7);
                setVector(&ribbon->outer[i], (u32)rcos(angle) >> 3, 48,
                          (u32)rsin(angle) >> 3);
            }
            ribbon->progress = -(k * 2048) / 3;
            ribbon->field_19C = 0;
        }
        for (i = 0, base = 0; i < 4; i++, ring++, base += 256) {
            for (k = 0, m = base; k < 8; m += 512, k++) {
                setVector(&ring->inner[k], (u32)rcos(m) >> 4, (u32)rsin(m) >> 4, 0);
                setVector(&ring->outer[k], (u32)(rcos(m) * 5) >> 6,
                          (u32)(rsin(m) * 5) >> 6, 0);
            }
            ring->inner_color.r = 64;
            ring->inner_color.g = 128;
            ring->inner_color.b = 255;
            ring->outer_color.r = 0;
            ring->outer_color.g = 0;
            ring->outer_color.b = 0;
            ring->size = i * 256;
            ring->state = 0;
        }
        for (k = 2; k >= 0; k--) {
            work->field_1558[k] = 0;
        }
        work->field_1554 = 255;
        work->field_157A = 1024;
        work->field_1550 = 0;
        work->field_1564 = 0;
        work->field_1566 = 0;
        work->field_1568 = 0;
        work->field_156C = 0;
        work->field_156A = 0;
        work->field_156E = 0;
        work->field_1578 = 0;
        work->field_157C = 0;
        work->field_1570 = 0;
        work->field_1572 = 0;
        work->field_1574 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->phase = 0;
        work->field_1584 = 0;
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
        GsGetLwUnit(work->parts[0], &work->matrix);
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
        work->index = 0;
        work->screen_delta.vx = dx;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->count != 0) {
            do {
                if (work->index == 0) {
                    GsGetLwUnit(work->parts[0], &work->matrix);
                } else if (work->index == 1) {
                    GsGetLwUnit(work->parts[1], &work->matrix);
                } else if (work->index == 2) {
                    GsGetLwUnit(work->parts[2], &work->matrix);
                }
                setVector(&work->direction, work->target.vx - work->matrix.t[0],
                          work->target.vy - work->matrix.t[1],
                          work->target.vz - work->matrix.t[2]);
                if (work->phase > 0) {
                    func_8013D908(context);
                }
                if (work->phase == 3) {
                    func_8013C204(context);
                }
                func_8013C918(context);
            } while (++work->index < work->config->count);
        }
        if (work->phase >= 4) {
            func_8013CEC8(context);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
        if (work->config->end <= work->frame && work->phase == 2) {
            work->phase = 3;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 7) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->phase >= 4 && work->phase <= 8) {
        result = 4;
    } else if (work->phase == 9) {
        result = 1;
        work->phase = 10;
    } else if (work->phase == 10) {
        if (work->fade < 64) {
            work->fade += work->step;
            if (work->fade >= 64) {
                work->fade = 64;
                work->phase = 11;
            }
        }
    } else if (work->phase == 11) {
        result = 2;
    }
    return result;
}
