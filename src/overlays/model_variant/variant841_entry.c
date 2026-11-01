#include "../../types.h"
#include "variant841_entry.h"

/* Header 841's entry: each side launches orbs from one of three model parts
 * at a fixed period. Each orb spins as a textured, animated sprite recording
 * a ten-point trail on its way to a spread point in front of the opposing
 * slot; on arrival it fades, bursts into a growing quartered flash and throws
 * up to four animated sparks. The ambient light dims while the orbs fly and
 * recovers once they are gone. */

static const VECTOR gModelVariant841_D16D0 = {4096, 4096, 4096, 0};

s32 ModelVariant841_HandleRequest(u8 *context, s32 command)
{
    Model841State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G4 g4;
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
    s32 i, j, k, l;
    s32 r;
    s16 sign;
    s32 otz;
    POLY_FT4 *poly;
    POLY_G4 *gouraud;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant841_D16D0;
    work = (Model841State *)context;
    poly = &ft4;
    gouraud = &g4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant841_D1718[command];
        for (j = 0; j < 2; j++) {
            for (i = 0; i < 40; i++) {
                setVector(&work->pos[i][j], 0, 0, 0);
                setVector(&work->burst_pos[i][j], 0, 0, 0);
                setVector(&work->vel[i][j], 0, 0, 0);
                if (work->config->mode == 0) {
                    work->angle[i][j] = rand() % 1024;
                } else if (work->config->mode == 1) {
                    work->angle[i][j] = 0x400;
                }
                work->state[i][j] = 0;
                work->alpha[i][j] = 0x80;
                work->trail_head[i][j] = 0;
                work->frame[i][j] = rand() % 8;
                for (k = 0; k < 10; k++) {
                    setVector(&work->trail[i][k][j], 0, 0, 0);
                }
                work->color[i].r = work->config->r;
                work->color[i].g = work->config->g;
                work->color[i].b = work->config->b;
                work->burst_color[i].r = work->config->burst_r;
                work->burst_color[i].g = work->config->burst_g;
                work->burst_color[i].b = work->config->burst_b;
                work->unknown_2A90[i][j] = 0;
                work->spark_count[i][j] = 0;
                work->scale[i] = 0;
                for (k = 0; k < 4; k++) {
                    setVector(&work->sparks[i][k],
                              work->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                              work->config->spark_spread * ((rand() - rand()) % 4096) / 4096, 0);
                    work->spark_frame[i][k][j] = 0;
                }
            }
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant841_D16E0[i]);
        }
        work->burst_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->burst_clut = 0x3D28;
#else
        work->burst_clut = 0x3D20;
