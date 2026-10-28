#include "../../types.h"
#include "variant763_entry.h"

/* Header 763's entry: after the delay two fans and a spray of shards flash in
 * front of the slot with a four-frame sprite, falling dots and a rolled
 * triangle while the ambient light dims and recovers; from the ring start two
 * pulsing rings of quads spread out and every colour fades. The first drawn
 * frame returns 1 and the end returns 2. */

static const VECTOR D_8013CB98 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model763State *work;
    MATRIX light;
    MATRIX m;
    POLY_G3 g3;
    POLY_FT4 ft4;
    POLY_G4 g4;
    POLY_G3 tri;
    GsBOXF box;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    u16 p_list[32];
    u16 flag_list[32];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 a, b;
    s32 otz;
    SVECTOR *v;
    SVECTOR *w;
    DVECTOR *d;
    s32 pulse;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CB98;
    work = (Model763State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CBE0[command];
        work->unknown_6B4 = 0;
        work->elapsed = 0;
        work->flash = 0;
        work->phase = 0;
        work->ambient = 0x7FE;
        v = &work->center;
        setVector(v, 0, 0, 0);
        v++;
        /* Pointer walks, with the angle recomputed inline rather than kept in
         * a variable, give retail's split address registers. */
        for (i = 0; i < 16; v++, i++) {
            v->vx = work->config->fan_radius * ccos((i << 8) % 4096) / 4096;
            v->vy = work->config->fan_radius * csin((i << 8) % 4096) / 4096;
            v->vz = 0;
        }
        v = &work->base[0];
        setVector(v, -work->config->base_half, 0, 0);
        v = &work->base[1];
        setVector(v, work->config->base_half, 0, 0);
        v = work->shards;
        for (i = 0; i < work->config->shard_count; v++, i++) {
            v->vx = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            v->vy = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            v->vz = 0;
        }
        v = work->inner;
        w = work->outer;
        for (i = 0; i < 16; v++, w++, i++) {
            v->vx = work->config->inner_radius * csin((i << 8) % 4096) / 4096;
            v->vy = 0;
            v->vz = work->config->inner_radius * ccos((i << 8) % 4096) / 4096;
            w->vx = work->config->outer_radius * csin((i << 8) % 4096) / 4096;
            w->vy = 0;
            w->vz = work->config->outer_radius * ccos((i << 8) % 4096) / 4096;
        }
        work->color[0] = work->config->r;
        work->color[1] = work->config->g;
        work->color[2] = work->config->b;
        work->color2[0] = 0xFF;
        work->color2[1] = 0xFF;
        work->color2[2] = 0xFF;
        setVector(&work->quad[0], -work->config->quad_half, -work->config->quad_half, 0);
        setVector(&work->quad[1], work->config->quad_half, -work->config->quad_half, 0);
        setVector(&work->quad[2], -work->config->quad_half, work->config->quad_half, 0);
        setVector(&work->quad[3], work->config->quad_half, work->config->quad_half, 0);
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CBA8[i]);
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
        v = &work->triangle[0];
        setVector(v, 0, -work->config->triangle_height, 0);
        v = &work->triangle[1];
        setVector(v, work->config->triangle_width / 2, work->config->triangle_height / 2, 0);
        v = &work->triangle[2];
        setVector(v, -work->config->triangle_width / 2, work->config->triangle_height / 2, 0);
        work->pulse = 0;
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
    } else if (work->phase > 0) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->ambient = 0x800;
            work->phase = 2;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }

    v = &work->center;
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyG3(&g3);
    setRGB0(&g3, work->color[0], work->color[1], work->color[2]);
    setRGB1(&g3, 0, 0, 0);
    setRGB2(&g3, 0, 0, 0);
    setVector(&pos, 0, -350, direction * 450);
    GsSetLsMatrix(&light);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    for (i = 0; i < 16; i++) {
        if (i == 15) {
            otz = RotAverage3(v, &v[16], &v[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
        } else {
            otz = RotAverage3(v, &v[i + 1], &v[i + 2], (PSXLONG *)&g3.x0,
                              (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
        }
    }
    setRGB0(&g3, work->color2[0], work->color2[1], work->color2[2]);
    setRGB1(&g3, 0, 0, 0);
    setRGB2(&g3, 0, 0, 0);
    setVector(&scale, 0x800, 0x800, 0x800);
    GsSetLsMatrix(&light);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    for (i = 0; i < 16; i++) {
        if (i == 15) {
            otz = RotAverage3(v, &v[16], &v[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
        } else {
            otz = RotAverage3(v, &v[i + 1], &v[i + 2], (PSXLONG *)&g3.x0,
                              (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
        }
    }
    setRGB0(&g3, 0, 0, 0);
    setRGB1(&g3, work->color[0], work->color[1], work->color[2]);
    setRGB2(&g3, work->color[0], work->color[1], work->color[2]);
    setVector(&scale, 0x1000, 0x1000, 0x1000);
    GsSetLsMatrix(&light);
    v = work->base;
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    w = work->shards;
    for (i = 0; i < work->config->shard_count; i++) {
        otz = RotAverage3(&w[i], v, &v[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                          (PSXLONG *)&g3.x2, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
        }
    }
    setRGB0(&g3, 0, 0, 0);
    setRGB1(&g3, work->color2[0], work->color2[1], work->color2[2]);
    setRGB2(&g3, work->color2[0], work->color2[1], work->color2[2]);
    setVector(&scale, 0x800, 0x800, 0x800);
    GsSetLsMatrix(&light);
    v = work->base;
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    w = work->shards;
    for (i = 0; i < work->config->shard_count; i++) {
        otz = RotAverage3(&w[i], v, &v[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                          (PSXLONG *)&g3.x2, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
        }
    }

    SetPolyFT4(&ft4);
    ft4.tpage = work->texture[0] >> 16;
    ft4.clut = work->texture[0];
    if (work->flash < 8) {
        setRGB0(&ft4, 0x80, 0x80, 0x80);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->quad[0], &work->quad[1], &work->quad[2], &work->quad[3], (PSXLONG *)&ft4.x0,
                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, work->flash * 32, 0x40, work->flash * 32 + 31, 0x40, work->flash * 32, 0x5F,
               work->flash * 32 + 31, 0x5F);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 0);
        }
        work->flash++;
    }

    box.h = 1;
    box.w = 1;
    box.r = 0x80;
    box.g = 0x80;
    box.b = 0x80;
    setVector(&pos, 0, -350, direction * 450);
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
                /* "/ 4" rather than ">> 2" keeps retail's extra copy. */
                GsSortBoxFill(&box, ot, work->depths[i] / 4);
            }
        }
    }
    v = work->dots;
    d = work->screen;
    RotTransPersN(v, d, work->depths, p_list, flag_list, work->config->dot_count);
    box.attribute = 0;
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
            flag = 1;
        }
    }
    gteMIMefunc(work->dots, work->dot_velocities, work->config->dot_count, 0x800);
    w = work->dot_velocities;
    for (i = 0; i < work->config->dot_count; w++, i++) {
        w->vy += (350 - w->vy) / work->config->dot_speed / 2;
    }

    SetPolyG3(&tri);
    setRGB0(&tri, work->color[0], work->color[1], work->color[2]);
    setRGB1(&tri, 0, 0, 0);
    setRGB2(&tri, 0, 0, 0);
    setVector(&pos, 0, -350, direction * 450);
    setVector(&rot, 0, 0, direction * work->config->triangle_roll);
    setVector(&scale, 0x1000, 0x1000, 0x1000);
    GsSetLsMatrix(&light);
    /* Assigned after the call so the scheduler orders it as retail does; the
     * repeated "direction * 450" is CSEd into the spilled slot retail uses. */
    v = work->triangle;
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    MulMatrix2(&light, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    otz = RotAverage3(v, &v[1], &v[2], (PSXLONG *)&tri.x0,
                      (PSXLONG *)&tri.x1, (PSXLONG *)&tri.x2, &p, &flag);
    if (otz >= 0) {
        Graphics_SubmitDrawModePacket((u32 *)&tri, ot, (u16)otz, 1);
    }
    if (work->phase == 2) {
        SetPolyG3(&tri);
        setRGB0(&tri, work->color2[0], work->color2[1], work->color2[2]);
        setRGB1(&tri, 0, 0, 0);
        setRGB2(&tri, 0, 0, 0);
        setVector(&pos, 0, -350, direction * 450);
        setVector(&rot, 0, 0, direction * work->config->triangle_roll);
        setVector(&scale, 0x800, 0x800, 0x800);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage3(v, &v[1], &v[2], (PSXLONG *)&tri.x0,
                          (PSXLONG *)&tri.x1, (PSXLONG *)&tri.x2, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&tri, ot, (u16)otz, 1);
        }

        pulse = work->pulse * 3 * 1024 + 4096;
        if (pulse > 0x7FCD) {
            pulse = 0x7FCD;
        }
        work->pulse++;
        SetPolyG4(&g4);
        setRGB0(&g4, work->color[0], work->color[1], work->color[2]);
        setRGB1(&g4, work->color[0], work->color[1], work->color[2]);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        setVector(&pos, -direction * work->config->ring_offset, 0, direction * 450);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, pulse, pulse, pulse);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }
        setVector(&scale, pulse / 2, pulse / 2, pulse / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }
        setRGB0(&g4, work->color2[0], work->color2[1], work->color2[2]);
        setRGB1(&g4, work->color2[0], work->color2[1], work->color2[2]);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        setVector(&scale, pulse - 255, pulse - 255, pulse - 255);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }
        setVector(&scale, (pulse - 255) / 2, (pulse - 255) / 2, (pulse - 255) / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p,
                                  &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }

        if (work->color[0] >= 32) {
            work->color[0] -= step * 8;
        } else {
            work->color[0] = 0;
        }
        if (work->color[1] >= 32) {
            work->color[1] -= step * 8;
        } else {
            work->color[1] = 0;
        }
        if (work->color[2] >= 32) {
            work->color[2] -= step * 8;
        } else {
            work->color[2] = 0;
        }
        if (work->color2[0] >= 32) {
            work->color2[0] -= step * 8;
        } else {
            work->color2[0] = 0;
        }
        if (work->color2[1] >= 32) {
            work->color2[1] -= step * 8;
        } else {
            work->color2[1] = 0;
        }
        if (work->color2[2] >= 32) {
            work->color2[2] -= step * 8;
        } else {
            work->color2[2] = 0;
        }
    }

    if (work->phase == 0) {
        work->phase = 1;
        PopMatrix();
        return 1;
    }
    if (work->elapsed < work->config->ring_start) {
        work->elapsed += step;
    } else {
        work->phase = 2;
        if (work->elapsed >= work->config->end) {
            PopMatrix();
            return 2;
        }
        work->elapsed += step;
    }
    PopMatrix();
    return 0;
}
