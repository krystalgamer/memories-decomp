#include "../../types.h"
#include "variant808_entry.h"
#include "../../psyq/abs.h"

/* Header 808's entry: sparkles stream from a model part while the ambient
 * light dims, then a spinning star comet flies from it to the opposing
 * slot, drawing a fading trail. On arrival a glow bursts, embers rise and
 * fall around it and a ring spreads out while the colour fades and the
 * light comes back. */

static const VECTOR D_8013D1AC = {4096, 4096, 4096, 0};

static const Model808Shape D_8013D1BC = {{
    {-190, 100, 0, 0}, {-150, -150, 0, 0}, {100, -190, 0, 0}, {210, 30, 0, 0},
    {30, 210, 0, 0}, {0, 0, 0, 0}, {-420, -70, 0, 0}, {-60, -420, 0, 0},
    {380, -190, 0, 0}, {300, 300, 0, 0}, {-200, 380, 0, 0},
}};

static const Model808Faces D_8013D214 = {{
    {8, 2, 3, 5}, {9, 3, 4, 5}, {2, 7, 5, 1}, {5, 1, 0, 6}, {4, 5, 10, 0},
}};

s32 func_8013B004(u8 *context, s32 command)
{
    Model808State *work;
    MATRIX light;
    MATRIX m;
    POLY_F4 f4;
    POLY_FT4 sparkle;
    POLY_G4 g4;
    POLY_FT4 ft4;
    POLY_GT4 gt4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    SVECTOR shape[11];
    u8 faces[5][4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    s32 q;
    GsOT *ot;
    u8 *face;
    s32 otz;
    s32 t;
    s32 k;
    SVECTOR *vtx;
    s32 i;
    s32 r;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013D1AC;
    *(Model808Shape *)shape = D_8013D1BC;
    *(Model808Faces *)faces = D_8013D214;
    work = (Model808State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D2B4[command];
        setVector(&work->center, 0, 0, 0);
        setVector(&work->velocity, 0, 0, 0);
        work->head = 0;
        for (i = 0; i < work->config->sparkle_count; i++) {
            setVector(&work->sparkles[i], 0, 0, 0);
            setVector(&work->sparkle_velocities[i], work->config->sparkle_spread * ((rand() - rand()) % 4096) / 4096,
                      -work->config->sparkle_spread * (rand() % 4096) / 4096,
                      work->config->sparkle_spread * ((rand() - rand()) % 4096) / 4096);
            work->frames[i] = 0;
        }
        for (t = 0; t < work->config->ember_count; t++) {
            setVector(&work->embers[t], 0, -350, direction * 450);
            setVector(&work->ember_velocities[t], work->config->ember_spread * ((rand() - rand()) % 4096) / 4096,
                      work->config->ember_spread * ((rand() - rand()) % 4096) / 4096,
                      -direction * work->config->ember_spread * (rand() % 4096) / 4096);
            setVector(&work->drifts[t], 0, -350, direction * 450);
            setVector(&work->drift_velocities[t], work->config->ember_spread * ((rand() - rand()) % 4096) / 4096,
                      work->config->ember_spread * ((rand() - rand()) % 4096) / 4096,
                      -direction * work->config->ember_spread * (rand() % 4096) / 4096);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->ring[i][0], work->config->inner_radius * ccos(i << 8) / 4096,
                      work->config->inner_radius * csin(i << 8) / 4096, -direction * 128);
            setVector(&work->ring[i][1], work->config->outer_radius * ccos(i << 8) / 4096,
                      work->config->outer_radius * csin(i << 8) / 4096, 0);
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->trail[i], 0, 0, 0);
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D228[i]);
        }
        work->color.c.trail_on = 0xFF;
        work->ring_scale = 0x1000;
        work->color.c.r = 0;
        work->color.c.g = 0;
        work->color.c.b = 0;
        work->count = 0;
        work->drawn = 0;
        work->level = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->launch) {
        if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
        } else {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -16);
        }
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, -8);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 9, &rot, -8);
    }
    if (work->phase == 0) {
        if (work->level > -0x200) {
            work->level -= step << 6;
        } else {
            work->level = -0x200;
        }
    } else if (work->phase == 3) {
        if (work->level < 0x800) {
            work->level += step << 6;
        } else {
            work->phase = 4;
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
    if (work->phase == 0) {
        GsSetLsMatrix(&light);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&work->center, m.t[0], m.t[1], m.t[2]);
        work->center.vy -= 64;
        work->velocity.vx = -work->center.vx / work->config->travel_time;
        work->velocity.vy = (-350 - work->center.vy) / work->config->travel_time;
        work->velocity.vz = (direction * 450 - work->center.vz) / work->config->travel_time;
        for (i = 0; i < 64; i++) {
            setVector(&work->trail[i], m.t[0], m.t[1], m.t[2]);
        }
        work->head = (work->head + 1) % 64;
        work->phase = 1;
    }

    for (i = 0; i < work->count; i++) {
        if (work->frames[i] < 48) {
            setPolyFT4(&sparkle);
            sparkle.tpage = work->texture[3] >> 16;
            sparkle.clut = work->texture[3];
            setRGB0(&sparkle, 0x80, 0x80, 0x80);
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
            setVector(&pos, m.t[0], m.t[1], m.t[2]);
            addVector(&pos, &work->sparkles[i]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->sparkle_size, -work->config->sparkle_size, 0);
            setVector(&quad[1], work->config->sparkle_size, -work->config->sparkle_size, 0);
            setVector(&quad[2], -work->config->sparkle_size, work->config->sparkle_size, 0);
            setVector(&quad[3], work->config->sparkle_size, work->config->sparkle_size, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&sparkle.x0,
                              (PSXLONG *)&sparkle.x1, (PSXLONG *)&sparkle.x2, (PSXLONG *)&sparkle.x3, &p, &flag);
            if (work->frames[i] < 24) {
                setUV4(&sparkle, work->frames[i] / 6 % 4 * 32 + 128, 0x90, work->frames[i] / 6 % 4 * 32 + 159, 0x90,
                       work->frames[i] / 6 % 4 * 32 + 128, 0xAF, work->frames[i] / 6 % 4 * 32 + 159, 0xAF);
            } else {
                setUV4(&sparkle, work->frames[i] / 6 % 4 * 32 + 128, 0xB0, work->frames[i] / 6 % 4 * 32 + 159, 0xB0,
                       work->frames[i] / 6 % 4 * 32 + 128, 0xCF, work->frames[i] / 6 % 4 * 32 + 159, 0xCF);
            }
            Graphics_SubmitDrawModePacket((u32 *)&sparkle, ot, (u16)otz, 1);
            addVector(&work->sparkles[i], &work->sparkle_velocities[i]);
            work->frames[i]++;
        }
    }
    if (work->elapsed < work->config->sparkle_end) {
        work->count++;
        if (work->count > work->config->sparkle_count) {
            work->count = work->config->sparkle_count;
        }
    } else if (work->phase < 2) {
        if (work->color.c.r < work->config->r - 15) {
            work->color.c.r += work->config->fade_in;
        } else {
            work->color.c.r = work->config->r;
        }
        if (work->color.c.g < work->config->g - 15) {
            work->color.c.g += work->config->fade_in;
        } else {
            work->color.c.g = work->config->g;
        }
        if (work->color.c.b < work->config->b - 15) {
            work->color.c.b += work->config->fade_in;
        } else {
            work->color.c.b = work->config->b;
        }
    }

    if (work->phase == 1) {
        setPolyF4(&f4);
        setRGB0(&f4, work->color.c.r, work->color.c.g, work->color.c.b);
        setVector(&scale, work->config->comet_scale, work->config->comet_scale, work->config->comet_scale);
        if (work->elapsed >= work->config->launch) {
            rot.vz = (work->elapsed * 64) & 0xFFF;
        }
        GsSetLsMatrix(&light);
        RotTrans(&work->center, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        vtx = shape;
        for (face = faces[0], r = 0; r < 5; face += 4, r++) {
            otz = RotTransPers4(&vtx[face[0]], &vtx[face[1]], &vtx[face[2]],
                                &vtx[face[3]], (PSXLONG *)&f4.x0, (PSXLONG *)&f4.x1,
                                (PSXLONG *)&f4.x2, (PSXLONG *)&f4.x3, &p, &flag);
            Graphics_SubmitDrawModePacket((u32 *)&f4, ot, (u16)otz, 1);
        }
        if (work->elapsed >= work->config->launch) {
            setVector(&work->trail[work->head], work->center.vx, work->center.vy, work->center.vz);
            work->head = (work->head + 1) % 64;
            addVector(&work->center, &work->velocity);
            if (ABS(work->center.vz) >= 450) {
                work->phase = 2;
            }
        }
    }

    if (work->elapsed >= work->config->launch) {
        setPolyG4(&g4);
        if (work->head > work->drawn && work->color.c.trail_on != 0) {
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = work->head - 1, q = 1; i >= (k = work->head - 1) - work->drawn; i--) {
                setRGB0(&g4, work->color.c.r / q, work->color.c.g / q, work->color.c.b / q);
                setRGB1(&g4, work->color.c.r / q, work->color.c.g / q, work->color.c.b / q);
                setRGB2(&g4, work->color.c.r / (q + 1), work->color.c.g / (q + 1), work->color.c.b / (q + 1));
                setRGB3(&g4, work->color.c.r / (q + 1), work->color.c.g / (q + 1), work->color.c.b / (q + 1));
                setVector(&quad[0], work->trail[i].vx, work->trail[i].vy - work->config->trail_width / q,
                          work->trail[i].vz);
                setVector(&quad[1], work->trail[i].vx, work->trail[i].vy + work->config->trail_width / q,
                          work->trail[i].vz);
                setVector(&quad[2], work->trail[i - 1].vx,
                          work->trail[i - 1].vy - work->config->trail_width / (q + 1), work->trail[i - 1].vz);
                setVector(&quad[3], work->trail[i - 1].vx,
                          work->trail[i - 1].vy + work->config->trail_width / (q + 1), work->trail[i - 1].vz);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
                q++;
            }
            if (work->phase < 2) {
                work->drawn++;
            }
        }
    }

    if (work->phase >= 2 && (work->color.word & 0xFFFFFF)) {
        setPolyFT4(&ft4);
        ft4.tpage = work->texture[0] >> 16;
        ft4.clut = work->texture[0];
        setRGB0(&ft4, work->color.c.r, work->color.c.g, work->color.c.b);
        setVector(&pos, 0, -350, direction * 450);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        saved = m;
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
        setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
        setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
        setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
        }
        setVector(&scale, 0x800, 0x800, 0x800);
        m = saved;
        GsSetLsMatrix(&light);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
        }
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        for (i = 0; i < work->config->ember_count; i++) {
            if (work->embers[i].vy < 0) {
                setPolyF4(&f4);
                setRGB0(&f4, work->color.c.r, work->color.c.g, work->color.c.b);
                setVector(&scale, work->config->comet_scale >> 1, work->config->comet_scale >> 1,
                          work->config->comet_scale >> 1);
                if (work->elapsed >= work->config->launch) {
                    rot.vz = (work->elapsed * 0x60) & 0xFFF;
                }
                GsSetLsMatrix(&light);
                RotTrans(&work->embers[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                vtx = shape;
                for (face = faces[0], r = 0; r < 5; face += 4, r++) {
                    otz = RotTransPers4(&vtx[face[0]], &vtx[face[1]],
                                        &vtx[face[2]], &vtx[face[3]], (PSXLONG *)&f4.x0,
                                        (PSXLONG *)&f4.x1, (PSXLONG *)&f4.x2, (PSXLONG *)&f4.x3, &p, &flag);
                    Graphics_SubmitDrawModePacket((u32 *)&f4, ot, (u16)otz, 1);
                }
                addVector(&work->embers[i], &work->ember_velocities[i]);
                work->ember_velocities[i].vy += work->config->gravity;
            }
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->color.c.r, work->color.c.g, work->color.c.b);
            ft4.tpage = work->texture[1] >> 16;
            ft4.clut = work->texture[1];
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->drifts[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->ember_size, -work->config->ember_size, 0);
            setVector(&quad[1], work->config->ember_size, -work->config->ember_size, 0);
            setVector(&quad[2], -work->config->ember_size, work->config->ember_size, 0);
            setVector(&quad[3], work->config->ember_size, work->config->ember_size, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            r = rand() % 3;
            setUV4(&ft4, r * 32 + 128, 0, r * 32 + 159, 0, r * 32 + 128, 0x1F, r * 32 + 159, 0x1F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
            addVector(&work->drifts[i], &work->drift_velocities[i]);
        }

        setVector(&scale, work->ring_scale, work->ring_scale, 0x1000);
        setPolyGT4(&gt4);
        setVector(&rot, 0, 0, 0);
        gt4.tpage = work->texture[2] >> 16;
        gt4.clut = work->texture[2];
        setRGB0(&gt4, 0, 0, 0);
        setRGB1(&gt4, 0, 0, 0);
        setRGB2(&gt4, work->color.c.r, work->color.c.g, work->color.c.b);
        setRGB3(&gt4, work->color.c.r, work->color.c.g, work->color.c.b);
        setVector(&pos, 0, -350, direction * 450);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&work->ring[15][0], &work->ring[0][0], &work->ring[15][1], &work->ring[0][1],
                                  (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2,
                                  (PSXLONG *)&gt4.x3, &p, &flag);
            } else {
                otz = RotAverage4(&work->ring[i][0], &work->ring[i + 1][0], &work->ring[i][1],
                                  &work->ring[i + 1][1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                  (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
            }
            setUV4(&gt4, 0, 0x40, 0x7F, 0x40, 0, 0x8F, 0x7F, 0x8F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
            }
        }
        if (work->ring_scale < 0x5000) {
            work->ring_scale += 0x800;
        } else {
            work->ring_scale += 0x200;
        }
        if (work->color.c.r >= 16) {
            work->color.c.r -= work->config->fade_out;
        } else {
            work->color.c.r = 0;
        }
        if (work->color.c.g >= 16) {
            work->color.c.g -= work->config->fade_out;
        } else {
            work->color.c.g = 0;
        }
        if (work->color.c.b >= 16) {
            work->color.c.b -= work->config->fade_out;
        } else {
            work->color.c.b = 0;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 4 && !(work->color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->phase == 2) {
        work->phase = 3;
        return 1;
    }
    return 0;
}
