#include "../../types.h"
#include "variant804_entry.h"

/* Header 804's entry: bolts charge on the caster, then fly with ribbon
 * trails to jittered points on the opposing slot; on arrival each flares
 * into a glow with two fans of rays and a burst of drifting sparks while
 * the ambient light dims and recovers. */

static const VECTOR gModelVariant804_D2334 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 ModelVariant804_HandleRequest(u8 *context, s32 command)
{
    Model804State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_FT4 glow;
    POLY_G3 g3;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, j, k;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant804_D2334;
    work = (Model804State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant804_D2398[command];
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            work->bright[i] = 0x80;
            work->track.s.arrived[i] = 0;
            work->track.s.trail_head[i] = 1;
            work->track.s.tail[i] = 0;
            work->bolt_r[i] = work->config->r;
            work->bolt_g[i] = work->config->g;
            work->bolt_b[i] = work->config->b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            work->spark_r[i] = work->config->spark_r;
            work->spark_g[i] = work->config->spark_g;
            work->spark_b[i] = work->config->spark_b;
            setVector(&work->target[i], RANDOM_SPREAD(pos.vx / 2), RANDOM_SPREAD(pos.vy / 2) - 350,
                      direction * 450);
            for (j = 0; j < work->config->spark_count; j++) {
                setVector(&work->spark[i][j], 0, 0, 0);
                setVector(&work->spark_step[i][j], RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread), RANDOM_SPREAD(work->config->spark_spread));
            }
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread),
                          -direction * work->config->ray_spread * (rand() % 4096) / 4096);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant804_D2344[i]);
        }
        work->ambient = 0x800;
        work->track.s.phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->launch) {
        Model_QueueCameraOrbitFromPreset(slot, 2, &rot, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -12);
    }
    if (work->track.s.phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->track.s.phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->track.s.phase = 2;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed <= work->config->charge_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->charge_part), &m);
        setVector(&rot, m.t[0], m.t[1], m.t[2]);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->angle[i], direction * (direction * ratan2(pos.vz - rot.vz, pos.vy - rot.vy) - 0x400),
                      0, 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, 0x80, 0x80, 0x80);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->size, 0);
            setVector(&quad[1], 0, -work->config->size, direction * work->config->depth);
            setVector(&quad[2], 0, work->config->size, 0);
            setVector(&quad[3], 0, work->config->size, direction * work->config->depth);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag) + 2;
            setUV4(&ft4, 0, 0x40, 0x5F, 0x40, 0, 0x5F, 0x5F, 0x5F);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    }

    if (work->elapsed <= work->config->launch) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->aim_part), &m);
        setVector(&rot, m.t[0], m.t[1], m.t[2]);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->angle[i], direction * (direction * ratan2(rot.vz - pos.vz, rot.vy - pos.vy) - 0x400),
                      0, 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, 0x80, 0x80, 0x80);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->size, 0);
            setVector(&quad[1], 0, -work->config->size, direction * work->config->depth);
            setVector(&quad[2], 0, work->config->size, 0);
            setVector(&quad[3], 0, work->config->size, direction * work->config->depth);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0x40, 0x5F, 0x40, 0, 0x5F, 0x5F, 0x5F);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
            copyVector(&work->pos[i], &pos);
            setVector(&work->step[i], (work->target[i].vx - work->pos[i].vx) / work->config->steps,
                      (work->target[i].vy - work->pos[i].vy) / work->config->steps,
                      (work->target[i].vz - work->pos[i].vz) / work->config->steps);
            for (j = 0; j < 32; j++) {
                copyVector(&work->trail[i][j], &work->pos[i]);
            }
        }
        PopMatrix();
        work->elapsed += step;
        return 0;
    }

    for (i = 0; i < work->config->count; i++) {
        if (work->bright[i] != 0) {
            setVector(&work->angle[i], ratan2(work->step[i].vz, work->step[i].vy) - 0x400,
                      direction * ratan2(work->step[i].vx, work->step[i].vz), 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->bright[i], work->bright[i], work->bright[i]);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->size, 0);
            setVector(&quad[1], 0, -work->config->size, direction * work->config->depth);
            setVector(&quad[2], 0, work->config->size, 0);
            setVector(&quad[3], 0, work->config->size, direction * work->config->depth);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0x40, 0x5F, 0x40, 0, 0x5F, 0x5F, 0x5F);
            if (work->track.s.arrived[i] == 0) {
                setSemiTrans(&ft4, 0);
            } else {
                setSemiTrans(&ft4, 1);
            }
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
            if (work->track.s.arrived[i] == 0) {
                for (j = 0; j < work->config->spark_count; j++) {
                    copyVector(&work->spark[i][j], &work->pos[i]);
                }
            }
            copyVector(&work->trail[i][work->track.s.trail_head[i]], &work->pos[i]);
            work->track.s.trail_head[i] = (work->track.s.trail_head[i] + 1) % 32;
            addVector(&work->pos[i], &work->step[i]);
            if (ABS(work->pos[i].vz) > 450) {
                if (work->track.s.arrived[i] == 0) {
                    work->track.s.arrived[i] = 1;
                }
            }
            if (work->track.s.arrived[i] != 0) {
                if (work->bright[i] >= 16) {
                    work->bright[i] -= work->config->fade;
                } else {
                    work->bright[i] = 0;
                }
            }
        }

        if (work->track.s.trail_head[i] > work->track.s.tail[i]) {
            if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
                setPolyG4(&g4);
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                /* "head - (tail + 1)" keeps "head - 1" shared with the start index. */
                for (j = work->track.s.trail_head[i] - 1, k = 1;
                     j >= work->track.s.trail_head[i] - (work->track.s.tail[i] + 1); j--, k++) {
                    setRGB0(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB1(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB2(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setRGB3(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx,
                              work->trail[i][j].vy - (u16)(work->config->size / 3) / k, work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx,
                              work->trail[i][j].vy + (u16)(work->config->size / 3) / k, work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j].vy - (u16)(work->config->size / 3) / (k + 1),
                              work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j].vy + (u16)(work->config->size / 3) / (k + 1),
                              work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                }
            }
        }

        if (work->track.s.arrived[i]) {
            if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
                setVector(&rot, 0, 0, 0);
                setVector(&pos, 0, 0, 0);
                setPolyFT4(&glow);
                setRGB0(&glow, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                glow.tpage = work->texture[0] >> 16;
                glow.clut = work->texture[0];
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                  (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                setUV4(&glow, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB2(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    copyVector(&quad[0], &work->rays[i][j]);
                    setVector(&quad[1], 0, -work->config->ray_width, 0);
                    setVector(&quad[2], 0, work->config->ray_width, 0);
                    Graphics_SubmitDrawModePacket(
                        (u32 *)&g3, ot,
                        (u16)RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                         (PSXLONG *)&g3.x2, &p, &flag),
                        1);
                }
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB2(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                m = saved;
                setVector(&scale, 0x800, 0x800, 0x800);
                GsSetLsMatrix(&light);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->config->ray_count; j++) {
                    copyVector(&quad[0], &work->rays[i][j]);
                    setVector(&quad[1], 0, -work->config->ray_width, 0);
                    setVector(&quad[2], 0, work->config->ray_width, 0);
                    Graphics_SubmitDrawModePacket(
                        (u32 *)&g3, ot,
                        (u16)RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                         (PSXLONG *)&g3.x2, &p, &flag),
                        1);
                }
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                for (j = 0; j < work->config->spark_count; j++) {
                    glow.clut = work->texture[1];
                    glow.tpage = work->texture[1] >> 16;
                    GsSetLsMatrix(&light);
                    RotTrans(&work->spark[i][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                    setUV4(&glow, work->elapsed % 3 * 32 + 0x80, 0, work->elapsed % 3 * 32 + 0x9F, 0,
                           work->elapsed % 3 * 32 + 0x80, 0x1F, work->elapsed % 3 * 32 + 0x9F, 0x1F);
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                    addVector(&work->spark[i][j], &work->spark_step[i][j]);
                }
                if (work->bolt_r[i] >= 16) {
                    work->bolt_r[i] -= work->config->fade;
                } else {
                    work->bolt_r[i] = 0;
                }
                if (work->bolt_g[i] >= 16) {
                    work->bolt_g[i] -= work->config->fade;
                } else {
                    work->bolt_g[i] = 0;
                }
                if (work->bolt_b[i] >= 16) {
                    work->bolt_b[i] -= work->config->fade;
                } else {
                    work->bolt_b[i] = 0;
                }
                if (work->glow_r[i] >= 16) {
                    work->glow_r[i] -= work->config->fade;
                } else {
                    work->glow_r[i] = 0;
                }
                if (work->glow_g[i] >= 16) {
                    work->glow_g[i] -= work->config->fade;
                } else {
                    work->glow_g[i] = 0;
                }
                if (work->glow_b[i] >= 16) {
                    work->glow_b[i] -= work->config->fade;
                } else {
                    work->glow_b[i] = 0;
                }
                if (work->spark_r[i] >= 16) {
                    work->spark_r[i] -= work->config->fade;
                } else {
                    work->spark_r[i] = 0;
                }
                if (work->spark_g[i] >= 16) {
                    work->spark_g[i] -= work->config->fade;
                } else {
                    work->spark_g[i] = 0;
                }
                if (work->spark_b[i] >= 16) {
                    work->spark_b[i] -= work->config->fade;
                } else {
                    work->spark_b[i] = 0;
                }
            }
        } else {
            work->track.s.tail[i]++;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->track.s.phase == 2 && work->bolt_r[0] == 0 && work->bolt_g[0] == 0 && work->bolt_b[0] == 0) {
        return 2;
    }
    /* The phase and the first arrival flag are tested as one word. */
    if (work->track.phase_arrived == 0x10000) {
        work->track.s.phase = 1;
        return 1;
    }
    return 0;
}
