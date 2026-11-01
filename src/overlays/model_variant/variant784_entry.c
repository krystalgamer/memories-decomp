#include "../../types.h"
#include "variant784_entry.h"

/* Header 784's entry: up to five shots leave a model point one after
 * another at the scheduled times and fly at a jittered target on the other
 * side, each wrapped in a ring of sprites with swirling strips and leaving
 * a fading ribbon, while smoke puffs drift from the launch point and a
 * flash flickers there. Landed shots spray sparks and bloom into a growing
 * layered fan. The ambient light dims and recovers. */

static const VECTOR gModelVariant784_D4080 = {4096, 4096, 4096, 0};

s32 ModelVariant784_HandleRequest(u8 *context, s32 command)
{
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_FT4 puff;
    POLY_FT4 glow;
    POLY_G4 g4;
    POLY_GT4 gt4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    Model784State *work;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 j;
    s32 k;
    s32 g;
    s32 b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant784_D4080;
    work = (Model784State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant784_D411C[command];
        for (i = 0; i < 5; i++) {
            work->tail[i] = 0;
            work->spark_count[i] = 0;
            work->state[i] = 0;
            work->flash_frame[i] = 0;
            work->trail_head[i] = 0;
            work->growth[i] = 0x1000;
            work->bolt_r[i] = work->config->r;
            work->bolt_g[i] = work->config->g;
            work->bolt_b[i] = work->config->b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            work->glow2_r[i] = work->config->glow2_r;
            work->glow2_g[i] = work->config->glow2_g;
            work->glow2_b[i] = work->config->glow2_b;
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->origin[i], 0, 0, 0);
            for (j = 0; j < work->config->ring_count; j++) {
                g = rand() % 4096;
                b = rand() % 4096;
                setVector(&work->ring[i][j], work->config->size * ccos(g) / 4096 * ccos(b) / 4096,
                          work->config->size * csin(g) / 4096 * ccos(b) / 4096,
                          work->config->size * ccos(g) / 4096 * csin(b) / 4096);
            }
            for (j = 0; j < work->config->puff_count; j++) {
                g = rand() % 4096 - rand() % 4096;
                b = rand() % 4096 - rand() % 4096;
                k = rand() % 4096;
                setVector(&work->puffs[i][j], work->config->puff_spread * g / 4096,
                          work->config->puff_spread * b / 4096, direction * work->config->puff_spread * k / 4096);
                setVector(&work->puff_velocity[i][j], work->config->puff_speed * g / 4096,
                          work->config->puff_speed * b / 4096, direction * work->config->puff_speed * k / 4096);
                work->puff_frame[i][j] = 0;
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            for (j = 0; j < 8; j++) {
                work->spark_frame[i][j] = 0;
                setVector(&work->sparks[i][j], work->config->fan_outer * ((rand() - rand()) % 4096) / 4096, 0,
                          work->config->fan_outer * ((rand() - rand()) % 4096) / 4096);
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->fan_inner[i], work->config->fan_inner * ccos(i * 256 % 4096) / 4096, 0,
                      work->config->fan_inner * csin(i * 256 % 4096) / 4096);
            setVector(&work->fan_outer[i], work->config->fan_outer * ccos(i * 256 % 4096) / 4096,
                      -work->config->fan_depth, work->config->fan_outer * csin(i * 256 % 4096) / 4096);
            setVector(&work->fan2_inner[i], work->config->fan2_inner * ((s32)((u32)ccos(i * 256 % 4096) << 1)) / 4096,
                      -work->config->fan_depth / 2,
                      work->config->fan2_inner * ((s32)((u32)csin(i * 256 % 4096) << 1)) / 4096);
            setVector(&work->fan2_outer[i], work->config->fan2_outer * ((s32)((u32)ccos(i * 256 % 4096) << 1)) / 4096,
                      -work->config->fan_depth / 2,
                      work->config->fan2_outer * ((s32)((u32)csin(i * 256 % 4096) << 1)) / 4096);
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant784_D4090[i]);
        }
        work->elapsed = 0;
        work->fired = 0;
        work->phase = 0;
        work->ambient = 0x800;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start[0]) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 32);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 32);
    }
    if (work->fired == 0) {
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
    if (work->elapsed < work->config->start[0]) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();

    if ((work->fired == 0 && work->elapsed >= work->config->start[0]) ||
        (work->fired == 1 && work->elapsed >= work->config->start[1]) ||
        (work->fired == 2 && work->elapsed >= work->config->start[2]) ||
        (work->fired == 3 && work->elapsed >= work->config->start[3]) ||
        (work->fired == 4 && work->elapsed >= work->config->start[4])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->pos[work->fired], m.t[0], m.t[1], m.t[2]);
        setVector(&work->origin[work->fired], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rot);
        rot.vx += work->config->target_spread * ((rand() - rand()) % 4096) / 4096;
        rot.vz += work->config->target_spread * ((rand() - rand()) % 4096) / 4096;
        setVector(&work->step[work->fired], (rot.vx - work->pos[work->fired].vx) / work->config->steps,
                  -work->pos[work->fired].vy / work->config->steps,
                  (rot.vz - work->pos[work->fired].vz) / work->config->steps);
        setVector(&work->heading[work->fired],
                  ratan2(work->step[work->fired].vz, work->step[work->fired].vy) - 0x400,
                  direction * ratan2(work->step[work->fired].vx, work->step[work->fired].vz), 0);
        addVector(&work->pos[work->fired], &work->step[work->fired]);
        setVector(&rot, 0, 0, 0);
        work->fired++;
    }

    for (i = 0; i < work->fired; i++) {
        if (work->state[i] < 2) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[4] >> 16;
            ft4.clut = work->texture[4];
            setRGB0(&ft4, 0x80, 0x80, 0x80);
            setVector(&quad[0], -work->config->size, -work->config->size, 0);
            setVector(&quad[1], work->config->size, -work->config->size, 0);
            setVector(&quad[2], -work->config->size, work->config->size, 0);
            setVector(&quad[3], work->config->size, work->config->size, 0);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
        }
        if (work->state[i] > 0 && (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i])) {
            setPolyG3(&g3);
            for (j = 0; j < work->config->ring_count; j++) {
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                copyVector(&pos, &work->pos[i]);
                addVector(&pos, &work->ring[i][j]);
                setRGB0(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB1(&g3, 0, 0, 0);
                setRGB2(&g3, 0, 0, 0);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[3] >> 16;
                ft4.clut = work->texture[3];
                setRGB0(&ft4, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setVector(&quad[0], -work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[1], work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[2], -work->config->ring_size, work->config->ring_size, 0);
                setVector(&quad[3], work->config->ring_size, work->config->ring_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                  (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                setSemiTrans(&ft4, 1);
                if (otz >= 0) {
                    GsSortPoly(&ft4, ot, (u16)otz);
                }
                setRGB0(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB1(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB2(&g3, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 6 * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 6 * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 8 * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 8 * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 7 * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 7 * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 7 * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / 7 * ((u32)-work->step[i].vz << 1));
                }
                for (k = 0; k < 4; k++) {
                    copyVector(&rot, &work->heading[i]);
                    rot.vz += k * 512 % 4096;
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                      (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                      (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                }
            }
        }
        if (work->pos[i].vy < 0) {
            copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
            work->trail_head[i] = (work->trail_head[i] + 1) % 16;
            addVector(&work->pos[i], &work->step[i]);
            if (work->pos[i].vy > 0) {
                work->pos[i].vy = 0;
                work->state[i] = 2;
            }
            if (work->state[i] == 0) {
                work->state[i] = 1;
            }
        }

        setPolyFT4(&puff);
        puff.tpage = work->texture[1] >> 16;
        puff.clut = work->texture[1];
        setRGB0(&puff, 0x80, 0x80, 0x80);
        setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
        setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
        setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
        setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
        for (j = 0; j < work->config->puff_count; j++) {
            if (work->puff_frame[i][j] < 16) {
                copyVector(&pos, &work->origin[i]);
                addVector(&pos, &work->puffs[i][j]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&puff.x0, (PSXLONG *)&puff.x1,
                                  (PSXLONG *)&puff.x2, (PSXLONG *)&puff.x3, &p, &flag);
                if (work->puff_frame[i][j] < 8) {
                    setUV4(&puff, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x40,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x40,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x5F,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x5F);
                } else {
                    setUV4(&puff, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x60,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x60,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x7F,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x7F);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&puff, ot, (u16)otz, 1);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        setPolyFT4(&glow);
        glow.tpage = work->texture[2] >> 16;
        glow.clut = work->texture[2];
        setRGB0(&glow, 0x80, 0x80, 0x80);
        setVector(&quad[0], -work->config->flash_size, -work->config->flash_size, 0);
        setVector(&quad[1], work->config->flash_size, -work->config->flash_size, 0);
        setVector(&quad[2], -work->config->flash_size, work->config->flash_size, 0);
        setVector(&quad[3], work->config->flash_size, work->config->flash_size, 0);
        if (work->flash_frame[i] < 8) {
            copyVector(&pos, &work->origin[i]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1,
                              (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag) - 8;
            if (work->flash_frame[i] < 4) {
                setUV4(&glow, work->flash_frame[i] % 4 * 32 + 0x80, 0x80, work->flash_frame[i] % 4 * 32 + 0x9F, 0x80,
                       work->flash_frame[i] % 4 * 32 + 0x80, 0x9F, work->flash_frame[i] % 4 * 32 + 0x9F, 0x9F);
            } else {
                setUV4(&glow, work->flash_frame[i] % 4 * 32 + 0x80, 0xA0, work->flash_frame[i] % 4 * 32 + 0x9F, 0xA0,
                       work->flash_frame[i] % 4 * 32 + 0x80, 0xBF, work->flash_frame[i] % 4 * 32 + 0x9F, 0xBF);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
            }
            work->flash_frame[i]++;
        }

        setPolyG4(&g4);
        if (work->trail_head[i] >= work->tail[i] && (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i])) {
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (k = 1, j = work->trail_head[i] - 1; j >= work->trail_head[i] - work->tail[i]; j--) {
                setRGB0(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                setRGB1(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                setRGB2(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                setRGB3(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->config->size / k,
                          work->trail[i][j].vz);
                setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->size / k,
                          work->trail[i][j].vz);
                setVector(&quad[2], work->trail[i][j - 1].vx, work->trail[i][j - 1].vy - work->config->size / (k + 1),
                          work->trail[i][j - 1].vz);
                setVector(&quad[3], work->trail[i][j - 1].vx, work->trail[i][j - 1].vy + work->config->size / (k + 1),
                          work->trail[i][j - 1].vz);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
                setVector(&quad[0], work->trail[i][j].vx - work->config->size / k, work->trail[i][j].vy,
                          work->trail[i][j].vz);
                setVector(&quad[1], work->trail[i][j].vx + work->config->size / k, work->trail[i][j].vy,
                          work->trail[i][j].vz);
                setVector(&quad[2], work->trail[i][j - 1].vx - work->config->size / (k + 1), work->trail[i][j - 1].vy,
                          work->trail[i][j - 1].vz);
                setVector(&quad[3], work->trail[i][j - 1].vx + work->config->size / (k + 1), work->trail[i][j - 1].vy,
                          work->trail[i][j - 1].vz);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
                k++;
            }
            if (work->state[i] >= 2) {
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
            } else {
                work->tail[i]++;
            }
        }
    }

    for (i = 0; i < 5; i++) {
        if (work->state[i] >= 2) {
            setPolyFT4(&glow);
            glow.tpage = work->texture[2] >> 16;
            glow.clut = work->texture[2];
            setRGB0(&glow, 0xFF, 0xFF, 0xFF);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
            for (j = 0; j < work->spark_count[i]; j++) {
                if (work->spark_frame[i][j] < 8) {
                    copyVector(&pos, &work->pos[i]);
                    addVector(&pos, &work->sparks[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                    if (work->spark_frame[i][j] < 4) {
                        setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0x80,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x80, work->spark_frame[i][j] % 4 * 32 + 0x80,
                               0x9F, work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x9F);
                    } else {
                        setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0xA0,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0xA0, work->spark_frame[i][j] % 4 * 32 + 0x80,
                               0xBF, work->spark_frame[i][j] % 4 * 32 + 0x9F, 0xBF);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                    }
                    work->spark_frame[i][j]++;
                }
            }
            if (work->glow_r[i] || work->glow_g[i] || work->glow_b[i]) {
                setPolyGT4(&gt4);
                gt4.tpage = work->texture[0] >> 16;
                gt4.clut = work->texture[0];
                setRGB0(&gt4, 0, 0, 0);
                setRGB1(&gt4, 0, 0, 0);
                setRGB2(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB3(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, work->growth[i] / 16 + 0x1000, work->growth[i] + 0x1000,
                          work->growth[i] / 16 + 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                setVector(&scale, work->growth[i] / 8 + 0x1000, work->growth[i] / 2 + 0x1000,
                          work->growth[i] / 8 + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                setVector(&scale, work->growth[i] / 4 + 0x1000, work->growth[i] / 4 + 0x1000,
                          work->growth[i] / 4 + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                setVector(&scale, work->growth[i] / 2 + 0x1000, work->growth[i] / 8,
                          work->growth[i] / 2 + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
            }
            if (work->glow2_r[i] || work->glow2_g[i] || work->glow2_b[i]) {
                setPolyGT4(&gt4);
                gt4.tpage = work->texture[0] >> 16;
                gt4.clut = work->texture[0];
                setRGB0(&gt4, 0, 0, 0);
                setRGB1(&gt4, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setRGB2(&gt4, work->glow2_r[i], work->glow2_g[i], work->glow2_b[i]);
                setRGB3(&gt4, work->glow2_r[i], work->glow2_g[i], work->glow2_b[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, work->growth[i] / 8 + 0x1000, work->growth[i] / 2 + 0x1000,
                          work->growth[i] / 8 + 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan2_outer[15], &work->fan2_outer[0], &work->fan2_inner[15],
                                          &work->fan2_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan2_outer[j], &work->fan2_outer[j + 1], &work->fan2_inner[j],
                                          &work->fan2_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                setVector(&scale, work->growth[i] / 12 + 0x1000, work->growth[i] + 0x1000,
                          work->growth[i] / 12 + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan2_outer[15], &work->fan2_outer[0], &work->fan2_inner[15],
                                          &work->fan2_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan2_outer[j], &work->fan2_outer[j + 1], &work->fan2_inner[j],
                                          &work->fan2_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
            }
            if (++work->spark_count[i] > work->config->spark_count) {
                work->spark_count[i] = work->config->spark_count;
            }
            if (work->growth[i] < 20000) {
                work->growth[i] += work->config->grow;
            } else {
                work->growth[i] += work->config->grow_late;
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
                if (work->glow2_r[i] >= 16) {
                    work->glow2_r[i] -= work->config->fade / 2;
                } else {
                    work->glow2_r[i] = 0;
                }
                if (work->glow2_g[i] >= 16) {
                    work->glow2_g[i] -= work->config->fade / 2;
                } else {
                    work->glow2_g[i] = 0;
                }
                if (work->glow2_b[i] >= 16) {
                    work->glow2_b[i] -= work->config->fade / 2;
                } else {
                    work->glow2_b[i] = 0;
                }
                if (work->phase == 1 && i == 4 && work->glow_r[4] == 0) {
                    work->phase = 2;
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3) {
        return 2;
    }
    if (work->state[4] == 2 && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->state[0] == 2 && work->phase == 0) {
        return 4;
    }
    return 0;
}
