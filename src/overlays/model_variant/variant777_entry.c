#include "../../types.h"
#include "variant777_entry.h"

/* Header 777's entry: every period a jittered lightning bolt of up to
 * twelve points grows from a model point towards the opposing slot. Each
 * point is a fading sprite with a sixteen-ray Gouraud fan; once a bolt is
 * complete it strikes, throwing eight animated debris sprites and sixteen
 * sparks from its tip, optionally flashing the screen while the ambient
 * light dims and recovers. */

static const VECTOR D_8013D430 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model777State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_F4 f4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 step;
    GsOT *ot;
    s32 slot, direction;
    s32 i, j, k;
    s32 dy;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013D430;
    work = (Model777State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D494[command];
        for (i = 0; i < 16; i++) {
            work->debris_frame[i] = 0;
            for (j = 0; j < 12; j++) {
                setVector(&work->points[i][j], 0, 0, 0);
                work->red[i][j] = work->config->r;
                work->green[i][j] = work->config->g;
                work->blue[i][j] = work->config->b;
            }
            for (j = 0; j < 8; j++) {
                setVector(&work->debris[i][j], 0, 0, 0);
                setVector(&work->debris_velocity[i][j], RANDOM_SPREAD(work->config->debris_speed),
                          -work->config->debris_speed * (rand() % 4096) / 4096,
                          RANDOM_SPREAD(work->config->debris_speed));
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->sparks[i][j], 0, 0, 0);
                setVector(&work->spark_velocity[i][j], RANDOM_SPREAD(work->config->spark_speed),
                          RANDOM_SPREAD(work->config->spark_speed),
                          RANDOM_SPREAD(work->config->spark_speed));
            }
            /* The original reuses the spark index here, one row past the
             * twelve points. */
            setVector(&work->spins[i][j], rand() % 4096, rand() % 4096, rand() % 4096);
        }
        for (k = 0; k < 16; k++) {
            setVector(&work->rays[k], RANDOM_SPREAD(work->config->ray_spread),
                      RANDOM_SPREAD(work->config->ray_spread), 0);
        }
        for (i = 0; i < 32; i++) {
            work->length[i] = 0;
            work->struck[i] = 0;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D440[i]);
        }
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->unknown_24D0 = 0;
        work->elapsed = 0;
        work->frames = 0;
        work->flash = 0;
        work->camera = rand() % 3;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->camera == 0) {
        if (work->elapsed < work->config->first_camera) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
        } else if (work->elapsed < work->config->second_camera) {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rot, -8);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -16);
        }
    } else if (work->camera == 1) {
        if (work->elapsed < work->config->first_camera) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
        } else if (work->elapsed < work->config->second_camera) {
            if (direction > 0) {
                Model_QueueCameraOrbitFromPreset(2, 12, &rot, -12);
            } else {
                Model_QueueCameraOrbitFromPreset(2, 10, &rot, -12);
            }
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rot, -16);
        }
    }
    if (work->head == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 32;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->ambient < 0x800) {
            work->ambient += step * 32;
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
    if (work->elapsed < work->config->spawn_end && work->frames % work->config->period == 0) {
        work->debris_frame[work->head % 16] = 0;
        for (j = 0; j < work->config->point_count; j++) {
            setVector(&work->points[work->head % 16][j], 0, 0, 0);
            setVector(&work->spins[work->head % 16][j], rand() % 4096, rand() % 4096, rand() % 4096);
            work->red[work->head % 16][j] = work->config->r;
            work->green[work->head % 16][j] = work->config->g;
            work->blue[work->head % 16][j] = work->config->b;
        }
        for (j = 0; j < 8; j++) {
            setVector(&work->debris[work->head % 16][j], 0, 0, 0);
            setVector(&work->debris_velocity[work->head % 16][j], RANDOM_SPREAD(work->config->debris_speed),
                      -work->config->debris_speed * (rand() % 4096) / 4096,
                      RANDOM_SPREAD(work->config->debris_speed));
        }
        for (j = 0; j < 16; j++) {
            setVector(&work->sparks[work->head % 16][j], 0, 0, 0);
            setVector(&work->spark_velocity[work->head % 16][j], RANDOM_SPREAD(work->config->spark_speed),
                      RANDOM_SPREAD(work->config->spark_speed),
                      RANDOM_SPREAD(work->config->spark_speed));
        }
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        pos.vx += RANDOM_SPREAD(rot.vx / 2);
        pos.vy += RANDOM_SPREAD(rot.vy / 2);
        pos.vz += RANDOM_SPREAD(rot.vz / 2);
        /* Taking the tip's address lets the "- 1" fold into the store offsets. */
        setVector(&work->points[work->head % 16][work->config->point_count - 1], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 8; i++) {
            setVector(&work->debris[work->head % 16][i], pos.vx, pos.vy, pos.vz);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->sparks[work->head % 16][i], pos.vx, pos.vy, pos.vz);
        }
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        pos.vx = m.t[0];
        pos.vy = m.t[1];
        pos.vz = m.t[2];
        setVector(&work->points[work->head % 16][0], pos.vx, pos.vy, pos.vz);
        /* The per-point steps reuse J for z and K for x, and only z divides
         * by the gap count; DY later holds the first bolt drawn. All three
         * reuses and the step order fix the register allocation. */
        j = (work->points[work->head % 16][work->config->point_count - 1].vz - pos.vz) / (work->config->point_count - 1);
        k = (work->points[work->head % 16][work->config->point_count - 1].vx - pos.vx) / work->config->point_count;
        dy = (work->points[work->head % 16][work->config->point_count - 1].vy - pos.vy) / work->config->point_count;
        for (i = 1; i < work->config->point_count - 1; i++) {
            setVector(&work->points[work->head % 16][i], pos.vx + k * i + RANDOM_SPREAD(work->config->jitter),
                      pos.vy + dy * i + RANDOM_SPREAD(work->config->jitter),
                      pos.vz + j * i);
        }
        work->head++;
    }

    dy = work->head > 16 ? work->head - 16 : 0;
    for (i = dy; i < work->head; i++) {
        for (j = 0; j < work->length[i]; j++) {
            if (work->red[i % 16][j] || work->green[i % 16][j] || work->blue[i % 16][j]) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, work->red[i % 16][j], work->green[i % 16][j], work->blue[i % 16][j]);
                ft4.tpage = work->texture[2] >> 16;
                ft4.clut = work->texture[2];
                setVector(&quad[0], -work->config->bolt_size, -work->config->bolt_size, 0);
                setVector(&quad[1], work->config->bolt_size, -work->config->bolt_size, 0);
                setVector(&quad[2], -work->config->bolt_size, work->config->bolt_size, 0);
                setVector(&quad[3], work->config->bolt_size, work->config->bolt_size, 0);
                if (j == work->length[i] - 1) {
                    setVector(&scale, work->config->tip_scale, work->config->tip_scale, work->config->tip_scale);
                } else {
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                }
                setVector(&rot, 0, 0, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->points[i % 16][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->red[i % 16][j], work->green[i % 16][j], work->blue[i % 16][j]);
                setRGB2(&g3, work->red[i % 16][j], work->green[i % 16][j], work->blue[i % 16][j]);
                setVector(&quad[0], -work->config->ray_width, 0, 0);
                setVector(&quad[1], work->config->ray_width, 0, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->points[i % 16][j], (VECTOR *)m.t, &flag);
                RotMatrix(&work->spins[i][j], &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (k = 0; k < 16; k++) {
                    otz = RotAverage3(&work->rays[k], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                }
                scale.vx /= 2;
                scale.vy /= 2;
                scale.vz /= 2;
                m = saved;
                GsSetLsMatrix(&light);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (k = 0; k < 16; k++) {
                    otz = RotAverage3(&work->rays[k], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }

                if (j == work->config->point_count - 1) {
                    if (work->red[i % 16][j] >= 16) {
                        work->red[i % 16][j] -= work->config->tip_fade;
                    } else {
                        work->red[i % 16][j] = 0;
                    }
                    if (work->green[i % 16][j] >= 16) {
                        work->green[i % 16][j] -= work->config->tip_fade;
                    } else {
                        work->green[i % 16][j] = 0;
                    }
                    if (work->blue[i % 16][j] >= 16) {
                        work->blue[i % 16][j] -= work->config->tip_fade;
                    } else {
                        work->blue[i % 16][j] = 0;
                    }
                } else {
                    if (work->red[i % 16][j] >= 32) {
                        work->red[i % 16][j] -= work->config->fade;
                    } else {
                        work->red[i % 16][j] = 0;
                    }
                    if (work->green[i % 16][j] >= 32) {
                        work->green[i % 16][j] -= work->config->fade;
                    } else {
                        work->green[i % 16][j] = 0;
                    }
                    if (work->blue[i % 16][j] >= 32) {
                        work->blue[i % 16][j] -= work->config->fade;
                    } else {
                        work->blue[i % 16][j] = 0;
                    }
                }
            }
        }

        if (work->length[i] == work->config->point_count) {
            if (work->phase == 0 && work->struck[i] == 0) {
                work->struck[i] = 1;
                work->phase = 1;
            }
            if (work->debris_frame[i % 16] < 24) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, 0x80, 0x80, 0x80);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setVector(&quad[0], -work->config->debris_size, -work->config->debris_size, 0);
                setVector(&quad[1], work->config->debris_size, -work->config->debris_size, 0);
                setVector(&quad[2], -work->config->debris_size, work->config->debris_size, 0);
                setVector(&quad[3], work->config->debris_size, work->config->debris_size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                for (j = 0; j < 8; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->debris[i % 16][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, work->debris_frame[i % 16] / 3 * 32, 0x40,
                           work->debris_frame[i % 16] / 3 * 32 + 0x1F, 0x40,
                           work->debris_frame[i % 16] / 3 * 32, 0x5F,
                           work->debris_frame[i % 16] / 3 * 32 + 0x1F, 0x5F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    addVector(&work->debris[i % 16][j], &work->debris_velocity[i % 16][j]);
                }
                work->debris_frame[i % 16]++;
            }
            if (work->red[i % 16][work->config->point_count - 1] ||
                work->green[i % 16][work->config->point_count - 1] ||
                work->blue[i % 16][work->config->point_count - 1]) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, work->red[i % 16][work->config->point_count - 1],
                        work->green[i % 16][work->config->point_count - 1],
                        work->blue[i % 16][work->config->point_count - 1]);
                ft4.tpage = work->texture[0] >> 16;
                ft4.clut = work->texture[0];
                setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                for (j = 0; j < 16; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->sparks[i % 16][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    /* The spark frame reuses K. */
                    k = rand() % 3;
                    setUV4(&ft4, k * 32, 0x60, k * 32 + 0x1F, 0x60, k * 32, 0x7F, k * 32 + 0x1F, 0x7F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    addVector(&work->sparks[i % 16][j], &work->spark_velocity[i % 16][j]);
                }
            }
        }
        work->length[i]++;
        if (work->length[i] > work->config->point_count) {
            work->length[i] = work->config->point_count;
        }
    }

    if (work->flash) {
        setPolyF4(&f4);
        setRGB0(&f4, work->flash, work->flash, work->flash);
#ifdef VERSION_FRENCH
        setXY4(&f4, 0, 0, 320, 0, 0, 256, 320, 256);
#else
        setXY4(&f4, 0, 0, 320, 0, 0, 240, 320, 240);
#endif
        Graphics_SubmitDrawModePacket((u32 *)&f4, ot, 0, 1);
        if (work->flash >= 32) {
            work->flash -= work->config->fade;
        } else {
            work->flash = 0;
        }
    }
    work->elapsed += step;
    work->frames++;
    PopMatrix();
    if (work->phase == 3 && work->elapsed >= work->config->end) {
        return 2;
    }
    if (work->elapsed >= work->config->spawn_end) {
        if (work->phase == 1) {
            work->phase = 2;
            if (work->config->flash == 1) {
                work->flash = 0xFF;
            }
            return 1;
        }
    } else if (work->phase == 1) {
        work->phase = 0;
        if (work->config->flash == 1) {
            work->flash = 0xFF;
        }
        return 4;
    }
    return 0;
}
