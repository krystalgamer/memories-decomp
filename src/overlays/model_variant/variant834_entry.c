#include "../../types.h"
#include "variant834_entry.h"
#include "../../game/screen_projection.h"

/* Header 834's entry: orbs well up from a model part and drift to the slot
 * while a beam rises in front of it; the beam then flies to the opposing side
 * leaving a fading trail and a textured ring, and bursts into a growing glow
 * with flares and sparks while the ambient light dims and recovers. */

static const VECTOR D_8013D610 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model834State *work;
    MATRIX base, matrix;
    POLY_FT4 quad_packet, *quad;
    POLY_GT4 ring_packet, *ring;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j, spread, age;
    PSXLONG depth;
    s16 sign;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D610;
    work = (Model834State *)context;
    quad = &quad_packet;
    ring = &ring_packet;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D674[command];
        work->position.vx = 0;
        work->position.vy = -512;
        work->position.vz = 0;
        work->rise.vx = 0;
        work->flight.vx = 0;
        work->flight.vy = 0;
        work->flight.vz = 0;
        work->rotation.vx = 0;
        work->rotation.vy = 0;
        work->rotation.vz = 0;
        work->rise.vz = -direction * 256 / 48;
        work->rise.vy = (work->position.vx + 464) / 48;
        for (i = 0; i < 16; i++) {
            setVector(&work->top[i], work->config->top_radius * ccos(i << 8) / 4096,
                work->config->beam_height + 64, work->config->top_radius * csin(i << 8) / 4096);
            setVector(&work->base[i], work->config->base_radius * ccos(i << 8) / 4096,
                work->config->beam_height - work->config->base_drop, work->config->base_radius * csin(i << 8) / 4096);
            setVector(&work->burst[i], work->config->burst_radius * ccos(i << 8) / 4096,
                work->config->burst_radius * csin(i << 8) / 4096, 0);
            setVector(&work->flare[i], work->config->flare_radius * ccos(i << 8) / 4096,
                work->config->flare_radius * csin(i << 8) / 4096, -direction * work->config->flare_depth);
        }
        for (i = 0; i < 8; i++) {
            setVector(&work->trail[i], 0, 0, 0);
        }
        work->ring_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->ring_clut = 0x3D28;
#else
        work->ring_clut = 0x3D20;
