#include "../../types.h"
#include "variant455_entry.h"

/* PAL images load the effect palette from VRAM x = 640. */
#ifdef VERSION_FRENCH
#define MODEL_VARIANT455_CLUT_X 640
#else
#define MODEL_VARIANT455_CLUT_X 512
#endif

/* Header 455's entry: eight bloom groups, a rainbow disc, twelve fans and a
 * four-quad sheet around the active slot's first part, aimed at the
 * opposite slot. */
s32 func_8013B004(u8 *context, s32 command)
{
    EntryState455 *work = (EntryState455 *)context;
    EntryBloom455 *bloom = work->blooms;
    EntryDisc455 *disc = work->discs;
    ModelVariantSheet *sheet = work->sheets;
    EntryFan455 *fan = work->fans;
    POLY_GT4 *textured = work->textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    POLY_GT4 *glow = work->glow;
    POLY_GT4 *beam = work->beam;
    POLY_GT4 *flare = work->flare;
    POLY_FT4 *flat = work->flat;
    POLY_FT4 *ring_packet = &work->ring_packet;
    POLY_F4 *mask = &work->mask;
    s32 result = 0;
    u16 flare_clut, glow_clut, beam_tpage, beam_clut;
    s32 angle, i, packed, beam_page;
    SVECTOR position, target;
    VECTOR squared;
    EntryProjection455 projection;
    s32 flat_page, glow_page;
    u16 tpage, clut, flat_clut;
    s32 j, k, m, n, p, radius, red, green, blue;
    GsRVIEW2 *view;
    u32 screen_x, frame;
    s32 screen_y, dx, dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = &D_8013DD30[command];
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
        clut = GetClut(MODEL_VARIANT455_CLUT_X, 244);
        beam_tpage = GetTPage(1, 1, 896, 0);
        beam_clut = GetClut(MODEL_VARIANT455_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DC34[7]);
        flat_page = packed >> 16;
        flat_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DC34[5]);
        glow_page = packed >> 16;
        glow_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DC34[1]);
        beam_page = packed >> 16;
        flare_clut = packed;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DC34[3]);
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyF4(mask);
        SetSemiTrans(mask, 1);
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
        SetPolyGT4(glow);
        glow->tpage = glow_page;
        setUV4(glow, 192, 64, 255, 64, 192, 127, 255, 127);
        glow->clut = glow_clut;
        SetSemiTrans(glow, 1);
        SetShadeTex(glow, 0);
        SetPolyFT4(ring_packet);
        ring_packet->tpage = packed >> 16;
        setUV4(ring_packet, 64, 64, 127, 64, 64, 127, 127, 127);
        ring_packet->clut = packed;
        SetSemiTrans(ring_packet, 1);
        SetShadeTex(ring_packet, 0);
        SetPolyGT4(beam);
        beam->tpage = beam_tpage;
        setUV4(beam, 0, 128, 0, 255, 31, 128, 31, 255);
        beam->clut = beam_clut;
        SetSemiTrans(beam, 1);
        SetShadeTex(beam, 0);
        SetPolyGT4(flare);
        flare->tpage = beam_page;
        setUV4(flare, 128, 207, 255, 207, 128, 128, 255, 128);
        flare->clut = flare_clut;
        SetSemiTrans(flare, 1);
        SetShadeTex(flare, 0);
        for (i = 0, angle = 0; i < 8; i++, angle = i * 512, bloom++) {
            for (k = 0; k < 12; k++) {
                setVector(&bloom->a[k], -256, -256, 0);
                setVector(&bloom->b[k], 256, -256, 0);
                setVector(&bloom->c[k], -256, 256, 0);
                setVector(&bloom->d[k], 256, 256, 0);
                bloom->color.r = 32;
                bloom->color.g = 32;
                bloom->color.b = 32;
                bloom->done[k] = 0;
                bloom->field_254[k] = 0;
                bloom->size[k] = -(k * 2048 / 12);
                bloom->origin.vx = work->matrix.t[0] + (rcos(angle) * 512 >> 12);
                bloom->origin.vy = work->matrix.t[1];
                bloom->origin.vz = work->matrix.t[2] + (rsin(angle) * 512 >> 12);
            }
        }
        for (k = 0; k < 1; k++, disc++) {
            for (m = 0, angle = 0; m < 7; m++, angle = m * 1024 / 6) {
                radius = 384 - (rcos(angle) * 192 >> 12);
                for (j = 0, n = 0; j < 17; j++, n = j * 256) {
                    if (j % 8 == 0) {
                        red = 255; green = 0; blue = 0;
                    } else if (j % 8 == 1) {
                        red = 255; green = 128; blue = 0;
                    } else if (j % 8 == 2) {
                        red = 255; green = 255; blue = 0;
                    } else if (j % 8 == 3) {
                        red = 0; green = 255; blue = 0;
                    } else if (j % 8 == 4) {
                        red = 0; green = 255; blue = 255;
                    } else if (j % 8 == 5) {
                        red = 0; green = 128; blue = 255;
                    } else if (j % 8 == 6) {
                        red = 0; green = 0; blue = 255;
                    } else {
                        red = 255; green = 0; blue = 255;
                    }
                    setVector(&disc->upper[m][j], rcos(n) * radius >> 12, m * 512 / 6,
                        rsin(n) * radius >> 12);
                    setVector(&disc->lower[m][j], rcos(n) * radius >> 12, m * 512 / 6,
                        rsin(n) * radius >> 12);
                    disc->colors[m][j].r = red;
                    disc->colors[m][j].g = green;
                    disc->colors[m][j].b = blue;
                }
            }
            disc->progress = -(k * 4096);
            disc->state = 0;
        }
        for (k = 0; k < 1; k++, sheet++) {
            for (p = 0; p < 4; p++) {
                if (p == 0) {
                    setVector(&sheet->v0[p], -256, -256, 0);
                    setVector(&sheet->v1[p], 0, -256, 0);
                    setVector(&sheet->v2[p], -256, 0, 0);
                    setVector(&sheet->v3[p], 0, 0, 0);
                } else if (p == 1) {
                    setVector(&sheet->v0[p], 256, -256, 0);
                    setVector(&sheet->v1[p], 256, 0, 0);
                    setVector(&sheet->v2[p], 0, -256, 0);
                    setVector(&sheet->v3[p], 0, 0, 0);
                } else if (p == 2) {
                    setVector(&sheet->v0[p], 256, 256, 0);
                    setVector(&sheet->v1[p], 0, 256, 0);
                    setVector(&sheet->v2[p], 256, 0, 0);
                    setVector(&sheet->v3[p], 0, 0, 0);
                } else if (p == 3) {
                    setVector(&sheet->v0[p], -256, 256, 0);
                    setVector(&sheet->v1[p], -256, 0, 0);
                    setVector(&sheet->v2[p], 0, 256, 0);
                    setVector(&sheet->v3[p], 0, 0, 0);
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
        for (i = 0; i < 12; i++, fan++) {
            for (j = 0; j < 4; j++) {
                fan->inner[j].r = work->config->inner.r * (3 - j) / 3;
                fan->inner[j].g = work->config->inner.g * (3 - j) / 3;
                fan->inner[j].b = work->config->inner.b * (3 - j) / 3;
                fan->outer[j].r = work->config->outer.r * (3 - j) / 3;
                fan->outer[j].g = work->config->outer.g * (3 - j) / 3;
                fan->outer[j].b = work->config->outer.b * (3 - j) / 3;
            }
        }
        work->field_2F14 = 4096;
        work->field_2F16 = 1024;
        work->field_2F1C = 0;
        work->field_2F20 = 0;
        work->field_2F24 = 0;
        work->field_2F28 = 0;
        work->field_2F2C = 0;
        work->field_2F10 = 0;
        work->field_2F18 = 0;
        work->field_2F30 = 0;
        work->field_2F12 = 0;
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
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        GsGetLwUnit(work->parts[0], &work->matrix);
        setVector(&work->direction, work->target.vx - work->matrix.t[0],
            work->target.vy - work->matrix.t[1], work->target.vz - work->matrix.t[2]);
        setVector(&work->origin, work->target.vx, -1024, work->target.vz);
        for (i = 0, n = 0, bloom = work->blooms; i < 8; i++, n = i * 512, bloom++) {
            bloom->origin.vx = work->target.vx;
            bloom->origin.vy = -1024;
            bloom->origin.vz = work->target.vz;
            bloom->point.vx = work->target.vx + (rcos(n) * 512 >> 12);
            bloom->point.vy = 0;
            bloom->point.vz = work->target.vz + (rsin(n) * 512 >> 12);
            bloom->direction.vx = bloom->point.vx - bloom->origin.vx;
            bloom->direction.vy = bloom->point.vy - bloom->origin.vy;
            bloom->direction.vz = bloom->point.vz - bloom->origin.vz;
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
        if (work->config->fan_start <= work->elapsed) {
            func_8013D888(context);
            func_8013CD3C(context);
        }
        if (work->config->mesh_start <= work->elapsed) {
            func_8013C738(context);
        }
        if (work->config->bloom_start <= work->elapsed) {
            func_8013C110(context);
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
    if ((u32)(work->phase - 3) < 2) {
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
