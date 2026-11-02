#include "../../types.h"
#include "variant825_entry.h"

/* Header 825's entry: between the start and end times a new bolt leaves a
 * model point every period, up to the row's bolt count, and flies towards a
 * jittered point on the opposing slot, wrapped in a ring of sprites and
 * spinning Gouraud strips and trailing a ribbon and smoke puffs; the bolt
 * colours fade once it arrives. On arrival each bolt flares into a glow
 * with two fans of rays, a burst of animated sparks and a growing spray of
 * animated Gouraud streaks while the ambient light dims and recovers. */

/* The streak sprites' bottom texel row: the French image samples one row
 * higher. */
#ifdef VERSION_FRENCH
#define MODEL825_STREAK_V_BOTTOM 0xD6
#else
#define MODEL825_STREAK_V_BOTTOM 0xD7
#endif

static const VECTOR gModelVariant825_D3CFC = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 ModelVariant825_HandleRequest(u8 *context, s32 command)
{
    Model825State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_G3 g3;
    POLY_G4 g4;
    POLY_FT4 puff;
    POLY_FT4 glow;
    POLY_GT4 gt4;
    POLY_FT4 ft4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, j, k;
    s32 a, b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant825_D3CFC;
    work = (Model825State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant825_D3D98[command];
        for (i = 0; i < work->config->count; i++) {
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
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread));
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->bolts.s.arrived[i] = 0;
            work->bolt_r[i] = work->config->bolt_r;
            work->bolt_g[i] = work->config->bolt_g;
            work->bolt_b[i] = work->config->bolt_b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            work->ray_r[i] = work->config->ray_r;
            work->ray_g[i] = work->config->ray_g;
            work->ray_b[i] = work->config->ray_b;
            work->tail[i] = 0;
            work->streak_count[i] = 0;
            work->bolts.s.trail_head[i] = 0;
            work->flight[i] = 0;
            for (j = 0; j < 16; j++) {
                setVector(&work->streaks[i][j], RANDOM_SPREAD(work->config->streak_spread),
                          -work->config->streak_spread * (rand() % 4096) / 4096,
                          RANDOM_SPREAD(work->config->streak_spread));
                work->streak_frame[i][j] = rand() % 4;
            }
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
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant825_D3D0C[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant825_D3D0C[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant825_D3D0C[2]);
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant825_D3D0C[3]);
        work->texture[4] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant825_D3D0C[4]);
        work->texture[5] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant825_D3D0C[4]);
        work->ambient = 0x800;
        work->bolts.s.head = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->frames = 0;
        work->unknown_1AD8 = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -8);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 16);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 16);
    }
    if (work->bolts.s.head == 0) {
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
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->start && work->elapsed < work->config->end &&
        work->frames % work->config->period == 0 && work->bolts.s.head < work->config->count) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->bolts.s.head], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        pos.vx += RANDOM_SPREAD(rot.vx / 2);
        pos.vy += RANDOM_SPREAD(rot.vy / 2);
        pos.vz += RANDOM_SPREAD(rot.vz / 2);
        setVector(&work->pos[work->bolts.s.head], m.t[0], m.t[1], m.t[2]);
        setVector(&work->step[work->bolts.s.head], (pos.vx - work->pos[work->bolts.s.head].vx) / work->config->steps,
                  (pos.vy - work->pos[work->bolts.s.head].vy) / work->config->steps,
                  (pos.vz - work->pos[work->bolts.s.head].vz) / work->config->steps);
        setVector(&work->angle[work->bolts.s.head], ratan2(work->step[work->bolts.s.head].vz, work->step[work->bolts.s.head].vy) - 0x400,
                  direction * ratan2(work->step[work->bolts.s.head].vx, work->step[work->bolts.s.head].vz), 0);
        addVector(&work->pos[work->bolts.s.head], &work->step[work->bolts.s.head]);
        setVector(&work->target[work->bolts.s.head], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 16; i++) {
            setVector(&work->trail[work->bolts.s.head][i], m.t[0], m.t[1], m.t[2]);
        }
        /* The original indexes the trail heads with the finished loop counter. */
        work->bolts.s.trail_head[work->bolts.s.head] = (work->bolts.flat[i] + 1) % 16;
        work->bolts.s.head++;
    }

    for (i = 0; i < work->bolts.s.head; i++) {
        if (work->bolts.s.arrived[i] == 0) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
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
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
        }

        if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
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
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setRGB0(&ft4, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setVector(&quad[0], -work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[1], work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[2], -work->config->ring_size, work->config->ring_size, 0);
                setVector(&quad[3], work->config->ring_size, work->config->ring_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }

                setPolyG3(&g3);
                setRGB0(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB1(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB2(&g3, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                /* The doubled depth is a shift of the negated step; "* 2" would
                 * fold into the quotient instead. */
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
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
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                }
            }
            if (work->bolts.s.arrived[i]) {
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
            }
        }

        if (direction > 0) {
            if (work->pos[i].vz < work->target[i].vz) {
                copyVector(&work->trail[i][work->bolts.s.trail_head[i]], &work->pos[i]);
                work->bolts.s.trail_head[i] = (work->bolts.s.trail_head[i] + 1) % 16;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->flight[i] == 8) {
                    work->flight[i] = 7;
                }
                if (work->pos[i].vz >= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->bolts.s.arrived[i] = 1;
                }
            }
        } else {
            if (work->pos[i].vz > work->target[i].vz) {
                copyVector(&work->trail[i][work->bolts.s.trail_head[i]], &work->pos[i]);
                work->bolts.s.trail_head[i] = (work->bolts.s.trail_head[i] + 1) % 16;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->flight[i] == 8) {
                    work->flight[i] = 7;
                }
                if (work->pos[i].vz <= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->bolts.s.arrived[i] = 1;
                }
            }
        }

        setPolyG4(&g4);
        if (work->bolts.s.trail_head[i] > work->tail[i]) {
            if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
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
                for (j = work->bolts.s.trail_head[i] - 1; j >= work->bolts.s.trail_head[i] - (work->tail[i] + 1); j--) {
                    setRGB0(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB1(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB2(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setRGB3(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
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
                if (work->bolts.s.arrived[i] != 1) {
                    work->tail[i]++;
                }
            }
        }

        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        setPolyFT4(&puff);
        puff.tpage = work->texture[4] >> 16;
        puff.clut = work->texture[4];
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
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&puff.x0,
                                  (PSXLONG *)&puff.x1, (PSXLONG *)&puff.x2, (PSXLONG *)&puff.x3, &p, &flag);
                if (work->puff_frame[i][j] < 8) {
                    setUV4(&puff, work->puff_frame[i][j] / 2 % 4 * 32, 0x40,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x40,
                           work->puff_frame[i][j] / 2 % 4 * 32, 0x5F,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x5F);
                } else {
                    setUV4(&puff, work->puff_frame[i][j] / 2 % 4 * 32, 0x60,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x60,
                           work->puff_frame[i][j] / 2 % 4 * 32, 0x7F,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x7F);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&puff, ot, (u16)otz, 1);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->bolts.s.arrived[i]) {
            if (work->glow_r[i] || work->glow_g[i] || work->glow_b[i]) {
                setPolyFT4(&glow);
                glow.tpage = work->texture[1] >> 16;
                glow.clut = work->texture[1];
                setRGB0(&glow, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                  (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                setUV4(&glow, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                }

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->ray_r[i], work->ray_g[i], work->ray_b[i]);
                setRGB2(&g3, work->ray_r[i], work->ray_g[i], work->ray_b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    setVector(&quad[0], -work->config->ray_width, 0, 0);
                    setVector(&quad[1], work->config->ray_width, 0, 0);
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }
                setVector(&scale, 0xC00, 0xC00, 0xC00);
                GsSetLsMatrix(&light);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB2(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    setVector(&quad[0], -work->config->ray_width, 0, 0);
                    setVector(&quad[1], work->config->ray_width, 0, 0);
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }

                setPolyFT4(&glow);
                glow.tpage = work->texture[3] >> 16;
                glow.clut = work->texture[3];
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                setRGB0(&glow, 0x80, 0x80, 0x80);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->spark_count[i]; j++) {
                    if (work->spark_frame[i][j] < 8) {
                        setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                        setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                        setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                        setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                        addVector(&quad[0], &work->sparks[i][j]);
                        addVector(&quad[1], &work->sparks[i][j]);
                        addVector(&quad[2], &work->sparks[i][j]);
                        addVector(&quad[3], &work->sparks[i][j]);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                          (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p,
                                          &flag);
                        if (work->spark_frame[i][j] < 4) {
                            setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0x40,
                                   work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x40,
                                   work->spark_frame[i][j] % 4 * 32 + 0x80, 0x5F,
                                   work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x5F);
                        } else {
                            setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0x60,
                                   work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x60,
                                   work->spark_frame[i][j] % 4 * 32 + 0x80, 0x7F,
                                   work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x7F);
                        }
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                        }
                        work->spark_frame[i][j]++;
                    }
                }

                setPolyGT4(&gt4);
                gt4.tpage = work->texture[2] >> 16;
                gt4.clut = work->texture[2];
                setRGB0(&gt4, 0, 0, 0);
                setRGB1(&gt4, 0, 0, 0);
                setRGB2(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB3(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                for (j = 0; j < work->streak_count[i]; j++) {
                    setVector(&quad[0], -work->config->streak_width, -work->config->streak_height, 0);
                    setVector(&quad[1], work->config->streak_width, -work->config->streak_height, 0);
                    setVector(&quad[2], -work->config->streak_width, work->config->streak_height, 0);
                    setVector(&quad[3], work->config->streak_width, work->config->streak_height, 0);
                    addVector(&quad[0], &work->streaks[i][j]);
                    addVector(&quad[1], &work->streaks[i][j]);
                    addVector(&quad[2], &work->streaks[i][j]);
                    addVector(&quad[3], &work->streaks[i][j]);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4.x0,
                                      (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    setUV4(&gt4, work->streak_frame[i][j] * 64, 0x80, work->streak_frame[i][j] * 64 + 0x3F, 0x80,
                           work->streak_frame[i][j] * 64, MODEL825_STREAK_V_BOTTOM, work->streak_frame[i][j] * 64 + 0x3F, MODEL825_STREAK_V_BOTTOM);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                    }
                    work->streak_frame[i][j] = (work->streak_frame[i][j] + 1) % 4;
                }
                work->spark_count[i] += step;
                if (work->spark_count[i] > work->config->spark_count) {
                    work->spark_count[i] = work->config->spark_count;
                }
                if (++work->streak_count[i] > 16) {
                    work->streak_count[i] = 16;
                }

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
                if (work->ray_r[i] >= 16) {
                    work->ray_r[i] -= work->config->ray_fade;
                } else {
                    work->ray_r[i] = 0;
                }
                if (work->ray_g[i] >= 16) {
                    work->ray_g[i] -= work->config->ray_fade;
                } else {
                    work->ray_g[i] = 0;
                }
                if (work->ray_b[i] >= 16) {
                    work->ray_b[i] -= work->config->ray_fade;
                } else {
                    work->ray_b[i] = 0;
                }
            }
        }
    }

    work->elapsed += step;
    work->frames++;
    PopMatrix();
    if (work->phase == 3 && work->glow_r[work->bolts.s.head - 1] == 0 && work->glow_g[work->bolts.s.head - 1] == 0 &&
        work->glow_b[work->bolts.s.head - 1] == 0) {
        return 2;
    }
    if (work->elapsed > work->config->end && work->bolts.s.arrived[work->bolts.s.head - 1] == 1 && work->phase == 0) {
        work->phase = 2;
        return 1;
    }
    if (work->bolts.s.arrived[0] == 1 && work->phase == 0) {
        return 4;
    }
    return 0;
}
