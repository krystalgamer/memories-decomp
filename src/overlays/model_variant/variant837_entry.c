#include "../../types.h"
#include "variant837_entry.h"

/* Header 837's entry: a glow with a spinning ring and streaks gathers on a
 * model part; at the launch time it flies to a point in front of the
 * opposing slot, shedding motes and orbiting sparks, where it bursts into a
 * flash, a quartered glow, scattered sparks, beams and expanding bursts
 * while the ambient light dims and recovers. */

static const VECTOR D_8013DF7C = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD() ((rand() - rand()) % 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model837State *work;
    MATRIX light;
    MATRIX m;
    POLY_G3 g3_packet;
    POLY_FT4 ft4_packet;
    POLY_GT4 gt4_packet;
    POLY_F4 flash_packet;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR v[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG interpolation;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    POLY_FT4 *ft4;
    POLY_GT4 *gt4;
    POLY_G3 *g3;
    POLY_F4 *flash;
    s32 i, j, k;
    s32 depth;
    s16 sign;
    s32 prev;
    s32 spread;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013DF7C;
    work = (Model837State *)context;
    g3 = &g3_packet;
    ft4 = &ft4_packet;
    gt4 = &gt4_packet;
    flash = &flash_packet;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013DFE0[command];
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DF8C[i]);
        }
        work->ring_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->ring_clut = 0x3D28;
#else
        work->ring_clut = 0x3D20;
