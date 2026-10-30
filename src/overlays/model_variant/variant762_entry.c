#include "../../types.h"
#include "variant762_entry.h"

/* Header 762's entry: after the delay staggered sparks shrink in towards the
 * slot while a flash sprite grows over it; from the wing state a fan and a
 * spray of shards burst from the drifting centre and fade, and after the
 * release the dots fall, a pulsing ring of quads spreads and four rolled
 * wings rise until they pass the side's limit. The first wing frame returns
 * 1 and the end returns 2 once every colour has faded. */

static const VECTOR D_8013CF4C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model762State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_FT4 wing;
    POLY_G3 g3;
    GsBOXF box;
    POLY_FT4 ring;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
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

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CF4C;
    work = (Model762State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CFCC[command];
        v = work->sparks;
        for (i = 0; i < 32; v++, i++) {
            a = rand() % 4096;
            b = rand() % 2048 + (~Model_GetActiveSlotIndex() & 1) * 2048 - 1024;
            v->vx = work->config->spark_radius * ccos(a) / 4096 * ccos(b) / 4096;
            v->vy = work->config->spark_radius * csin(a) / 4096 * ccos(b) / 4096;
            v->vz = work->config->spark_radius * ccos(b) / 4096;
        }
        v = &work->fan[0];
        setVector(v, 0, 0, 0);
        v++;
        for (i = 0; i < 16; v++, i++) {
            v->vx = work->config->spark_radius * ccos((i << 8) % 4096) / 4096;
            v->vy = work->config->spark_radius * csin((i << 8) % 4096) / 4096;
            v->vz = 0;
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CF5C[i]);
        }
        setVector(&work->drift, 0, 0, 0);
        v = work->shard.s.shards;
        for (i = 0; i < work->config->shard_count; v++, i++) {
            v->vx = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            v->vy = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            v->vz = 0;
        }
        setVector(&work->shard.flat[0], -work->config->base_half, -work->config->base_half, 0);
        setVector(&work->shard.flat[1], work->config->base_half, -work->config->base_half, 0);
        setVector(&work->shard.flat[2], -work->config->base_half, work->config->base_half, 0);
        setVector(&work->shard.flat[3], work->config->base_half, work->config->base_half, 0);
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
            v->vz = direction * work->config->inner_radius * ccos(0x80) / 4096;
            w->vx = work->config->outer_radius * ccos((i << 8) % 4096) / 4096;
            w->vy = work->config->outer_radius * csin((i << 8) % 4096) / 4096;
            w->vz = direction * work->config->outer_radius * ccos(0x80) / 4096;
        }
        work->colors.c.color[0] = 0x80;
        work->colors.c.color[1] = 0x80;
        work->colors.c.color[2] = 0x80;
        work->colors.c.color2[0] = 0x80;
        work->colors.c.color2[1] = 0x80;
        work->colors.c.color2[2] = 0x80;
        work->colors.c.color3[0] = work->config->r;
        work->colors.c.color3[1] = work->config->g;
        work->colors.c.color3[2] = work->config->b;
        work->ambient = 0x800;
        work->wing_scale = 0x1000;
        work->pulse = 0;
        work->flash = 0;
        work->phase = 0;
        work->state = 0;
        work->elapsed = 0;
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
    } else if (work->phase == 1) {
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

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed <= work->config->release) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        D_8013CFFC.vx = pos.vx = m.t[0];
        pos.vy = m.t[1];
        pos.vz = m.t[2];
        D_8013CFFC.vy = pos.vy;
        D_8013CFFC.vz = pos.vz;
    }

    if (work->elapsed < work->config->spark_end) {
        SetPolyFT4(&ft4);
        ft4.tpage = work->texture[3] >> 16;
        ft4.clut = work->texture[3];
        setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
        GsSetLsMatrix(&light);
        RotTrans(&D_8013CFFC, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->sparks;
        for (i = 0; i < 32; v++, i++) {
            if (work->elapsed >= i * work->config->spark_stagger
                && work->elapsed < i * work->config->spark_stagger + work->config->spark_life) {
                otz = RotTransPers(v, (PSXLONG *)&ft4.x0, &p, &flag);
                /* Reusing "a" for the sprite size gives retail's saved register. */
                a = 16 - (work->elapsed - i * work->config->spark_stagger) * 16 / work->config->spark_life;
                setUV4(&ft4, 0, 0x40, 31, 0x40, 0, 0x5F, 31, 0x5F);
                ft4.x1 = ft4.x0 + a;
                ft4.y1 = ft4.y0 - a;
                ft4.x2 = ft4.x0 - a;
                ft4.y2 = ft4.y0 + a;
                ft4.x3 = ft4.x0 + a;
                ft4.y3 = ft4.y0 + a;
                ft4.x0 = ft4.x0 - a;
                ft4.y0 = ft4.y0 - a;
                SetSemiTrans(&ft4, 1);
                if ((work->elapsed + i) % 4 < 2) {
                    setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
                } else {
                    setRGB0(&ft4, 0, 0, 0);
                }
                if (otz >= 0) {
                    GsSortPoly(&ft4, ot, otz);
                }
                a = work->config->spark_life + i * work->config->spark_stagger - work->elapsed;
                v->vx = v->vx * (a - 1) / a;
                v->vy = v->vy * (a - 1) / a;
                v->vz = v->vz * (a - 1) / a;
            }
        }
    }
    if (work->elapsed < work->config->spark_end) {
        work->flash += work->config->flash_growth;
    }

    SetPolyFT4(&ft4);
    ft4.tpage = work->texture[0] >> 16;
    ft4.clut = work->texture[0];
    setRGB0(&ft4, work->colors.c.color[0], work->colors.c.color[1], work->colors.c.color[2]);
    if (work->flash >= 0x100) {
        work->flash = 0xFF;
    }
    setVector(&quad[0], -work->flash, -work->flash, 0);
    setVector(&quad[1], work->flash, -work->flash, 0);
    setVector(&quad[2], -work->flash, work->flash, 0);
    setVector(&quad[3], work->flash, work->flash, 0);
    GsSetLsMatrix(&light);
    RotTrans(&D_8013CFFC, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                      (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
    setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
    SetSemiTrans(&ft4, 1);
    if (otz >= 0) {
        GsSortPoly(&ft4, ot, otz);
    }

    if (work->state > 0) {
        SetPolyG3(&g3);
        setVector(&pos, D_8013CFFC.vx, D_8013CFFC.vy, D_8013CFFC.vz);
        pos.vx += work->drift.vx;
        pos.vy += work->drift.vy;
        pos.vz += work->drift.vz;
        if (pos.vz > direction * 450) {
            pos.vz = direction * 450;
        }
        v = work->fan;
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setRGB0(&g3, work->colors.c.color3[0], work->colors.c.color3[1], work->colors.c.color3[2]);
        setRGB1(&g3, 0, 0, 0);
        setRGB2(&g3, 0, 0, 0);
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
        v = work->fan;
        setRGB0(&g3, work->colors.c.color2[0], work->colors.c.color2[1], work->colors.c.color2[2]);
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
        setRGB1(&g3, work->colors.c.color3[0], work->colors.c.color3[1], work->colors.c.color3[2]);
        setRGB2(&g3, work->colors.c.color3[0], work->colors.c.color3[1], work->colors.c.color3[2]);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        v = work->shard.s.base;
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        w = work->shard.s.shards;
        for (i = 0; i < work->config->shard_count; i++) {
            otz = RotAverage3(&w[i], v, &v[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
            }
        }
        setRGB0(&g3, 0, 0, 0);
        setRGB1(&g3, work->colors.c.color2[0], work->colors.c.color2[1], work->colors.c.color2[2]);
        setRGB2(&g3, work->colors.c.color2[0], work->colors.c.color2[1], work->colors.c.color2[2]);
        setVector(&scale, 0x800, 0x800, 0x800);
        GsSetLsMatrix(&light);
        v = work->shard.s.base;
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        w = work->shard.s.shards;
        for (i = 0; i < work->config->shard_count; i++) {
            otz = RotAverage3(&w[i], v, &v[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
        }
        if (work->colors.c.color3[0] >= 16) {
            work->colors.c.color3[0] -= 12;
        } else {
            work->colors.c.color3[0] = 0;
        }
        if (work->colors.c.color3[1] >= 16) {
            work->colors.c.color3[1] -= 12;
        } else {
            work->colors.c.color3[1] = 0;
        }
        if (work->colors.c.color3[2] >= 16) {
            work->colors.c.color3[2] -= 12;
        } else {
            work->colors.c.color3[2] = 0;
        }
    }

    if (work->elapsed > work->config->release) {
        box.h = 1;
        box.w = 1;
        box.r = 0x80;
        box.g = 0x80;
        box.b = 0x80;
        setVector(&pos, D_8013CFFC.vx, D_8013CFFC.vy, D_8013CFFC.vz);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        box.attribute = 0x40000000;
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

        SetPolyFT4(&ring);
        v = work->inner;
        w = work->outer;
        setRGB0(&ring, work->colors.c.color[0], work->colors.c.color[1], work->colors.c.color[2]);
        ring.tpage = work->texture[1] >> 16;
        ring.clut = work->texture[1];
        setUV4(&ring, 0x60, 0x40, 0x60, 0xBF, 0xDF, 0x40, 0xDF, 0xBF);
        setVector(&pos, D_8013CFFC.vx, D_8013CFFC.vy, D_8013CFFC.vz);
        scale.vx = work->pulse + 0x1000;
        scale.vy = work->pulse + 0x1000;
        scale.vz = 0x1000;
        work->pulse += step << 9;
        if (work->pulse > 0x7FCD) {
            work->pulse = 0x7FCD;
        }
        setVector(&rot, 0, 0, 0);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w, (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1,
                                  (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&ring.x0,
                                  (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)otz, 3);
            }
        }
        if (work->colors.c.color[0] >= 16) {
            work->colors.c.color[0] -= 12;
        } else {
            work->colors.c.color[0] = 0;
        }
        if (work->colors.c.color[1] >= 16) {
            work->colors.c.color[1] -= 12;
        } else {
            work->colors.c.color[1] = 0;
        }
        if (work->colors.c.color[2] >= 16) {
            work->colors.c.color[2] -= 12;
        } else {
            work->colors.c.color[2] = 0;
        }

        SetPolyFT4(&wing);
        setRGB0(&wing, work->colors.c.color2[0], work->colors.c.color2[1], work->colors.c.color2[2]);
        wing.tpage = work->texture[2] >> 16;
        wing.clut = work->texture[2];
        setVector(&pos, D_8013CFFC.vx, D_8013CFFC.vy, D_8013CFFC.vz);
        pos.vx += work->drift.vx;
        pos.vy += work->drift.vy;
        pos.vz += work->drift.vz;
        if (direction > 0) {
            if (pos.vz > direction * 450) {
                if (work->state == 0) {
                    work->wing_scale = 0x2000;
                    work->state = 1;
                } else {
                    work->wing_scale += work->config->wing_growth;
                }
                if (work->colors.c.color2[0] >= 16) {
                    work->colors.c.color2[0] -= 12;
                } else {
                    work->colors.c.color2[0] = 0;
                }
                if (work->colors.c.color2[1] >= 16) {
                    work->colors.c.color2[1] -= 12;
                } else {
                    work->colors.c.color2[1] = 0;
                }
                if (work->colors.c.color2[2] >= 16) {
                    work->colors.c.color2[2] -= 12;
                } else {
                    work->colors.c.color2[2] = 0;
                }
            }
        } else {
            if (pos.vz < direction * 450) {
                if (work->state == 0) {
                    work->wing_scale = 0x2000;
                    work->state = 1;
                } else {
                    work->wing_scale += work->config->wing_growth;
                }
                if (work->colors.c.color2[0] >= 16) {
                    work->colors.c.color2[0] -= 12;
                } else {
                    work->colors.c.color2[0] = 0;
                }
                if (work->colors.c.color2[1] >= 16) {
                    work->colors.c.color2[1] -= 12;
                } else {
                    work->colors.c.color2[1] = 0;
                }
                if (work->colors.c.color2[2] >= 16) {
                    work->colors.c.color2[2] -= 12;
                } else {
                    work->colors.c.color2[2] = 0;
                }
            }
        }
        setRGB0(&wing, work->colors.c.color2[0], work->colors.c.color2[1], work->colors.c.color2[2]);
        setVector(&quad[0], 0, -work->config->wing_half, direction * -work->config->wing_half);
        setVector(&quad[1], 0, -work->config->wing_half, direction * work->config->wing_half);
        setVector(&quad[2], 0, work->config->wing_half, direction * -work->config->wing_half);
        setVector(&quad[3], 0, work->config->wing_half, direction * work->config->wing_half);
        scale.vx = work->wing_scale;
        scale.vy = work->wing_scale;
        scale.vz = 0x1000;
        for (i = 0; i < 4; i++) {
            setVector(&rot, 0, 0, i << 9);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&wing.x0,
                              (PSXLONG *)&wing.x1, (PSXLONG *)&wing.x2, (PSXLONG *)&wing.x3, &p, &flag);
            setUV4(&wing, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F, 0);
            SetSemiTrans(&wing, 1);
            if (otz >= 0) {
                GsSortPoly(&wing, ot, otz);
            }
        }
        if (work->state > 0) {
            work->drift.vz += direction * work->config->wing_climb / 3;
        } else {
            work->drift.vz += direction * work->config->wing_climb;
        }
    }

    PopMatrix();
    if (work->state == 1) {
        work->phase = 1;
        work->state = 2;
        work->elapsed += step;
        return 1;
    }
    if (work->colors.w.w0 != 0 || work->colors.w.w1 != 0 || work->colors.w.last != 0) {
        work->elapsed += step;
        return 0;
    }
    return 2;
}
