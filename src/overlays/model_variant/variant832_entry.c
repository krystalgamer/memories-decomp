#include "../../types.h"
#include "variant832_entry.h"

/* Header 832's entry: up to four bolts leave a model point one after
 * another and fly towards jittered points on the opposing slot. Each bolt
 * draws a star sprite at every point of its ring, wrapped in spinning
 * Gouraud strips, and trails a ribbon. At each origin a muzzle flash of
 * sprites, smoke and embers fades out; on arrival every bolt flares into a
 * glow with sixteen drifting star motes, shards, debris, sparks, a fan of
 * stretching blades and a growing sixteen-segment shock ring while the
 * ambient light dims and recovers and the camera cuts between cues. */

static const VECTOR gModelVariant832_D4D3C = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 ModelVariant832_HandleRequest(u8 *context, s32 command)
{
    Model832State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 sprite;
    POLY_G3 strip;
    POLY_G4 ribbon;
    POLY_GT4 blade;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    POLY_FT4 *ft4;
    POLY_G3 *g3;
    POLY_G4 *g4;
    POLY_GT4 *gt4;
    s32 slot;
    s16 sign;
    s32 y0;
    s32 i, j, k, l;
    s32 a, b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant832_D4D3C;
    work = (Model832State *)context;
    /* The packets are reached through pointers set up front, which keeps
     * their addresses in the registers and spill slots the image uses. */
    ft4 = &sprite;
    g3 = &strip;
    g4 = &ribbon;
    gt4 = &blade;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant832_D4E9C[command];
        for (i = 0; i < work->config->count; i++) {
            work->growth[i] = 0;
            work->unknown_2BB6[i] = 0;
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
                work->puff_frame[i][j] = 0;
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->motes[i][j], 0, 0, 0);
                setVector(&work->mote_velocity[i][j], (rand() - rand()) % 256 / 16,
                          (rand() - rand()) % 256 / 16, (rand() - rand()) % 256 / 16);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->debris[i][j], 0, 0, 0);
                setVector(&work->debris_velocity[i][j], (rand() - rand()) % 256 * 12 / 256,
                          (rand() - rand()) % 256 * 12 / 256, (rand() - rand()) % 256 * 12 / 256);
                work->debris_frame[i] = 0;
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->shards[i][j], 0, 0, 0);
                setVector(&work->shard_velocity[i][j], (rand() - rand()) % 256 * 24 / 256,
                          (rand() - rand()) % 256 * 24 / 256, (rand() - rand()) % 256 * 24 / 256);
            }
            for (j = 0; j < 8; j++) {
                setVector(&work->embers[i][j], 0, 0, 0);
                setVector(&work->ember_velocity[i][j], (rand() - rand()) % 256 / 16,
                          (rand() - rand()) % 256 / 16, direction * (rand() % 256 << 4) / 256);
                /* "<< 4" stops the multiply by direction being hoisted. */
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->flares[i][j], 0, 0, 0);
                setVector(&work->flare_velocity[i][j], (rand() - rand()) % 256 / 16,
                          (rand() - rand()) % 256 / 16, (rand() - rand()) % 256 / 16);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->blades[i][j], (rand() - rand()) % 4096, (rand() - rand()) % 4096,
                          (rand() - rand()) % 4096);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->arrived[i] = 0;
            work->bolt[i][0] = work->config->bolt_r;
            work->bolt[i][1] = work->config->bolt_g;
            work->bolt[i][2] = work->config->bolt_b;
            work->glow[i][0] = work->config->glow_r;
            work->glow[i][1] = work->config->glow_g;
            work->glow[i][2] = work->config->glow_b;
            work->flare[i][0] = work->config->glow_r;
            work->flare[i][1] = work->config->glow_g;
            work->flare[i][2] = work->config->glow_b;
            work->tail[i] = 0;
            work->trail_head[i] = 0;
            work->stretch[i] = 0;
            work->flight[i] = 0;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->fan[i][0], work->config->fan_radius * ccos(i * 256) / 4096,
                      work->config->fan_radius * csin(i * 256) / 4096, 0);
            setVector(&work->fan[i][1], work->config->fan_outer * ccos(i * 256) / 4096,
                      work->config->fan_outer * csin(i * 256) / 4096, -direction * work->config->fan_depth);
        }
        for (i = 0; i < work->config->count; i++) {
            for (j = 0; j < work->config->spark_count; j++) {
                setVector(&work->sparks[i][j], RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread));
                work->spark_frame[i][j] = 0;
            }
            work->spark_count[i] = 0;
        }
        work->tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->clut = 0x3D28;