#endif
        setVector(&work->position, 0, 0, 0);
        setVector(&work->velocity, 0, 0, 0);
        for (i = 0; i < 32; i++) {
            setVector(&work->ring[i], work->config->radius * ccos(i << 7) / 4096,
                      work->config->radius * csin(i << 7) / 4096, 0);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->rotations[i], RANDOM_SPREAD(), RANDOM_SPREAD(), RANDOM_SPREAD());
            for (j = 0; j < 4; j++) {
                spread = 192 - j * 48;
                setVector(&work->streaks[i][j], spread + RANDOM_SPREAD() / 128,
                          spread + RANDOM_SPREAD() / 128, spread + RANDOM_SPREAD() / 128);
            }
            work->streak_colors[i].r = 0x80;
            work->streak_colors[i].g = 0x80;
            work->streak_colors[i].b = 0x80;
            work->lengths[i] = 0;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->motes[i], 0, 0, 0);
            setVector(&work->mote_velocities[i], RANDOM_SPREAD() / 64, RANDOM_SPREAD() / 64, RANDOM_SPREAD() / 64);
            work->mote_colors[i].r = work->config->r >> 1;
            work->mote_colors[i].g = work->config->g >> 1;
            work->mote_colors[i].b = work->config->b >> 1;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->orbits[i], 0, 0, 0);
            setVector(&work->orbit_velocities[i], RANDOM_SPREAD() * 48 / 4096, RANDOM_SPREAD() * 48 / 4096,
                      RANDOM_SPREAD() * 48 / 4096);
            work->orbit_colors[i].r = work->config->r >> 1;
            work->orbit_colors[i].g = work->config->g >> 1;
            work->orbit_colors[i].b = work->config->b >> 1;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->bursts[i], RANDOM_SPREAD() / 16, RANDOM_SPREAD() / 16, RANDOM_SPREAD() / 16);
            work->burst_colors[i].r = work->config->r;
            work->burst_colors[i].g = work->config->g;
            work->burst_colors[i].b = work->config->b;
            work->burst_scales[i] = 0;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->beams[i], RANDOM_SPREAD() / 8, RANDOM_SPREAD() / 8, RANDOM_SPREAD() / 8);
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->spark_velocities[i], RANDOM_SPREAD() / 256, RANDOM_SPREAD() / 256,
                      RANDOM_SPREAD() / 256);
        }
        work->glow.rgb.r = 0x80;
        work->glow.rgb.g = 0x80;
        work->glow.rgb.b = 0x80;
        work->halo.rgb.r = work->config->r >> 2;
        work->halo.rgb.g = work->config->g >> 2;
        work->halo.rgb.b = work->config->b >> 2;
        work->core.rgb.r = work->config->r >> 1;
        work->core.rgb.g = work->config->g >> 1;
        work->core.rgb.b = work->config->b >> 1;
        work->impact.rgb.r = work->config->r;
        work->impact.rgb.g = work->config->g;
        work->impact.rgb.b = work->config->b;
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->scale = 0;
        work->streak_count = 0;
        work->mote_count = 0;
        work->orbit_count = 0;
        work->burst_count = 0;
        work->flash = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->delay) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -4);
    } else if (work->elapsed < work->config->launch_time) {
        Model_QueueCameraOrbitFromPreset(slot, 2, &rot, -16);
    } else if (work->elapsed < work->config->end_time) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rot, -32);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -32);
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
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    setPolyG3(g3);
    setPolyFT4(ft4);
    setPolyGT4(gt4);
    if (work->elapsed < work->config->end_time) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        if (!(work->frame & 1)) {
            /* "* 128" keeps the full-word frame load that "<< 7" narrows. */
            setVector(&rot, 0, 0, work->frame * 128);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            ft4->tpage = work->ring_tpage;
            ft4->clut = work->ring_clut;
            sign = 1;
            for (j = 0; j < 4; j++) {
                setRGB0(ft4, work->core.rgb.r, work->core.rgb.g, work->core.rgb.b);
                /* "*= -1" reproduces the re-extension that "= -sign" drops. */
                sign *= -1;
                setVector(&v[0], -sign * 64, j - 2 >= 0 ? -64 : 64, 0);
                setVector(&v[1], 0, j - 2 >= 0 ? -64 : 64, 0);
                setVector(&v[2], -sign * 64, 0, 0);
                setVector(&v[3], 0, 0, 0);
                setUV4(ft4, 64, 0, 127, 0, 64, 63, 127, 63);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0) {
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                    }
                }
                setUV4(ft4, 64, 64, 127, 64, 64, 127, 127, 127);
                setRGB0(ft4, work->core.rgb.r >> 1, work->core.rgb.g >> 1, work->core.rgb.b >> 1);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                }
            }
        }
    } else if (work->phase == 2) {
        work->phase = 3;
    }
    if (work->elapsed >= work->config->launch_time && work->phase == 0) {
        work->position.vx = pos.vx;
        work->position.vy = pos.vy;
        work->position.vz = pos.vz;
        work->velocity.vx = -work->position.vx / 8;
        work->velocity.vy = (-350 - work->position.vy) / 8;
        work->velocity.vz = (direction * 450 - work->position.vz) / 8;
        work->phase = 1;
    }
    if (work->phase != 0) {
        pos.vx = work->position.vx;
        pos.vy = work->position.vy;
        pos.vz = work->position.vz;
    }
    if (work->elapsed < work->config->end_time) {
        gt4->tpage = work->texture[0] >> 16;
        gt4->clut = work->texture[0];
        setUV4(gt4, 64, 0, 111, 0, 64, 127, 111, 127);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        saved = m;
        if (work->phase >= 2) {
            setVector(&scale, 0x2000, 0x2000, 0x2000);
        } else {
            setVector(&scale, 0x1000, 0x1000, 0x1000);
        }
        for (i = 0; i < work->streak_count; i++) {
            m = saved;
            RotMatrix(&work->rotations[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            /* A while loop with a trailing increment keeps the original delay
             * slot filling; PREV holds j - 1 so GCC does not fold it into
             * the divisors. */
            j = 0;
            while (j < work->lengths[i] - 1) {
                prev = j - 1;
                setRGB0(gt4, work->streak_colors[i].r / (work->lengths[i] - prev),
                        work->streak_colors[i].g / (work->lengths[i] - prev),
                        work->streak_colors[i].b / (work->lengths[i] - prev));
                setRGB1(gt4, work->streak_colors[i].r / (work->lengths[i] - prev),
                        work->streak_colors[i].g / (work->lengths[i] - prev),
                        work->streak_colors[i].b / (work->lengths[i] - prev));
                setRGB2(gt4, work->streak_colors[i].r / (work->lengths[i] - j),
                        work->streak_colors[i].g / (work->lengths[i] - j),
                        work->streak_colors[i].b / (work->lengths[i] - j));
                setRGB3(gt4, work->streak_colors[i].r / (work->lengths[i] - j),
                        work->streak_colors[i].g / (work->lengths[i] - j),
                        work->streak_colors[i].b / (work->lengths[i] - j));
                setVector(&v[0], -8 / (work->lengths[i] - prev), 0, 0);
                setVector(&v[1], 8 / (work->lengths[i] - prev), 0, 0);
                setVector(&v[2], -8 / (work->lengths[i] - j), 0, 0);
                setVector(&v[3], 8 / (work->lengths[i] - j), 0, 0);
                addVector(&v[0], &work->streaks[i][j]);
                addVector(&v[1], &work->streaks[i][j]);
                addVector(&v[2], &work->streaks[i][j + 1]);
                addVector(&v[3], &work->streaks[i][j + 1]);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&gt4->x0, (PSXLONG *)&gt4->x1,
                                    (PSXLONG *)&gt4->x2, (PSXLONG *)&gt4->x3, &interpolation, &flag) - 64;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)gt4, ot, (u16)depth, 1);
                }
                j++;
            }
            if (++work->lengths[i] >= 5) {
                work->lengths[i] = 4;
                if (work->streak_colors[i].r >= 16) work->streak_colors[i].r -= 15;
                else work->streak_colors[i].r = 0;
                if (work->streak_colors[i].g >= 16) work->streak_colors[i].g -= 15;
                else work->streak_colors[i].g = 0;
                if (work->streak_colors[i].b >= 16) work->streak_colors[i].b -= 15;
                else work->streak_colors[i].b = 0;
                if (work->streak_colors[i].r == 0 && work->streak_colors[i].g == 0 &&
                    work->streak_colors[i].b == 0) {
                    work->streak_colors[i].r = 0x80;
                    work->streak_colors[i].g = 0x80;
                    work->streak_colors[i].b = 0x80;
                    work->lengths[i] = 0;
                }
            }
        }
        if (work->glow.word & 0xFFFFFF) {
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            saved = m;
            setRGB0(g3, 0, 0, 0);
            if (!(work->frame & 1)) {
                setVector(&scale, work->scale * 3 / 4, work->scale * 3 / 4, work->scale * 3 / 4);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB1(g3, work->glow.rgb.r, work->glow.rgb.g, work->glow.rgb.b);
                setRGB2(g3, work->glow.rgb.r, work->glow.rgb.g, work->glow.rgb.b);
            } else {
                setVector(&scale, work->scale, work->scale, work->scale);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB1(g3, work->glow.rgb.r >> 2, work->glow.rgb.g >> 2, work->glow.rgb.b >> 2);
                setRGB2(g3, work->glow.rgb.r >> 2, work->glow.rgb.g >> 2, work->glow.rgb.b >> 2);
            }
            setVector(&v[0], 0, 0, 0);
            for (i = 0; i < 32; i++) {
                depth = RotAverage3(&v[0], &work->ring[i], &work->ring[(i + 1) % 32], (PSXLONG *)&g3->x0,
                                    (PSXLONG *)&g3->x1, (PSXLONG *)&g3->x2, &interpolation, &flag) - 64;
                if (flag >= 0 && depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)g3, ot, (u16)depth, 2);
                }
            }
        }
        m = saved;
        setVector(&scale, work->scale, work->scale, work->scale);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        if (work->halo.word & 0xFFFFFF) {
            ft4->tpage = work->ring_tpage;
            ft4->clut = work->ring_clut;
            setUV4(ft4, 64, 0, 127, 0, 64, 63, 127, 63);
            setRGB0(ft4, work->halo.rgb.r, work->halo.rgb.g, work->halo.rgb.b);
            sign = 1;
            for (i = 0; i < 4; i++) {
                sign *= -1;
                setVector(&v[0], -sign * 96, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&v[1], 0, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&v[2], -sign * 96, 0, 0);
                setVector(&v[3], 0, 0, 0);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                }
            }
        }
        if (!(work->frame & 1)) {
            setRGB0(ft4, 32, 32, 32);
            setVector(&rot, 0, 0, work->elapsed * 128 % 4096);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            ft4->tpage = work->texture[0] >> 16;
            ft4->clut = work->texture[0];
            setUV4(ft4, 0, 0, 63, 0, 0, 63, 63, 63);
            sign = 1;
            for (i = 0; i < 4; i++) {
                sign *= -1;
                setVector(&v[0], -sign * 96, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&v[1], 0, i - 2 >= 0 ? -96 : 96, 0);
                setVector(&v[2], -sign * 96, 0, 0);
                setVector(&v[3], 0, 0, 0);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                }
            }
        }
        if (work->phase >= 2) {
            ft4->tpage = work->ring_tpage;
            ft4->clut = work->ring_clut;
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < work->mote_count; i++) {
                setRGB0(ft4, work->mote_colors[i].r, work->mote_colors[i].g, work->mote_colors[i].b);
                j = rand() % 4;
                setUV4(ft4, j % 2 << 5, (j / 2 << 5) + 64, (j % 2 << 5) + 31, (j / 2 << 5) + 64,
                       j % 2 << 5, (j / 2 << 5) + 95, (j % 2 << 5) + 31, (j / 2 << 5) + 95);
                setVector(&v[0], -8, -8, 0);
                setVector(&v[1], 8, -8, 0);
                setVector(&v[2], -8, 8, 0);
                setVector(&v[3], 8, 8, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&v[j], &work->motes[i]);
                }
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0) {
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->motes[i], &work->mote_velocities[i]);
                if (work->mote_colors[i].r >= 9) work->mote_colors[i].r -= 8;
                else work->mote_colors[i].r = 0;
                if (work->mote_colors[i].g >= 9) work->mote_colors[i].g -= 8;
                else work->mote_colors[i].g = 0;
                if (work->mote_colors[i].b >= 9) work->mote_colors[i].b -= 8;
                else work->mote_colors[i].b = 0;
                if (work->mote_colors[i].r == 0 && work->mote_colors[i].g == 0 && work->mote_colors[i].b == 0) {
                    setVector(&work->motes[i], 0, 0, 0);
                    work->mote_colors[i].r = work->config->r >> 1;
                    work->mote_colors[i].g = work->config->g >> 1;
                    work->mote_colors[i].b = work->config->b >> 1;
                }
            }
            for (i = 0; i < work->orbit_count; i++) {
                setRGB0(ft4, work->orbit_colors[i].r, work->orbit_colors[i].g, work->orbit_colors[i].b);
                setUV4(ft4, 64, 0, 127, 0, 64, 63, 127, 63);
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    setVector(&v[0], -sign * 12, j - 2 >= 0 ? -12 : 12, 0);
                    setVector(&v[1], 0, j - 2 >= 0 ? -12 : 12, 0);
                    setVector(&v[2], -sign * 12, 0, 0);
                    setVector(&v[3], 0, 0, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&v[k], &work->orbits[i]);
                    }
                    depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                        (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->orbits[i], &work->orbit_velocities[i]);
                if (work->orbit_colors[i].r >= 9) work->orbit_colors[i].r -= 8;
                else work->orbit_colors[i].r = 0;
                if (work->orbit_colors[i].g >= 9) work->orbit_colors[i].g -= 8;
                else work->orbit_colors[i].g = 0;
                if (work->orbit_colors[i].b >= 9) work->orbit_colors[i].b -= 8;
                else work->orbit_colors[i].b = 0;
                if (work->orbit_colors[i].r == 0 && work->orbit_colors[i].g == 0 && work->orbit_colors[i].b == 0) {
                    setVector(&work->orbits[i], 0, 0, 0);
                    work->orbit_colors[i].r = work->config->r >> 1;
                    work->orbit_colors[i].g = work->config->g >> 1;
                    work->orbit_colors[i].b = work->config->b >> 1;
                }
            }
            if (!(work->frame & 3)) {
                if (++work->mote_count >= 33) {
                    work->mote_count = 32;
                }
            }
            if (!(work->frame & 7)) {
                if (++work->orbit_count >= 17) {
                    work->orbit_count = 16;
                }
            }
        }
        if (work->phase >= 2) {
            work->scale = 0x2000;
        } else if (work->scale < 0x1000) {
            work->scale += 0x80;
        }
        if (work->phase == 1) {
            work->position.vx += work->velocity.vx;
            work->position.vy += work->velocity.vy;
            work->position.vz += work->velocity.vz;
            if ((work->position.vz >= 0 ? work->position.vz : -work->position.vz) >= 450) {
                work->phase = 2;
            }
        }
        if (++work->streak_count >= 33) {
            work->streak_count = 32;
        }
    }
    if (work->phase >= 3) {
        setVector(&pos, 0, -350, direction * 450);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        saved = m;
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        sign = 1;
        if (work->impact.word & 0xFFFFFF) {
            ft4->tpage = work->ring_tpage;
            ft4->clut = work->ring_clut;
            setRGB0(ft4, work->impact.rgb.r, work->impact.rgb.g, work->impact.rgb.b);
            for (i = 0; i < 4; i++) {
                sign *= -1;
                setVector(&v[0], -sign * 192, i - 2 >= 0 ? -192 : 192, 0);
                setVector(&v[1], 0, i - 2 >= 0 ? -192 : 192, 0);
                setVector(&v[2], -sign * 192, 0, 0);
                setVector(&v[3], 0, 0, 0);
                setUV4(ft4, 64, 0, 127, 0, 64, 63, 127, 63);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                }
            }
            for (i = 0; i < 64; i++) {
                j = rand() % 4;
                setUV4(ft4, j % 2 << 5, (j / 2 << 5) + 64, (j % 2 << 5) + 31, (j / 2 << 5) + 64,
                       j % 2 << 5, (j / 2 << 5) + 95, (j % 2 << 5) + 31, (j / 2 << 5) + 95);
                setVector(&v[0], -16, -16, 0);
                setVector(&v[1], 16, -16, 0);
                setVector(&v[2], -16, 16, 0);
                setVector(&v[3], 16, 16, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&v[j], &work->sparks[i]);
                }
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                    (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                if (depth >= 0) {
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->sparks[i], &work->spark_velocities[i]);
            }
            gt4->tpage = work->texture[2] >> 16;
            gt4->clut = work->texture[2];
            setRGB0(gt4, 0, 0, 0);
            setRGB1(gt4, 0, 0, 0);
            setRGB2(gt4, work->impact.rgb.r >> 2, work->impact.rgb.g >> 2, work->impact.rgb.b >> 2);
            setRGB3(gt4, work->impact.rgb.r >> 2, work->impact.rgb.g >> 2, work->impact.rgb.b >> 2);
            setUV4(gt4, 0, 64, 63, 64, 0, 191, 63, 191);
            for (i = 0; i < 32; i++) {
                setVector(&v[0], work->beams[i].vx - 6, work->beams[i].vy, work->beams[i].vz);
                setVector(&v[1], work->beams[i].vx + 6, work->beams[i].vy, work->beams[i].vz);
                setVector(&v[2], -8, 0, 0);
                setVector(&v[3], 8, 0, 0);
                depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&gt4->x0, (PSXLONG *)&gt4->x1,
                                    (PSXLONG *)&gt4->x2, (PSXLONG *)&gt4->x3, &interpolation, &flag) - 64;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)gt4, ot, (u16)depth, 1);
                }
            }
            if (work->impact.rgb.r >= 5) work->impact.rgb.r -= 4;
            else work->impact.rgb.r = 0;
            if (work->impact.rgb.g >= 5) work->impact.rgb.g -= 4;
            else work->impact.rgb.g = 0;
            if (work->impact.rgb.b >= 5) work->impact.rgb.b -= 4;
            else work->impact.rgb.b = 0;
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->burst_colors[i].r != 0 || work->burst_colors[i].g != 0 || work->burst_colors[i].b != 0) {
                setVector(&scale, work->burst_scales[i], work->burst_scales[i], work->burst_scales[i]);
                setVector(&pos, 0, -350, direction * 450);
                addVector(&pos, &work->bursts[i]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                ft4->tpage = work->ring_tpage;
                ft4->clut = work->ring_clut;
                setRGB0(ft4, work->burst_colors[i].r, work->burst_colors[i].g, work->burst_colors[i].b);
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    setVector(&v[0], -sign * 64, j - 2 >= 0 ? -64 : 64, 0);
                    setVector(&v[1], 0, j - 2 >= 0 ? -64 : 64, 0);
                    setVector(&v[2], -sign * 64, 0, 0);
                    setVector(&v[3], 0, 0, 0);
                    setUV4(ft4, 64, 0, 127, 0, 64, 63, 127, 63);
                    depth = RotAverage4(&v[0], &v[1], &v[2], &v[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                        (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &interpolation, &flag) - 64;
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)depth, 1);
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
    if (work->flash != 0) {
        setPolyF4(flash);
        setRGB0(flash, work->flash, work->flash, work->flash);
        setXY4(flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        Graphics_SubmitDrawModePacket((u32 *)flash, ot, 0, 1);
        if (work->flash >= 9) work->flash -= 8;
        else work->flash = 0;
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 5 && !(work->impact.word & 0xFFFFFF)) {
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
