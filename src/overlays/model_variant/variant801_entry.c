#include "../../types.h"
#include "variant801_entry.h"

/* Header 801's entry: a ring of sprites spins up around the active slot,
 * then each flies towards a jittered point on the opposing slot trailing a
 * fading Gouraud ribbon; on arrival it bursts into animated sparks while
 * the ambient light dims and recovers. */

static const VECTOR gModelVariant801_D1544 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 ModelVariant801_HandleRequest(u8 *context, s32 command)
{
    Model801State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_FT4 spark;
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
    s32 a, b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant801_D1544;
    work = (Model801State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant801_D158C[command];
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rot);
        for (i = 0; i < work->config->count; i++) {
            /* J holds the angular step here, which fixes the register allocation. */
            j = 4096 / work->config->count;
            a = b = j * i;
            a %= 4096;
            setVector(&work->pos[i], work->config->radius * ccos(a) / 4096,
                      work->config->radius * csin(a) / 4096 - 350, -direction * 449);
            setVector(&work->target[i], rot.vx + RANDOM_SPREAD(pos.vx / 2), rot.vy + RANDOM_SPREAD(pos.vy / 2),
                      direction * 450);
            setVector(&work->step[i], (work->target[i].vx - work->pos[i].vx) / work->config->steps,
                      (work->target[i].vy - work->pos[i].vy) / work->config->steps,
                      (work->target[i].vz - work->pos[i].vz) / work->config->steps);
            setVector(&work->angle[i], 0, 0, b);
            work->arrived[i] = 0;
            work->level[i] = 0x80;
            work->tail[i] = 0;
            work->trail_head[i] = 1;
            for (j = 0; j < work->config->spark_count; j++) {
                setVector(&work->sparks[i][j], RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread));
                work->spark_frame[i][j] = 0;
            }
            for (j = 0; j < 32; j++) {
                copyVector(&work->trail[i][j], &work->pos[i]);
            }
        }
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant801_D1554[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant801_D1554[1]);
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->shown = 0;
        work->moving = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->launch) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rot, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -16);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 3;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < work->shown; i++) {
        if (work->level[i]) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->level[i], work->level[i], work->level[i]);
            setVector(&quad[0], 0, -work->config->height, 0);
            setVector(&quad[1], work->config->width, -work->config->height, 0);
            setVector(&quad[2], 0, work->config->height, 0);
            setVector(&quad[3], work->config->width, work->config->height, 0);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0x20, 0x3F, 0x20, 0, 0x3F, 0x3F, 0x3F);
            /* Each branch sorts its own quad; cross-jumping merges the two
             * calls, and the per-branch copy keeps &ft4 in a hoisted register. */
            if (work->arrived[i] == 0) {
                setSemiTrans(&ft4, 0);
                if (otz >= 0) {
                    GsSortPoly(&ft4, ot, (u16)otz);
                }
            } else {
                setSemiTrans(&ft4, 1);
                if (otz >= 0) {
                    GsSortPoly(&ft4, ot, (u16)otz);
                }
            }
        }
        if (work->arrived[i]) {
            setPolyFT4(&spark);
            spark.tpage = work->texture[1] >> 16;
            spark.clut = work->texture[1];
            setRGB0(&spark, 0x80, 0, 0);
            setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
            for (j = 0; j < work->config->spark_count; j++) {
                if (work->spark_frame[i][j] < 8) {
                    copyVector(&pos, &work->target[i]);
                    addVector(&pos, &work->sparks[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&spark.x0,
                                      (PSXLONG *)&spark.x1, (PSXLONG *)&spark.x2, (PSXLONG *)&spark.x3, &p,
                                      &flag);
                    setUV4(&spark, work->spark_frame[i][j] * 32, 0, work->spark_frame[i][j] * 32 + 31, 0,
                           work->spark_frame[i][j] * 32, 31, work->spark_frame[i][j] * 32 + 31, 31);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&spark, ot, (u16)otz, 1);
                    }
                    work->spark_frame[i][j]++;
                }
            }
        }
    }

    if (work->elapsed >= work->config->launch) {
        for (i = 0; i < work->moving; i++) {
            if (work->level[i]) {
                setVector(&work->angle[i], ratan2(work->step[i].vz, work->step[i].vy) - 0x400,
                          direction * (ratan2(work->step[i].vx, work->step[i].vz) - 0x400), 0);
                addVector(&work->pos[i], &work->step[i]);
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 32;
                setPolyG4(&g4);
                if (work->trail_head[i] > work->tail[i] && work->level[i]) {
                    setVector(&pos, 0, 0, 0);
                    setVector(&rot, 0, 0, 0);
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    k = 1;
                    /* "+ (-1 - tail)" keeps "trail_head - 1" computed before the tail is subtracted. */
                    for (j = work->trail_head[i] - 1; j >= work->trail_head[i] + (-1 - work->tail[i]); j--) {
                        setRGB0(&g4, work->level[i] / k, work->level[i] / k, work->level[i] / k);
                        setRGB1(&g4, work->level[i] / k, work->level[i] / k, work->level[i] / k);
                        setRGB2(&g4, work->level[i] / (k + 1), work->level[i] / (k + 1), work->level[i] / (k + 1));
                        setRGB3(&g4, work->level[i] / (k + 1), work->level[i] / (k + 1), work->level[i] / (k + 1));
                        setVector(&quad[0], work->trail[i][j].vx,
                                  work->trail[i][j].vy - work->config->height / 3 / k, work->trail[i][j].vz);
                        setVector(&quad[1], work->trail[i][j].vx,
                                  work->trail[i][j].vy + work->config->height / 3 / k, work->trail[i][j].vz);
                        setVector(&quad[2], work->trail[i][j - 1].vx,
                                  work->trail[i][j - 1].vy - work->config->height / 3 / (k + 1),
                                  work->trail[i][j - 1].vz);
                        setVector(&quad[3], work->trail[i][j - 1].vx,
                                  work->trail[i][j - 1].vy + work->config->height / 3 / (k + 1),
                                  work->trail[i][j - 1].vz);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                          (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                        }
                        k++;
                    }
                }
                if (ABS(work->pos[i].vz) > 450) {
                    work->arrived[i] = 1;
                    if (work->level[i] >= 16) {
                        work->level[i] -= work->config->fade;
                    } else {
                        work->level[i] = 0;
                    }
                } else {
                    work->tail[i]++;
                }
            }
        }
        if (++work->moving > work->config->count) {
            work->moving = work->config->count;
        }
    }
    if (++work->shown > work->config->count) {
        work->shown = work->config->count;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3 && work->level[work->config->count - 1] == 0) {
        return 2;
    }
    if (work->arrived[work->config->count - 1] && work->phase == 1) {
        work->phase = 2;
        return 1;
    }
    if (work->arrived[0]) {
        if (work->phase == 0) {
            work->phase = 1;
            return 4;
        }
    }
    return 0;
}