#else
        work->clut = 0x3D20;
#endif
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant832_D4D4C[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant832_D4D4C[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[2]);
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[3]);
        work->texture[4] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[4]);
        work->texture[5] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[5]);
        work->texture[6] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[6]);
        work->texture[7] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[7]);
        work->texture[8] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[8]);
        work->texture[9] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[9]);
        work->texture[10] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant832_D4D4C[10]);
        work->texture[11] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant832_D4D4C[10]);
        work->ambient = 0x800;
        work->launched = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->unknown_2D96 = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 4, &rot, -16);
    } else if (work->elapsed < work->config->cue[0]) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rot, work->config->steps >> 1);
    } else if (work->elapsed < work->config->cue[1]) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rot, 0);
    } else if (work->elapsed < work->config->cue[2]) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -8);
    } else if (work->elapsed < work->config->launch[2]) {
        Model_QueueCameraOrbitFromPreset(slot, 3, &rot, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -8);
    }
    if (work->launched == 0) {
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
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    setPolyFT4(ft4);
    setPolyG3(g3);
    setPolyG4(g4);
    setPolyGT4(gt4);
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if ((work->launched == 0 && work->elapsed >= work->config->start) ||
        (work->launched == 1 && work->elapsed >= work->config->launch[0]) ||
        (work->launched == 2 && work->elapsed >= work->config->launch[1]) ||
        (work->launched == 3 && work->elapsed >= work->config->launch[2])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->launched], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        pos.vx += RANDOM_SPREAD(rot.vx / 2);
        pos.vy += RANDOM_SPREAD(rot.vy / 2);
        /* The depth jitter only pushes the target further from the caster. */
        pos.vz += -direction * (rot.vz / 2) * (rand() % 4096) / 4096;
        setVector(&work->pos[work->launched], m.t[0], m.t[1], m.t[2]);
        setVector(&work->step[work->launched], (pos.vx - work->pos[work->launched].vx) / work->config->steps,
                  (pos.vy - work->pos[work->launched].vy) / work->config->steps,
                  (pos.vz - work->pos[work->launched].vz) / work->config->steps);
        setVector(&work->angle[work->launched],
                  ratan2(work->step[work->launched].vz, work->step[work->launched].vy) - 0x400,
                  direction * ratan2(work->step[work->launched].vx, work->step[work->launched].vz), 0);
        addVector(&work->pos[work->launched], &work->step[work->launched]);
        setVector(&work->target[work->launched], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 16; i++) {
            setVector(&work->trail[work->launched][i], m.t[0], m.t[1], m.t[2]);
        }
        work->trail_head[work->launched] = (work->trail_head[work->launched] + 1) % 16;
        work->launched++;
    }

    for (i = 0; i < work->launched; i++) {
        if (work->arrived[i] == 0) {
            ft4->tpage = work->texture[0] >> 16;
            ft4->clut = work->texture[0];
            setRGB0(ft4, work->config->r, work->config->g, work->config->b);
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
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                              (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
            setUV4(ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
            }
        }

        if (work->bolt[i][0] || work->bolt[i][1] || work->bolt[i][2]) {
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
                /* Each pass draws one quadrant of the star. "sign *= -1" keeps the
                 * re-extension of the s16 side; the corner heights go through y0
                 * so the negation joins before a single store. */
                sign = 1;
                ft4->tpage = work->tpage;
                ft4->clut = work->clut;
                setUV4(ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(ft4, work->bolt[i][0], work->bolt[i][1], work->bolt[i][2]);
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    quad[0].vx = -sign * work->config->ring_size;
                    y0 = work->config->ring_size;
                    if (k - 2 >= 0) {
                        y0 = -y0;
                    }
                    quad[0].vy = y0;
                    quad[0].vz = 0;
                    quad[1].vx = 0;
                    y0 = work->config->ring_size;
                    if (k - 2 >= 0) {
                        y0 = -y0;
                    }
                    quad[1].vy = y0;
                    quad[1].vz = 0;
                    setVector(&quad[2], -sign * work->config->ring_size, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }
                setRGB0(g3, work->bolt[i][0], work->bolt[i][1], work->bolt[i][2]);
                setRGB1(g3, work->bolt[i][0], work->bolt[i][1], work->bolt[i][2]);
                setRGB2(g3, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * (s32)((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * (s32)((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * (s32)((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * (s32)((u32)-work->step[i].vz << 1));
                }
                for (k = 0; k < 4; k++) {
                    copyVector(&rot, &work->angle[i]);
                    rot.vz += k * 512 % 4096;
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3->x0, (PSXLONG *)&g3->x1,
                                      (PSXLONG *)&g3->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g3, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3->x0, (PSXLONG *)&g3->x1,
                                      (PSXLONG *)&g3->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g3, ot, (u16)otz, 3);
                    }
                }
            }
        }

        if (direction > 0) {
            if (work->pos[i].vz < work->target[i].vz) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->flight[i] == 8) {
                    work->flight[i] = 7;
                }
                if (work->pos[i].vz >= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->arrived[i] = 1;
                }
            }
        } else {
            if (work->pos[i].vz > work->target[i].vz) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->flight[i] == 8) {
                    work->flight[i] = 7;
                }
                if (work->pos[i].vz <= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->arrived[i] = 1;
                }
            }
        }

        if (work->trail_head[i] > work->tail[i]) {
            if (work->bolt[i][0] || work->bolt[i][1] || work->bolt[i][2]) {
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
                /* Writing the bound as "- (tail + 1)" keeps "trail_head - 1" computed first. */
                /*
                 * The last segment can reach j == 0, so retail reads trail[i][-1], which
                 * aliases the previous bolt's trail[i - 1][15] (target[3] for bolt 0).
                 */
                for (j = work->trail_head[i] - 1; j >= work->trail_head[i] - (work->tail[i] + 1); j--) {
                    setRGB0(g4, work->bolt[i][0] / k, work->bolt[i][1] / k, work->bolt[i][2] / k);
                    setRGB1(g4, work->bolt[i][0] / k, work->bolt[i][1] / k, work->bolt[i][2] / k);
                    setRGB2(g4, work->bolt[i][0] / (k + 1), work->bolt[i][1] / (k + 1), work->bolt[i][2] / (k + 1));
                    setRGB3(g4, work->bolt[i][0] / (k + 1), work->bolt[i][1] / (k + 1), work->bolt[i][2] / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy - work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy + work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4->x0,
                                      (PSXLONG *)&g4->x1, (PSXLONG *)&g4->x2, (PSXLONG *)&g4->x3, &p, &flag) + 64;
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g4, ot, (u16)otz, 1);
                    }
                    setVector(&quad[0], work->trail[i][j].vx - work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx + work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx - work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx + work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4->x0,
                                      (PSXLONG *)&g4->x1, (PSXLONG *)&g4->x2, (PSXLONG *)&g4->x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g4, ot, (u16)otz, 1);
                    }
                    k++;
                }
                if (work->arrived[i]) {
                    if (work->bolt[i][0] >= 16) {
                        work->bolt[i][0] -= work->config->fade;
                    } else {
                        work->bolt[i][0] = 0;
                    }
                    if (work->bolt[i][1] >= 16) {
                        work->bolt[i][1] -= work->config->fade;
                    } else {
                        work->bolt[i][1] = 0;
                    }
                    if (work->bolt[i][2] >= 16) {
                        work->bolt[i][2] -= work->config->fade;
                    } else {
                        work->bolt[i][2] = 0;
                    }
                } else {
                    work->tail[i]++;
                }
            }
        }

        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        ft4->tpage = work->texture[1] >> 16;
        ft4->clut = work->texture[1];
        setRGB0(ft4, 0x80, 0x80, 0x80);
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
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                  (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                setUV4(ft4, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->puff_frame[i][j] / 8 * 32,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->puff_frame[i][j] / 8 * 32,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->puff_frame[i][j] / 8 * 32 + 0x1F,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->puff_frame[i][j] / 8 * 32 + 0x1F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 0);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->flare[i][0] || work->flare[i][1] || work->flare[i][2]) {
            ft4->tpage = work->tpage;
            ft4->clut = work->clut;
            setUV4(ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
            setRGB0(ft4, work->flare[i][0], work->flare[i][1], work->flare[i][2]);
            GsSetLsMatrix(&light);
            RotTrans(&work->origin[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                sign = 1;
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * 12, k - 2 >= 0 ? -12 : 12, 0);
                    setVector(&quad[1], 0, k - 2 >= 0 ? -12 : 12, 0);
                    setVector(&quad[2], -sign * 12, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    /* A separate index for the offset loops leaves k in its saved
                     * register; the image keeps this one in a scratch register. */
                    for (l = 0; l < 4; l++) {
                        addVector(&quad[l], &work->flares[i][j]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }
                addVector(&work->flares[i][j], &work->flare_velocity[i][j]);
            }
            ft4->tpage = work->texture[11] >> 16;
            ft4->clut = work->texture[11];
            setRGB0(ft4, work->flare[i][0], work->flare[i][1], work->flare[i][2]);
            setUV4(ft4, 0xC0, 0, 0xFF, 0, 0xC0, 0x3F, 0xFF, 0x3F);
            setVector(&quad[0], -0x40, -0x40, 0);
            setVector(&quad[1], 0x40, -0x40, 0);
            setVector(&quad[2], -0x40, 0x40, 0);
            setVector(&quad[3], 0x40, 0x40, 0);
            for (j = 0; j < 8; j++) {
                copyVector(&pos, &work->origin[i]);
                addVector(&pos, &work->embers[i][j]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                  (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                }
                addVector(&work->embers[i][j], &work->ember_velocity[i][j]);
            }
            if (work->flare[i][0] >= 16) {
                work->flare[i][0] -= 15;
            } else {
                work->flare[i][0] = 0;
            }
            if (work->flare[i][1] >= 16) {
                work->flare[i][1] -= 15;
            } else {
                work->flare[i][1] = 0;
            }
            if (work->flare[i][2] >= 16) {
                work->flare[i][2] -= 15;
            } else {
                work->flare[i][2] = 0;
            }
        }

        if (work->arrived[i] && (work->glow[i][0] || work->glow[i][1] || work->glow[i][2])) {
            GsSetLsMatrix(&light);
            RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
            saved = m;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            sign = 1;
            ft4->tpage = work->tpage;
            ft4->clut = work->clut;
            setUV4(ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
            setRGB0(ft4, work->glow[i][0], work->glow[i][1], work->glow[i][2]);
            for (k = 0; k < 4; k++) {
                sign *= -1;
                quad[0].vx = -sign * work->config->glow_size;
                y0 = work->config->glow_size;
                if (k - 2 >= 0) {
                    y0 = -y0;
                }
                quad[0].vy = y0;
                quad[0].vz = 0;
                quad[1].vx = 0;
                y0 = work->config->glow_size;
                if (k - 2 >= 0) {
                    y0 = -y0;
                }
                quad[1].vy = y0;
                quad[1].vz = 0;
                setVector(&quad[2], -sign * work->config->glow_size, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                  (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                }
            }

            for (l = 0; l < 16; l++) {
                sign = 1;
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    quad[0].vx = -sign * 16;
                    if (k - 2 >= 0) {
                        quad[0].vy = -16;
                    } else {
                        quad[0].vy = 16;
                    }
                    quad[0].vz = 0;
                    quad[1].vx = 0;
                    if (k - 2 >= 0) {
                        quad[1].vy = -16;
                    } else {
                        quad[1].vy = 16;
                    }
                    quad[1].vz = 0;
                    setVector(&quad[2], -sign * 16, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    for (j = 0; j < 4; j++) {
                        addVector(&quad[j], &work->motes[i][l]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }
                addVector(&work->motes[i][l], &work->mote_velocity[i][l]);
            }

            ft4->tpage = work->texture[10] >> 16;
            ft4->clut = work->texture[10];
            setRGB0(ft4, work->glow[i][0], work->glow[i][1], work->glow[i][2]);
            setUV4(ft4, 0xC0, 0, 0xFF, 0, 0xC0, 0x3F, 0xFF, 0x3F);
            for (j = 0; j < 64; j++) {
                setVector(&quad[0], -0x40, -0x40, 0);
                setVector(&quad[1], 0x40, -0x40, 0);
                setVector(&quad[2], -0x40, 0x40, 0);
                setVector(&quad[3], 0x40, 0x40, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&quad[k], &work->shards[i][j]);
                }
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                  (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                }
                addVector(&work->shards[i][j], &work->shard_velocity[i][j]);
            }

            ft4->tpage = work->texture[1] >> 16;
            ft4->clut = work->texture[1];
            setRGB0(ft4, 0x80, 0x80, 0x80);
            if (work->debris_frame[i] < 16) {
                setUV4(ft4, work->debris_frame[i] / 2 % 4 * 32 + 0x40, work->debris_frame[i] / 8 * 32,
                       work->debris_frame[i] / 2 % 4 * 32 + 0x5F, work->debris_frame[i] / 8 * 32,
                       work->debris_frame[i] / 2 % 4 * 32 + 0x40, work->debris_frame[i] / 8 * 32 + 0x1F,
                       work->debris_frame[i] / 2 % 4 * 32 + 0x5F, work->debris_frame[i] / 8 * 32 + 0x1F);
                for (j = 0; j < 16; j++) {
                    setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
                    setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
                    setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
                    setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&quad[k], &work->debris[i][j]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                    addVector(&work->debris[i][j], &work->debris_velocity[i][j]);
                }
                work->debris_frame[i]++;
            }

            setRGB0(ft4, 0x40, 0x40, 0x40);
            for (j = 0; j < work->spark_count[i]; j++) {
                if (work->spark_frame[i][j] < 8) {
                    ft4->tpage = work->texture[work->spark_frame[i][j] + 2] >> 16;
                    ft4->clut = work->texture[work->spark_frame[i][j] + 2];
                    setUV4(ft4, work->spark_frame[i][j] % 4 * 64, work->spark_frame[i][j] / 4 * 64 + 0x40,
                           work->spark_frame[i][j] % 4 * 64 + 0x3F, work->spark_frame[i][j] / 4 * 64 + 0x40,
                           work->spark_frame[i][j] % 4 * 64, work->spark_frame[i][j] / 4 * 64 + 0x7F,
                           work->spark_frame[i][j] % 4 * 64 + 0x3F, work->spark_frame[i][j] / 4 * 64 + 0x7F);
                    setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                    for (l = 0; l < 4; l++) {
                        addVector(&quad[l], &work->sparks[i][j]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                      (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                    work->spark_frame[i][j]++;
                }
            }

            gt4->tpage = work->tpage;
            gt4->clut = work->clut;
            setSemiTrans(gt4, 1);
            setUV4(gt4, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
            setVector(&scale, 0x1000, work->stretch[i], 0x1000);
            setRGB0(gt4, 0, 0, 0);
            setRGB1(gt4, 0, 0, 0);
            setRGB2(gt4, work->glow[i][0], work->glow[i][1], work->glow[i][2]);
            setRGB3(gt4, work->glow[i][0], work->glow[i][1], work->glow[i][2]);
            setVector(&quad[0], -4, -0xC0, 0);
            setVector(&quad[1], 4, -0xC0, 0);
            setVector(&quad[2], -12, 0, 0);
            setVector(&quad[3], 12, 0, 0);
            for (j = 0; j < 32; j++) {
                m = saved;
                RotMatrix(&work->blades[i][j], &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4->x0, (PSXLONG *)&gt4->x1,
                                  (PSXLONG *)&gt4->x2, (PSXLONG *)&gt4->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(gt4, ot, (u16)otz);
                }
            }

            setVector(&rot, 0, 0, 0);
            /*
             * Retail indexes the fan growth with i - count, which is negative for every
             * launched bolt (i < count), so these accesses alias flight[4 + i - count].
             */
            setVector(&scale, work->growth[i - work->config->count] + 0x1000,
                      work->growth[i - work->config->count] + 0x1000,
                      (work->growth[i - work->config->count] >> 3) + 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setRGB0(ft4, work->glow[i][0] >> 2, work->glow[i][1] >> 2, work->glow[i][2] >> 2);
            ft4->tpage = work->tpage;
            ft4->clut = work->clut;
            setUV4(ft4, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
            for (j = 0; j < 16; j++) {
                otz = RotAverage4(&work->fan[j][1], &work->fan[(j + 1) % 16][1], &work->fan[j][0],
                                  &work->fan[(j + 1) % 16][0], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                  (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                }
            }
            if (work->growth[i - work->config->count] < 20000) {
                work->growth[i - work->config->count] += work->config->grow;
            } else {
                work->growth[i - work->config->count] += work->config->grow_late;
            }
            work->spark_count[i] += 8;
            if (work->spark_count[i] > work->config->spark_count) {
                work->spark_count[i] = work->config->spark_count;
            }
            if (work->stretch[i] < 0x2000) {
                work->stretch[i] += 0x800;
            } else {
                work->stretch[i] += 0x100;
            }
            if (work->glow[i][0] >= 16) {
                work->glow[i][0] -= 15;
            } else {
                work->glow[i][0] = 0;
            }
            if (work->glow[i][1] >= 16) {
                work->glow[i][1] -= 15;
            } else {
                work->glow[i][1] = 0;
            }
            if (work->glow[i][2] >= 16) {
                work->glow[i][2] -= 15;
            } else {
                work->glow[i][2] = 0;
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 5 && work->glow[work->config->count - 1][0] == 0 &&
        work->glow[work->config->count - 1][1] == 0 && work->glow[work->config->count - 1][2] == 0) {
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
