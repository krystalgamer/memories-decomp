#include "../../types.h"
#include "variant805_entry.h"

/* Header 805's entry: a charge sprite builds between two model points
 * while a flash flickers in front of the caster; at launch the bolts fly
 * to jittered points on the opposing slot with ribbon trails, and on
 * arrival each flares into a glow with two fans of rays and a ring of
 * animated sprites while the ambient light dims and recovers. */

static const VECTOR D_8013DC9C = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model805State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_FT4 glow;
    POLY_FT4 flash;
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
    s32 slot;
    s32 i, j, k;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013DC9C;
    work = (Model805State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013DD00[command];
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->target[i], RANDOM_SPREAD(pos.vx / 2), RANDOM_SPREAD(pos.vy / 2) - 350,
                      direction * 450);
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            work->arrived[i] = 0;
            work->trail_head[i] = 1;
            work->tail[i] = 0;
            work->ring_shown[i] = 0;
            work->bolt_r[i] = work->config->r;
            work->bolt_g[i] = work->config->g;
            work->bolt_b[i] = work->config->b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            for (j = 0; j < work->config->ring_count; j++) {
                setVector(&work->ring[i][j], RANDOM_SPREAD(work->config->ring_spread),
                          RANDOM_SPREAD(work->config->ring_spread),
                          work->config->ring_spread * (rand() % 4096) / 4096);
                work->frame[i][j] = rand() % 9;
                work->ring_r[i][j] = work->config->ring_r;
                work->ring_g[i][j] = work->config->ring_g;
                work->ring_b[i][j] = work->config->ring_b;
            }
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread),
                          -direction * work->config->ray_spread * (rand() % 4096) / 4096);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DCAC[i]);
        }
        work->flash = 0;
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->launch) {
        if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(slot, 0, &rot, -16);
        } else {
            Model_QueueCameraOrbitFromPreset(slot, 1, &rot, -16);
        }
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, -8);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 9, &rot, -8);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 2;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed <= work->config->charge_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->charge_part), &m);
        setVector(&rot, m.t[0], m.t[1], m.t[2]);
        setVector(&rot, direction * (direction * ratan2(pos.vz - rot.vz, pos.vy - rot.vy) - 0x400), 0, 0);
        setPolyFT4(&ft4);
        setRGB0(&ft4, 0x80, 0x80, 0x80);
        ft4.tpage = work->texture[2] >> 16;
        ft4.clut = work->texture[2];
        setVector(&quad[0], 0, -work->config->size, 0);
        setVector(&quad[1], 0, -work->config->size, direction * work->config->depth);
        setVector(&quad[2], 0, work->config->size, 0);
        setVector(&quad[3], 0, work->config->size, direction * work->config->depth);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag) + 2;
        setUV4(&ft4, 0, 0x40, 0x5F, 0x40, 0, 0x5F, 0x5F, 0x5F);
        setSemiTrans(&ft4, 0);
        if (otz >= 0) {
            GsSortPoly(&ft4, ot, (u16)otz);
        }
        if (work->elapsed >= work->config->flash_start) {
            pos.vz += direction * work->config->depth;
            setVector(&rot, 0, 0, 0);
            setPolyFT4(&flash);
            flash.clut = work->texture[0];
            flash.tpage = work->texture[0] >> 16;
            setRGB0(&flash, work->flash, work->flash, work->flash);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->flash_width, -work->config->flash_top, 0);
            setVector(&quad[1], work->config->flash_width, -work->config->flash_top, 0);
            setVector(&quad[2], -work->config->flash_width, work->config->flash_bottom, 0);
            setVector(&quad[3], work->config->flash_width, work->config->flash_bottom, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&flash.x0,
                              (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2, (PSXLONG *)&flash.x3, &p, &flag);
            setUV4(&flash, work->elapsed % 9 * 32, 0x60, work->elapsed % 9 * 32 + 0x1F, 0x60,
                   work->elapsed % 9 * 32, 0x9F, work->elapsed % 9 * 32 + 0x1F, 0x9F);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 1);
            work->flash += work->config->fade;
            if (work->flash > 0x80) {
                work->flash = 0x80;
            }
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    }

    if (work->elapsed <= work->config->launch) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->aim_part), &m);
        setVector(&rot, m.t[0], m.t[1], m.t[2]);
        setVector(&rot, direction * (direction * ratan2(rot.vz - pos.vz, rot.vy - pos.vy) - 0x400), 0, 0);
        setPolyFT4(&ft4);
        setRGB0(&ft4, 0x80, 0x80, 0x80);
        ft4.tpage = work->texture[2] >> 16;
        ft4.clut = work->texture[2];
        setVector(&quad[0], 0, -work->config->size, 0);
        setVector(&quad[1], 0, -work->config->size, direction * work->config->depth);
        setVector(&quad[2], 0, work->config->size, 0);
        setVector(&quad[3], 0, work->config->size, direction * work->config->depth);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, 0, 0x40, 0x5F, 0x40, 0, 0x5F, 0x5F, 0x5F);
        setSemiTrans(&ft4, 0);
        if (otz >= 0) {
            GsSortPoly(&ft4, ot, (u16)otz);
        }
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->angle[i],
                      direction * (direction * ratan2(rot.vz - pos.vz, rot.vy - pos.vy) - 0x400), 0, 0);
            copyVector(&work->pos[i], &pos);
            setVector(&work->step[i], (work->target[i].vx - work->pos[i].vx) / work->config->steps,
                      (work->target[i].vy - work->pos[i].vy) / work->config->steps,
                      (work->target[i].vz - work->pos[i].vz) / work->config->steps);
            for (j = 0; j < 16; j++) {
                copyVector(&work->trail[i][j], &work->pos[i]);
            }
        }
        pos.vz += direction * work->config->depth;
        setVector(&rot, 0, 0, 0);
        setPolyFT4(&flash);
        flash.clut = work->texture[0];
        flash.tpage = work->texture[0] >> 16;
        setRGB0(&flash, work->flash, work->flash, work->flash);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setVector(&quad[0], -work->config->flash_width, -work->config->flash_top, 0);
        setVector(&quad[1], work->config->flash_width, -work->config->flash_top, 0);
        setVector(&quad[2], -work->config->flash_width, work->config->flash_bottom, 0);
        setVector(&quad[3], work->config->flash_width, work->config->flash_bottom, 0);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1,
                          (PSXLONG *)&flash.x2, (PSXLONG *)&flash.x3, &p, &flag);
        setUV4(&flash, work->elapsed % 9 * 32, 0x60, work->elapsed % 9 * 32 + 0x1F, 0x60,
               work->elapsed % 9 * 32, 0x9F, work->elapsed % 9 * 32 + 0x1F, 0x9F);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 1);
        PopMatrix();
        work->elapsed += step;
        return 0;
    }

    for (i = 0; i < work->config->count; i++) {
        if (work->arrived[i] == 0) {
            setVector(&work->angle[i], ratan2(work->step[i].vz, work->step[i].vy) - 0x400,
                      direction * ratan2(work->step[i].vx, work->step[i].vz), 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, 0x80, 0x80, 0x80);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->size, 0);
            setVector(&quad[1], 0, -work->config->size, direction * work->config->depth);
            setVector(&quad[2], 0, work->config->size, 0);
            setVector(&quad[3], 0, work->config->size, direction * work->config->depth);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0x40, 0x5F, 0x40, 0, 0x5F, 0x5F, 0x5F);
            if (work->arrived[i] == 0) {
                setSemiTrans(&ft4, 0);
            } else {
                setSemiTrans(&ft4, 1);
            }
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }

            copyVector(&pos, &work->pos[i]);
            pos.vz += direction * work->config->depth;
            setVector(&rot, 0, 0, 0);
            setPolyFT4(&flash);
            flash.clut = work->texture[0];
            flash.tpage = work->texture[0] >> 16;
            setRGB0(&flash, work->flash, work->flash, work->flash);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->flash_width - direction * 32, -work->config->flash_top, 0);
            setVector(&quad[1], work->config->flash_width - direction * 32, -work->config->flash_top, 0);
            setVector(&quad[2], -work->config->flash_width, work->config->flash_bottom, 0);
            setVector(&quad[3], work->config->flash_width, work->config->flash_bottom, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&flash.x0,
                              (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2, (PSXLONG *)&flash.x3, &p, &flag);
            setUV4(&flash, work->elapsed % 9 * 32, 0x60, work->elapsed % 9 * 32 + 0x1F, 0x60,
                   work->elapsed % 9 * 32, 0x9F, work->elapsed % 9 * 32 + 0x1F, 0x9F);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 1);
            pos.vz -= direction * work->config->depth;

            copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
            work->trail_head[i] = (work->trail_head[i] + 1) % 16;
            addVector(&work->pos[i], &work->step[i]);
            /* Both directions share one arrival store through the label. */
            if (direction > 0) {
                if (work->pos[i].vz > direction * 450) {
                    goto arrive;
                }
            } else if (work->pos[i].vz < direction * 450) {
            arrive:
                if (work->arrived[i] == 0) {
                    work->arrived[i] = 1;
                }
            }
        }

        if (work->trail_head[i] > work->tail[i]) {
            if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
                setPolyG4(&g4);
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                /* "head - (tail + 1)" keeps "head - 1" shared with the start index. */
                for (j = work->trail_head[i] - 1, k = 1; j >= work->trail_head[i] - (work->tail[i] + 1); j--, k++) {
                    setRGB0(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB1(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB2(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setRGB3(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx,
                              work->trail[i][j].vy - (u16)(work->config->size / 3) / k, work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx,
                              work->trail[i][j].vy + (u16)(work->config->size / 3) / k, work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j].vy - (u16)(work->config->size / 3) / (k + 1),
                              work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j].vy + (u16)(work->config->size / 3) / (k + 1),
                              work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                }
            }
        }

        if (work->arrived[i]) {
            for (j = 0; j < work->ring_shown[i]; j++) {
                if (work->ring_r[i][j] || work->ring_g[i][j] || work->ring_b[i][j]) {
                    setPolyFT4(&flash);
                    flash.clut = work->texture[0];
                    flash.tpage = work->texture[0] >> 16;
                    setRGB0(&flash, work->ring_r[i][j], work->ring_g[i][j], work->ring_b[i][j]);
                    copyVector(&pos, &work->target[i]);
                    addVector(&pos, &work->ring[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    setVector(&quad[0], -work->config->ring_width, -work->config->ring_height, 0);
                    setVector(&quad[1], work->config->ring_width, -work->config->ring_height, 0);
                    setVector(&quad[2], -work->config->ring_width, work->config->ring_height, 0);
                    setVector(&quad[3], work->config->ring_width, work->config->ring_height, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&flash.x0,
                                      (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2, (PSXLONG *)&flash.x3, &p,
                                      &flag);
                    setUV4(&flash, work->frame[i][j] % 9 * 32, 0x60, work->frame[i][j] % 9 * 32 + 0x1F, 0x60,
                           work->frame[i][j] % 9 * 32, 0x9F, work->frame[i][j] % 9 * 32 + 0x1F, 0x9F);
                    Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 1);
                    if (work->ring_r[i][j] >= 16) {
                        work->ring_r[i][j] -= work->config->ring_fade;
                    } else {
                        work->ring_r[i][j] = 0;
                    }
                    if (work->ring_g[i][j] >= 16) {
                        work->ring_g[i][j] -= work->config->ring_fade;
                    } else {
                        work->ring_g[i][j] = 0;
                    }
                    if (work->ring_b[i][j] >= 16) {
                        work->ring_b[i][j] -= work->config->ring_fade;
                    } else {
                        work->ring_b[i][j] = 0;
                    }
                    work->frame[i][j] = (work->frame[i][j] + 1) % 9;
                }
            }
            work->ring_shown[i] += work->config->ring_rate;
            if (work->ring_shown[i] > work->config->ring_count) {
                work->ring_shown[i] = work->config->ring_count;
            }

            if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i] || work->glow_r[i] || work->glow_g[i] ||
                work->glow_b[i]) {
                setVector(&rot, 0, 0, 0);
                setVector(&pos, 0, 0, 0);
                setPolyFT4(&glow);
                setRGB0(&glow, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                glow.tpage = work->texture[1] >> 16;
                glow.clut = work->texture[1];
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
                setUV4(&glow, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB2(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    copyVector(&quad[0], &work->rays[i][j]);
                    setVector(&quad[1], 0, -work->config->ray_width, 0);
                    setVector(&quad[2], 0, work->config->ray_width, 0);
                    Graphics_SubmitDrawModePacket(
                        (u32 *)&g3, ot,
                        (u16)RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                         (PSXLONG *)&g3.x2, &p, &flag),
                        1);
                }
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB2(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                m = saved;
                setVector(&scale, 0x800, 0x800, 0x800);
                GsSetLsMatrix(&light);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->config->ray_count; j++) {
                    copyVector(&quad[0], &work->rays[i][j]);
                    setVector(&quad[1], 0, -work->config->ray_width, 0);
                    setVector(&quad[2], 0, work->config->ray_width, 0);
                    Graphics_SubmitDrawModePacket(
                        (u32 *)&g3, ot,
                        (u16)RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                         (PSXLONG *)&g3.x2, &p, &flag),
                        1);
                }
                setVector(&scale, 0x1000, 0x1000, 0x1000);
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
            }
        } else {
            work->tail[i]++;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 && work->elapsed >= work->config->end && work->bolt_r[0] == 0 && work->bolt_g[0] == 0 &&
        work->bolt_b[0] == 0) {
        return 2;
    }
    /* The phase and the first arrival flag are tested as one word. */
    if (*(u32 *)&work->phase == 0x10000) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