#endif
        work->level = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_times[0]) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rot, -4);
    } else if (work->elapsed < work->config->camera_times[1]) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -32);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -16);
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 2) {
        if (work->level < 0x800) {
            work->level += step << 6;
        } else {
            work->phase = 3;
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
    setPolyFT4(poly);
    setPolyG4(gouraud);
    for (j = 0; j < 2; j++) {
        for (i = 0; i < work->count; i++) {
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            poly->tpage = work->texture[0] >> 16;
            poly->clut = work->texture[0];
            setUV4(poly, (work->frame[i][j] % 4) << 6, (work->frame[i][j] >> 2) << 6,
                   ((work->frame[i][j] % 4) << 6) + 63, (work->frame[i][j] >> 2) << 6,
                   (work->frame[i][j] % 4) << 6, ((work->frame[i][j] >> 2) << 6) + 63,
                   ((work->frame[i][j] % 4) << 6) + 63, ((work->frame[i][j] >> 2) << 6) + 63);
            work->frame[i][j] = (work->frame[i][j] + 1) % 8;
            if (work->state[i][j] == 0) {
                r = rand() % 3;
                if (r == 0) {
                    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part_a[j]), &m);
                } else if (r == 1) {
                    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part_b[j]), &m);
                } else {
                    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part_c[j]), &m);
                }
                setVector(&work->pos[i][j], m.t[0], m.t[1], m.t[2]);
                for (k = 0; k < 10; k++) {
                    setVector(&work->trail[i][k][j], m.t[0], m.t[1], m.t[2]);
                }
                setVector(&work->vel[i][j],
                          (work->config->spread * ((rand() - rand()) % 4096) / 4096 - work->pos[i][j].vx) / work->config->steps,
                          (work->config->spread * ((rand() - rand()) % 4096) / 4096 - 350 - work->pos[i][j].vy) / work->config->steps,
                          (direction * 450 - work->pos[i][j].vz) / work->config->steps);
                work->state[i][j] = 1;
            }
            if (work->alpha[i][j] != 0) {
                setRGB0(poly, work->alpha[i][j], work->alpha[i][j], work->alpha[i][j]);
                setVector(&quad[0], 0, -64, direction << 6);
                setVector(&quad[1], 0, -64, -direction << 6);
                setVector(&quad[2], 0, 64, direction << 6);
                setVector(&quad[3], 0, 64, -direction << 6);
                rot.vx = 0;
                rot.vy = 0;
                rot.vz = work->angle[i][j];
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[2], &quad[0], &quad[3], &quad[1], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                setSemiTrans(poly, 1);
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
                }
                copyVector(&work->trail[i][work->trail_head[i][j]][j], &work->pos[i][j]);
                work->trail_head[i][j] = (work->trail_head[i][j] + 1) % 10;
                addVector(&work->pos[i][j], &work->vel[i][j]);
                if (work->config->mode == 0) {
                    work->angle[i][j] = (work->angle[i][j] + 0x80) % 4096;
                }
                if (work->state[i][j] == 1) {
                    if (direction > 0) {
                        if (work->pos[i][j].vz >= 450) {
                            copyVector(&work->burst_pos[i][j], &work->pos[i][j]);
                            work->state[i][j] = 2;
                        }
                    } else if (work->pos[i][j].vz < -449) {
                        copyVector(&work->burst_pos[i][j], &work->pos[i][j]);
                        work->state[i][j] = 2;
                    }
                }
                if (work->state[i][j] == 2) {
                    if (work->alpha[i][j] > 8) {
                        work->alpha[i][j] -= 8;
                    } else {
                        work->alpha[i][j] = 0;
                    }
                }
            }
            if (work->state[i][j] >= 2
                && (work->burst_color[i].r != 0 || work->burst_color[i].g != 0 || work->burst_color[i].b != 0)) {
                setVector(&scale, work->scale[i] + 0x1000, work->scale[i] + 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                poly->tpage = work->burst_tpage;
                poly->clut = work->burst_clut;
                GsSetLsMatrix(&light);
                RotTrans(&work->burst_pos[i][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                sign = 1;
                for (k = 0; k < 4; k++) {
                    poly->r0 = work->burst_color[i].r;
                    poly->g0 = work->burst_color[i].g;
                    sign *= -1;
                    poly->b0 = work->burst_color[i].b;
                    setVector(&quad[0], -(sign * 32), k - 2 < 0 ? 32 : -32, 0);
                    setVector(&quad[1], 0, k - 2 >= 0 ? -32 : 32, 0);
                    setVector(&quad[2], -(sign * 32), 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    setUV4(poly, 64, 0, 127, 0, 64, 63, 127, 63);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                    (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                    setUV4(poly, 64, 64, 127, 64, 64, 127, 127, 127);
                    poly->r0 = work->burst_color[i].r >> 2;
                    poly->g0 = work->burst_color[i].g >> 2;
                    poly->b0 = work->burst_color[i].b >> 2;
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                    (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                }
                m = saved;
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                poly->tpage = work->texture[1] >> 16;
                poly->clut = work->texture[1];
                setRGB0(poly, 0x80, 0x10, 0x10);
                for (k = 0; k < work->spark_count[i][j]; k++) {
                    /* Dead store; it keeps the loop large enough that GCC
                     * leaves the spark row address to the orb loop. */
                    sign = 1;
                    if (work->spark_frame[i][k][j] < 8) {
                        setUV4(poly, (work->spark_frame[i][k][j] % 4) << 5, ((work->spark_frame[i][k][j] >> 2) << 5) + 128,
                               ((work->spark_frame[i][k][j] % 4) << 5) + 31, ((work->spark_frame[i][k][j] >> 2) << 5) + 128,
                               (work->spark_frame[i][k][j] % 4) << 5, ((work->spark_frame[i][k][j] >> 2) << 5) + 159,
                               ((work->spark_frame[i][k][j] % 4) << 5) + 31, ((work->spark_frame[i][k][j] >> 2) << 5) + 159);
                        setVector(&quad[0], -64, -64, 0);
                        setVector(&quad[1], 64, -64, 0);
                        setVector(&quad[2], -64, 64, 0);
                        setVector(&quad[3], 64, 64, 0);
                        for (l = 0; l < 4; l++) {
                            addVector(&quad[l], &work->sparks[i][k]);
                        }
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                        (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                        if (otz >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                        }
                        work->spark_frame[i][k][j]++;
                    }
                }
                work->spark_count[i][j]++;
                if (work->spark_count[i][j] > work->config->spark_max) {
                    work->spark_count[i][j] = work->config->spark_max;
                }
                if (j == 1) {
                    if (work->scale[i] < 0x6000) {
                        work->scale[i] += 0x1000;
                    } else {
                        work->scale[i] += 0x100;
                    }
                    if (work->burst_color[i].r > 8) {
                        work->burst_color[i].r -= 8;
                    } else {
                        work->burst_color[i].r = 0;
                    }
                    if (work->burst_color[i].g > 8) {
                        work->burst_color[i].g -= 8;
                    } else {
                        work->burst_color[i].g = 0;
                    }
                    if (work->burst_color[i].b > 8) {
                        work->burst_color[i].b -= 8;
                    } else {
                        work->burst_color[i].b = 0;
                    }
                }
            }
        }
    }
    if (work->elapsed < work->config->stop && work->tick % work->config->period == 0) {
        work->count++;
        if (work->count > work->config->max_count) {
            work->count = work->config->max_count;
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 3 && work->burst_color[work->count - 1].r == 0 && work->burst_color[work->count - 1].g == 0
        && work->burst_color[work->count - 1].b == 0
        && work->spark_frame[work->count - 1][work->config->spark_max - 1][0] >= 8
        && work->spark_frame[work->count - 1][work->config->spark_max - 1][1] >= 8) {
        return 2;
    }
    if (work->state[work->count - 1][0] >= 2 && work->state[work->count - 1][1] >= 2 && work->phase == 0
        && work->elapsed >= work->config->stop) {
        work->phase = 2;
        return 1;
    }
    if (work->state[0][0] >= 2 && work->phase == 0) {
        return 4;
    }
    return 0;
}
