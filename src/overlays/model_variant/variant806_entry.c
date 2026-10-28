#include "../../types.h"
#include "../../game/graphics_constants.h"
#include "variant806_entry.h"

/* Header 806's entry: a charge sprite brightens on a model part while two
 * glowing trails stream from two parts; then a core of spinning rings
 * opens on the part behind a white flash and sparks burst out of the
 * opposing slot, each throwing off tumbling embers, while the ambient light
 * dims and recovers. */

static const VECTOR D_8013CF58 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model806State *work;
    MATRIX light;
    MATRIX m;
    POLY_G4 g4;
    POLY_FT4 ft4;
    POLY_F4 f4;
    POLY_FT4 sprite;
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
    s32 frame;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CF58;
    work = (Model806State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CFD8[command];
        setVector(&work->center, 0, 0, 0);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rot);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->pos[i], rot.vx + RANDOM_SPREAD(pos.vx), rot.vy + RANDOM_SPREAD(pos.vy),
                      rot.vz + RANDOM_SPREAD(pos.vz));
            work->spark_r[i] = work->config->spark_r;
            work->spark_g[i] = work->config->spark_g;
            work->spark_b[i] = work->config->spark_b;
            for (j = 0; j < work->config->ember_count; j++) {
                copyVector(&work->embers[i][j], &work->pos[i]);
                setVector(&work->velocities[i][j], RANDOM_SPREAD(work->config->spread),
                          -work->config->spread * (rand() % 4096) / 4096, RANDOM_SPREAD(work->config->spread));
                setVector(&work->angles[i][j], ratan2(work->velocities[i][j].vz, work->velocities[i][j].vy) - 0x400,
                          direction * ratan2(work->velocities[i][j].vx, work->velocities[i][j].vz), 0);
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->trail[i][0], 0, 0, 0);
            setVector(&work->tail[i][0], 0, 0, 0);
            setVector(&work->trail[i][1], 0, 0, 0);
            setVector(&work->tail[i][1], 0, 0, 0);
        }
        work->trail_r[0] = work->config->r;
        work->trail_r[1] = work->config->r;
        work->trail_g[0] = work->config->g;
        work->trail_g[1] = work->config->g;
        work->trail_b[0] = work->config->b;
        work->trail_b[1] = work->config->b;
        work->head[0] = 0;
        work->head[1] = 0;
        work->length[0] = 0;
        work->length[1] = 0;
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CF68[i]);
        }
        work->ambient = 0x800;
        work->glow[0] = 0x80;
        work->glow[1] = 0x80;
        work->phase = 0;
        work->charge_r = 0;
        work->charge_g = 0;
        work->charge_b = 0;
        work->core_r = 0x20;
        work->core_g = 0x10;
        work->core_b = 0x20;
        work->flash = 0xFF;
        work->elapsed = 0;
        work->tick = 0;
        work->shown = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->burst) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 16);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 16);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 3) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 4;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (*(u32 *)&work->charge_r & 0xFFFFFF) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        setPolyFT4(&ft4);
        ft4.tpage = work->texture[0] >> 16;
        ft4.clut = work->texture[0];
        setRGB0(&ft4, work->charge_r, work->charge_g, work->charge_b);
        setVector(&quad[0], -work->config->charge_size, -work->config->charge_size, 0);
        setVector(&quad[1], work->config->charge_size, -work->config->charge_size, 0);
        setVector(&quad[2], -work->config->charge_size, work->config->charge_size, 0);
        setVector(&quad[3], work->config->charge_size, work->config->charge_size, 0);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, 0, 0x80, 0x3F, 0x80, 0, 0xBF, 0x3F, 0xBF);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
        }
    }
    if (work->elapsed < work->config->charge_end) {
        work->charge_r += work->config->glow_fade;
        work->charge_g += work->config->glow_fade;
        work->charge_b += work->config->glow_fade;
        if (work->charge_r > work->config->r - 15) {
            work->charge_r = work->config->r;
        }
        if (work->charge_g > work->config->g - 15) {
            work->charge_g = work->config->g;
        }
        if (work->charge_b > work->config->b - 15) {
            work->charge_b = work->config->b;
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    if (work->elapsed <= work->config->first_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&work->trail[work->head[0]][0], m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->tail_part), &m);
        setVector(&work->tail[work->head[0]][0], m.t[0], m.t[1], m.t[2]);
        work->head[0] = (work->head[0] + 1) % 16;
    }
    if (work->elapsed >= work->config->second_start && work->elapsed <= work->config->second_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&work->trail[work->head[1]][1], m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->tail_part), &m);
        setVector(&work->tail[work->head[1]][1], m.t[0], m.t[1], m.t[2]);
        work->head[1] = (work->head[1] + 1) % 16;
    }
    if (work->elapsed >= work->config->burst && work->phase == 0) {
        work->charge_r = 0;
        work->charge_g = 0;
        work->charge_b = 0;
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&work->center, m.t[0], m.t[1], m.t[2]);
        work->center.vy -= work->config->core_size / 2;
        work->phase = 1;
    }
    if (work->elapsed >= work->config->flash_start && work->phase == 1) {
        work->flash = 0xFF;
        work->phase = 2;
    }

    for (k = 0; k < 2; k++) {
        if (work->head[k] > work->length[k] - 1 && (work->trail_r[k] || work->trail_g[k] || work->trail_b[k])) {
            setPolyG4(&g4);
            setRGB2(&g4, 0, 0, 0);
            setRGB3(&g4, 0, 0, 0);
            GsSetLsMatrix(&light);
            setVector(&pos, 0, 0, 0);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = work->head[k] - 1, frame = 1; j >= work->head[k] - work->length[k]; frame++, j--) {
                setRGB0(&g4, work->trail_r[k] / frame, work->trail_g[k] / frame, work->trail_b[k] / frame);
                setRGB1(&g4, work->trail_r[k] / (frame + 1), work->trail_g[k] / (frame + 1),
                        work->trail_b[k] / (frame + 1));
                if (j == 0) {
                    otz = RotAverage4(&work->trail[j][k], &work->trail[0][k], &work->tail[j][k], &work->tail[0][k],
                                      (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                      &p, &flag);
                } else {
                    otz = RotAverage4(&work->trail[j][k], &work->trail[j - 1][k], &work->tail[j][k],
                                      &work->tail[j - 1][k], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                      (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
            }
        }
        if (work->glow[k]) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[1] >> 16;
            ft4.clut = work->texture[1];
            setRGB0(&ft4, work->glow[k], work->glow[k], work->glow[k]);
            setVector(&quad[0], -work->config->glow_width, -work->config->glow_height, 0);
            setVector(&quad[1], work->config->glow_width, -work->config->glow_height, 0);
            setVector(&quad[2], -work->config->glow_width, 0, 0);
            setVector(&quad[3], work->config->glow_width, 0, 0);
            for (j = 0; j < work->head[k]; j++) {
                GsSetLsMatrix(&light);
                RotTrans(&work->trail[j][k], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                  (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                frame = rand() % 9;
                setUV4(&ft4, frame * 32, 0x40, frame * 32 + 31, 0x40, frame * 32, 0x7F, frame * 32 + 31, 0x7F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }
            }
        }
        if (k == 0) {
            if (work->elapsed <= work->config->first_end) {
                work->length[k]++;
            } else {
                if (work->trail_r[k] >= 16) {
                    work->trail_r[k] -= work->config->fade;
                } else {
                    work->trail_r[k] = 0;
                }
                if (work->trail_g[k] >= 16) {
                    work->trail_g[k] -= work->config->fade;
                } else {
                    work->trail_g[k] = 0;
                }
                if (work->trail_b[k] >= 16) {
                    work->trail_b[k] -= work->config->fade;
                } else {
                    work->trail_b[k] = 0;
                }
                if (work->glow[k] >= 16) {
                    work->glow[k] -= work->config->glow_fade;
                } else {
                    work->glow[k] = 0;
                }
            }
        } else if (work->elapsed >= work->config->second_start && work->elapsed <= work->config->second_end) {
            work->length[k]++;
        } else if (work->elapsed > work->config->second_end) {
                if (work->trail_r[k] >= 16) {
                    work->trail_r[k] -= work->config->fade;
                } else {
                    work->trail_r[k] = 0;
                }
                if (work->trail_g[k] >= 16) {
                    work->trail_g[k] -= work->config->fade;
                } else {
                    work->trail_g[k] = 0;
                }
                if (work->trail_b[k] >= 16) {
                    work->trail_b[k] -= work->config->fade;
                } else {
                    work->trail_b[k] = 0;
                }
                if (work->glow[k] >= 16) {
                    work->glow[k] -= work->config->glow_fade;
                } else {
                    work->glow[k] = 0;
                }
            }
    }

    if (work->phase != 0) {
        if (work->flash) {
            setPolyF4(&f4);
            setRGB0(&f4, work->flash, work->flash, work->flash);
            setXY4(&f4, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
            Graphics_SubmitDrawModePacket((u32 *)&f4, ot, 0, 1);
            if (work->flash >= 16) {
                work->flash -= work->config->flash_fade;
            } else {
                work->flash = 0;
            }
        }
        if (*(u32 *)&work->core_r & 0xFFFFFF) {
            setPolyFT4(&sprite);
            sprite.tpage = work->texture[3] >> 16;
            sprite.clut = work->texture[3];
            setVector(&quad[0], -work->config->core_size, -work->config->core_size, 0);
            setVector(&quad[1], work->config->core_size, -work->config->core_size, 0);
            setVector(&quad[2], -work->config->core_size, work->config->core_size, 0);
            setVector(&quad[3], work->config->core_size, work->config->core_size, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->center, (VECTOR *)m.t, &flag);
            saved = m;
            for (i = 0; i < 4; i++) {
                setRGB0(&sprite, work->core_r * (i + 1), work->core_g * (i + 1), work->core_b * (i + 1));
                m = saved;
                setVector(&rot, 0, 0, work->elapsed * work->config->spin[i]);
                GsSetLsMatrix(&light);
                RotMatrix(&rot, &m);
                setVector(&scale, 0x1000 - i * 512, 0x1000 - i * 512, 0x1000 - i * 512);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite.x0,
                                  (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &p, &flag);
                setUV4(&sprite, 0x80, 0x80, 0xFF, 0x80, 0x80, 0xFE, 0xFF, 0xFE);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                }
            }
            if (work->phase >= 2) {
                if (work->core_r >= 3) {
                    work->core_r -= work->config->core_fade;
                } else {
                    work->core_r = 0;
                }
                if (work->core_g >= 3) {
                    work->core_g -= work->config->core_fade;
                } else {
                    work->core_g = 0;
                }
                if (work->core_b >= 3) {
                    work->core_b -= work->config->core_fade;
                } else {
                    work->core_b = 0;
                }
            }
        }
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        for (i = 0; i < work->shown; i++) {
            if (work->spark_r[i] || work->spark_g[i] || work->spark_b[i]) {
                setPolyFT4(&sprite);
                sprite.tpage = work->texture[0] >> 16;
                sprite.clut = work->texture[0];
                setRGB0(&sprite, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
                setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite.x0,
                                  (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &p, &flag);
                setUV4(&sprite, 0, 0x80, 0x3F, 0x80, 0, 0xBF, 0x3F, 0xBF);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                }
                sprite.tpage = work->texture[2] >> 16;
                sprite.clut = work->texture[2];
                setRGB0(&sprite, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
                setVector(&quad[0], -work->config->ember_width, -work->config->ember_height, 0);
                setVector(&quad[1], work->config->ember_width, -work->config->ember_height, 0);
                setVector(&quad[2], -work->config->ember_width, work->config->ember_height, 0);
                setVector(&quad[3], work->config->ember_width, work->config->ember_height, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                for (j = 0; j < work->config->ember_count; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->embers[i][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sprite.x0,
                                      (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &p,
                                      &flag);
                    frame = rand() % 9;
                    setUV4(&sprite, frame * 32, 0, frame * 32 + 31, 0, frame * 32, 0x3F, frame * 32 + 31, 0x3F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                    }
                    addVector(&work->embers[i][j], &work->velocities[i][j]);
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
        }
        if (work->phase < 2 && work->tick % 3 == 0) {
            work->shown++;
            if (work->shown > work->config->count) {
                work->shown = work->config->count;
            }
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 4
        && (work->spark_r[work->shown - 1] == 0 || work->spark_g[work->shown - 1] == 0
            || work->spark_b[work->shown - 1] == 0)
        && work->flash == 0) {
        return 2;
    }
    if (work->phase != 2) {
        return (work->phase == 1) * 4;
    }
    work->phase = 3;
    return 1;
}