#endif
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D620[i]);
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->orbs[i], 0, 0, 0);
            setVector(&work->orb_velocities[i], 0, 0, 0);
            work->states[i] = 0;
            work->colors[i].r = work->config->r;
            work->colors[i].g = work->config->g;
            work->colors[i].b = work->config->b;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            /* The assignment keeps GCC from folding -direction into the 24. */
            setVector(&work->spark_velocities[i], (rand() - rand()) % 4096 * 24 / 4096,
                (rand() - rand()) % 4096 * 24 / 4096, -direction * (spread = rand() % 4096 * 24) / 4096);
        }
        work->beam_top.rgb.r = 1;
        work->beam_top.rgb.g = 1;
        work->beam_top.rgb.b = 1;
        work->beam_base.rgb.r = 1;
        work->beam_base.rgb.g = 1;
        work->beam_base.rgb.b = 1;
        work->burst_color.rgb.r = work->config->burst_r;
        work->burst_color.rgb.g = work->config->burst_g;
        work->burst_color.rgb.b = work->config->burst_b;
        work->trail_count = 0;
        work->orb_count = 0;
        work->level = 2048;
        work->phase = 0;
        work->unused = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->scale = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->orb_time) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rotation, -16);
    } else if (work->elapsed < work->config->fly_time) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, -80);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rotation, -16);
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 3) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 4;
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
    setPolyGT4(ring);
    setPolyFT4(quad);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    for (i = 0; i < work->orb_count; i++) {
        if (work->colors[i].r != 0 || work->colors[i].g != 0 || work->colors[i].b != 0) {
            if (work->states[i] == 0) {
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
                setVector(&work->orbs[i], matrix.t[0], matrix.t[1], matrix.t[2]);
                setVector(&work->orb_velocities[i], -work->orbs[i].vx / 48,
                    (-(rand() % 4096 << 9) / 4096 - work->orbs[i].vy) / 48, -work->orbs[i].vz / 48);
                work->states[i] = 1;
            } else if (work->states[i] == 1) {
                setRGB0(quad, work->colors[i].r, work->colors[i].g, work->colors[i].b);
                quad->tpage = work->texture[0] >> 16;
                quad->clut = work->texture[0];
                setUV4(quad, 96, 0, 159, 0, 96, 63, 159, 63);
                setVector(&vertices[0], -work->config->orb_size, -work->config->orb_size, 0);
                setVector(&vertices[1], work->config->orb_size, -work->config->orb_size, 0);
                setVector(&vertices[2], -work->config->orb_size, work->config->orb_size, 0);
                setVector(&vertices[3], work->config->orb_size, work->config->orb_size, 0);
                /* "* 32" keeps the full-word frame load that "<< 5" narrows. */
                setVector(&rotation, 0, 0, work->frame * 32);
                GsSetLsMatrix(&base);
                RotTrans(&work->orbs[i], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                setSemiTrans(quad, 1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
                if (!(i & 3)) {
                    j = rand() % 4;
                    quad->tpage = work->texture[1] >> 16;
                    quad->clut = work->texture[1];
                    setUV4(quad, (j % 2 << 5) + 160, 0, (j % 2 << 5) + 191, 0,
                        (j % 2 << 5) + 160, 31, (j % 2 << 5) + 191, 31);
                    setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                        &interpolation, &flag);
                    setSemiTrans(quad, 1);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(quad, ot, (u16)depth);
                    }
                }
                addVector(&work->orbs[i], &work->orb_velocities[i]);
                if (-direction * work->orbs[i].vz < 0) {
                    if (work->phase == 0) {
                        work->phase = 1;
                    }
                    if (work->colors[i].r >= 16) work->colors[i].r -= 15;
                    else work->colors[i].r = 0;
                    if (work->colors[i].g >= 16) work->colors[i].g -= 15;
                    else work->colors[i].g = 0;
                    if (work->colors[i].b >= 16) work->colors[i].b -= 15;
                    else work->colors[i].b = 0;
                    if (work->colors[i].r == 0 && work->colors[i].g == 0 && work->colors[i].b == 0) {
                        if (work->elapsed < work->config->orb_time) {
                            work->colors[i].r = work->config->r;
                            work->colors[i].g = work->config->g;
                            work->colors[i].b = work->config->b;
                            work->states[i] = 0;
                        }
                    }
                }
            }
        }
    }
    if (work->elapsed < work->config->spawn_time) {
        if (++work->orb_count > 64) {
            work->orb_count = 64;
        }
    }
    /* The original tests the colour channel addresses, which are never null. */
    if ((&work->beam_top.rgb.r != NULL || &work->beam_top.rgb.g != NULL || &work->beam_top.rgb.b != NULL)
        && (&work->beam_base.rgb.r != NULL || &work->beam_base.rgb.g != NULL || &work->beam_base.rgb.b != NULL)) {
        if (work->phase != 0) {
            ring->tpage = work->texture[2] >> 16;
            ring->clut = work->texture[2];
            setRGB0(ring, work->beam_top.rgb.r, work->beam_top.rgb.g, work->beam_top.rgb.b);
            setRGB1(ring, work->beam_top.rgb.r, work->beam_top.rgb.g, work->beam_top.rgb.b);
            setRGB2(ring, work->beam_base.rgb.r, work->beam_base.rgb.g, work->beam_base.rgb.b);
            setRGB3(ring, work->beam_base.rgb.r, work->beam_base.rgb.g, work->beam_base.rgb.b);
            GsSetLsMatrix(&base);
            RotTrans(&work->position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&work->rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&vertices[0], -work->config->beam_width, 0, 0);
            setVector(&vertices[1], work->config->beam_width, 0, 0);
            setVector(&vertices[2], -work->config->beam_width, work->config->beam_height, 0);
            setVector(&vertices[3], work->config->beam_width, work->config->beam_height, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ring->x0, (PSXLONG *)&ring->x1, (PSXLONG *)&ring->x2, (PSXLONG *)&ring->x3,
                &interpolation, &flag);
            setUV4(ring, 0, 0, 47, 0, 0, 127, 47, 127);
            if ((work->beam_top.word & 0xFFFFFF) == 0x808080
                && (work->beam_base.word & 0xFFFFFF) == (work->beam_top.word & 0xFFFFFF)) {
                setSemiTrans(ring, 0);
            } else {
                setSemiTrans(ring, 1);
            }
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(ring, ot, (u16)depth);
            }
            if (work->phase >= 2) {
                if (work->beam_top.rgb.r >= 16) work->beam_top.rgb.r -= 15;
                else work->beam_top.rgb.r = 0;
                if (work->beam_top.rgb.g >= 16) work->beam_top.rgb.g -= 15;
                else work->beam_top.rgb.g = 0;
                if (work->beam_top.rgb.b >= 16) work->beam_top.rgb.b -= 15;
                else work->beam_top.rgb.b = 0;
                if (work->beam_base.rgb.r >= 16) work->beam_base.rgb.r -= 15;
                else work->beam_base.rgb.r = 0;
                if (work->beam_base.rgb.g >= 16) work->beam_base.rgb.g -= 15;
                else work->beam_base.rgb.g = 0;
                if (work->beam_base.rgb.b >= 16) work->beam_base.rgb.b -= 15;
                else work->beam_base.rgb.b = 0;
            } else {
                if (work->beam_top.rgb.r < 120) work->beam_top.rgb.r += 8;
                else work->beam_top.rgb.r = 128;
                if (work->beam_top.rgb.g < 120) work->beam_top.rgb.g += 8;
                else work->beam_top.rgb.g = 128;
                if (work->beam_top.rgb.b < 120) work->beam_top.rgb.b += 8;
                else work->beam_top.rgb.b = 128;
                if (work->beam_base.rgb.r < 126) work->beam_base.rgb.r += 2;
                else work->beam_base.rgb.r = 128;
                if (work->beam_base.rgb.g < 126) work->beam_base.rgb.g += 2;
                else work->beam_base.rgb.g = 128;
                if (work->beam_base.rgb.b < 126) work->beam_base.rgb.b += 2;
                else work->beam_base.rgb.b = 128;
            }
            if (work->elapsed >= work->config->launch_time && work->elapsed < work->config->fly_time) {
                if (work->rotation.vz > -3072) {
                    if (work->position.vy < -464) {
                        work->position.vy += work->rise.vy;
                    }
                    work->position.vz += work->rise.vz;
                    work->rotation.vz -= 64;
                } else {
                    work->flight.vx = 0;
                    work->flight.vy = (-350 - work->position.vy) / 4;
                    work->flight.vz = (direction * 450 - work->position.vz) / 4;
                }
            }
            if (work->elapsed >= work->config->fly_time) {
                if (work->phase == 1) {
                    ring->tpage = work->ring_tpage;
                    ring->clut = work->ring_clut;
                    setRGB0(ring, 64, 64, 64);
                    setRGB1(ring, 64, 64, 64);
                    setRGB2(ring, 32, 32, 32);
                    setRGB3(ring, 32, 32, 32);
                    for (i = 0; i < 16; i++) {
                        depth = RotAverage4(&work->top[i], &work->top[(i + 1) % 16], &work->base[i], &work->base[(i + 1) % 16],
                            (PSXLONG *)&ring->x0, (PSXLONG *)&ring->x1, (PSXLONG *)&ring->x2, (PSXLONG *)&ring->x3,
                            &interpolation, &flag);
                        setUV4(ring, 0, 128, 31, 128, 0, 255, 31, 255);
                        setSemiTrans(ring, 1);
                        if (depth >= 0 && flag >= 0) {
                            GsSortPoly(ring, ot, (u16)depth);
                        }
                    }
                    copyVector(&work->trail[work->trail_count], &work->position);
                    work->trail_count++;
                    addVector(&work->position, &work->flight);
                    if (direction > 0) {
                        if (work->position.vz >= 450 - work->config->beam_height) {
                            work->phase = 2;
                        }
                    } else if (work->position.vz <= work->config->beam_height - 450) {
                        work->phase = 2;
                    }
                }
            }
            if (work->trail_count != 0) {
                for (i = 0; i < work->trail_count; i++) {
                    age = i - 1;
                    ring->tpage = work->texture[2] >> 16;
                    ring->clut = work->texture[2];
                    setUV4(ring, 0, 0, 47, 0, 0, 127, 47, 127);
                    setRGB0(ring, work->beam_top.rgb.r / (work->trail_count - age),
                        work->beam_top.rgb.g / (work->trail_count - age), work->beam_top.rgb.b / (work->trail_count - age));
                    setRGB1(ring, work->beam_top.rgb.r / (work->trail_count - age),
                        work->beam_top.rgb.g / (work->trail_count - age), work->beam_top.rgb.b / (work->trail_count - age));
                    setRGB2(ring, work->beam_base.rgb.r / (work->trail_count - age),
                        work->beam_base.rgb.g / (work->trail_count - age), work->beam_base.rgb.b / (work->trail_count - age));
                    setRGB3(ring, work->beam_base.rgb.r / (work->trail_count - age),
                        work->beam_base.rgb.g / (work->trail_count - age), work->beam_base.rgb.b / (work->trail_count - age));
                    GsSetLsMatrix(&base);
                    RotTrans(&work->trail[i], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&work->rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&ring->x0, (PSXLONG *)&ring->x1, (PSXLONG *)&ring->x2, (PSXLONG *)&ring->x3,
                        &interpolation, &flag);
                    setSemiTrans(ring, 1);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(ring, ot, (u16)depth);
                    }
                }
            }
        }
    }
    if (work->phase >= 2 && (work->burst_color.word & 0xFFFFFF)) {
        setVector(&rotation, 0, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&scale, work->scale + 4096, work->scale + 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        saved = matrix;
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        ring->tpage = work->ring_tpage;
        ring->clut = work->ring_clut;
        setRGB0(ring, work->burst_color.rgb.r >> 2, work->burst_color.rgb.g >> 2, work->burst_color.rgb.b >> 2);
        setRGB1(ring, work->burst_color.rgb.r >> 2, work->burst_color.rgb.g >> 2, work->burst_color.rgb.b >> 2);
        setRGB2(ring, work->burst_color.rgb.r >> 3, work->burst_color.rgb.g >> 3, work->burst_color.rgb.b >> 3);
        setRGB3(ring, work->burst_color.rgb.r >> 3, work->burst_color.rgb.g >> 3, work->burst_color.rgb.b >> 3);
        for (i = 0; i < 16; i++) {
            depth = RotAverage4(&work->burst[i], &work->burst[(i + 1) % 16], &work->flare[i], &work->flare[(i + 1) % 16],
                (PSXLONG *)&ring->x0, (PSXLONG *)&ring->x1, (PSXLONG *)&ring->x2, (PSXLONG *)&ring->x3,
                &interpolation, &flag);
            setUV4(ring, 0, 128, 31, 128, 0, 255, 31, 255);
            setSemiTrans(ring, 1);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(ring, ot, (u16)depth);
            }
        }
        matrix = saved;
        setVector(&scale, (work->scale >> 1) + 4096, (work->scale >> 1) + 4096, (work->scale >> 2) + 4096);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setRGB0(ring, work->burst_color.rgb.r >> 1, work->burst_color.rgb.g >> 1, work->burst_color.rgb.b >> 1);
        setRGB1(ring, work->burst_color.rgb.r >> 1, work->burst_color.rgb.g >> 1, work->burst_color.rgb.b >> 1);
        setRGB2(ring, work->burst_color.rgb.r >> 2, work->burst_color.rgb.g >> 2, work->burst_color.rgb.b >> 2);
        setRGB3(ring, work->burst_color.rgb.r >> 2, work->burst_color.rgb.g >> 2, work->burst_color.rgb.b >> 2);
        for (i = 0; i < 16; i++) {
            depth = RotAverage4(&work->burst[i], &work->burst[(i + 1) % 16], &work->flare[i], &work->flare[(i + 1) % 16],
                (PSXLONG *)&ring->x0, (PSXLONG *)&ring->x1, (PSXLONG *)&ring->x2, (PSXLONG *)&ring->x3,
                &interpolation, &flag);
            setUV4(ring, 0, 128, 31, 128, 0, 255, 31, 255);
            setSemiTrans(ring, 1);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(ring, ot, (u16)depth);
            }
        }
        matrix = saved;
        setVector(&scale, (work->scale >> 3) + 4096, (work->scale >> 3) + 4096, (work->scale >> 1) + 4096);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setRGB0(ring, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
        setRGB1(ring, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
        setRGB2(ring, work->burst_color.rgb.r >> 1, work->burst_color.rgb.g >> 1, work->burst_color.rgb.b >> 1);
        setRGB3(ring, work->burst_color.rgb.r >> 1, work->burst_color.rgb.g >> 1, work->burst_color.rgb.b >> 1);
        for (i = 0; i < 16; i++) {
            depth = RotAverage4(&work->burst[i], &work->burst[(i + 1) % 16], &work->flare[i], &work->flare[(i + 1) % 16],
                (PSXLONG *)&ring->x0, (PSXLONG *)&ring->x1, (PSXLONG *)&ring->x2, (PSXLONG *)&ring->x3,
                &interpolation, &flag);
            setUV4(ring, 0, 128, 31, 128, 0, 255, 31, 255);
            setSemiTrans(ring, 1);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(ring, ot, (u16)depth);
            }
        }
        setVector(&scale, work->scale + 4096, work->scale + 4096, 4096);
        quad->tpage = work->ring_tpage;
        quad->clut = work->ring_clut;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        saved = matrix;
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        sign = 1;
        for (i = 0; i < 4; i++) {
            /* "*= -1" reproduces the re-extension that "= -sign" drops. */
            sign *= -1;
            setVector(&vertices[0], -sign * 32, i - 2 < 0 ? 32 : -32, 0);
            setVector(&vertices[1], 0, i - 2 >= 0 ? -32 : 32, 0);
            setVector(&vertices[2], -sign * 32, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            setUV4(quad, 64, 0, 127, 0, 64, 63, 127, 63);
            setRGB0(quad, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                &interpolation, &flag);
            if (depth >= 0) {
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
            setUV4(quad, 64, 64, 127, 64, 64, 127, 127, 127);
            setRGB0(quad, work->burst_color.rgb.r >> 1, work->burst_color.rgb.g >> 1, work->burst_color.rgb.b >> 1);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
            }
        }
        matrix = saved;
        setVector(&scale, 4096, 4096, 4096);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setRGB0(quad, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
        for (i = 0; i < 32; i++) {
            j = rand() % 4;
            setUV4(quad, j % 2 << 5, (j / 2 << 5) + 64, (j % 2 << 5) + 31, (j / 2 << 5) + 64,
                j % 2 << 5, (j / 2 << 5) + 95, (j % 2 << 5) + 31, (j / 2 << 5) + 95);
            setVector(&vertices[0], -24, -24, 0);
            setVector(&vertices[1], 24, -24, 0);
            setVector(&vertices[2], -24, 24, 0);
            setVector(&vertices[3], 24, 24, 0);
            for (j = 0; j < 4; j++) {
                addVector(&vertices[j], &work->sparks[i]);
            }
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                &interpolation, &flag);
            if (depth >= 0) {
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
            addVector(&work->sparks[i], &work->spark_velocities[i]);
        }
        if (work->scale >= 0x7000) work->scale += 0x100;
        else work->scale += 0x1000;
        if (work->burst_color.rgb.r >= 9) work->burst_color.rgb.r -= 8;
        else work->burst_color.rgb.r = 0;
        if (work->burst_color.rgb.g >= 9) work->burst_color.rgb.g -= 8;
        else work->burst_color.rgb.g = 0;
        if (work->burst_color.rgb.b >= 9) work->burst_color.rgb.b -= 8;
        else work->burst_color.rgb.b = 0;
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 4 && !(work->burst_color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->phase == 2) {
        work->phase = 3;
        return 1;
    }
    return 0;
}
