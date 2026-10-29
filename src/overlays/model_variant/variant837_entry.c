#include "../../types.h"
#include "variant837_entry.h"
#include "../../game/graphics_constants.h"

/* Header 837's entry: streaks and a glowing disc gather on a model part
 * under a flickering quartered glow while the camera turns. The gathered
 * orb then flies towards a point in front of the opposing slot, shedding
 * sparks and glints, and bursts there into a large quartered flare,
 * flying debris, falling rays, growing rings and a white screen flash
 * while the ambient light dims and recovers. */

static const VECTOR D_8013DF7C = {4096, 4096, 4096, 0};

/* A random offset in (-4096, 4096). */
#define RANDOM_OFFSET() ((rand() - rand()) % 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model837State *work;
    MATRIX light;
    MATRIX m;
    POLY_G3 g3;
    POLY_FT4 ft4;
    POLY_GT4 gt4;
    POLY_F4 f4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, j, k;
    s32 otz;
    s16 sign;
    s32 base;
    s32 prev;
    POLY_G3 *fan;
    POLY_FT4 *sprite;
    POLY_GT4 *ray;
    POLY_F4 *screen;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013DF7C;
    work = (Model837State *)context;
    fan = &g3;
    sprite = &ft4;
    ray = &gt4;
    screen = &f4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013DFE0[command];
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DF8C[i]);
        }
        work->ring_tpage = 0xAE;
        work->ring_clut = 0x3D20;
        setVector(&work->orb, 0, 0, 0);
        setVector(&work->orb_step, 0, 0, 0);
        for (i = 0; i < 32; i++) {
            setVector(&work->rim[i], work->config->radius * ccos(i * 128) / 4096,
                      work->config->radius * csin(i * 128) / 4096, 0);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->streak_angle[i], RANDOM_OFFSET(), RANDOM_OFFSET(), RANDOM_OFFSET());
            for (j = 0; j < 4; j++) {
                base = 192 - j * 48;
                setVector(&work->streaks[i][j], base + RANDOM_OFFSET() / 128, base + RANDOM_OFFSET() / 128,
                          base + RANDOM_OFFSET() / 128);
            }
            work->streak_color[i].r = 0x80;
            work->streak_color[i].g = 0x80;
            work->streak_color[i].b = 0x80;
            work->streak_length[i] = 0;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->spark_velocity[i], RANDOM_OFFSET() / 64, RANDOM_OFFSET() / 64, RANDOM_OFFSET() / 64);
            work->spark_color[i].r = work->config->r / 2;
            work->spark_color[i].g = work->config->g / 2;
            work->spark_color[i].b = work->config->b / 2;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->glints[i], 0, 0, 0);
            setVector(&work->glint_velocity[i], RANDOM_OFFSET() * 48 / 4096, RANDOM_OFFSET() * 48 / 4096,
                      RANDOM_OFFSET() * 48 / 4096);
            work->glint_color[i].r = work->config->r / 2;
            work->glint_color[i].g = work->config->g / 2;
            work->glint_color[i].b = work->config->b / 2;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->rings[i], RANDOM_OFFSET() / 16, RANDOM_OFFSET() / 16, RANDOM_OFFSET() / 16);
            work->ring_color[i].r = work->config->r;
            work->ring_color[i].g = work->config->g;
            work->ring_color[i].b = work->config->b;
            work->ring_scale[i] = 0;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->rays[i], RANDOM_OFFSET() / 8, RANDOM_OFFSET() / 8, RANDOM_OFFSET() / 8);
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->debris[i], 0, 0, 0);
            setVector(&work->debris_velocity[i], RANDOM_OFFSET() / 256, RANDOM_OFFSET() / 256,
                      RANDOM_OFFSET() / 256);
        }
        work->glow_color.rgb.r = 0x80;
        work->glow_color.rgb.g = 0x80;
        work->glow_color.rgb.b = 0x80;
        work->halo_color.rgb.r = work->config->r / 4;
        work->halo_color.rgb.g = work->config->g / 4;
        work->halo_color.rgb.b = work->config->b / 4;
        work->core_color.r = work->config->r / 2;
        work->core_color.g = work->config->g / 2;
        work->core_color.b = work->config->b / 2;
        work->burst_color.rgb.r = work->config->r;
        work->burst_color.rgb.g = work->config->g;
        work->burst_color.rgb.b = work->config->b;
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->glow_scale = 0;
        work->streak_count = 0;
        work->spark_count = 0;
        work->glint_count = 0;
        work->ring_count = 0;
        work->flash = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->orbit_time) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -4);
    } else if (work->elapsed < work->config->launch_time) {
        Model_QueueCameraOrbitFromPreset(slot, 2, &rot, -16);
    } else if (work->elapsed < work->config->stop_time) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rot, -32);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -32);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 4) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 5;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->orbit_time) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    setPolyG3(fan);
    setPolyFT4(sprite);
    setPolyGT4(ray);
    if (work->elapsed < work->config->stop_time) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        pos.vx = m.t[0];
        pos.vy = m.t[1];
        pos.vz = m.t[2];
        if (!(work->frame & 1)) {
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = work->frame * 128;
            GsSetLsMatrix(&light);
            sign = 1;
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            sprite->tpage = work->ring_tpage;
            sprite->clut = work->ring_clut;
            for (j = 0; j < 4; j++) {
                setRGB0(sprite, work->core_color.r, work->core_color.g, work->core_color.b);
                sign *= -1;
                quad[0].vx = -sign * 64;
                if (j - 2 >= 0) {
                    quad[0].vy = -64;
                } else {
                    quad[0].vy = 64;
                }
                quad[0].vz = 0;
                quad[1].vx = 0;
                if (j - 2 >= 0) {
                    quad[1].vy = -64;
                } else {
                    quad[1].vy = 64;
                }
                quad[1].vz = 0;
                setVector(&quad[2], -sign * 64, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                setUV4(sprite, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
                setUV4(sprite, 0x40, 0x40, 0x7F, 0x40, 0x40, 0x7F, 0x7F, 0x7F);
                setRGB0(sprite, work->core_color.r / 2, work->core_color.g / 2, work->core_color.b / 2);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
            }
        }
    } else if (work->phase == 2) {
        work->phase = 3;
    }

    if (work->elapsed >= work->config->launch_time && work->phase == 0) {
        work->orb.vx = pos.vx;
        work->orb.vy = pos.vy;
        work->orb.vz = pos.vz;
        work->orb_step.vx = -work->orb.vx / 8;
        work->orb_step.vy = (-350 - work->orb.vy) / 8;
        work->orb_step.vz = (direction * 450 - work->orb.vz) / 8;
        work->phase = 1;
    }
    if (work->phase != 0) {
        pos.vx = work->orb.vx;
        pos.vy = work->orb.vy;
        pos.vz = work->orb.vz;
    }

    if (work->elapsed < work->config->stop_time) {
        ray->tpage = work->texture[0] >> 16;
        ray->clut = work->texture[0];
        setUV4(ray, 0x40, 0, 0x6F, 0, 0x40, 0x7F, 0x6F, 0x7F);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        saved = m;
        if (work->phase < 2) {
            setVector(&scale, 0x1000, 0x1000, 0x1000);
        } else {
            setVector(&scale, 0x2000, 0x2000, 0x2000);
        }
        for (i = 0; i < work->streak_count; i++) {
            m = saved;
            RotMatrix(&work->streak_angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            j = 0;
            while (j < work->streak_length[i] - 1) {
                prev = j - 1;
                setRGB0(ray, work->streak_color[i].r / (work->streak_length[i] - prev),
                        work->streak_color[i].g / (work->streak_length[i] - prev),
                        work->streak_color[i].b / (work->streak_length[i] - prev));
                setRGB1(ray, work->streak_color[i].r / (work->streak_length[i] - prev),
                        work->streak_color[i].g / (work->streak_length[i] - prev),
                        work->streak_color[i].b / (work->streak_length[i] - prev));
                setRGB2(ray, work->streak_color[i].r / (work->streak_length[i] - j),
                        work->streak_color[i].g / (work->streak_length[i] - j),
                        work->streak_color[i].b / (work->streak_length[i] - j));
                setRGB3(ray, work->streak_color[i].r / (work->streak_length[i] - j),
                        work->streak_color[i].g / (work->streak_length[i] - j),
                        work->streak_color[i].b / (work->streak_length[i] - j));
                setVector(&quad[0], -8 / (work->streak_length[i] - prev), 0, 0);
                setVector(&quad[1], 8 / (work->streak_length[i] - prev), 0, 0);
                setVector(&quad[2], -8 / (work->streak_length[i] - j), 0, 0);
                setVector(&quad[3], 8 / (work->streak_length[i] - j), 0, 0);
                addVector(&quad[0], &work->streaks[i][j]);
                addVector(&quad[1], &work->streaks[i][j]);
                addVector(&quad[2], &work->streaks[i][j + 1]);
                addVector(&quad[3], &work->streaks[i][j + 1]);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ray->x0, (PSXLONG *)&ray->x1,
                                  (PSXLONG *)&ray->x2, (PSXLONG *)&ray->x3, &p, &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ray, ot, (u16)otz, 1);
                }
                j++;
            }
            work->streak_length[i]++;
            if (work->streak_length[i] > 4) {
                work->streak_length[i] = 4;
                if (work->streak_color[i].r > 15) {
                    work->streak_color[i].r -= 15;
                } else {
                    work->streak_color[i].r = 0;
                }
                if (work->streak_color[i].g > 15) {
                    work->streak_color[i].g -= 15;
                } else {
                    work->streak_color[i].g = 0;
                }
                if (work->streak_color[i].b > 15) {
                    work->streak_color[i].b -= 15;
                } else {
                    work->streak_color[i].b = 0;
                }
                if (work->streak_color[i].r == 0 && work->streak_color[i].g == 0 && work->streak_color[i].b == 0) {
                    work->streak_color[i].r = 0x80;
                    work->streak_color[i].g = 0x80;
                    work->streak_color[i].b = 0x80;
                    work->streak_length[i] = 0;
                }
            }
        }

        if (work->glow_color.word & 0xFFFFFF) {
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            saved = m;
            setRGB0(fan, 0, 0, 0);
            if (!(work->frame & 1)) {
                scale.vx = work->glow_scale * 3 / 4;
                scale.vy = work->glow_scale * 3 / 4;
                scale.vz = work->glow_scale * 3 / 4;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB1(fan, work->glow_color.rgb.r, work->glow_color.rgb.g, work->glow_color.rgb.b);
                setRGB2(fan, work->glow_color.rgb.r, work->glow_color.rgb.g, work->glow_color.rgb.b);
            } else {
                scale.vx = work->glow_scale;
                scale.vy = work->glow_scale;
                scale.vz = work->glow_scale;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB1(fan, work->glow_color.rgb.r / 4, work->glow_color.rgb.g / 4, work->glow_color.rgb.b / 4);
                setRGB2(fan, work->glow_color.rgb.r / 4, work->glow_color.rgb.g / 4, work->glow_color.rgb.b / 4);
            }
            setVector(&quad[0], 0, 0, 0);
            for (i = 0; i < 32; i++) {
                otz = RotAverage3(&quad[0], &work->rim[i], &work->rim[(i + 1) % 32], (PSXLONG *)&fan->x0,
                                  (PSXLONG *)&fan->x1, (PSXLONG *)&fan->x2, &p, &flag) - 64;
                if (flag >= 0 && otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)fan, ot, (u16)otz, 2);
                }
            }
        }

        m = saved;
        scale.vx = work->glow_scale;
        scale.vy = work->glow_scale;
        scale.vz = work->glow_scale;
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        if (work->halo_color.word & 0xFFFFFF) {
            sign = 1;
            sprite->tpage = work->ring_tpage;
            sprite->clut = work->ring_clut;
            setUV4(sprite, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
            setRGB0(sprite, work->halo_color.rgb.r, work->halo_color.rgb.g, work->halo_color.rgb.b);
            for (i = 0; i < 4; i++) {
                sign *= -1;
                quad[0].vx = -sign * 96;
                if (i - 2 >= 0) {
                    quad[0].vy = -96;
                } else {
                    quad[0].vy = 96;
                }
                quad[0].vz = 0;
                quad[1].vx = 0;
                if (i - 2 >= 0) {
                    quad[1].vy = -96;
                } else {
                    quad[1].vy = 96;
                }
                quad[1].vz = 0;
                setVector(&quad[2], -sign * 96, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
            }
        }

        if (!(work->frame & 1)) {
            sign = 1;
            setRGB0(sprite, 0x20, 0x20, 0x20);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = work->elapsed * 128 % 4096;
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            sprite->tpage = work->texture[0] >> 16;
            sprite->clut = work->texture[0];
            setUV4(sprite, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            for (i = 0; i < 4; i++) {
                sign *= -1;
                quad[0].vx = -sign * 96;
                if (i - 2 >= 0) {
                    quad[0].vy = -96;
                } else {
                    quad[0].vy = 96;
                }
                quad[0].vz = 0;
                quad[1].vx = 0;
                if (i - 2 >= 0) {
                    quad[1].vy = -96;
                } else {
                    quad[1].vy = 96;
                }
                quad[1].vz = 0;
                setVector(&quad[2], -sign * 96, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
            }
        }

        if (work->phase >= 2) {
            sprite->tpage = work->ring_tpage;
            sprite->clut = work->ring_clut;
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < work->spark_count; i++) {
                setRGB0(sprite, work->spark_color[i].r, work->spark_color[i].g, work->spark_color[i].b);
                j = rand() % 4;
                setUV4(sprite, j % 2 * 32, j / 2 * 32 + 0x40, j % 2 * 32 + 0x1F, j / 2 * 32 + 0x40, j % 2 * 32,
                       j / 2 * 32 + 0x5F, j % 2 * 32 + 0x1F, j / 2 * 32 + 0x5F);
                setVector(&quad[0], -8, -8, 0);
                setVector(&quad[1], 8, -8, 0);
                setVector(&quad[2], -8, 8, 0);
                setVector(&quad[3], 8, 8, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&quad[j], &work->sparks[i]);
                }
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
                addVector(&work->sparks[i], &work->spark_velocity[i]);
                if (work->spark_color[i].r > 8) {
                    work->spark_color[i].r -= 8;
                } else {
                    work->spark_color[i].r = 0;
                }
                if (work->spark_color[i].g > 8) {
                    work->spark_color[i].g -= 8;
                } else {
                    work->spark_color[i].g = 0;
                }
                if (work->spark_color[i].b > 8) {
                    work->spark_color[i].b -= 8;
                } else {
                    work->spark_color[i].b = 0;
                }
                if (work->spark_color[i].r == 0 && work->spark_color[i].g == 0 && work->spark_color[i].b == 0) {
                    setVector(&work->sparks[i], 0, 0, 0);
                    work->spark_color[i].r = work->config->r / 2;
                    work->spark_color[i].g = work->config->g / 2;
                    work->spark_color[i].b = work->config->b / 2;
                }
            }
            for (i = 0; i < work->glint_count; i++) {
                sign = 1;
                setRGB0(sprite, work->glint_color[i].r, work->glint_color[i].g, work->glint_color[i].b);
                setUV4(sprite, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    quad[0].vx = -sign * 12;
                    if (j - 2 >= 0) {
                        quad[0].vy = -12;
                    } else {
                        quad[0].vy = 12;
                    }
                    quad[0].vz = 0;
                    quad[1].vx = 0;
                    if (j - 2 >= 0) {
                        quad[1].vy = -12;
                    } else {
                        quad[1].vy = 12;
                    }
                    quad[1].vz = 0;
                    setVector(&quad[2], -sign * 12, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&quad[k], &work->glints[i]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                      (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                      &flag) - 64;
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                    }
                }
                addVector(&work->glints[i], &work->glint_velocity[i]);
                if (work->glint_color[i].r > 8) {
                    work->glint_color[i].r -= 8;
                } else {
                    work->glint_color[i].r = 0;
                }
                if (work->glint_color[i].g > 8) {
                    work->glint_color[i].g -= 8;
                } else {
                    work->glint_color[i].g = 0;
                }
                if (work->glint_color[i].b > 8) {
                    work->glint_color[i].b -= 8;
                } else {
                    work->glint_color[i].b = 0;
                }
                if (work->glint_color[i].r == 0 && work->glint_color[i].g == 0 && work->glint_color[i].b == 0) {
                    setVector(&work->glints[i], 0, 0, 0);
                    work->glint_color[i].r = work->config->r / 2;
                    work->glint_color[i].g = work->config->g / 2;
                    work->glint_color[i].b = work->config->b / 2;
                }
            }
            if (!(work->frame & 3)) {
                work->spark_count++;
                if (work->spark_count > 32) {
                    work->spark_count = 32;
                }
            }
            if (!(work->frame & 7)) {
                work->glint_count++;
                if (work->glint_count > 16) {
                    work->glint_count = 16;
                }
            }
        }
        if (work->phase >= 2) {
            work->glow_scale = 0x2000;
        } else if (work->glow_scale < 0x1000) {
            work->glow_scale += 0x80;
        }
        if (work->phase == 1) {
            addVector(&work->orb, &work->orb_step);
            if (ABS(work->orb.vz) >= 450) {
                work->phase = 2;
            }
        }
        work->streak_count++;
        if (work->streak_count > 32) {
            work->streak_count = 32;
        }
    }

    if (work->phase >= 3) {
        setVector(&pos, 0, -350, direction * 450);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        saved = m;
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        if (work->burst_color.word & 0xFFFFFF) {
            sign = 1;
            sprite->tpage = work->ring_tpage;
            sprite->clut = work->ring_clut;
            setRGB0(sprite, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            for (i = 0; i < 4; i++) {
                sign *= -1;
                quad[0].vx = -sign * 192;
                if (i - 2 >= 0) {
                    quad[0].vy = -192;
                } else {
                    quad[0].vy = 192;
                }
                quad[0].vz = 0;
                quad[1].vx = 0;
                if (i - 2 >= 0) {
                    quad[1].vy = -192;
                } else {
                    quad[1].vy = 192;
                }
                quad[1].vz = 0;
                setVector(&quad[2], -sign * 192, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                setUV4(sprite, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
            }
            for (i = 0; i < 64; i++) {
                j = rand() % 4;
                setUV4(sprite, j % 2 * 32, j / 2 * 32 + 0x40, j % 2 * 32 + 0x1F, j / 2 * 32 + 0x40, j % 2 * 32,
                       j / 2 * 32 + 0x5F, j % 2 * 32 + 0x1F, j / 2 * 32 + 0x5F);
                setVector(&quad[0], -16, -16, 0);
                setVector(&quad[1], 16, -16, 0);
                setVector(&quad[2], -16, 16, 0);
                setVector(&quad[3], 16, 16, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&quad[j], &work->debris[i]);
                }
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                  (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                  &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                }
                addVector(&work->debris[i], &work->debris_velocity[i]);
            }
            ray->tpage = work->texture[2] >> 16;
            ray->clut = work->texture[2];
            setRGB0(ray, 0, 0, 0);
            setRGB1(ray, 0, 0, 0);
            setRGB2(ray, work->burst_color.rgb.r / 4, work->burst_color.rgb.g / 4, work->burst_color.rgb.b / 4);
            setRGB3(ray, work->burst_color.rgb.r / 4, work->burst_color.rgb.g / 4, work->burst_color.rgb.b / 4);
            setUV4(ray, 0, 0x40, 0x3F, 0x40, 0, 0xBF, 0x3F, 0xBF);
            for (i = 0; i < 32; i++) {
                setVector(&quad[0], work->rays[i].vx - 6, work->rays[i].vy, work->rays[i].vz);
                setVector(&quad[1], work->rays[i].vx + 6, work->rays[i].vy, work->rays[i].vz);
                setVector(&quad[2], -8, 0, 0);
                setVector(&quad[3], 8, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ray->x0, (PSXLONG *)&ray->x1,
                                  (PSXLONG *)&ray->x2, (PSXLONG *)&ray->x3, &p, &flag) - 64;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ray, ot, (u16)otz, 1);
                }
            }
            if (work->burst_color.rgb.r > 4) {
                work->burst_color.rgb.r -= 4;
            } else {
                work->burst_color.rgb.r = 0;
            }
            if (work->burst_color.rgb.g > 4) {
                work->burst_color.rgb.g -= 4;
            } else {
                work->burst_color.rgb.g = 0;
            }
            if (work->burst_color.rgb.b > 4) {
                work->burst_color.rgb.b -= 4;
            } else {
                work->burst_color.rgb.b = 0;
            }
        }
        for (i = 0; i < work->ring_count; i++) {
            if (work->ring_color[i].r || work->ring_color[i].g || work->ring_color[i].b) {
                scale.vx = work->ring_scale[i];
                scale.vy = work->ring_scale[i];
                scale.vz = work->ring_scale[i];
                setVector(&pos, 0, -350, direction * 450);
                addVector(&pos, &work->rings[i]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                sign = 1;
                sprite->tpage = work->ring_tpage;
                sprite->clut = work->ring_clut;
                setRGB0(sprite, work->ring_color[i].r, work->ring_color[i].g, work->ring_color[i].b);
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    quad[0].vx = -sign * 64;
                    if (j - 2 >= 0) {
                        quad[0].vy = -64;
                    } else {
                        quad[0].vy = 64;
                    }
                    quad[0].vz = 0;
                    quad[1].vx = 0;
                    if (j - 2 >= 0) {
                        quad[1].vy = -64;
                    } else {
                        quad[1].vy = 64;
                    }
                    quad[1].vz = 0;
                    setVector(&quad[2], -sign * 64, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    setUV4(sprite, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite->x0,
                                      (PSXLONG *)&sprite->x1, (PSXLONG *)&sprite->x2, (PSXLONG *)&sprite->x3, &p,
                                      &flag) - 64;
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)sprite, ot, (u16)otz, 1);
                    }
                }
                if (work->ring_scale[i] < 0x1000) {
                    work->ring_scale[i] += 0x400;
                } else {
                    work->ring_scale[i] = 0x1000;
                    if (work->ring_color[i].r > 31) {
                        work->ring_color[i].r -= 31;
                    } else {
                        work->ring_color[i].r = 0;
                    }
                    if (work->ring_color[i].g > 31) {
                        work->ring_color[i].g -= 31;
                    } else {
                        work->ring_color[i].g = 0;
                    }
                    if (work->ring_color[i].b > 31) {
                        work->ring_color[i].b -= 31;
                    } else {
                        work->ring_color[i].b = 0;
                    }
                }
            }
        }
        work->ring_count += step;
        if (work->ring_count > 32) {
            work->ring_count = 32;
        }
    }

    if (work->flash) {
        setPolyF4(screen);
        setRGB0(screen, work->flash, work->flash, work->flash);
        setXY4(screen, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        Graphics_SubmitDrawModePacket((u32 *)screen, ot, 0, 1);
        if (work->flash > 8) {
            work->flash -= 8;
        } else {
            work->flash = 0;
        }
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 5 && !(work->burst_color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->phase == 3) {
        work->flash = 0xFF;
        work->phase = 4;
        return 1;
    }
    if (work->phase == 2) {
        return 4;
    }
    return 0;
}
