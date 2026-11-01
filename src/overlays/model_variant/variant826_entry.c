#include "../../types.h"
#include "variant826_entry.h"

/* Header 826's entry: the camera holds on the slot and then swings round
 * while the ambient light dims and a scrolling tint washes over the whole
 * screen. A pulsing, spinning glow follows a model part, and up to sixteen
 * shards appear around the opposing slot, flare to the row's colour and fade.
 * After spawning ends the tint and glow fade while the light comes back. */

static const VECTOR gModelVariant826_D2208 = {4096, 4096, 4096, 0};

s32 ModelVariant826_HandleRequest(u8 *context, s32 command)
{
    Model826State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 tint;
    POLY_GT4 quad;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j;
    s32 sign;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant826_D2208;
    work = (Model826State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant826_D2250[command];
        for (i = 0; i < 16; i++) {
            setVector(&work->positions[i], 0, 0, 0);
            work->r[i] = 1;
            work->g[i] = 1;
            work->b[i] = 1;
            work->flared[i] = 0;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant826_D2218[i]);
        }
        work->scale = 0x1000;
        work->color.c.r = 0;
        work->color.c.g = 0;
        work->color.c.b = 0;
        work->color.c.glow = 0;
        work->count = 0;
        work->ambient = 0x800;
        work->phase = 0;
        work->pulse = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 3, &rot, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 7, &rot, -48);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 5, &rot, -48);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x1000) {
            work->ambient -= step << 6;
        } else {
            work->ambient = -0x1000;
        }
    } else if (work->phase == 3) {
        if (work->ambient < 0x800) {
            work->ambient += step << 6;
        } else {
            work->phase = 4;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->start && work->elapsed < work->config->end && work->count < 16) {
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        setVector(&work->positions[work->count], pos.vx + rot.vx / 2 * ((rand() - rand()) % 4096) / 4096,
                  pos.vy + rot.vy / 2 * ((rand() - rand()) % 4096) / 4096,
                  pos.vz + rot.vz / 2 * ((rand() - rand()) % 4096) / 4096);
        if (!(work->tick & 3)) {
            work->count++;
        }
    }
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
    pos.vx = m.t[0];
    pos.vy = m.t[1];
    pos.vz = m.t[2];

    if (work->color.word & 0xFFFFFF) {
        setPolyFT4(&tint);
        tint.tpage = work->texture[1] >> 16;
        tint.clut = work->texture[1];
        setRGB0(&tint, work->color.c.r >> 2, work->color.c.g >> 2, work->color.c.b >> 2);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                s32 u;

                setXY4(&tint, i * 40, j * (GRAPHICS_DEFAULT_HEIGHT / 8), (i + 1) * 40, j * (GRAPHICS_DEFAULT_HEIGHT / 8),
                       i * 40, (j + 1) * (GRAPHICS_DEFAULT_HEIGHT / 8), (i + 1) * 40, (j + 1) * (GRAPHICS_DEFAULT_HEIGHT / 8));
                u = i % 2 * 48;
                setUV4(&tint, u + work->tick % 96, 0, u + work->tick % 96 + 48, 0,
                       u + work->tick % 96, 96, u + work->tick % 96 + 48, 96);
                Graphics_SubmitDrawModePacket((u32 *)&tint, ot, 0x400, 1);
                u += 96;
                setUV4(&tint, u - work->tick * 2 % 96, 48, u - work->tick * 2 % 96 + 48, 48,
                       u - work->tick * 2 % 96, 144, u - work->tick * 2 % 96 + 48, 144);
                Graphics_SubmitDrawModePacket((u32 *)&tint, ot, 0x3FF, 1);
            }
        }
        setRGB0(&tint, work->color.c.r >> 1, work->color.c.g >> 1, work->color.c.b >> 1);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                s32 u;

                setXY4(&tint, i * 160, j * (GRAPHICS_DEFAULT_HEIGHT / 2), (i + 1) * 160, j * (GRAPHICS_DEFAULT_HEIGHT / 2),
                       i * 160, (j + 1) * (GRAPHICS_DEFAULT_HEIGHT / 2), (i + 1) * 160, (j + 1) * (GRAPHICS_DEFAULT_HEIGHT / 2));
                u = i % 2 * 48 + 96;
                setUV4(&tint, u - work->tick * 2 % 96, 64, u - work->tick * 2 % 96 + 48, 64,
                       u - work->tick * 2 % 96, 160, u - work->tick * 2 % 96 + 48, 160);
                Graphics_SubmitDrawModePacket((u32 *)&tint, ot, 0, 1);
            }
        }
    }

    if (work->phase == 0) {
        if (work->color.c.r < work->config->r - 5) {
            work->color.c.r += work->config->tint_step;
        } else {
            work->color.c.r = work->config->r;
        }
        if (work->color.c.g < work->config->g - 5) {
            work->color.c.g += work->config->tint_step;
        } else {
            work->color.c.g = work->config->g;
        }
        if (work->color.c.b < work->config->b - 5) {
            work->color.c.b += work->config->tint_step;
        } else {
            work->color.c.b = work->config->b;
        }
        if (work->color.c.r == work->config->r && work->color.c.g == work->config->g
            && work->color.c.b == work->config->b) {
            work->phase = 1;
        }
    } else if (work->phase == 1) {
        if (work->color.c.r > (work->config->r >> 2) + 5) {
            work->color.c.r -= work->config->tint_step;
        } else {
            work->color.c.r = work->config->r >> 2;
        }
        if (work->color.c.g > (work->config->g >> 2) + 5) {
            work->color.c.g -= work->config->tint_step;
        } else {
            work->color.c.g = work->config->g >> 2;
        }
        if (work->color.c.b > (work->config->b >> 2) + 5) {
            work->color.c.b -= work->config->tint_step;
        } else {
            work->color.c.b = work->config->b >> 2;
        }
        if (work->color.c.r == work->config->r >> 2 && work->color.c.g == work->config->g >> 2
            && work->color.c.b == work->config->b >> 2) {
            work->phase = 0;
        }
    } else {
        if (work->color.c.r > 5) {
            work->color.c.r -= work->config->tint_step;
        } else {
            work->color.c.r = 0;
        }
        if (work->color.c.g > 5) {
            work->color.c.g -= work->config->tint_step;
        } else {
            work->color.c.g = 0;
        }
        if (work->color.c.b > 5) {
            work->color.c.b -= work->config->tint_step;
        } else {
            work->color.c.b = 0;
        }
        if (work->color.c.r != 0 && work->color.c.g != 0 && work->color.c.b != 0) {
            work->phase = 3;
        }
    }

    if (work->color.c.glow != 0) {
        setPolyGT4(&quad);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        for (i = 0; i < 6; i++) {
            setRGB0(&quad, work->color.c.glow / (i + 1), work->color.c.glow / (i + 1), work->color.c.glow / (i + 1));
            setRGB1(&quad, work->color.c.glow / (i + 1), work->color.c.glow / (i + 1), work->color.c.glow / (i + 1));
            setRGB2(&quad, work->color.c.glow / (i + 1), work->color.c.glow / (i + 1), work->color.c.glow / (i + 1));
            setRGB3(&quad, 0, 0, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = (i % 2 * 2 - 1) * (work->tick * 128 & 0xFFF);
            scale.vx = work->scale / 6 * (i + 1);
            scale.vy = work->scale / 6 * (i + 1);
            scale.vz = work->scale / 6 * (i + 1);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setUV4(&quad, 0, 0xC0, 0x3F, 0xC0, 0, 0xFE, 0x3F, 0xFE);
            setVector(&vertices[0], -work->config->glow_size, -work->config->glow_size, 0);
            setVector(&vertices[1], 0, -work->config->glow_size, 0);
            setVector(&vertices[2], -work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                              (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
            }
            setVector(&vertices[0], work->config->glow_size, -work->config->glow_size, 0);
            setVector(&vertices[1], 0, -work->config->glow_size, 0);
            setVector(&vertices[2], work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                              (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
            }
            setVector(&vertices[0], -work->config->glow_size, work->config->glow_size, 0);
            setVector(&vertices[1], 0, work->config->glow_size, 0);
            setVector(&vertices[2], -work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                              (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
            }
            setVector(&vertices[0], work->config->glow_size, work->config->glow_size, 0);
            setVector(&vertices[1], 0, work->config->glow_size, 0);
            setVector(&vertices[2], work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                              (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
            }
        }
    }

    for (i = 0; i < work->count; i++) {
        if (work->r[i] != 0 || work->g[i] != 0 || work->b[i] != 0) {
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&rot, 0, 0, 0);
            setPolyGT4(&quad);
            quad.tpage = work->texture[1] >> 16;
            quad.clut = work->texture[1];
            GsSetLsMatrix(&light);
            RotTrans(&work->positions[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            saved = m;
            for (j = 0; j < 2; j++) {
                setVector(&scale, (j + 1) << 11, (j + 1) << 11, (j + 1) << 11);
                m = saved;
                sign = j % 2 * 2 - 1;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setVector(&vertices[0], sign * work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[1], 0, -work->config->shard_size, 0);
                setVector(&vertices[2], sign * work->config->shard_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                setRGB0(&quad, 0, 0, 0);
                setRGB1(&quad, 0, 0, 0);
                setRGB2(&quad, 0, 0, 0);
                setRGB3(&quad, work->r[i] / (j + 1), work->g[i] / (j + 1), work->b[i] / (j + 1));
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                                  (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
                setUV4(&quad, work->tick % 96, 0, work->tick % 96 + 48, 0, work->tick % 96, 96,
                       work->tick % 96 + 48, 96);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
                }
                setVector(&vertices[0], 0, -work->config->shard_size, 0);
                setVector(&vertices[1], -sign * work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[2], 0, 0, 0);
                setVector(&vertices[3], -sign * work->config->shard_size, 0, 0);
                setRGB0(&quad, 0, 0, 0);
                setRGB1(&quad, 0, 0, 0);
                setRGB2(&quad, work->r[i] / (j + 1), work->g[i] / (j + 1), work->b[i] / (j + 1));
                setRGB3(&quad, 0, 0, 0);
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                                  (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
                setUV4(&quad, work->tick % 96 + 48, 0, work->tick % 96 + 96, 0, work->tick % 96 + 48, 96,
                       work->tick % 96 + 96, 96);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
                }
                setVector(&vertices[0], -sign * work->config->shard_size, work->config->shard_size, 0);
                setVector(&vertices[1], 0, work->config->shard_size, 0);
                setVector(&vertices[2], -sign * work->config->shard_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                setRGB0(&quad, 0, 0, 0);
                setRGB1(&quad, 0, 0, 0);
                setRGB2(&quad, 0, 0, 0);
                setRGB3(&quad, work->r[i] / (j + 1), work->g[i] / (j + 1), work->b[i] / (j + 1));
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                                  (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
                setUV4(&quad, 96 - work->tick % 96, 0, 144 - work->tick % 96, 0, 96 - work->tick % 96, 96,
                       144 - work->tick % 96, 96);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
                }
                setVector(&vertices[0], 0, work->config->shard_size, 0);
                setVector(&vertices[1], sign * work->config->shard_size, work->config->shard_size, 0);
                setVector(&vertices[2], 0, 0, 0);
                setVector(&vertices[3], sign * work->config->shard_size, 0, 0);
                setRGB0(&quad, 0, 0, 0);
                setRGB1(&quad, 0, 0, 0);
                setRGB2(&quad, work->r[i] / (j + 1), work->g[i] / (j + 1), work->b[i] / (j + 1));
                setRGB3(&quad, 0, 0, 0);
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                                  (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
                setUV4(&quad, 144 - work->tick % 96, 0, 192 - work->tick % 96, 0, 144 - work->tick % 96, 96,
                       192 - work->tick % 96, 96);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
                }
            }
        }
        if (work->flared[i] == 0) {
            if (work->r[i] < work->config->shard_r - 31) {
                work->r[i] += work->config->shard_step;
            } else {
                work->r[i] = work->config->shard_r;
            }
            if (work->g[i] < work->config->shard_g - 31) {
                work->g[i] += work->config->shard_step;
            } else {
                work->g[i] = work->config->shard_g;
            }
            if (work->b[i] < work->config->shard_b - 31) {
                work->b[i] += work->config->shard_step;
            } else {
                work->b[i] = work->config->shard_b;
            }
            if (work->r[i] == work->config->shard_r && work->g[i] == work->config->shard_g
                && work->b[i] == work->config->shard_b) {
                work->flared[i] = 1;
            }
        }
        if (work->r[i] > 15) {
            work->r[i] -= work->config->shard_fade;
        } else {
            work->r[i] = 0;
        }
        if (work->g[i] > 15) {
            work->g[i] -= work->config->shard_fade;
        } else {
            work->g[i] = 0;
        }
        if (work->b[i] > 15) {
            work->b[i] -= work->config->shard_fade;
        } else {
            work->b[i] = 0;
        }
    }

    if (work->phase < 2) {
        if (work->color.c.glow < 0x57) {
            work->color.c.glow += work->config->glow_step;
        } else {
            work->color.c.glow = 0x60;
        }
    } else if (work->color.c.glow > 15) {
        work->color.c.glow -= work->config->glow_step;
    } else {
        work->color.c.glow = 0;
    }
    if (work->pulse == 0) {
        work->scale -= 0x80;
        if (work->scale < 0xC00) {
            work->pulse = 1;
        }
    } else if (work->pulse == 1) {
        work->scale += 0x80;
        if (work->scale > 0x1800) {
            work->pulse = 0;
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 4 && work->r[work->count - 1] == 0 && work->g[work->count - 1] == 0
        && work->b[work->count - 1] == 0) {
        return 2;
    }
    if (work->elapsed >= work->config->end && work->phase < 2) {
        work->phase = 2;
        return 1;
    }
    if (work->elapsed >= work->config->start && work->elapsed <= work->config->end) {
        return 4;
    }
    return 0;
}
