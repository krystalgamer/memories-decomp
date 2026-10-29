#include "../../types.h"
#include "variant838_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 838's entry: a comet forms on a model part as a spinning fan with
 * fading comet trails, then flies to the opposing side, shedding smoke puffs
 * and sparks. On arrival it bursts into a glow with flying debris, light
 * streaks and expanding sparks, flashes the screen, and sends a textured shock
 * ring out from the launch point while the ambient light dims and recovers. */

static const VECTOR D_8013E220 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model838State *work;
    MATRIX base, matrix;
    POLY_G3 fan_packet, *fan;
    POLY_FT4 quad_packet, *quad;
    POLY_GT4 strip_packet, *strip;
    POLY_F4 flash_packet;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    POLY_F4 *flash;
    s32 i, j, k;
    s32 offset;
    PSXLONG depth;
    s16 sign;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013E220;
    work = (Model838State *)context;
    fan = &fan_packet;
    quad = &quad_packet;
    strip = &strip_packet;
    flash = &flash_packet;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013E284[command];
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E230[i]);
        }
        work->glow_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->glow_clut = 0x3D28;
#else
        work->glow_clut = 0x3D20;
#endif
        setVector(&work->position, 0, 0, 0);
        setVector(&work->velocity, 0, 0, 0);
        for (i = 0; i < 32; i++) {
            setVector(&work->rim[i], work->config->radius * ccos(i << 7) / 4096,
                work->config->radius * csin(i << 7) / 4096, 0);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->trail_rotations[i], (rand() - rand()) % 4096, (rand() - rand()) % 4096,
                (rand() - rand()) % 4096);
            for (j = 0; j < 4; j++) {
                /* The hoisted offset keeps j * 48 as the induction value. */
                offset = 192 - j * 48;
                setVector(&work->trails[i][j], offset + (rand() - rand()) % 4096 / 128,
                    offset + (rand() - rand()) % 4096 / 128, offset + (rand() - rand()) % 4096 / 128);
            }
            work->trail_colors[i].rgb.r = 0x80;
            work->trail_colors[i].rgb.g = 0x80;
            work->trail_colors[i].rgb.b = 0x80;
            work->trail_lengths[i] = 0;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->puffs[i], 0, 0, 0);
            setVector(&work->puff_velocities[i], (rand() - rand()) % 4096 / 64, (rand() - rand()) % 4096 / 64,
                (rand() - rand()) % 4096 / 64);
            work->puff_colors[i].r = work->config->r >> 1;
            work->puff_colors[i].g = work->config->g >> 1;
            work->puff_colors[i].b = work->config->b >> 1;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->spark_velocities[i], (rand() - rand()) % 4096 * 48 / 4096,
                (rand() - rand()) % 4096 * 48 / 4096, (rand() - rand()) % 4096 * 48 / 4096);
            work->spark_colors[i].r = work->config->r >> 1;
            work->spark_colors[i].g = work->config->g >> 1;
            work->spark_colors[i].b = work->config->b >> 1;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->bursts[i], (rand() - rand()) % 4096 / 16, (rand() - rand()) % 4096 / 16,
                (rand() - rand()) % 4096 / 16);
            work->burst_colors[i].r = work->config->r;
            work->burst_colors[i].g = work->config->g;
            work->burst_colors[i].b = work->config->b;
            work->burst_scales[i] = 0;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->streaks[i], (rand() - rand()) % 4096 / 8, (rand() - rand()) % 4096 / 8,
                (rand() - rand()) % 4096 / 8);
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->debris[i], 0, 0, 0);
            setVector(&work->debris_velocities[i], (rand() - rand()) % 4096 / 256, (rand() - rand()) % 4096 / 256,
                (rand() - rand()) % 4096 / 256);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->ring_inner[i], ccos(i << 8) / 128, csin(i << 8) / 128, 0);
            setVector(&work->ring_outer[i], ccos(i << 8) * 96 / 4096, csin(i << 8) * 96 / 4096, direction << 5);
        }
        setVector(&work->origin, 0, 0, 0);
        work->core_color.rgb.r = 0x80;
        work->core_color.rgb.g = 0x80;
        work->core_color.rgb.b = 0x80;
        work->glow_color.rgb.r = work->config->r >> 2;
        work->glow_color.rgb.g = work->config->g >> 2;
        work->glow_color.rgb.b = work->config->b >> 2;
        work->unused_color.r = work->config->r >> 1;
        work->unused_color.g = work->config->g >> 1;
        work->unused_color.b = work->config->b >> 1;
        work->burst_color.rgb.r = work->config->r;
        work->burst_color.rgb.g = work->config->g;
        work->burst_color.rgb.b = work->config->b;
        work->flash.c.r = work->config->r >> 2;
        work->flash.c.g = work->config->g >> 2;
        work->flash.c.b = work->config->b >> 2;
        work->level = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->scale = 0;
        work->ring_scale = 0;
        work->trail_count = 0;
        work->puff_count = 0;
        work->spark_count = 0;
        work->burst_count = 0;
        work->flash.c.flash = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->delay) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rotation, -4);
    } else if (work->elapsed < work->config->launch_time) {
        Model_QueueCameraOrbitFromPreset(slot, 3, &rotation, -32);
    } else if (work->elapsed < work->config->arrive_time) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, -32);
    } else if (work->elapsed < work->config->end_time) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, 8);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rotation, -32);
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 4) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 5;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    setPolyG3(fan);
    setPolyFT4(quad);
    setPolyGT4(strip);
    if (work->elapsed < work->config->end_time) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
    } else if (work->phase == 2) {
        work->phase = 3;
    }
    if (work->elapsed >= work->config->launch_time && work->phase == 0) {
        setVector(&work->position, position.vx, position.vy, position.vz);
        setVector(&work->origin, position.vx, position.vy, position.vz);
        setVector(&work->velocity, -work->position.vx / 8, (-350 - work->position.vy) / 8,
            (direction * 450 - work->position.vz) / 8);
        work->phase = 1;
    }
    if (work->phase != 0) {
        setVector(&position, work->position.vx, work->position.vy, work->position.vz);
    }
    if (work->elapsed < work->config->end_time) {
        strip->tpage = work->texture[0] >> 16;
        strip->clut = work->texture[0];
        setUV4(strip, 64, 0, 111, 0, 64, 127, 111, 127);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        saved = matrix;
        if (work->phase >= 2) {
            setVector(&scale, 0x2000, 0x2000, 0x2000);
        } else {
            setVector(&scale, 0x1000, 0x1000, 0x1000);
        }
        for (i = 0; i < work->trail_count; i++) {
            matrix = saved;
            RotMatrix(&work->trail_rotations[i], &matrix);
            MulMatrix2((MATRIX *)&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            /* A while loop, not a for loop, gives the retail delay-slot fill
             * on the flag test; "+ 1 - j" keeps the j - 1 divisor form. */
            j = 0;
            while (j < work->trail_lengths[i] - 1) {
                setRGB0(strip, work->trail_colors[i].rgb.r / (work->trail_lengths[i] + 1 - j),
                    work->trail_colors[i].rgb.g / (work->trail_lengths[i] + 1 - j),
                    work->trail_colors[i].rgb.b / (work->trail_lengths[i] + 1 - j));
                setRGB1(strip, work->trail_colors[i].rgb.r / (work->trail_lengths[i] + 1 - j),
                    work->trail_colors[i].rgb.g / (work->trail_lengths[i] + 1 - j),
                    work->trail_colors[i].rgb.b / (work->trail_lengths[i] + 1 - j));
                setRGB2(strip, work->trail_colors[i].rgb.r / (work->trail_lengths[i] - j),
                    work->trail_colors[i].rgb.g / (work->trail_lengths[i] - j),
                    work->trail_colors[i].rgb.b / (work->trail_lengths[i] - j));
                setRGB3(strip, work->trail_colors[i].rgb.r / (work->trail_lengths[i] - j),
                    work->trail_colors[i].rgb.g / (work->trail_lengths[i] - j),
                    work->trail_colors[i].rgb.b / (work->trail_lengths[i] - j));
                setVector(&vertices[0], -8 / (work->trail_lengths[i] + 1 - j), 0, 0);
                setVector(&vertices[1], 8 / (work->trail_lengths[i] + 1 - j), 0, 0);
                setVector(&vertices[2], -8 / (work->trail_lengths[i] - j), 0, 0);
                setVector(&vertices[3], 8 / (work->trail_lengths[i] - j), 0, 0);
                addVector(&vertices[0], &work->trails[i][j]);
                addVector(&vertices[1], &work->trails[i][j]);
                addVector(&vertices[2], &work->trails[i][j + 1]);
                addVector(&vertices[3], &work->trails[i][j + 1]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&strip->x0, (PSXLONG *)&strip->x1, (PSXLONG *)&strip->x2, (PSXLONG *)&strip->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0) {
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)strip, ot, (u16)depth, 1);
                    }
                }
                j++;
            }
            work->trail_lengths[i]++;
            if (work->trail_lengths[i] >= 5) {
                work->trail_lengths[i] = 4;
                if (work->trail_colors[i].rgb.r >= 16) work->trail_colors[i].rgb.r -= 15;
                else work->trail_colors[i].rgb.r = 0;
                if (work->trail_colors[i].rgb.g >= 16) work->trail_colors[i].rgb.g -= 15;
                else work->trail_colors[i].rgb.g = 0;
                if (work->trail_colors[i].rgb.b >= 16) work->trail_colors[i].rgb.b -= 15;
                else work->trail_colors[i].rgb.b = 0;
                if (work->trail_colors[i].rgb.r == 0 && work->trail_colors[i].rgb.g == 0
                    && work->trail_colors[i].rgb.b == 0) {
                    work->trail_colors[i].rgb.r = 0x80;
                    work->trail_colors[i].rgb.g = 0x80;
                    work->trail_colors[i].rgb.b = 0x80;
                    work->trail_lengths[i] = 0;
                }
            }
        }
        if (work->core_color.word & 0xFFFFFF) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            saved = matrix;
            setRGB0(fan, 0, 0, 0);
            if (!(work->frame & 1)) {
                setVector(&scale, work->scale * 3 / 4, work->scale * 3 / 4, work->scale * 3 / 4);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                setRGB1(fan, work->core_color.rgb.r, work->core_color.rgb.g, work->core_color.rgb.b);
                setRGB2(fan, work->core_color.rgb.r, work->core_color.rgb.g, work->core_color.rgb.b);
            } else {
                setVector(&scale, work->scale, work->scale, work->scale);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                setRGB1(fan, work->core_color.rgb.r >> 2, work->core_color.rgb.g >> 2, work->core_color.rgb.b >> 2);
                setRGB2(fan, work->core_color.rgb.r >> 2, work->core_color.rgb.g >> 2, work->core_color.rgb.b >> 2);
            }
            setVector(&vertices[0], 0, 0, 0);
            for (i = 0; i < 32; i++) {
                depth = RotAverage3(&vertices[0], &work->rim[i], &work->rim[(i + 1) % 32],
                    (PSXLONG *)&fan->x0, (PSXLONG *)&fan->x1, (PSXLONG *)&fan->x2, &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (flag >= 0 && depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)fan, ot, (u16)depth, 2);
                }
            }
        }
        matrix = saved;
        setVector(&scale, work->scale, work->scale, work->scale);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        if (work->glow_color.word & 0xFFFFFF) {
            quad->tpage = work->glow_tpage;
            quad->clut = work->glow_clut;
            setUV4(quad, 64, 0, 127, 0, 64, 63, 127, 63);
            setRGB0(quad, work->glow_color.rgb.r, work->glow_color.rgb.g, work->glow_color.rgb.b);
            sign = 1;
            for (i = 0; i < 4; i++) {
                sign *= -1;
                setVector(&vertices[0], -sign * 96, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&vertices[1], 0, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&vertices[2], -sign * 96, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
        if (!(work->frame & 1)) {
            setRGB0(quad, 0x20, 0x20, 0x20);
            /* "* 128" keeps the full-word elapsed load that "<< 7" narrows. */
            setVector(&rotation, 0, 0, work->elapsed * 128 & 0xFFF);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            quad->tpage = work->texture[0] >> 16;
            quad->clut = work->texture[0];
            setUV4(quad, 0, 0, 63, 0, 0, 63, 63, 63);
            sign = 1;
            for (i = 0; i < 4; i++) {
                sign *= -1;
                setVector(&vertices[0], -sign * 96, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&vertices[1], 0, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&vertices[2], -sign * 96, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
        if (work->phase >= 2) {
            quad->tpage = work->glow_tpage;
            quad->clut = work->glow_clut;
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < work->puff_count; i++) {
                setRGB0(quad, work->puff_colors[i].r, work->puff_colors[i].g, work->puff_colors[i].b);
                j = rand() % 4;
                setUV4(quad, j % 2 << 5, (j / 2 << 5) + 64, (j % 2 << 5) + 31, (j / 2 << 5) + 64,
                    j % 2 << 5, (j / 2 << 5) + 95, (j % 2 << 5) + 31, (j / 2 << 5) + 95);
                setVector(&vertices[0], -8, -8, 0);
                setVector(&vertices[1], 8, -8, 0);
                setVector(&vertices[2], -8, 8, 0);
                setVector(&vertices[3], 8, 8, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->puffs[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0) {
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->puffs[i], &work->puff_velocities[i]);
                if (work->puff_colors[i].r >= 9) work->puff_colors[i].r -= 8;
                else work->puff_colors[i].r = 0;
                if (work->puff_colors[i].g >= 9) work->puff_colors[i].g -= 8;
                else work->puff_colors[i].g = 0;
                if (work->puff_colors[i].b >= 9) work->puff_colors[i].b -= 8;
                else work->puff_colors[i].b = 0;
                if (work->puff_colors[i].r == 0 && work->puff_colors[i].g == 0 && work->puff_colors[i].b == 0) {
                    setVector(&work->puffs[i], 0, 0, 0);
                    work->puff_colors[i].r = work->config->r >> 1;
                    work->puff_colors[i].g = work->config->g >> 1;
                    work->puff_colors[i].b = work->config->b >> 1;
                }
            }
            for (i = 0; i < work->spark_count; i++) {
                setRGB0(quad, work->spark_colors[i].r, work->spark_colors[i].g, work->spark_colors[i].b);
                setUV4(quad, 64, 0, 127, 0, 64, 63, 127, 63);
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    setVector(&vertices[0], -sign * 12, j - 2 >= 0 ? -12 : 12, 0);
                    setVector(&vertices[1], 0, j - 2 >= 0 ? -12 : 12, 0);
                    setVector(&vertices[2], -sign * 12, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&vertices[k], &work->sparks[i]);
                    }
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                        &interpolation, &flag);
                    if (work->elapsed >= work->config->arrive_time) {
                        depth -= 64;
                    }
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->sparks[i], &work->spark_velocities[i]);
                if (work->spark_colors[i].r >= 9) work->spark_colors[i].r -= 8;
                else work->spark_colors[i].r = 0;
                if (work->spark_colors[i].g >= 9) work->spark_colors[i].g -= 8;
                else work->spark_colors[i].g = 0;
                if (work->spark_colors[i].b >= 9) work->spark_colors[i].b -= 8;
                else work->spark_colors[i].b = 0;
                if (work->spark_colors[i].r == 0 && work->spark_colors[i].g == 0 && work->spark_colors[i].b == 0) {
                    setVector(&work->sparks[i], 0, 0, 0);
                    work->spark_colors[i].r = work->config->r >> 1;
                    work->spark_colors[i].g = work->config->g >> 1;
                    work->spark_colors[i].b = work->config->b >> 1;
                }
            }
            if (!(work->frame & 3)) {
                work->puff_count++;
                if (work->puff_count >= 33) {
                    work->puff_count = 32;
                }
            }
            if (!(work->frame & 7)) {
                work->spark_count++;
                if (work->spark_count >= 17) {
                    work->spark_count = 16;
                }
            }
        }
        if (work->phase >= 2) {
            work->scale = 0x2000;
        } else if (work->scale < 0x1000) {
            work->scale += 0x80;
        }
        if (work->phase == 1 && work->elapsed >= work->config->arrive_time) {
            addVector(&work->position, &work->velocity);
            if ((work->position.vz >= 0 ? work->position.vz : -work->position.vz) >= 450) {
                work->phase = 2;
            }
        }
        if (work->elapsed >= work->config->arrive_time && (work->flash.word & ~0xFF)) {
            strip->tpage = work->glow_tpage;
            strip->clut = work->glow_clut;
            setVector(&scale, work->ring_scale, work->ring_scale, work->ring_scale);
            setUV4(strip, 0, 128, 31, 128, 0, 255, 31, 255);
            setRGB0(strip, work->flash.c.r, work->flash.c.g, work->flash.c.b);
            setRGB1(strip, work->flash.c.r, work->flash.c.g, work->flash.c.b);
            setRGB2(strip, work->flash.c.r, work->flash.c.g, work->flash.c.b);
            setRGB3(strip, work->flash.c.r, work->flash.c.g, work->flash.c.b);
            GsSetLsMatrix(&base);
            RotTrans(&work->origin, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2((MATRIX *)&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < 16; i++) {
                depth = RotAverage4(&work->ring_inner[i], &work->ring_inner[(i + 1) % 16], &work->ring_outer[i],
                    &work->ring_outer[(i + 1) % 16], (PSXLONG *)&strip->x0, (PSXLONG *)&strip->x1,
                    (PSXLONG *)&strip->x2, (PSXLONG *)&strip->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)strip, ot, (u16)depth, 1);
                }
            }
            if (work->ring_scale < 0x4000) {
                work->ring_scale += 0x800;
            } else {
                work->scale += 0x100;
                if (work->flash.c.r >= 16) work->flash.c.r -= 15;
                else work->flash.c.r = 0;
                if (work->flash.c.g >= 16) work->flash.c.g -= 15;
                else work->flash.c.g = 0;
                if (work->flash.c.b >= 16) work->flash.c.b -= 15;
                else work->flash.c.b = 0;
            }
        }
        work->trail_count++;
        if (work->trail_count >= 33) {
            work->trail_count = 32;
        }
    }
    if (work->phase >= 3) {
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        saved = matrix;
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        sign = 1;
        if (work->burst_color.word & 0xFFFFFF) {
            quad->tpage = work->glow_tpage;
            quad->clut = work->glow_clut;
            setRGB0(quad, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            for (i = 0; i < 4; i++) {
                sign *= -1;
                setVector(&vertices[0], -sign * 192, i - 2 >= 0 ? -192 : 192, 0);
                setVector(&vertices[1], 0, i - 2 >= 0 ? -192 : 192, 0);
                setVector(&vertices[2], -sign * 192, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                setUV4(quad, 64, 0, 127, 0, 64, 63, 127, 63);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
            for (i = 0; i < 64; i++) {
                j = rand() % 4;
                setUV4(quad, j % 2 << 5, (j / 2 << 5) + 64, (j % 2 << 5) + 31, (j / 2 << 5) + 64,
                    j % 2 << 5, (j / 2 << 5) + 95, (j % 2 << 5) + 31, (j / 2 << 5) + 95);
                setVector(&vertices[0], -16, -16, 0);
                setVector(&vertices[1], 16, -16, 0);
                setVector(&vertices[2], -16, 16, 0);
                setVector(&vertices[3], 16, 16, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->debris[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0) {
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->debris[i], &work->debris_velocities[i]);
            }
            strip->tpage = work->texture[2] >> 16;
            strip->clut = work->texture[2];
            setRGB0(strip, 0, 0, 0);
            setRGB1(strip, 0, 0, 0);
            setRGB2(strip, work->burst_color.rgb.r >> 2, work->burst_color.rgb.g >> 2, work->burst_color.rgb.b >> 2);
            setRGB3(strip, work->burst_color.rgb.r >> 2, work->burst_color.rgb.g >> 2, work->burst_color.rgb.b >> 2);
            setUV4(strip, 0, 64, 63, 64, 0, 191, 63, 191);
            for (i = 0; i < 32; i++) {
                setVector(&vertices[0], work->streaks[i].vx - 6, work->streaks[i].vy, work->streaks[i].vz);
                setVector(&vertices[1], work->streaks[i].vx + 6, work->streaks[i].vy, work->streaks[i].vz);
                setVector(&vertices[2], -8, 0, 0);
                setVector(&vertices[3], 8, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&strip->x0, (PSXLONG *)&strip->x1, (PSXLONG *)&strip->x2, (PSXLONG *)&strip->x3,
                    &interpolation, &flag);
                if (work->elapsed >= work->config->arrive_time) {
                    depth -= 64;
                }
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)strip, ot, (u16)depth, 1);
                }
            }
            if (work->burst_color.rgb.r >= 5) work->burst_color.rgb.r -= 4;
            else work->burst_color.rgb.r = 0;
            if (work->burst_color.rgb.g >= 5) work->burst_color.rgb.g -= 4;
            else work->burst_color.rgb.g = 0;
            if (work->burst_color.rgb.b >= 5) work->burst_color.rgb.b -= 4;
            else work->burst_color.rgb.b = 0;
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->burst_colors[i].r != 0 || work->burst_colors[i].g != 0 || work->burst_colors[i].b != 0) {
                setVector(&scale, work->burst_scales[i], work->burst_scales[i], work->burst_scales[i]);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, &work->bursts[i]);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                quad->tpage = work->glow_tpage;
                quad->clut = work->glow_clut;
                setRGB0(quad, work->burst_colors[i].r, work->burst_colors[i].g, work->burst_colors[i].b);
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    setVector(&vertices[0], -sign * 64, j - 2 >= 0 ? -64 : 64, 0);
                    setVector(&vertices[1], 0, j - 2 >= 0 ? -64 : 64, 0);
                    setVector(&vertices[2], -sign * 64, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    setUV4(quad, 64, 0, 127, 0, 64, 63, 127, 63);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                        &interpolation, &flag);
                    if (work->elapsed >= work->config->arrive_time) {
                        depth -= 64;
                    }
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                    }
                }
                if (work->burst_scales[i] < 0x1000) {
                    work->burst_scales[i] += 0x400;
                } else {
                    work->burst_scales[i] = 0x1000;
                    if (work->burst_colors[i].r >= 32) work->burst_colors[i].r -= 31;
                    else work->burst_colors[i].r = 0;
                    if (work->burst_colors[i].g >= 32) work->burst_colors[i].g -= 31;
                    else work->burst_colors[i].g = 0;
                    if (work->burst_colors[i].b >= 32) work->burst_colors[i].b -= 31;
                    else work->burst_colors[i].b = 0;
                }
            }
        }
        work->burst_count += step;
        if (work->burst_count >= 33) {
            work->burst_count = 32;
        }
    }
    if (work->flash.c.flash != 0) {
        setPolyF4(flash);
        setRGB0(flash, work->flash.c.flash, work->flash.c.flash, work->flash.c.flash);
        setXY4(flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        Graphics_SubmitDrawModePacket((u32 *)flash, ot, 0, 1);
        if (work->flash.c.flash >= 9) work->flash.c.flash -= 8;
        else work->flash.c.flash = 0;
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 5 && !(work->burst_color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->phase != 3) {
        return (work->phase == 2) << 2;
    }
    work->flash.c.flash = 0xFF;
    work->phase = 4;
    return 1;
}
