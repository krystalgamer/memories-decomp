#include "../../types.h"
#include "variant768_entry.h"

/* Header 768's entry: four model parts leave fading quad trails; from the
 * burst start sparks fan out from the burst centre under a flash quad and a
 * pulsing ring, two shrinking columns of textured quads drift sideways and,
 * once they pass the slot, dots fall and a textured ring grows. */

static const VECTOR D_8013D464 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model768State *work;
    MATRIX light;
    MATRIX m;
    POLY_GT4 gt4;
    POLY_G4 g4;
    POLY_G3 g3;
    POLY_FT4 ft4;
    POLY_FT4 ring;
    GsBOXF box;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    u16 p_list[32];
    u16 flag_list[32];
    SVECTOR quad[4];
    SVECTOR base[2];
    /* Never referenced, but retail's frame still reserves its 16 bytes. */
    VECTOR unused;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 side;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 a, b;
    s32 otz;
    s32 sx, sy, sz;
    SVECTOR *v;
    SVECTOR *w;
    SVECTOR *x;
    SVECTOR *y;
    DVECTOR *d;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013D464;
    work = (Model768State *)context;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D4E4[command];
        v = &work->trails.flat[0];
        w = &work->trails.flat[32];
        x = &work->trails.flat[64];
        y = &work->trails.flat[96];
        for (i = 0; i < 32; v++, w++, x++, y++, i++) {
            setVector(v, 0, 0, 0);
            setVector(w, 0, 0, 0);
            setVector(x, 0, 0, 0);
            setVector(y, 0, 0, 0);
        }
        work->color[0] = work->config->r;
        work->color[1] = work->config->g;
        work->color[2] = work->config->b;
        work->color2[0] = work->config->r;
        work->color2[1] = work->config->g;
        work->color2[2] = work->config->b;
        /* Retail keeps walking w through the flat view, so it seeds the
         * first four part2 entries rather than the left column. */
        v = work->right;
        for (i = 0; i < 4; v++, w++, i++) {
            setVector(v, 0, -350, -side * 450);
            setVector(w, 0, -350, -side * 450);
        }
        v = work->sparks;
        for (i = 0; i < work->config->spark_count; v++, i++) {
            v->vx = work->config->spark_radius * ccos(rand() % 4096) / 4096;
            v->vy = work->config->spark_radius * csin(rand() % 4096) / 4096;
            v->vz = side * work->config->spark_radius * csin(rand() % 4096) / 4096;
        }
        v = work->dots;
        w = work->dot_velocities;
        d = work->screen;
        for (i = 0; i < work->config->dot_count; v++, w++, d++, i++) {
            setVector(v, 0, 0, 0);
            a = rand() % 4096;
            b = rand() % 4096;
            w->vx = work->config->dot_speed * ccos(a) / 4096 * ccos(b) / 4096;
            w->vy = work->config->dot_speed * csin(a) / 4096 * ccos(b) / 4096;
            w->vz = work->config->dot_speed * csin(a) / 4096 * ccos(b) / 4096;
            d->vy = 0;
            d->vx = 0;
            work->depths[i] = 0;
        }
        v = work->inner;
        w = work->outer;
        for (i = 0; i < 16; v++, w++, i++) {
            v->vx = work->config->inner_radius * ccos((i << 8) % 4096) / 4096;
            v->vy = work->config->inner_radius * csin((i << 8) % 4096) / 4096;
            v->vz = work->config->inner_radius * ccos(160) / 4096;
            w->vx = work->config->outer_radius * ccos((i << 8) % 4096) / 4096;
            w->vy = work->config->outer_radius * csin((i << 8) % 4096) / 4096;
            w->vz = work->config->outer_radius * ccos(160) / 4096;
        }
        Model_GetSlotDimensionsWithMinimums(slot, (ModelEffectAdjustment *)&pos);
        v = work->ring_inner;
        w = work->ring_outer;
        if (pos.vx > pos.vz) {
            for (i = 0; i < 9; v++, w++, i++) {
                v->vx = pos.vx * (ccos(i << 8) % 4096) / 4096;
                v->vy = 0;
                v->vz = -side * pos.vx * (csin(i << 8) % 4096) / 4096;
                w->vx = pos.vx * ((s32)((u32)(ccos(i << 8) % 4096) << 1)) / 4096;
                w->vy = -work->config->ring_height * (csin(i << 8) % 4096) / 4096;
                w->vz = side * -(pos.vx * 2) * (csin(i << 8) % 4096) / 4096;
            }
        } else {
            for (i = 0; i < 9; v++, w++, i++) {
                v->vx = pos.vz * (ccos(i << 8) % 4096) / 4096;
                v->vy = 0;
                v->vz = -side * pos.vz * (csin(i << 8) % 4096) / 4096;
                w->vx = pos.vz * ((s32)((u32)(ccos(i << 8) % 4096) << 1)) / 4096;
                w->vy = -work->config->ring_height * (csin(i << 8) % 4096) / 4096;
                w->vz = side * -(pos.vz * 2) * (csin(i << 8) % 4096) / 4096;
            }
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D474[i]);
        }
        work->ring_scale = 0x1000;
        work->ambient = 0x800;
        work->center.vx = 0;
        work->center.vy = 0;
        work->center.vz = 0;
        work->elapsed = 0;
        work->phase = 0;
        work->started = 0;
        work->frame = 0;
        work->burst_level = 0x80;
        work->quad_level = 0x80;
        work->burst_count = 0;
        work->flash = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase >= 2) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        PopMatrix();
        return 0;
    }

    if (work->elapsed <= work->config->trail_end) {
        v = work->trails.s.part0;
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[0]), &m);
        v[work->frame].vx = m.t[0];
        v[work->frame].vy = m.t[1];
        v[work->frame].vz = m.t[2];
        w = work->trails.s.part1;
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[1]), &m);
        w[work->frame].vx = m.t[0];
        w[work->frame].vy = m.t[1];
        w[work->frame].vz = m.t[2];
        x = work->trails.s.part2;
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[2]), &m);
        x[work->frame].vx = m.t[0];
        x[work->frame].vy = m.t[1];
        x[work->frame].vz = m.t[2];
        y = work->trails.s.part3;
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[3]), &m);
        y[work->frame].vx = m.t[0];
        y[work->frame].vy = m.t[1];
        y[work->frame].vz = m.t[2];
        if (work->elapsed >= work->config->burst_start && work->frame >= 2 && work->phase == 0) {
            work->phase = 1;
            work->center.vx = (work->trails.s.part0[work->frame].vx + work->trails.s.part2[work->frame].vx) / 2;
            work->center.vy = work->trails.s.part0[work->frame].vy;
            work->center.vz = work->trails.s.part0[work->frame].vz + side * 64;
            for (i = 0; i < 4; i++) {
                setVector(&work->left[i], (work->trails.s.part0 + work->frame)->vx,
                          (work->trails.s.part0 + work->frame)->vy, (work->trails.s.part0 + work->frame)->vz);
            }
            for (i = 0; i < 4; i++) {
                setVector(&work->right[i], (work->trails.s.part2 + work->frame)->vx,
                          (work->trails.s.part2 + work->frame)->vy, (work->trails.s.part2 + work->frame)->vz);
            }
        }
        work->frame = (work->frame + 1) % 32;
        if (work->frame < 2) {
            PopMatrix();
            work->elapsed += step;
            return 0;
        }
    }

    SetPolyG4(&g4);
    v = work->trails.s.part0;
    w = work->trails.s.part1;
    x = work->trails.s.part2;
    GsSetLsMatrix(&light);
    setVector(&pos, 0, 0, 0);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    MulMatrix2(&light, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    y = work->trails.s.part3;
    for (i = work->frame - 1; i > 0; i--) {
        otz = RotAverage4(&v[i], &v[i - 1], &w[i], &w[i - 1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                          (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
        setRGB0(&g4, work->color[0], work->color[1], work->color[2]);
        setRGB1(&g4, work->color[0], work->color[1], work->color[2]);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
        otz = RotAverage4(&x[i], &x[i - 1], &y[i], &y[i - 1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                          (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
    }

    if (work->elapsed > work->config->burst_start) {
        if (work->color[0] >= 32) {
            work->color[0] -= step * 4;
        } else {
            work->color[0] = 0;
        }
        if (work->color[1] >= 32) {
            work->color[1] -= step * 4;
        } else {
            work->color[1] = 0;
        }
        if (work->color[2] >= 32) {
            work->color[2] -= step * 4;
        } else {
            work->color[2] = 0;
        }
        SetPolyG3(&g3);
        setVector(&base[0], -work->config->base_half, 0, 0);
        setVector(&base[1], work->config->base_half, 0, 0);
        setRGB0(&g3, 0, 0, 0);
        setRGB1(&g3, work->color2[0], work->color2[1], work->color2[2]);
        setRGB2(&g3, work->color2[0], work->color2[1], work->color2[2]);
        GsSetLsMatrix(&light);
        setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->sparks;
        for (i = 0; i < work->config->spark_count; i++) {
            otz = RotAverage3(&v[i], &base[0], &base[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
        }

        setVector(&scale, 0x1000, 0x1000, 0x1000);
        SetPolyFT4(&ft4);
        ft4.tpage = work->texture[0] >> 16;
        ft4.clut = work->texture[0];
        setUV4(&ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
        setRGB0(&ft4, work->quad_level, work->quad_level, work->quad_level);
        setVector(&quad[0], -work->config->quad_half, -work->config->quad_half, 0);
        setVector(&quad[1], work->config->quad_half, -work->config->quad_half, 0);
        setVector(&quad[2], -work->config->quad_half, work->config->quad_half, 0);
        setVector(&quad[3], work->config->quad_half, work->config->quad_half, 0);
        GsSetLsMatrix(&light);
        setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
        }

        if (work->flash < 64) {
            SetPolyFT4(&ring);
            setRGB0(&ring, work->quad_level, work->quad_level, work->quad_level);
            v = work->ring_inner;
            ring.tpage = work->texture[2] >> 16;
            ring.clut = work->texture[2];
            setUV4(&ring, 0x7F, 0xBF, 0x7F, 0x40, 0, 0xBF, 0, 0x40);
            setVector(&pos, 0, 0, -side * 450);
            w = work->ring_outer;
            sz = work->flash * 2048 + 4096;
            sx = sz;
            sy = sz;
            if (sx > 0x7FCD) {
                sx = 0x7FCD;
            }
            if (sz > 0x7FCD) {
                sz = 0x7FCD;
            }
            if (sy > 0x7FCD) {
                sy = 0x7FCD;
            }
            setVector(&scale, sx, sz, sy);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < 8; i++) {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&ring.x0,
                                  (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)otz, 1);
                }
            }
            work->flash++;
        }
        if (work->quad_level >= 16) {
            work->quad_level -= step * 4;
        } else {
            work->quad_level = 0;
        }
        if (work->color2[0] >= 16) {
            work->color2[0] -= step * 4;
        } else {
            work->color2[0] = 0;
        }
        if (work->color2[1] >= 16) {
            work->color2[1] -= step * 4;
        } else {
            work->color2[1] = 0;
        }
        if (work->color2[2] >= 16) {
            work->color2[2] -= step * 4;
        } else {
            work->color2[2] = 0;
        }
    }

    if (work->elapsed >= work->config->burst_start && work->burst_level != 0 && work->phase > 0 &&
        work->frame >= 2) {
        SetPolyGT4(&gt4);
        gt4.tpage = work->texture[3] >> 16;
        gt4.clut = work->texture[3];
        setUV4(&gt4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
        setVector(&quad[0], 0, -work->config->burst_half_height, side * -work->config->burst_half_depth);
        setVector(&quad[1], 0, -work->config->burst_half_height, side * work->config->burst_half_depth);
        setVector(&quad[2], 0, work->config->burst_half_height, side * -work->config->burst_half_depth);
        setVector(&quad[3], 0, work->config->burst_half_height, side * work->config->burst_half_depth);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        v = work->left;
        w = work->right;
        GsSetLsMatrix(&light);
        for (i = 0; i < 4; i++, w++) {
            setRGB0(&gt4, work->burst_level / (i + 1), work->burst_level / (i + 1), work->burst_level / (i + 1));
            setRGB1(&gt4, work->burst_level / (i * 3 + 1), work->burst_level / (i * 3 + 1),
                    work->burst_level / (i * 3 + 1));
            setRGB2(&gt4, work->burst_level / (i * 2 + 4), work->burst_level / (i * 2 + 4),
                    work->burst_level / (i * 2 + 4));
            setRGB3(&gt4, 0, 0, 0);
            setVector(&rot, 0, 0, -side * 512);
            setVector(&pos, v->vx, v->vy, v->vz);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                              (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
            }
            v++;
            setVector(&rot, 0, 0, side * 512);
            GsSetLsMatrix(&light);
            setVector(&pos, w->vx, w->vy, w->vz);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                              (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
            }
        }
        v = work->left;
        w = work->right;
        for (i = 0; i < work->burst_count + 1; v++, w++, i++) {
            v->vz += side * work->config->burst_drift;
            w->vz += side * work->config->burst_drift;
        }
        if (work->burst_count >= 5) {
            work->burst_count = 3;
        } else if (work->elapsed % 3 != 0) {
            work->burst_count++;
        }
    }

    if (side > 0) {
        if (work->left[0].vz > side * 450) {
            work->started = 1;
        }
    } else if (work->left[0].vz < side * 450) {
        work->started = 1;
    }
    if (work->started > 0) {
        if (work->phase >= 2) {
            box.w = 2;
            box.h = 1;
            box.r = 0xF0;
            box.g = 0;
            box.b = 0;
            setVector(&pos, 0, -414, side * 450);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            box.attribute = 0x40000000;
            flag = 0;
            v = work->dots;
            d = work->screen;
            for (i = 0; i < work->config->dot_count; v++, d++, i++) {
                if (v->vy < 350) {
                    box.x = d->vx;
                    box.y = d->vy;
                    if (work->depths[i] != 0) {
                        GsSortBoxFill(&box, ot, work->depths[i] / 4);
                    }
                }
            }
            v = work->dots;
            d = work->screen;
            RotTransPersN(v, d, work->depths, p_list, flag_list, work->config->dot_count);
            box.attribute = 0;
            flag = 0;
            for (i = 0; i < work->config->dot_count; v++, d++, i++) {
                if (v->vy < 350) {
                    box.x = d->vx;
                    box.y = d->vy;
                    if (work->depths[i] != 0) {
                        GsSortBoxFill(&box, ot, work->depths[i] / 4);
                    }
                    flag = 1;
                }
            }
            gteMIMefunc(work->dots, work->dot_velocities, work->config->dot_count, 0x800);
            w = work->dot_velocities;
            for (i = 0; i < work->config->dot_count; w++, i++) {
                w->vy += (350 - w->vy) / work->config->dot_speed / 2;
            }

            setPolyFT4(&ring);
            v = work->inner;
            w = work->outer;
            ring.tpage = work->texture[2] >> 16;
            ring.clut = work->texture[2];
            setUV4(&ring, 0x7F, 0xBF, 0x7F, 0x40, 0, 0xBF, 0, 0x40);
            setRGB0(&ring, work->burst_level, work->burst_level, work->burst_level);
            setVector(&pos, 0, -414, side * 450);
            setVector(&scale, work->ring_scale, work->ring_scale, work->ring_scale);
            setVector(&rot, 0, 0, 0);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < 16; i++) {
                if (i == 15) {
                    otz = RotAverage4(&v[i], v, &w[15], w, (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1,
                                      (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&ring.x0,
                                      (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)otz, 1);
                }
            }
            work->ring_scale += step * 2048;
            if (work->ring_scale > 0x7FD7) {
                work->ring_scale = 0x7FD7;
            }
        }
        if (work->burst_level >= 16) {
            work->burst_level -= step * 4;
        } else {
            work->burst_level = 0;
        }
        if (work->phase == 1) {
            work->phase = 2;
            work->elapsed += step;
            PopMatrix();
            return 1;
        }
        if (work->burst_level == 0 && work->phase == 2) {
            work->elapsed += step;
            PopMatrix();
            return 2;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->elapsed > work->config->report_start && work->elapsed < work->config->end) {
        return 1;
    }
    return (work->elapsed > work->config->end) * 2;
}
