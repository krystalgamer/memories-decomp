#include "../../types.h"
#include "variant751_entry.h"
#include "../../psyq/abs.h"

/* Header 751's entry: an orb gathers in front of a model part while its
 * colour fades in, then flies across with a cloud of quads and spinning rays
 * around it. When it lands it opens into a growing ring with layered glows
 * and a fountain of falling sparks while the colour fades out and the ambient
 * light dims and recovers. */

static const VECTOR D_8013CA40 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model751State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_GT4 gt4;
    SVECTOR rot;
    SVECTOR position;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i, j;
    s32 theta, phi;
    s32 angle;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CA40;
    work = (Model751State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CAA4[command];
        setVector(&work->pos, 0, 0, 0);
        setVector(&work->velocity, 0, 0, 0);
        work->state = 0;
        work->scale = 0x1000;
        work->spin = 0;
        work->lift = work->config->lift;
        work->color.rgb.r = 0;
        work->color.rgb.g = 0;
        work->color.rgb.b = 0;
        for (i = 0; i < 16; i++) {
            theta = rand() % 4096;
            phi = rand() % 4096;
            setVector(&work->offsets[i], work->config->size * ccos(theta) / 4096 * ccos(phi) / 4096,
                      work->config->size * csin(theta) / 4096 * ccos(phi) / 4096,
                      work->config->size * ccos(theta) / 4096 * csin(phi) / 4096);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->sparks[i], 0, -350, direction * 450);
            setVector(&work->spark_velocity[i], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                      work->config->spread * ((rand() - rand()) % 4096) / 4096,
                      -direction * work->config->spread * (rand() % 4096) / 4096);
        }
        for (j = 0; j < 16; j++) {
            angle = (j << 8) % 4096;
            setVector(&work->ring[j][0], work->config->ring_outer * ccos(angle) / 4096,
                      work->config->ring_outer * csin(angle) / 4096, -direction * work->config->ring_depth);
            setVector(&work->ring[j][1], work->config->ring_inner * ccos(angle) / 4096,
                      work->config->ring_inner * csin(angle) / 4096, 0);
        }
        work->bob = 0;
        for (j = 0; j < 3; j++) {
            work->texture[j] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CA50[j]);
        }
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_time[0]) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rot, -4);
    } else if (work->elapsed < work->config->camera_time[1]) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rot, -work->config->steps);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -16);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step << 6;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step << 6;
        } else {
            work->phase = 2;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, 0x800, work->ambient, work->ambient);
    if (work->elapsed < work->config->start_time) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed <= work->config->launch_time) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&position, m.t[0], m.t[1], m.t[2]);
        position.vx += -direction * 96;
        position.vz += -direction * work->bob;
        position.vy -= 48;
        if (work->elapsed <= work->config->bob_time) {
            work->bob += 8;
        } else {
            work->bob -= 18;
        }
        if (work->elapsed <= work->config->launch_time) {
            if (work->color.rgb.r < work->config->r - 31) {
                work->color.rgb.r += work->config->fade_in;
            } else {
                work->color.rgb.r = work->config->r;
            }
            if (work->color.rgb.g < work->config->g - 31) {
                work->color.rgb.g += work->config->fade_in;
            } else {
                work->color.rgb.g = work->config->g;
            }
            if (work->color.rgb.b < work->config->b - 31) {
                work->color.rgb.b += work->config->fade_in;
            } else {
                work->color.rgb.b = work->config->b;
            }
            copyVector(&work->pos, &position);
            work->velocity.vx = -work->pos.vx / work->config->steps;
            work->velocity.vy = (-350 - work->pos.vy) / work->config->steps;
            work->velocity.vz = (direction * 450 - work->pos.vz) / work->config->steps;
        }
    }

    if (work->state == 0) {
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        setPolyFT4(&ft4);
        ft4.tpage = work->texture[0] >> 16;
        ft4.clut = work->texture[0];
        setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
        setVector(&quad[0], -work->config->size, -work->config->size, 0);
        setVector(&quad[1], work->config->size, -work->config->size, 0);
        setVector(&quad[2], -work->config->size, work->config->size, 0);
        setVector(&quad[3], work->config->size, work->config->size, 0);
        GsSetLsMatrix(&light);
        RotTrans(&work->pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
        setSemiTrans(&ft4, 0);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 0);
        }
    }

    if (work->color.word & 0xFFFFFF) {
        setVector(&rot, 0, 0, 0);
        setPolyFT4(&ft4);
        ft4.tpage = work->texture[1] >> 16;
        ft4.clut = work->texture[1];
        setRGB0(&ft4, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        setVector(&quad[0], -work->config->cloud_size, -work->config->cloud_size, 0);
        setVector(&quad[1], work->config->cloud_size, -work->config->cloud_size, 0);
        setVector(&quad[2], -work->config->cloud_size, work->config->cloud_size, 0);
        setVector(&quad[3], work->config->cloud_size, work->config->cloud_size, 0);
        for (i = 0; i < 16; i++) {
            copyVector(&position, &work->pos);
            addVector(&position, &work->offsets[i]);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
            setSemiTrans(&ft4, 1);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
        }

        setPolyG3(&g3);
        setRGB0(&g3, 0, 0, 0);
        setRGB1(&g3, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        setRGB2(&g3, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        setVector(&quad[0], work->config->swing * ccos((work->elapsed << 6) % 4096) / 4096, -work->lift,
                  work->config->swing * csin((work->elapsed << 6) % 4096) / 4096);
        setVector(&quad[1], -work->config->size / 4, 0, 0);
        setVector(&quad[2], work->config->size / 4, 0, 0);
        for (i = 0; i < 16; i++) {
            setVector(&rot, 0, 0, 0);
            for (j = 0; j < 4; j++) {
                rot.vx = work->spin;
                rot.vy += (j << 9) % 4096;
                copyVector(&position, &work->pos);
                addVector(&position, &work->offsets[i]);
                GsSetLsMatrix(&light);
                RotTrans(&position, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                  (PSXLONG *)&g3.x2, &p, &flag);
                setSemiTrans(&g3, 1);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                }
            }
        }

        if (work->state != 0) {
            setVector(&rot, 0, 0, 0);
            setVector(&scale, work->scale, work->scale, work->scale);
            setPolyGT4(&gt4);
            gt4.tpage = work->texture[2] >> 16;
            gt4.clut = work->texture[2];
            setRGB0(&gt4, work->color.rgb.r / 3, work->color.rgb.g / 3, work->color.rgb.b / 3);
            setRGB1(&gt4, work->color.rgb.r / 3, work->color.rgb.g / 3, work->color.rgb.b / 3);
            setRGB2(&gt4, 0, 0, 0);
            setRGB3(&gt4, 0, 0, 0);
            copyVector(&position, &work->pos);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            saved = m;
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < 16; i++) {
                otz = RotAverage4(&work->ring[i][1], &work->ring[(i + 1) % 16][1], &work->ring[i][0],
                                  &work->ring[(i + 1) % 16][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                  (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                setUV4(&gt4, 0xC0, 0, 0xDF, 0, 0xC0, 0x7F, 0xDF, 0x7F);
                setSemiTrans(&gt4, 1);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                }
            }

            setPolyFT4(&ft4);
            ft4.tpage = work->texture[1] >> 16;
            ft4.clut = work->texture[1];
            setRGB0(&ft4, work->color.rgb.r / 2, work->color.rgb.g / 2, work->color.rgb.b / 2);
            setVector(&quad[0], -work->config->burst_size, -work->config->burst_size, 0);
            setVector(&quad[1], work->config->burst_size, -work->config->burst_size, 0);
            setVector(&quad[2], -work->config->burst_size, work->config->burst_size, 0);
            setVector(&quad[3], work->config->burst_size, work->config->burst_size, 0);
            for (i = 0; i < 4; i++) {
                setVector(&scale, work->scale / (i + 1), work->scale / (i + 1), work->scale / (i + 1));
                GsSetLsMatrix(&light);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                  (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                setSemiTrans(&ft4, 1);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }
            }

            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&rot, 0, 0, 0);
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
            setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
            for (i = 0; i < 32; i++) {
                if (work->sparks[i].vy < 0) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->sparks[i], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                    setSemiTrans(&ft4, 1);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 0);
                    }
                    addVector(&work->sparks[i], &work->spark_velocity[i]);
                    work->spark_velocity[i].vy += 2;
                }
            }
        }
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
    }

    if (work->elapsed > work->config->launch_time && work->state == 0) {
        addVector(&work->pos, &work->velocity);
        work->lift += 12;
        if (ABS(work->spin) < 0x400) {
            work->spin += direction * (step << 7);
        } else {
            work->spin = direction << 10;
        }
        if (direction > 0) {
            if (work->pos.vz > 450) {
                work->state = 1;
            }
        } else if (work->pos.vz < direction * 450) {
            work->state = 1;
        }
    } else if (work->state != 0) {
        if (work->scale < 0x3000) {
            work->scale += work->config->grow;
        } else {
            work->scale += work->config->slow_grow;
        }
        if (work->color.rgb.r >= 16) {
            work->color.rgb.r -= work->config->fade;
        } else {
            work->color.rgb.r = 0;
        }
        if (work->color.rgb.g >= 16) {
            work->color.rgb.g -= work->config->fade;
        } else {
            work->color.rgb.g = 0;
        }
        if (work->color.rgb.b >= 16) {
            work->color.rgb.b -= work->config->fade;
        } else {
            work->color.rgb.b = 0;
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 && !(work->color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->state == 1) {
        work->state = 2;
        work->phase = 1;
        return 1;
    }
    return 0;
}
