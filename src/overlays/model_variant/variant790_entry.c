#include "../../types.h"
#include "variant790_entry.h"
#include "../../psyq/abs.h"

/* Header 790's entry: every period a spinning star leaves a model part and
 * flies to a jittered point before the opposing slot, drawing a fading
 * red, green and blue ribbon behind it. On arrival it bounces back, glows
 * and throws animated sparks while the ambient light dims and recovers. */

static const VECTOR D_8013CEB0 = {4096, 4096, 4096, 0};

static const Model790Shape D_8013CEC0 = {{
    {-190, 100, 0, 0}, {-150, -150, 0, 0}, {100, -190, 0, 0}, {210, 30, 0, 0},
    {30, 210, 0, 0}, {0, 0, 0, 0}, {-420, -70, 0, 0}, {-60, -420, 0, 0},
    {380, -190, 0, 0}, {300, 300, 0, 0}, {-200, 380, 0, 0},
}};

static const Model790Faces D_8013CF18 = {{
    {8, 2, 3, 5}, {9, 3, 4, 5}, {2, 7, 5, 1}, {5, 1, 0, 6}, {4, 5, 10, 0},
}};

s32 func_8013B004(u8 *context, s32 command)
{
    Model790State *work;
    MATRIX light;
    MATRIX m;
    POLY_F4 f4;
    POLY_FT4 ft4;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    Model790Shape shape;
    Model790Faces faces;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 k;
    s32 q;
    s32 otz;
    s32 last;
    u8 *face;
    SVECTOR *vtx;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CEB0;
    shape = D_8013CEC0;
    faces = D_8013CF18;
    work = (Model790State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CF64[command];
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->velocity[i], 0, 0, 0);
            setVector(&work->target[i], rot.vx / 2 * ((rand() - rand()) % 4096) / 4096,
                      rot.vy / 2 * ((rand() - rand()) % 4096) / 4096 - 350, direction * 450);
            work->bounced[i] = 0;
            work->r[i] = work->config->r;
            work->g[i] = work->config->g;
            work->b[i] = work->config->b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            work->trail_level[i] = 0x80;
            work->head[i] = 0;
            work->drawn[i] = 0;
            for (k = 0; k < work->config->spark_count; k++) {
                copyVector(&work->sparks[i][k], &work->target[i]);
                setVector(&work->spark_velocities[i][k],
                          work->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                          work->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                          -direction * work->config->spark_spread * (rand() % 4096) / 4096);
            }
            for (k = 0; k < 64; k++) {
                setVector(&work->trail[i][k], 0, 0, 0);
            }
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CF2C[i]);
        }
        work->level = 0x800;
        work->count = 0;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
        } else {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -16);
        }
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, -8);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 9, &rot, -8);
    }
    if (work->count == 0) {
        if (work->level > -0x200) {
            work->level -= step << 6;
        } else {
            work->level = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->level < 0x800) {
            work->level += step << 6;
        } else {
            work->phase = 2;
            work->level = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->start && work->elapsed < work->config->end &&
        work->frames % work->config->period == 0) {
        GsSetLsMatrix(&light);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&work->pos[work->count], m.t[0], m.t[1], m.t[2]);
        setVector(&work->velocity[work->count],
                  (work->target[work->count].vx - work->pos[work->count].vx) / work->config->travel_time,
                  (work->target[work->count].vy - work->pos[work->count].vy) / work->config->travel_time,
                  (work->target[work->count].vz - work->pos[work->count].vz) / work->config->travel_time);
        for (i = 0; i < 64; i++) {
            setVector(&work->trail[work->count][i], m.t[0], m.t[1], m.t[2]);
        }
        work->head[work->count] = (work->head[work->count] + 1) % 64;
        work->count++;
    }

    for (i = 0; i < work->count; i++) {
        if (work->r[i] != 0 || work->g[i] != 0 || work->b[i] != 0) {
            setPolyF4(&f4);
            setRGB0(&f4, work->r[i], work->g[i], work->b[i]);
            setVector(&scale, work->config->scale, work->config->scale, work->config->scale);
            rot.vz = (work->elapsed * 64) & 0xFFF;
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            vtx = shape.v;
            for (face = faces.flat, k = 0; k < 5; face += 4, k++) {
                Graphics_SubmitDrawModePacket(
                    (u32 *)&f4, ot,
                    (u16)RotTransPers4(&vtx[face[0]], &vtx[face[1]], &vtx[face[2]], &vtx[face[3]],
                                       (PSXLONG *)&f4.x0, (PSXLONG *)&f4.x1, (PSXLONG *)&f4.x2,
                                       (PSXLONG *)&f4.x3, &p, &flag),
                    1);
            }
            copyVector(&work->trail[i][work->head[i]], &work->pos[i]);
            work->head[i] = (work->head[i] + 1) % 64;
            addVector(&work->pos[i], &work->velocity[i]);
            if (ABS(work->pos[i].vz) >= 450 && work->bounced[i] == 0) {
                work->pos[i].vz = direction * 450;
                /* The pointer-form self copy keeps the original's dead x/y loads. */
                setVector(&work->velocity[i], (&work->velocity[i])->vx, (&work->velocity[i])->vy,
                          -((&work->velocity[i])->vz / 4));
                work->bounced[i] = 1;
            }
            setPolyG4(&g4);
            if (work->head[i] > work->drawn[i] && (work->r[i] != 0 || work->g[i] != 0 || work->b[i] != 0)) {
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                /* LAST stops GCC folding head - 1 - drawn into head - (drawn + 1). */
                for (k = work->head[i] - 1, q = 1; k >= (last = work->head[i] - 1) - work->drawn[i]; k--, q++) {
                    setRGB0(&g4, work->trail_level[i] / q, 0, 0);
                    setRGB1(&g4, work->trail_level[i] / q, 0, 0);
                    setRGB2(&g4, work->trail_level[i] / (q + 1), 0, 0);
                    setRGB3(&g4, work->trail_level[i] / (q + 1), 0, 0);
                    setVector(&quad[0], work->trail[i][k].vx, work->trail[i][k].vy - work->config->trail_width / q,
                              work->trail[i][k].vz);
                    setVector(&quad[1], work->trail[i][k].vx, work->trail[i][k].vy, work->trail[i][k].vz);
                    setVector(&quad[2], work->trail[i][k - 1].vx,
                              work->trail[i][k - 1].vy - work->config->trail_width / (q + 1),
                              work->trail[i][k - 1].vz);
                    setVector(&quad[3], work->trail[i][k - 1].vx, work->trail[i][k - 1].vy,
                              work->trail[i][k - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    setRGB0(&g4, 0, work->trail_level[i] / q, 0);
                    setRGB1(&g4, 0, work->trail_level[i] / q, 0);
                    setRGB2(&g4, 0, work->trail_level[i] / (q + 1), 0);
                    setRGB3(&g4, 0, work->trail_level[i] / (q + 1), 0);
                    setVector(&quad[0], work->trail[i][k].vx - work->config->trail_width / q,
                              work->trail[i][k].vy + work->config->trail_width / q, work->trail[i][k].vz);
                    setVector(&quad[1], work->trail[i][k].vx, work->trail[i][k].vy, work->trail[i][k].vz);
                    setVector(&quad[2], work->trail[i][k - 1].vx - work->config->trail_width / (q + 1),
                              work->trail[i][k - 1].vy + work->config->trail_width / (q + 1),
                              work->trail[i][k - 1].vz);
                    setVector(&quad[3], work->trail[i][k - 1].vx, work->trail[i][k - 1].vy,
                              work->trail[i][k - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    setRGB0(&g4, 0, 0, work->trail_level[i] / q);
                    setRGB1(&g4, 0, work->trail_level[i] / q, work->trail_level[i] / q);
                    setRGB2(&g4, 0, work->trail_level[i] / (q + 1), work->trail_level[i] / (q + 1));
                    setRGB3(&g4, 0, work->trail_level[i] / (q + 1), work->trail_level[i] / (q + 1));
                    setVector(&quad[0], work->trail[i][k].vx + work->config->trail_width / q,
                              work->trail[i][k].vy + work->config->trail_width / q, work->trail[i][k].vz);
                    setVector(&quad[1], work->trail[i][k].vx, work->trail[i][k].vy, work->trail[i][k].vz);
                    setVector(&quad[2], work->trail[i][k - 1].vx + work->config->trail_width / (q + 1),
                              work->trail[i][k - 1].vy + work->config->trail_width / (q + 1),
                              work->trail[i][k - 1].vz);
                    setVector(&quad[3], work->trail[i][k - 1].vx, work->trail[i][k - 1].vy,
                              work->trail[i][k - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                }
                work->drawn[i]++;
            }
            if (work->bounced[i] != 0) {
                if (work->r[i] >= 16) {
                    work->r[i] -= work->config->fade;
                } else {
                    work->r[i] = 0;
                }
                if (work->g[i] >= 16) {
                    work->g[i] -= work->config->fade;
                } else {
                    work->g[i] = 0;
                }
                if (work->b[i] >= 16) {
                    work->b[i] -= work->config->fade;
                } else {
                    work->b[i] = 0;
                }
                if (work->trail_level[i] >= 16) {
                    work->trail_level[i] -= work->config->fade;
                } else {
                    work->trail_level[i] = 0;
                }
            }
        }
        if (work->bounced[i] != 0 && (work->glow_r[i] != 0 || work->glow_g[i] != 0 || work->glow_b[i] != 0)) {
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&rot, 0, 0, 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
            ft4.clut = work->texture[0];
            ft4.tpage = work->texture[0] >> 16;
            GsSetLsMatrix(&light);
            RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
            setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
            setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
            setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            for (k = 0; k < work->config->spark_count; k++) {
                ft4.clut = work->texture[1];
                ft4.tpage = work->texture[1] >> 16;
                GsSetLsMatrix(&light);
                RotTrans(&work->sparks[i][k], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, work->elapsed % 3 * 32 + 128, 0, work->elapsed % 3 * 32 + 159, 0,
                       work->elapsed % 3 * 32 + 128, 0x1F, work->elapsed % 3 * 32 + 159, 0x1F);
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                addVector(&work->sparks[i][k], &work->spark_velocities[i][k]);
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
        }
    }
    work->elapsed += step;
    work->frames++;
    PopMatrix();
    if (work->phase == 2 && work->glow_r[work->count - 1] == 0 && work->glow_g[work->count - 1] == 0 &&
        work->glow_b[work->count - 1] == 0) {
        return 2;
    }
    if (work->elapsed > work->config->end && work->bounced[work->count - 1] == 1 && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->bounced[0] != 0 && work->phase == 0) {
        return 4;
    }
    return 0;
}
