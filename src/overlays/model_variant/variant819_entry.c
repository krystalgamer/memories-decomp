#include "../../types.h"
#include "variant819_entry.h"

/* Header 819's entry: four bolts drop from a model point at their own
 * start times onto a jittered point beside the opposing slot, wrapped in a
 * ring of sprite crosses and spinning Gouraud strips and trailing a ribbon
 * and smoke puffs; each ribbon fades once its bolt lands. On landing each
 * flares into a glow, sprays falling debris and shards, plays an animated
 * flash and grows a sixteen-segment shock ring while the ambient light
 * dims and recovers. */

static const VECTOR D_8013ECB8 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model819State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_GT4 gt4;
    POLY_G3 g3;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    POLY_FT4 *poly;
    POLY_GT4 *shock;
    POLY_G3 *strip;
    s32 slot;
    s32 i, j, k;
    s32 a, b;
    s32 otz;
    s16 sign;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013ECB8;
    work = (Model819State *)context;
    poly = &ft4;
    shock = &gt4;
    strip = &g3;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013ED8C[command];
        for (i = 0; i < work->config->count; i++) {
            work->state.s.timers.split.growth[i] = 0;
            work->state.s.swell[i] = 0;
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            setVector(&work->target[i], 0, 0, 0);
            setVector(&work->origin[i], 0, 0, 0);
            for (j = 0; j < work->config->ring_count; j++) {
                a = rand() % 4096;
                b = rand() % 4096;
                setVector(&work->ring[i][j],
                          work->config->size * ccos(a) / 4096 * ccos(b) / 4096,
                          work->config->size * csin(a) / 4096 * ccos(b) / 4096,
                          work->config->size * ccos(a) / 4096 * csin(b) / 4096);
            }
            for (j = 0; j < work->config->puff_count; j++) {
                a = rand() % 4096 - rand() % 4096;
                b = rand() % 4096 - rand() % 4096;
                k = rand() % 4096;
                setVector(&work->puffs[i][j], work->config->puff_spread * a / 4096,
                          work->config->puff_spread * b / 4096,
                          direction * work->config->puff_spread * k / 4096);
                setVector(&work->puff_velocity[i][j], work->config->puff_speed * a / 4096,
                          work->config->puff_speed * b / 4096,
                          direction * work->config->puff_speed * k / 4096);
                work->state.s.puff_frame[i][j] = 0;
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->debris[i][j], 0, 0, 0);
                setVector(&work->debris_velocity[i][j], (rand() - rand()) % 256 / 8,
                          -(rand() % 256 / 8 + 32), (rand() - rand()) % 256 / 8);
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->shards[i][j], 0, 0, 0);
                setVector(&work->shard_velocity[i][j], (rand() - rand()) % 256 * 36 / 256,
                          -(rand() % 256 * 36 / 256 + 36), (rand() - rand()) % 256 * 36 / 256);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->arrived[i] = 0;
            work->bolt[i].r = work->config->bolt_r;
            work->bolt[i].g = work->config->bolt_g;
            work->bolt[i].b = work->config->bolt_b;
            work->glow[i].r = work->config->glow_r;
            work->glow[i].g = work->config->glow_g;
            work->glow[i].b = work->config->glow_b;
            work->tail[i] = 0;
            work->trail_head[i] = 0;
            work->state.s.timers.split.flight[i] = 0;
            work->state.s.flash_frame[i] = 0;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->fan[i][0], work->config->fan_radius * ccos(i * 256) / 4096, 0,
                      work->config->fan_radius * csin(i * 256) / 4096);
            setVector(&work->fan[i][1], work->config->fan_outer * ccos(i * 256) / 4096, -work->config->fan_depth,
                      work->config->fan_outer * csin(i * 256) / 4096);
        }
        for (i = 0; i < work->config->count; i++) {
            for (j = 0; j < work->config->spark_count; j++) {
                setVector(&work->state.sparks[j], RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread));
                work->spark_frame[i][j] = 0;
            }
            work->spark_count[i] = 0;
        }
        work->ring_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->ring_clut = 0x3D28;
#else
        work->ring_clut = 0x3D20;
