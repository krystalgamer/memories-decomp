#include "../../types.h"
#include "variant835_entry.h"

/* Header 835's entry: one more orb joins each frame up to the command's
 * count. Each orb leaves a model part and flies, as a textured sprite with
 * a fading sixteen-point gradient trail, towards a spread point in front of
 * the opposing slot. On arrival it fades, bursts into a growing quartered
 * flash and throws the command's number of animated sparks. The ambient
 * light dims while the orbs fly and recovers once they are gone. */

static const VECTOR gModelVariant835_D1800 = {4096, 4096, 4096, 0};

s32 ModelVariant835_HandleRequest(u8 *context, s32 command)
{
    Model835State *work;
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
    s32 i, k, l;
    s16 sign;
    s32 otz;
    POLY_FT4 *poly;
    POLY_G4 *gouraud;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant835_D1800;
    work = (Model835State *)context;
    poly = &ft4;
    gouraud = &g4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant835_D1848[command];
        for (i = 0; i < 8; i++) {
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->vel[i], 0, 0, 0);
            work->state[i] = 0;
            work->alpha[i] = 0x80;
            work->trail_head[i] = 0;
            for (k = 0; k < 16; k++) {
                setVector(&work->trail[i][k], 0, 0, 0);
            }
            work->color[i].r = work->config->r;
            work->color[i].g = work->config->g;
            work->color[i].b = work->config->b;
            work->burst_color[i].r = work->config->burst_r;
            work->burst_color[i].g = work->config->burst_g;
            work->burst_color[i].b = work->config->burst_b;
            work->trail_start[i] = 0;
            work->spark_count[i] = 0;
            work->scale[i] = 0;
            for (k = 0; k < 8; k++) {
                setVector(&work->sparks[i][k],
                          work->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                          work->config->spark_spread * ((rand() - rand()) % 4096) / 4096, 0);
                work->spark_frame[i][k] = 0;
            }
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant835_D1810[i]);
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
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 1, &rot, -4);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rot, -8);
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
    for (i = 0; i < work->count; i++) {
        setVector(&pos, 0, 0, 0);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        poly->tpage = work->texture[0] >> 16;
        poly->clut = work->texture[0];
        setUV4(poly, 0, 0, 55, 0, 0, 31, 55, 31);
        if (work->state[i] == 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
            setVector(&work->pos[i], m.t[0], m.t[1], m.t[2]);
            for (k = 0; k < 16; k++) {
                setVector(&work->trail[i][k], m.t[0], m.t[1], m.t[2]);
            }
            setVector(&work->vel[i],
                      (work->config->spread * ((rand() - rand()) % 4096) / 4096 - work->pos[i].vx) / work->config->steps,
                      (work->config->spread * ((rand() - rand()) % 4096) / 4096 - 350 - work->pos[i].vy) / work->config->steps,
                      (direction * 450 - work->pos[i].vz) / work->config->steps);
            work->state[i] = 1;
        }
        if (work->alpha[i] != 0) {
            setRGB0(poly, work->alpha[i], work->alpha[i], work->alpha[i]);
            setVector(&quad[0], 0, -32, -direction * 48);
            setVector(&quad[1], 0, -32, direction * 48);
            setVector(&quad[2], 0, 32, -direction * 48);
            setVector(&quad[3], 0, 32, direction * 48);
            for (k = 0; k < 4; k++) {
                addVector(&quad[k], &work->pos[i]);
            }
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                              (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
            if (work->alpha[i] == 0x80) {
                setSemiTrans(poly, 0);
            } else {
                setSemiTrans(poly, 1);
            }
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
            if (work->state[i] == 1) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                if (ABS(work->pos[i].vz) < 450) {
                    addVector(&work->pos[i], &work->vel[i]);
                } else {
                    work->state[i] = 2;
                }
            }
            if (work->state[i] == 2) {
                if (work->alpha[i] > 8) {
                    work->alpha[i] -= 8;
                } else {
                    work->alpha[i] = 0;
                }
            }
        }
        if (work->trail_head[i] > work->trail_start[i]
            && (work->color[i].r != 0 || work->color[i].g != 0 || work->color[i].b != 0)) {
            l = 1;
            for (k = work->trail_head[i] - 1; k > work->trail_head[i] - (work->trail_start[i] + 1); k--, l++) {
                setRGB0(gouraud, work->color[i].r / l, work->color[i].g / l, work->color[i].b / l);
                setRGB1(gouraud, work->color[i].r / l, work->color[i].g / l, work->color[i].b / l);
                setRGB2(gouraud, work->color[i].r / (l + 1), work->color[i].g / (l + 1), work->color[i].b / (l + 1));
                setRGB3(gouraud, work->color[i].r / (l + 1), work->color[i].g / (l + 1), work->color[i].b / (l + 1));
                setVector(&quad[0], work->trail[i][k].vx, work->trail[i][k].vy - 16 / l, work->trail[i][k].vz);
                setVector(&quad[1], work->trail[i][k].vx, work->trail[i][k].vy + 16 / l, work->trail[i][k].vz);
                setVector(&quad[2], work->trail[i][k - 1].vx, work->trail[i][k - 1].vy - 16 / (l + 1), work->trail[i][k - 1].vz);
                setVector(&quad[3], work->trail[i][k - 1].vx, work->trail[i][k - 1].vy + 16 / (l + 1), work->trail[i][k - 1].vz);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gouraud->x0,
                                  (PSXLONG *)&gouraud->x1, (PSXLONG *)&gouraud->x2, (PSXLONG *)&gouraud->x3, &p,
                                  &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)gouraud, ot, (u16)otz, 1);
                }
            }
            if (work->state[i] >= 2) {
                if (work->color[i].r > 15) {
                    work->color[i].r -= 15;
                } else {
                    work->color[i].r = 0;
                }
                if (work->color[i].g > 15) {
                    work->color[i].g -= 15;
                } else {
                    work->color[i].g = 0;
                }
                if (work->color[i].b > 15) {
                    work->color[i].b -= 15;
                } else {
                    work->color[i].b = 0;
                }
            } else {
                work->trail_start[i]++;
            }
        }
        if (work->state[i] >= 2
            && (work->burst_color[i].r != 0 || work->burst_color[i].g != 0 || work->burst_color[i].b != 0)) {
            setVector(&scale, work->scale[i] + 0x1000, work->scale[i] + 0x1000, 0x1000);
            poly->tpage = work->burst_tpage;
            poly->clut = work->burst_clut;
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            sign = 1;
            RotMatrix(&rot, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
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
            for (k = 0; k < work->spark_count[i]; k++) {
                if (work->spark_frame[i][k] < 8) {
                    setUV4(poly, ((work->spark_frame[i][k] % 4) << 5) + 56, (work->spark_frame[i][k] >> 2) << 5,
                           ((work->spark_frame[i][k] % 4) << 5) + 87, (work->spark_frame[i][k] >> 2) << 5,
                           ((work->spark_frame[i][k] % 4) << 5) + 56, ((work->spark_frame[i][k] >> 2) << 5) + 31,
                           ((work->spark_frame[i][k] % 4) << 5) + 87, ((work->spark_frame[i][k] >> 2) << 5) + 31);
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
                    work->spark_frame[i][k]++;
                }
            }
            work->spark_count[i]++;
            if (work->spark_count[i] > work->config->spark_max) {
                work->spark_count[i] = work->config->spark_max;
            }
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
    work->count++;
    if (work->count > work->config->max_count) {
        work->count = work->config->max_count;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3 && work->burst_color[work->config->max_count - 1].r == 0
        && work->burst_color[work->config->max_count - 1].g == 0
        && work->burst_color[work->config->max_count - 1].b == 0
        && work->spark_frame[work->config->max_count - 1][work->config->spark_max - 1] >= 8) {
        return 2;
    }
    if (work->state[work->config->max_count - 1] >= 2 && work->phase == 0) {
        work->phase = 2;
        return 1;
    }
    if (work->state[0] >= 2 && work->phase == 0) {
        return 4;
    }
    return 0;
}
