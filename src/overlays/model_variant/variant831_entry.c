#include "../../types.h"
#include "variant831_entry.h"
#include "../../game/graphics_constants.h"

/* Header 831's entry: sparks gather on a model part while a spinning glow
 * grows there, the glow flies towards a second part, then the sparks burst
 * out with beams, a fading core and halo, and a white flash, while the
 * ambient light dims and recovers. */

#define SPARK_ABS(x) ((x) >= 0 ? (x) : -(x))

static const VECTOR D_8013D2C0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model831State *work;
    MATRIX base, matrix;
    POLY_GT4 ring;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR points[2];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 step;
    GsOT *ot;
    s32 i, j, angle, sign;
    PSXLONG depth;
    s16 side;
    s32 speed, k, value, m;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D2C0;
    work = (Model831State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &D_8013D340[command];
        for (i = 0; i < 64; i++) {
            value = rand();
            angle = value % 4096;
            setVector(&work->sparks[i], work->config->radius * ccos(angle) / 4096,
                work->config->radius * csin(angle) / 4096, work->config->radius * ccos(angle) / 4096);
            setVector(&work->velocities[i], -work->sparks[i].vx / work->config->gather_time,
                -work->sparks[i].vy / work->config->gather_time, -work->sparks[i].vz / work->config->gather_time);
            work->r[i] = 0;
            work->g[i] = 0;
            work->b[i] = 0;
        }
        work->halo_r = 1;
        work->halo_g = 1;
        work->halo_b = 1;
        work->core_tpage = 0xAE;
        work->position.vx = 0;
        work->position.vy = 0;
        work->position.vz = 0;
        work->core_clut = 0x3D20;
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D2D0[i]);
        }
        work->glow = 1;
        work->level = 2048;
        work->flash = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->count = 0;
        work->scale = 4096;
        work->core_r = 1;
        work->core_g = 1;
        work->core_b = 1;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 5) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 6;
            work->level = 2048;
        }
    }
    value = work->level;
    Model_SetSlotAmbient(2, value, value, value);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed <= work->config->gather_end) {
        i = 1;
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        work->position.vx = position.vx;
        work->position.vy = position.vy;
        work->position.vz = position.vz;
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->target_part), &matrix);
        setVector(&points[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        for (; i < 2; i++) {
            setVector(&points[i], points[0].vx + (position.vx - points[0].vx) / 2 * i,
                points[0].vy + (position.vy - points[0].vy) / 2 * i, points[0].vz + (position.vz - points[0].vz) / 2 * i);
        }
    } else {
        position.vx = work->position.vx;
        position.vy = work->position.vy;
        position.vz = work->position.vz;
        work->scale = 0x4000;
        if (work->phase == 4) {
            for (i = 0; i < 64; i++) {
                setVector(&work->sparks[i], 0, 0, 0);
                setVector(&work->velocities[i], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096);
            }
            work->phase = 5;
        }
    }
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    saved = matrix;
    if (work->glow != 0) {
        setPolyGT4(&ring);
        ring.tpage = work->texture[3] >> 16;
        ring.clut = work->texture[3];
        setUV4(&ring, 64, 0, 127, 0, 64, 62, 127, 62);
        setRGB0(&ring, 0, 0, 0);
        setRGB1(&ring, 0, 0, 0);
        setRGB2(&ring, 0, 0, 0);
        for (i = 0; i < 4; i++) {
            /* Strength-reduced into its own register, initialised after the
             * hoisted address constants as in the original. */
            sign = i * 2 - 1;
            setRGB3(&ring, work->glow / (i + 1), work->glow / (i + 1), work->glow / (i + 1));
            scale.vx = (work->scale >> 2) * (i + 1);
            scale.vy = (work->scale >> 2) * (i + 1);
            scale.vz = (work->scale >> 2) * (i + 1);
            /* Dividing in each arm keeps (i + 1) inside the arms; loading the
             * constant into speed itself puts it in the quotient's register. */
            if (work->phase >= 4) {
                speed = 64;
                speed /= i + 1;
            } else {
                speed = 256;
                speed /= i + 1;
            }
            setVector(&rotation, 0, 0, (speed = sign * (work->frame * speed)) & 0xFFF);
            /* Dead store: deleted by flow, but the branch survives to register
             * allocation and raises sign's priority so it keeps s4. */
            if (sign != 0) {
                speed = 0;
            }
            matrix = saved;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&vertices[0], -work->config->glow_size, -work->config->glow_size, 0);
            setVector(&vertices[1], 0, -work->config->glow_size, 0);
            setVector(&vertices[2], -work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
            setVector(&vertices[0], work->config->glow_size, -work->config->glow_size, 0);
            setVector(&vertices[1], 0, -work->config->glow_size, 0);
            setVector(&vertices[2], work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
            setVector(&vertices[0], -work->config->glow_size, work->config->glow_size, 0);
            setVector(&vertices[1], 0, work->config->glow_size, 0);
            setVector(&vertices[2], -work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
            setVector(&vertices[0], work->config->glow_size, work->config->glow_size, 0);
            setVector(&vertices[1], 0, work->config->glow_size, 0);
            setVector(&vertices[2], work->config->glow_size, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
        }
        if (work->phase < 4) {
            if (work->glow < 248) {
                work->glow += work->config->glow_fade;
            } else {
                work->glow = 255;
            }
        } else if (work->glow >= 10) {
            work->glow -= work->config->glow_fade;
        } else {
            work->glow = 0;
        }
    }
    if (work->phase >= 5) {
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        setVector(&rotation, 0, 0, work->frame * 64 & 0xFFF);
        matrix = saved;
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        if (work->core_r != 0 || work->core_g != 0 || work->core_b != 0) {
            for (i = 0; i < 64; i++) {
                setPolyFT4(&quad);
                quad.tpage = work->texture[2] >> 16;
                quad.clut = work->texture[2];
                setRGB0(&quad, work->core_r, work->core_g, work->core_b);
                setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
                addVector(&vertices[0], &work->sparks[i]);
                addVector(&vertices[1], &work->sparks[i]);
                addVector(&vertices[2], &work->sparks[i]);
                addVector(&vertices[3], &work->sparks[i]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                j = rand() % 3;
                setUV4(&quad, (j << 5) + 32, 64, (j << 5) + 63, 64, (j << 5) + 32, 95, (j << 5) + 63, 95);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                addVector(&work->sparks[i], &work->velocities[i]);
            }
        }
    }
    if (work->core_r != 0 || work->core_g != 0 || work->core_b != 0) {
        scale.vx = work->scale;
        scale.vy = work->scale;
        scale.vz = work->scale;
        setPolyFT4(&quad);
        quad.tpage = work->core_tpage;
        quad.clut = work->core_clut;
        setUV4(&quad, 64, 0, 127, 0, 64, 63, 127, 63);
        setRGB0(&quad, work->core_r >> 1, work->core_g >> 1, work->core_b >> 1);
        setVector(&vertices[0], -work->config->halo_size, -work->config->halo_size, 0);
        setVector(&vertices[1], 0, -work->config->halo_size, 0);
        setVector(&vertices[2], -work->config->halo_size, 0, 0);
        setVector(&vertices[3], 0, 0, 0);
        for (i = 0; i < 4; i++) {
            if (work->phase >= 4) {
                setVector(&rotation, 0, 0, i * 1024);
            } else {
                setVector(&rotation, 0, 0, (i * 1024 + work->frame * 64) & 0xFFF);
            }
            matrix = saved;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            }
        }
        if (work->phase >= 4) {
            if (work->core_r >= 6) work->core_r -= work->config->core_fade;
            else work->core_r = 0;
            if (work->core_g >= 6) work->core_g -= work->config->core_fade;
            else work->core_g = 0;
            if (work->core_b >= 6) work->core_b -= work->config->core_fade;
            else work->core_b = 0;
        } else {
            if (work->core_r < work->config->r - 15) work->core_r += work->config->glow_fade;
            else work->core_r = work->config->r;
            if (work->core_g < work->config->g - 15) work->core_g += work->config->glow_fade;
            else work->core_g = work->config->g;
            if (work->core_b < work->config->b - 15) work->core_b += work->config->glow_fade;
            else work->core_b = work->config->b;
        }
    }
    if (work->halo_r != 0 || work->halo_g != 0 || work->halo_b != 0) {
        setPolyFT4(&quad);
        quad.tpage = work->core_tpage;
        quad.clut = work->core_clut;
        setUV4(&quad, 64, 0, 127, 0, 64, 63, 127, 63);
        setRGB0(&quad, work->halo_r, work->halo_g, work->halo_b);
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        setVector(&rotation, 0, 0, 0);
        for (i = 0; i < 2; i++) {
            GsSetLsMatrix(&base);
            RotTrans(&points[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            side = 1;
            for (j = 0; j < 4; j++) {
                side *= -1;
                setVector(&vertices[0], -side * work->config->halo_size,
                    (j - 2 < 0 ? 1 : -1) * work->config->halo_size, 0);
                setVector(&vertices[1], 0, (j - 2 < 0 ? 1 : -1) * work->config->halo_size, 0);
                setVector(&vertices[2], -side * work->config->halo_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
            }
        }
        if (work->phase >= 4) {
            work->halo_r = 0;
            work->halo_g = 0;
            work->halo_b = 0;
        } else {
            if (work->halo_r < work->config->halo_r - 1) work->halo_r++;
            else work->halo_r = work->config->halo_r;
            if (work->halo_g < work->config->halo_g - 1) work->halo_g++;
            else work->halo_g = work->config->halo_g;
            if (work->halo_b < work->config->halo_b - 1) work->halo_b++;
            else work->halo_b = work->config->halo_b;
        }
    }
    if (work->elapsed <= work->config->burst_time) {
        {
            for (i = 0; i < work->count; i++) {
                if (SPARK_ABS(work->sparks[i].vx) > 0 && SPARK_ABS(work->sparks[i].vy) > 0 && SPARK_ABS(work->sparks[i].vz) > 0) {
                    setPolyFT4(&quad);
                    quad.tpage = work->texture[2] >> 16;
                    quad.clut = work->texture[2];
                    setRGB0(&quad, work->r[i], work->g[i], work->b[i]);
                    setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
                    addVector(&vertices[0], &work->sparks[i]);
                    addVector(&vertices[1], &work->sparks[i]);
                    addVector(&vertices[2], &work->sparks[i]);
                    addVector(&vertices[3], &work->sparks[i]);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    j = rand() % 3;
                    setUV4(&quad, (j << 5) + 32, 64, (j << 5) + 63, 64, (j << 5) + 32, 95, (j << 5) + 63, 95);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                    }
                    addVector(&work->sparks[i], &work->velocities[i]);
                    if (SPARK_ABS(work->sparks[i].vx) < 16 || SPARK_ABS(work->sparks[i].vy) < 16 || SPARK_ABS(work->sparks[i].vz) < 16) {
                        setVector(&work->sparks[i], 0, 0, 0);
                    }
                    if (work->r[i] < work->config->r - 15) work->r[i] += work->config->spark_fade;
                    else work->r[i] = work->config->r;
                    if (work->g[i] < work->config->g - 15) work->g[i] += work->config->spark_fade;
                    else work->g[i] = work->config->g;
                    if (work->b[i] < work->config->b - 15) work->b[i] += work->config->spark_fade;
                    else work->b[i] = work->config->b;
                }
            }
        }
        if (work->elapsed < work->config->spawn_end) {
            if (++work->count > 64) {
                work->count = 64;
            }
        }
    } else if (work->phase == 0) {
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i], (rand() - rand()) % 4096, (rand() - rand()) % 4096,
                (rand() - rand()) % 4096);
            angle = rand() % 2 * 2 - 1;
            k = rand() % 2 * 2 - 1;
            /* Separate from sign so the last direction stays in v0. */
            m = rand();
            setVector(&work->velocities[i], angle * work->config->burst_speed, k * work->config->burst_speed,
                (m % 2 * 2 - 1) * work->config->burst_speed);
        }
        work->flash = 255;
        work->phase = 1;
    }
    if (work->elapsed >= work->config->burst_time && work->elapsed <= work->config->gather_end && work->phase != 0) {
        work->phase = 2;
        work->scale = 0x4000;
        setPolyGT4(&ring);
        ring.tpage = work->texture[1] >> 16;
        ring.clut = work->texture[1];
        setUV4(&ring, 0, 96, 31, 96, 0, 223, 31, 223);
        setRGB0(&ring, 0, 0, 0);
        setRGB1(&ring, 0, 0, 0);
        setVector(&vertices[0], -work->config->beam_top, -work->config->beam_height, 0);
        setVector(&vertices[1], work->config->beam_top, -work->config->beam_height, 0);
        setVector(&vertices[2], -work->config->beam_base, 0, 0);
        setVector(&vertices[3], work->config->beam_base, 0, 0);
        for (i = 0; i < 64; i++) {
            setRGB2(&ring, work->config->r >> 1, work->config->g >> 1, work->config->b >> 1);
            setRGB3(&ring, work->config->r >> 1, work->config->g >> 1, work->config->b >> 1);
            scale.vx = 0x2000;
            scale.vy = 0x2000;
            scale.vz = 0x2000;
            matrix = saved;
            RotMatrix(&work->sparks[i], &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
            setRGB2(&ring, work->config->r, work->config->g, work->config->b);
            setRGB3(&ring, work->config->r, work->config->g, work->config->b);
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            matrix = saved;
            RotMatrix(&work->sparks[i], &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
            addVector(&work->sparks[i], &work->velocities[i]);
        }
    }
    if (work->flash != 0) {
        setPolyF4(&flash);
        setRGB0(&flash, work->flash, work->flash, work->flash);
        setXY4(&flash, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
            GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 0, 1);
        if (work->flash >= 32) {
            work->flash -= work->config->spark_fade;
        } else {
            work->flash = 0;
        }
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 6 && work->core_r == 0 && work->core_g == 0 && work->core_b == 0) {
        return 2;
    }
    if (work->phase == 3) {
        work->phase = 4;
        work->flash = 255;
        return 1;
    }
    if (work->phase == 2) {
        work->phase = 3;
        return 4;
    }
    return 0;
}
