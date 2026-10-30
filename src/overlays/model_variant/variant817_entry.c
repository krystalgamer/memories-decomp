#include "../../types.h"
#include "variant817_entry.h"

/* Header 817's entry: every period a jagged bolt is strung between a model
 * point and a jittered point on the opposing slot, drawn as sprites with
 * two layers of Gouraud strands. Each finished bolt flashes the screen and
 * throws off animated burst and smoke sprites, while a glow on the model
 * point sheds fading line sparks and the ambient light dims and recovers. */

static const VECTOR D_8013E19C = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model817State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_F4 f4;
    GsGLINE line;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR base;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 step;
    GsOT *ot;
    s32 a, b, c;
    s32 direction;
    s32 i, j, k;
    s32 otz;
    s32 x, y, z;
    s32 near, far;
    s32 first;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013E19C;
    memset(&base, 0, 8);
    work = (Model817State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013E21C[command];
        for (i = 0; i < 3; i++) {
            work->bolts.s.burst_frame[i] = 0;
            for (j = 0; j < 16; j++) {
                setVector(&work->points[i][j], 0, 0, 0);
                work->red[i][j] = work->config->r;
                work->green[i][j] = work->config->g;
                work->blue[i][j] = work->config->b;
                for (k = 0; k < 16; k++) {
                    setVector(&work->strands[i][j][k], RANDOM_SPREAD(work->config->strand_spread),
                              RANDOM_SPREAD(work->config->strand_spread), 0);
                }
            }
            for (j = 0; j < 8; j++) {
                setVector(&work->burst[i][j], 0, 0, 0);
                setVector(&work->burst_velocity[i][j], RANDOM_SPREAD(work->config->burst_spread),
                          -work->config->burst_spread * (rand() % 4096) / 4096,
                          RANDOM_SPREAD(work->config->burst_spread));
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->smoke[i][j], 0, 0, 0);
                setVector(&work->smoke_velocity[i][j], RANDOM_SPREAD(work->config->smoke_spread),
                          RANDOM_SPREAD(work->config->smoke_spread),
                          RANDOM_SPREAD(work->config->smoke_spread));
            }
        }
        for (i = 0; i < 88; i++) {
            work->spark_r[i] = work->config->r;
            work->spark_g[i] = work->config->g;
            work->spark_b[i] = work->config->b;
            setVector(&work->sparks[i][0], 0, 0, 0);
            a = (rand() - rand()) % 4096;
            b = (rand() - rand()) % 4096;
            c = (rand() - rand()) % 4096;
            for (j = 1; j < 6; j++) {
                x = RANDOM_SPREAD(work->config->spark_spread);
                y = RANDOM_SPREAD(work->config->spark_spread);
                z = RANDOM_SPREAD(work->config->spark_spread);
                setVector(&work->sparks[i][j], x + (u16)(work->config->spark_reach / 5) * j * a / 4096,
                          y + (u16)(work->config->spark_reach / 5) * j * b / 4096,
                          z + (u16)(work->config->spark_reach / 5) * j * c / 4096);
            }
        }
        for (i = 0; i < 16; i++) {
            work->bolts.s.length[i] = 0;
            work->bolts.s.flashed[i] = 0;
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E1AC[i]);
        }
        work->flash = 0;
        work->tint_r = 0;
        work->tint_g = 0;
        work->tint_b = 0;
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->bolts.s.unknown_2EB0 = 0;
        work->bolts.s.spark_count = 0;
        work->elapsed = 0;
        work->frames = 0;
        work->bolts.s.variant = rand() % 3;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->bolts.s.variant == 0) {
        if (work->elapsed < work->config->camera_start) {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -16);
        } else if (work->elapsed < work->config->camera_switch) {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 13, &rot, -8);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rot, -16);
        }
    } else if (work->bolts.s.variant == 1) {
        if (work->elapsed < work->config->camera_start) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
        } else if (work->elapsed < work->config->camera_switch) {
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
    Model_SetSlotAmbient(2, work->ambient, work->ambient, 0x800);

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->flash != 0) {
        setPolyF4(&f4);
        setRGB0(&f4, work->flash, work->flash, work->flash);
        setXY4(&f4, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT, GRAPHICS_DEFAULT_WIDTH,
               GRAPHICS_DEFAULT_HEIGHT);
        Graphics_SubmitDrawModePacket((u32 *)&f4, ot, 0, 1);
        if (work->flash >= 32) {
            work->flash -= work->config->fade;
        } else {
            work->flash = 0;
        }
    }

    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
    setPolyFT4(&ft4);
    setVector(&pos, m.t[0], m.t[1], m.t[2]);
    setVector(&base, m.t[0], m.t[1], m.t[2]);
    ft4.tpage = work->texture[0] >> 16;
    ft4.clut = work->texture[0];
    setRGB0(&ft4, work->tint_r, work->tint_g, work->tint_b);
    setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
    setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
    setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
    setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
    setVector(&scale, 0x1000, 0x1000, 0x1000);
    setVector(&rot, 0, 0, 0);
    GsSetLsMatrix(&light);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                      (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
    /* Subtracting separately adds the reference that ranks OTZ ahead of
     * the work pointer in register allocation. */
    otz -= 32;
    setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
    if (otz >= 0) {
        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
    }

    if (work->elapsed >= work->config->spawn_end) {
        if (work->tint_r >= 16) {
            work->tint_r -= work->config->tint_fade;
        } else {
            work->tint_r = 0;
        }
        if (work->tint_g >= 16) {
            work->tint_g -= work->config->tint_fade;
        } else {
            work->tint_g = 0;
        }
        if (work->tint_b >= 16) {
            work->tint_b -= work->config->tint_fade;
        } else {
            work->tint_b = 0;
        }
    } else {
        if (work->tint_r < work->config->r - 15) {
            work->tint_r += work->config->tint_fade;
        } else {
            work->tint_r = work->config->r;
        }
        if (work->tint_g < work->config->g - 15) {
            work->tint_g += work->config->tint_fade;
        } else {
            work->tint_g = work->config->g;
        }
        if (work->tint_b < work->config->b - 15) {
            work->tint_b += work->config->tint_fade;
        } else {
            work->tint_b = work->config->b;
        }
    }

    for (i = 0; i < work->bolts.s.spark_count; i++) {
        if (work->spark_r[i] || work->spark_g[i] || work->spark_b[i]) {
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
            setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
            copyVector(&pos, &base);
            addVector(&pos, &work->sparks[i][5]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }

            line.attribute = 0x50000000;
            GsSetLsMatrix(&light);
            RotTrans(&base, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 5; j++) {
                line.r0 = work->spark_r[i] / (j + 1);
                line.g0 = work->spark_g[i] / (j + 1);
                line.b0 = work->spark_b[i] / (j + 1);
                line.r1 = work->spark_r[i] / (j + 2);
                line.g1 = work->spark_g[i] / (j + 2);
                line.b1 = work->spark_b[i] / (j + 2);
                near = RotTransPers(&work->sparks[i][j], (PSXLONG *)&line.x0, &p, &flag);
                far = RotTransPers(&work->sparks[i][j + 1], (PSXLONG *)&line.x1, &p, &flag);
                if (otz >= 0) {
                    GsSortGLine(&line, ot, (near + far) / 2);
                }
            }
            if (work->spark_r[i] >= 32) {
                work->spark_r[i] -= work->config->fade;
            } else {
                work->spark_r[i] = 0;
            }
            if (work->spark_g[i] >= 32) {
                work->spark_g[i] -= work->config->fade;
            } else {
                work->spark_g[i] = 0;
            }
            if (work->spark_b[i] >= 32) {
                work->spark_b[i] -= work->config->fade;
            } else {
                work->spark_b[i] = 0;
            }
        }
    }

    if (work->elapsed < work->config->spark_end) {
        PopMatrix();
        work->bolts.s.spark_count++;
        if (work->bolts.s.spark_count > 88) {
            work->bolts.s.spark_count = 88;
        }
        work->elapsed += step;
        return 0;
    }

    if (work->elapsed < work->config->spawn_end && work->frames % work->config->period == 0) {
        work->bolts.s.burst_frame[work->head % 3] = 0;
        for (j = 0; j < work->config->count; j++) {
            setVector(&work->points[work->head % 3][j], 0, 0, 0);
            work->red[work->head % 3][j] = work->config->r;
            work->green[work->head % 3][j] = work->config->g;
            work->blue[work->head % 3][j] = work->config->b;
            for (k = 0; k < 16; k++) {
                setVector(&work->strands[work->head % 3][j][k], RANDOM_SPREAD(work->config->strand_spread),
                          RANDOM_SPREAD(work->config->strand_spread), 0);
            }
        }
        for (j = 0; j < 8; j++) {
            setVector(&work->burst[work->head % 3][j], 0, 0, 0);
            setVector(&work->burst_velocity[work->head % 3][j], RANDOM_SPREAD(work->config->burst_spread),
                      -work->config->burst_spread * (rand() % 4096) / 4096,
                      RANDOM_SPREAD(work->config->burst_spread));
        }
        for (j = 0; j < 16; j++) {
            setVector(&work->smoke[work->head % 3][j], 0, 0, 0);
            setVector(&work->smoke_velocity[work->head % 3][j], RANDOM_SPREAD(work->config->smoke_spread),
                      RANDOM_SPREAD(work->config->smoke_spread), RANDOM_SPREAD(work->config->smoke_spread));
        }
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        pos.vx += RANDOM_SPREAD(rot.vx / 2);
        pos.vy += RANDOM_SPREAD(rot.vy / 2);
        pos.vz += RANDOM_SPREAD(rot.vz / 2);
        setVector(&work->points[work->head % 3][work->config->count - 1], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 8; i++) {
            setVector(&work->burst[work->head % 3][i], pos.vx, pos.vy, pos.vz);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->smoke[work->head % 3][i], pos.vx, pos.vy, pos.vz);
        }
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        setVector(&work->points[work->head % 3][0], pos.vx, pos.vy, pos.vz);
        j = (work->points[work->head % 3][work->config->count - 1].vz - pos.vz) / (work->config->count - 1);
        for (i = 1; i < work->config->count - 1; i++) {
            setVector(&work->points[work->head % 3][i], pos.vx + RANDOM_SPREAD(work->config->jitter),
                      pos.vy + RANDOM_SPREAD(work->config->jitter), pos.vz + j * i);
        }
        work->head++;
    }

    /* Only the three newest bolts are still drawn. */
    first = work->head > 3 ? work->head - 3 : 0;
    for (i = first; i < work->head; i++) {
        for (j = 0; j < (work->bolts.all + 18)[i]; j++) {
            if (work->red[i % 3][j] || work->green[i % 3][j] || work->blue[i % 3][j]) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, work->red[i % 3][j], work->green[i % 3][j], work->blue[i % 3][j]);
                ft4.tpage = work->texture[0] >> 16;
                ft4.clut = work->texture[0];
                setVector(&quad[0], -work->config->bolt_size, -work->config->bolt_size, 0);
                setVector(&quad[1], work->config->bolt_size, -work->config->bolt_size, 0);
                setVector(&quad[2], -work->config->bolt_size, work->config->bolt_size, 0);
                setVector(&quad[3], work->config->bolt_size, work->config->bolt_size, 0);
                if (j == (work->bolts.all + 18)[i] - 1) {
                    setVector(&scale, work->config->tip_scale, work->config->tip_scale, work->config->tip_scale);
                } else {
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                }
                setVector(&rot, 0, 0, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->points[i % 3][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                  (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->red[i % 3][j], work->green[i % 3][j], work->blue[i % 3][j]);
                setRGB2(&g3, work->red[i % 3][j], work->green[i % 3][j], work->blue[i % 3][j]);
                setVector(&quad[0], -work->config->strand_width, 0, 0);
                setVector(&quad[1], work->config->strand_width, 0, 0);
                for (k = 0; k < 16; k++) {
                    otz = RotAverage3(&work->strands[i % 3][j][k], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
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
                    otz = RotAverage3(&work->strands[i % 3][j][k], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }
                if (j == work->config->count - 1) {
                    if (work->red[i % 3][j] >= 16) {
                        work->red[i % 3][j] -= work->config->tint_fade;
                    } else {
                        work->red[i % 3][j] = 0;
                    }
                    if (work->green[i % 3][j] >= 16) {
                        work->green[i % 3][j] -= work->config->tint_fade;
                    } else {
                        work->green[i % 3][j] = 0;
                    }
                    if (work->blue[i % 3][j] >= 16) {
                        work->blue[i % 3][j] -= work->config->tint_fade;
                    } else {
                        work->blue[i % 3][j] = 0;
                    }
                } else {
                    if (work->red[i % 3][j] >= 32) {
                        work->red[i % 3][j] -= work->config->fade;
                    } else {
                        work->red[i % 3][j] = 0;
                    }
                    if (work->green[i % 3][j] >= 32) {
                        work->green[i % 3][j] -= work->config->fade;
                    } else {
                        work->green[i % 3][j] = 0;
                    }
                    if (work->blue[i % 3][j] >= 32) {
                        work->blue[i % 3][j] -= work->config->fade;
                    } else {
                        work->blue[i % 3][j] = 0;
                    }
                }
            }
        }

        if ((work->bolts.all + 18)[i] == work->config->count) {
            if (work->phase == 0 && work->bolts.all[i] == 0) {
                work->flash = 0xFF;
                work->bolts.all[i] = 1;
                work->phase = 1;
            }
            if (work->bolts.s.burst_frame[i % 3] < 24) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, 0x80, 0x80, 0x80);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setVector(&quad[0], -work->config->burst_size, -work->config->burst_size, 0);
                setVector(&quad[1], work->config->burst_size, -work->config->burst_size, 0);
                setVector(&quad[2], -work->config->burst_size, work->config->burst_size, 0);
                setVector(&quad[3], work->config->burst_size, work->config->burst_size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                for (j = 0; j < 8; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->burst[i % 3][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, work->bolts.s.burst_frame[i % 3] / 3 * 32, 0x40, work->bolts.s.burst_frame[i % 3] / 3 * 32 + 31,
                           0x40, work->bolts.s.burst_frame[i % 3] / 3 * 32, 0x5F, work->bolts.s.burst_frame[i % 3] / 3 * 32 + 31,
                           0x5F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    addVector(&work->burst[i % 3][j], &work->burst_velocity[i % 3][j]);
                }
                work->bolts.s.burst_frame[i % 3]++;
            }
            if (work->red[i % 3][work->config->count - 1] || work->green[i % 3][work->config->count - 1] ||
                work->blue[i % 3][work->config->count - 1]) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, work->red[i % 3][work->config->count - 1], work->green[i % 3][work->config->count - 1],
                        work->blue[i % 3][work->config->count - 1]);
                ft4.tpage = work->texture[3] >> 16;
                ft4.clut = work->texture[3];
                setVector(&quad[0], -work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&quad[1], work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&quad[2], -work->config->smoke_size, work->config->smoke_size, 0);
                setVector(&quad[3], work->config->smoke_size, work->config->smoke_size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                for (j = 0; j < 16; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->smoke[i % 3][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    /* The random smoke frame reuses K. */
                    k = rand() % 3;
                    setUV4(&ft4, k * 32, 0x60, k * 32 + 31, 0x60, k * 32, 0x7F, k * 32 + 31, 0x7F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    addVector(&work->smoke[i % 3][j], &work->smoke_velocity[i % 3][j]);
                }
            }
        }
        (work->bolts.all + 18)[i]++;
        if ((work->bolts.all + 18)[i] > work->config->count) {
            (work->bolts.all + 18)[i] = work->config->count;
        }
    }

    work->elapsed += step;
    work->frames++;
    PopMatrix();
    if (work->phase == 3 && work->elapsed >= work->config->end) {
        return 2;
    }
    if (work->elapsed >= work->config->spawn_end + work->config->count && work->phase < 2) {
        work->phase = 2;
        work->flash = 0xFF;
        return 1;
    }
    if (work->phase == 1) {
        work->phase = 0;
        return 4;
    }
    return 0;
}