#endif
        for (i = 0; i < 7; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013ECC8[i]);
        }
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->state.s.elapsed = 0;
        work->unknown_2C60 = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    setPolyFT4(poly);
    setPolyGT4(shock);
    setPolyG3(strip);
    if (work->state.s.elapsed < work->config->start[0]) {
        Model_QueueCameraOrbitFromPreset(slot, 3, &rot, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 9, &rot, -32);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 11, &rot, -32);
    }
    if (work->head == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 4) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 5;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->state.s.elapsed < work->config->start[0]) {
        work->state.s.elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if ((work->head == 0 && work->state.s.elapsed >= work->config->start[0]) ||
        (work->head == 1 && work->state.s.elapsed >= work->config->start[1]) ||
        (work->head == 2 && work->state.s.elapsed >= work->config->start[2]) ||
        (work->head == 3 && work->state.s.elapsed >= work->config->start[3])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->head], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        pos.vx += RANDOM_SPREAD(work->config->jitter);
        pos.vy = 0;
        pos.vz += -direction * work->config->jitter * (rand() % 4096) / 4096;
        setVector(&work->pos[work->head], m.t[0], m.t[1], m.t[2]);
        setVector(&work->step[work->head], (pos.vx - work->pos[work->head].vx) / work->config->steps,
                  (pos.vy - work->pos[work->head].vy) / work->config->steps,
                  (pos.vz - work->pos[work->head].vz) / work->config->steps);
        setVector(&work->angle[work->head], ratan2(work->step[work->head].vz, work->step[work->head].vy) - 0x400,
                  direction * ratan2(work->step[work->head].vx, work->step[work->head].vz), 0);
        addVector(&work->pos[work->head], &work->step[work->head]);
        setVector(&work->target[work->head], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 32; i++) {
            setVector(&work->trail[work->head][i], m.t[0], m.t[1], m.t[2]);
        }
        work->trail_head[work->head] = (work->trail_head[work->head] + 1) % 32;
        work->head++;
    }

    for (i = 0; i < work->head; i++) {
        if (work->arrived[i] == 0) {
            setPolyFT4(poly);
            poly->tpage = work->texture[0] >> 16;
            poly->clut = work->texture[0];
            setRGB0(poly, work->config->r, work->config->g, work->config->b);
            setVector(&quad[0], -work->config->size, -work->config->size, 0);
            setVector(&quad[1], work->config->size, -work->config->size, 0);
            setVector(&quad[2], -work->config->size, work->config->size, 0);
            setVector(&quad[3], work->config->size, work->config->size, 0);
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                              (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
            setUV4(poly, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
            }
        }

        if (work->bolt[i].r || work->bolt[i].g || work->bolt[i].b) {
            for (j = 0; j < work->config->ring_count; j++) {
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                copyVector(&pos, &work->pos[i]);
                addVector(&pos, &work->ring[i][j]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                sign = 1;
                poly->tpage = work->ring_tpage;
                poly->clut = work->ring_clut;
                setUV4(poly, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(poly, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * work->config->ring_size,
                              work->config->ring_size * (k - 2 < 0 ? 1 : -1), 0);
                    setVector(&quad[1], 0, work->config->ring_size * (k - 2 < 0 ? 1 : -1), 0);
                    setVector(&quad[2], -sign * work->config->ring_size, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                      (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                }

                setRGB0(strip, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                setRGB1(strip, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                setRGB2(strip, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->state.s.timers.split.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->state.s.timers.split.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->state.s.timers.split.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->state.s.timers.split.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->state.s.timers.split.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->state.s.timers.split.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->state.s.timers.split.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->state.s.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->state.s.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->state.s.timers.split.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                }
                for (k = 0; k < 2; k++) {
                    copyVector(&rot, &work->angle[i]);
                    rot.vz += k * 1024 % 4096;
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&strip->x0,
                                      (PSXLONG *)&strip->x1, (PSXLONG *)&strip->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)strip, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&strip->x0,
                                      (PSXLONG *)&strip->x1, (PSXLONG *)&strip->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)strip, ot, (u16)otz, 3);
                    }
                }
            }
        }

        if (work->pos[i].vy < 0) {
            copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
            work->trail_head[i] = (work->trail_head[i] + 1) % 32;
            addVector(&work->pos[i], &work->step[i]);
            if (++work->state.s.timers.split.flight[i] == 8) {
                work->state.s.timers.split.flight[i] = 7;
            }
            if (work->pos[i].vy >= 0) {
                copyVector(&work->pos[i], &work->target[i]);
                work->arrived[i] = 1;
            }
        }

        setPolyG4(&g4);
        if (work->trail_head[i] > work->tail[i]) {
            if (work->bolt[i].r || work->bolt[i].g || work->bolt[i].b) {
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                k = 1;
                for (j = work->trail_head[i] - 1; j >= work->trail_head[i] - (work->tail[i] + 1); j--) {
                    setRGB0(&g4, work->bolt[i].r / k, work->bolt[i].g / k, work->bolt[i].b / k);
                    setRGB1(&g4, work->bolt[i].r / k, work->bolt[i].g / k, work->bolt[i].b / k);
                    setRGB2(&g4, work->bolt[i].r / (k + 1), work->bolt[i].g / (k + 1), work->bolt[i].b / (k + 1));
                    setRGB3(&g4, work->bolt[i].r / (k + 1), work->bolt[i].g / (k + 1), work->bolt[i].b / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy - work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy + work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    setVector(&quad[0], work->trail[i][j].vx - work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx + work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx - work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx + work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    k++;
                }
                if (work->arrived[i]) {
                    if (work->bolt[i].r >= 16) {
                        work->bolt[i].r -= work->config->fade;
                    } else {
                        work->bolt[i].r = 0;
                    }
                    if (work->bolt[i].g >= 16) {
                        work->bolt[i].g -= work->config->fade;
                    } else {
                        work->bolt[i].g = 0;
                    }
                    if (work->bolt[i].b >= 16) {
                        work->bolt[i].b -= work->config->fade;
                    } else {
                        work->bolt[i].b = 0;
                    }
                } else {
                    work->tail[i]++;
                }
            }
        }

        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        setPolyFT4(poly);
        poly->tpage = work->texture[1] >> 16;
        poly->clut = work->texture[1];
        setRGB0(poly, 0x80, 0x80, 0x80);
        setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
        setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
        setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
        setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
        for (j = 0; j < work->config->puff_count; j++) {
            if (work->state.s.puff_frame[i][j] < 16) {
                copyVector(&pos, &work->origin[i]);
                addVector(&pos, &work->puffs[i][j]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                  (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                setUV4(poly, work->state.s.puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->state.s.puff_frame[i][j] / 8 * 32,
                       work->state.s.puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->state.s.puff_frame[i][j] / 8 * 32,
                       work->state.s.puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->state.s.puff_frame[i][j] / 8 * 32 + 0x1F,
                       work->state.s.puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->state.s.puff_frame[i][j] / 8 * 32 + 0x1F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 0);
                }
                work->state.s.puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->arrived[i]) {
            if (work->glow[i].r || work->glow[i].g || work->glow[i].b) {
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                sign = 1;
                poly->tpage = work->ring_tpage;
                poly->clut = work->ring_clut;
                setUV4(poly, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(poly, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * work->config->glow_size,
                              work->config->glow_size * (k - 2 < 0 ? 1 : -1), 0);
                    setVector(&quad[1], 0, work->config->glow_size * (k - 2 < 0 ? 1 : -1), 0);
                    setVector(&quad[2], -sign * work->config->glow_size, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                      (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                }
                sign = 1;
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * work->config->glow_size / 2,
                              work->config->glow_size * (k - 2 < 0 ? 1 : -1) / 2, 0);
                    setVector(&quad[1], 0, work->config->glow_size * (k - 2 < 0 ? 1 : -1) / 2, 0);
                    setVector(&quad[2], -sign * work->config->glow_size / 2, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                      (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                }

                for (j = 0; j < 64; j++) {
                    if (work->debris[i][j].vy <= 0) {
                        setVector(&quad[0], -24, -24, 0);
                        setVector(&quad[1], 24, -24, 0);
                        setVector(&quad[2], -24, 24, 0);
                        setVector(&quad[3], 24, 24, 0);
                        for (k = 0; k < 4; k++) {
                            addVector(&quad[k], &work->debris[i][j]);
                        }
                        a = rand() % 4;
                        setUV4(poly, a % 2 * 32, a / 2 * 32 + 0x40, a % 2 * 32 + 0x1F, a / 2 * 32 + 0x40,
                               a % 2 * 32, a / 2 * 32 + 0x5F, a % 2 * 32 + 0x1F, a / 2 * 32 + 0x5F);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                          (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p,
                                          &flag);
                        if (otz >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                        }
                        addVector(&work->debris[i][j], &work->debris_velocity[i][j]);
                        work->debris_velocity[i][j].vy += 4;
                    }
                }

                poly->tpage = work->texture[6] >> 16;
                poly->clut = work->texture[6];
                setUV4(poly, 0xC0, 0, 0xFF, 0, 0xC0, 0x3F, 0xFF, 0x3F);
                for (j = 0; j < 64; j++) {
                    if (work->shards[i][j].vy <= 0) {
                        setVector(&quad[0], -64, -64, 0);
                        setVector(&quad[1], 64, -64, 0);
                        setVector(&quad[2], -64, 64, 0);
                        setVector(&quad[3], 64, 64, 0);
                        for (k = 0; k < 4; k++) {
                            addVector(&quad[k], &work->shards[i][j]);
                        }
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                          (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p,
                                          &flag);
                        if (otz >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                        }
                        addVector(&work->shards[i][j], &work->shard_velocity[i][j]);
                        work->shard_velocity[i][j].vy += 4;
                    }
                }

                m = saved;
                setVector(&scale, 0x1000, work->state.s.swell[i], 0x1000);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                poly->tpage = work->texture[work->state.s.flash_frame[i] + 2] >> 16;
                poly->clut = work->texture[work->state.s.flash_frame[i] + 2];
                setUV4(poly, work->state.s.flash_frame[i] * 32, 0x40, work->state.s.flash_frame[i] * 32 + 0x1F, 0x40,
                       work->state.s.flash_frame[i] * 32, 0xBF, work->state.s.flash_frame[i] * 32 + 0x1F, 0xBF);
                setRGB0(poly, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                setVector(&quad[0], -64, -256, 0);
                setVector(&quad[1], 64, -256, 0);
                setVector(&quad[2], -64, 0, 0);
                setVector(&quad[3], 64, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0,
                                  (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                }
                if (work->state.s.flash_frame[i] < 3) {
                    work->state.s.flash_frame[i]++;
                }

                /* The shock ring indexes its growth with i - count, as the
                 * original does: rows[1][-4..-1] stays inside the timers
                 * union and reads the flight counters in row 0. */
                setVector(&rot, 0, 0, 0);
                setVector(&scale, work->state.s.timers.rows[1][i - work->config->count] + 0x1000, 0x1000,
                          work->state.s.timers.rows[1][i - work->config->count] + 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB0(shock, 0, 0, 0);
                setRGB1(shock, 0, 0, 0);
                setRGB2(shock, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                setRGB3(shock, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                shock->tpage = work->ring_tpage;
                shock->clut = work->ring_clut;
                setUV4(shock, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
                for (j = 0; j < 16; j++) {
                    otz = RotAverage4(&work->fan[j][1], &work->fan[(j + 1) % 16][1], &work->fan[j][0],
                                      &work->fan[(j + 1) % 16][0], (PSXLONG *)&shock->x0, (PSXLONG *)&shock->x1,
                                      (PSXLONG *)&shock->x2, (PSXLONG *)&shock->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)shock, ot, (u16)otz, 1);
                    }
                }
                setVector(&scale, work->state.s.timers.rows[1][i - work->config->count] / 2 + 0x1000,
                          work->state.s.timers.rows[1][i - work->config->count] / 4 + 0x1000,
                          work->state.s.timers.rows[1][i - work->config->count] / 2 + 0x1000);
                GsSetLsMatrix(&light);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    otz = RotAverage4(&work->fan[j][1], &work->fan[(j + 1) % 16][1], &work->fan[j][0],
                                      &work->fan[(j + 1) % 16][0], (PSXLONG *)&shock->x0, (PSXLONG *)&shock->x1,
                                      (PSXLONG *)&shock->x2, (PSXLONG *)&shock->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)shock, ot, (u16)otz, 1);
                    }
                }
                if (work->state.s.timers.rows[1][i - work->config->count] < 20000) {
                    work->state.s.timers.rows[1][i - work->config->count] += work->config->grow;
                } else {
                    work->state.s.timers.rows[1][i - work->config->count] += work->config->grow_late;
                }
                if (work->state.s.swell[i] < 0x2000) {
                    work->state.s.swell[i] += 0x800;
                } else {
                    work->state.s.swell[i] += 0x100;
                }
                work->spark_count[i] += step;
                if (work->spark_count[i] > work->config->spark_count) {
                    work->spark_count[i] = work->config->spark_count;
                }

                if (work->glow[i].r >= 16) {
                    work->glow[i].r -= work->config->fade;
                } else {
                    work->glow[i].r = 0;
                }
                if (work->glow[i].g >= 16) {
                    work->glow[i].g -= work->config->fade;
                } else {
                    work->glow[i].g = 0;
                }
                if (work->glow[i].b >= 16) {
                    work->glow[i].b -= work->config->fade;
                } else {
                    work->glow[i].b = 0;
                }
            }
        }
    }

    work->state.s.elapsed += step;
    PopMatrix();
    if (work->phase == 5 && work->glow[work->config->count - 1].r == 0 &&
        work->glow[work->config->count - 1].g == 0 && work->glow[work->config->count - 1].b == 0) {
        return 2;
    }
    if (work->arrived[work->config->count - 1] == 1 && work->phase == 3) {
        work->phase = 4;
        return 1;
    }
    if (work->arrived[2] == 1 && work->phase == 2) {
        work->phase = 3;
        return 4;
    }
    if (work->arrived[1] == 1 && work->phase == 1) {
        work->phase = 2;
        return 4;
    }
    if (work->arrived[0] == 1 && work->phase == 0) {
        work->phase = 1;
        return 4;
    }
    return 0;
}
