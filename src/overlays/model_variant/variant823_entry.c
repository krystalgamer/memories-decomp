#include "../../types.h"
#include "variant823_entry.h"
#include "../../psyq/abs.h"

/* Header 823's entry: while the active slot's model point flashes behind a
 * ring of glow, up to 36 beams leave it one per frame, swinging sideways as
 * they cross towards the opposing slot and dragging a fading ribbon. A beam
 * that passes the far line stops, fades its head and bursts into sparks
 * while the ambient light dims and recovers. */

static const VECTOR D_8013CE9C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model823State *work;
    MATRIX light;
    MATRIX m;
    POLY_GT4 gt4;
    POLY_FT4 ft4;
    POLY_G4 g4;
    POLY_G3 g3;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    SVECTOR target;
    SVECTOR zero;
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i, j, k;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CE9C;
    memset(&zero, 0, 8);
    work = (Model823State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CF00[command];
        for (i = 0; i < 36; i++) {
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->hit[i], 0, 0, 0);
            setVector(&work->origin[i], 0, 0, 0);
            work->swing[i].vx = (rand() - rand()) % work->config->swing;
            if (rand() % 2 == 0) {
                work->swing[i].vy = work->config->swing - ABS(work->swing[i].vx);
            } else {
                work->swing[i].vy = ABS(work->swing[i].vx) - work->config->swing;
            }
            work->swing[i].vz = 0;
            work->hit_flag[i] = 0;
            work->frame[i] = 0;
            work->alpha[i] = 0xFF;
            work->r[i] = work->config->r;
            work->g[i] = work->config->g;
            work->b[i] = work->config->b;
            work->drawn[i] = 0;
            for (j = 0; j < 24; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->sparks[i][j], work->config->spark_spread * (ccos(rand()) % 4096) / 4096,
                          work->config->spark_spread * (csin(rand()) % 4096) / 4096,
                          work->config->spark_spread * (ccos(rand()) % 4096) / 4096);
            }
        }
        for (i = 0; i < 12; i++) {
            setVector(&work->ring[i], work->config->ring_radius * ccos((i * 128 - 0x300) % 4096) / 4096,
                      work->config->ring_radius * csin((i * 128 - 0x300) % 4096) / 4096, 0);
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CEAC[i]);
        }
        work->count = 0;
        work->ambient = 0x800;
        work->phase = 0;
        work->blink = 0;
        work->elapsed = 0;
        work->frames = 0;
        work->flash.c.r = work->config->r;
        work->flash.c.g = work->config->g;
        work->flash.c.b = work->config->b;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rot, -16);
    } else if (work->elapsed < work->config->end) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -32);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rot, -16);
    }
    if (work->phase == 0) {
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
    Model_CopySlotPosition(slot ^ 1, (u16 *)&target);
    if (work->elapsed <= work->config->end && work->count < 36) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        setVector(&work->pos[work->count], m.t[0], m.t[1], m.t[2]);
        setVector(&work->origin[work->count], m.t[0], m.t[1], m.t[2]);
        work->swing[work->count].vz = (target.vz - m.t[2]) / work->config->period;
        for (i = 0; i < 24; i++) {
            setVector(&work->trail[work->count][i], m.t[0], m.t[1], m.t[2]);
        }
        work->frame[work->count]++;
        work->count++;
    }

    if (work->flash.word & 0xFFFFFF) {
        setPolyFT4(&ft4);
        ft4.tpage = work->texture[1] >> 16;
        ft4.clut = work->texture[1];
        setRGB0(&ft4, work->flash.c.r, work->flash.c.g, work->flash.c.b);
        setVector(&quad[0], -work->config->flash_size, -work->config->flash_size, 0);
        setVector(&quad[1], work->config->flash_size, -work->config->flash_size, 0);
        setVector(&quad[2], -work->config->flash_size, work->config->flash_size, 0);
        setVector(&quad[3], work->config->flash_size, work->config->flash_size, 0);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        RotTrans(&work->origin[work->count - 1], (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
        if (otz >= 0)
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);

        if (work->blink == 0) {
            setPolyGT4(&gt4);
            gt4.tpage = work->texture[2] >> 16;
            gt4.clut = work->texture[2];
            setUV4(&gt4, 0, 0x40, 0x7F, 0x40, 0, 0x8F, 0x7F, 0x8F);
            for (i = 0; i < 11; i++) {
                otz = RotAverage4(&work->ring[i], &work->ring[i + 1], &zero, &zero, (PSXLONG *)&gt4.x0,
                                  (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                if (i == 0) {
                    setRGB0(&gt4, 0, 0, 0);
                    setRGB1(&gt4, 0, 0, 0);
                    setRGB2(&gt4, 0, 0, 0);
                    setRGB3(&gt4, work->flash.c.r / 2, work->flash.c.g / 2, work->flash.c.b / 2);
                } else if (i == 10) {
                    setRGB0(&gt4, 0, 0, 0);
                    setRGB1(&gt4, 0, 0, 0);
                    setRGB2(&gt4, work->flash.c.r / 2, work->flash.c.g / 2, work->flash.c.b / 2);
                    setRGB3(&gt4, 0, 0, 0);
                } else {
                    setRGB0(&gt4, 0, 0, 0);
                    setRGB1(&gt4, 0, 0, 0);
                    setRGB2(&gt4, work->flash.c.r / 2, work->flash.c.g / 2, work->flash.c.b / 2);
                    setRGB3(&gt4, work->flash.c.r / 2, work->flash.c.g / 2, work->flash.c.b / 2);
                }
                if (otz >= 0)
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
            }
        }
        if (work->elapsed >= work->config->end) {
            work->blink = 0;
            if (work->flash.c.r >= 32) {
                work->flash.c.r -= work->config->fade;
            } else {
                work->flash.c.r = 0;
            }
            if (work->flash.c.g >= 32) {
                work->flash.c.g -= work->config->fade;
            } else {
                work->flash.c.g = 0;
            }
            if (work->flash.c.b >= 32) {
                work->flash.c.b -= work->config->fade;
            } else {
                work->flash.c.b = 0;
            }
        } else if (work->blink == 0) {
            work->blink = 1;
        } else {
            work->blink = 0;
        }
    }

    for (i = 0; i < work->count; i++) {
        if (work->alpha[i] != 0) {
            setPolyGT4(&gt4);
            gt4.tpage = work->texture[0] >> 16;
            gt4.clut = work->texture[0];
            setRGB0(&gt4, work->alpha[i], work->alpha[i], work->alpha[i]);
            setRGB1(&gt4, work->alpha[i], work->alpha[i], work->alpha[i]);
            setRGB2(&gt4, 0, 0, 0);
            setRGB3(&gt4, 0, 0, 0);
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
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                              (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
            setUV4(&gt4, 0x80, 0, 0xBF, 0, 0x80, 0x3F, 0xBF, 0x3F);
            if (otz >= 0)
                Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
        }
        if (ABS(work->pos[i].vz) > 450 && work->hit_flag[i] == 0) {
            if (work->phase == 0) {
                work->phase = 1;
            }
            work->hit_flag[i] = 1;
            copyVector(&work->hit[i], &work->pos[i]);
        }
        if (work->hit_flag[i] != 0) {
            if (work->alpha[i] >= 32) {
                work->alpha[i] -= work->config->fade;
            } else {
                work->alpha[i] = 0;
            }
        }
        /* The original tests the alpha array itself, which is always set. */
        if (work->alpha || work->r[i] || work->g[i] || work->b[i]) {
            copyVector(&work->trail[i][work->frame[i]], &work->pos[i]);
            setVector(&work->pos[i],
                      work->origin[i].vx +
                          work->swing[i].vx * csin(0x800 / work->config->period * work->frame[i] % 4096) / 4096,
                      work->origin[i].vy +
                          work->swing[i].vy * csin(0x800 / work->config->period * work->frame[i] % 4096) / 4096,
                      work->origin[i].vz + work->swing[i].vz * work->frame[i]);
            work->frame[i]++;
        }
        if (work->frame[i] > work->drawn[i] && (work->r[i] || work->g[i] || work->b[i])) {
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
            for (j = work->frame[i] - 1, k = 1; j >= work->frame[i] - (work->drawn[i] + 1); j--, k++) {
                setRGB0(&g4, work->r[i] / k, work->g[i] / k, work->b[i] / k);
                setRGB1(&g4, work->r[i] / k, work->g[i] / k, work->b[i] / k);
                setRGB2(&g4, work->r[i] / (k + 1), work->g[i] / (k + 1), work->b[i] / (k + 1));
                setRGB3(&g4, work->r[i] / (k + 1), work->g[i] / (k + 1), work->b[i] / (k + 1));
                setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy + work->config->top / k,
                          work->trail[i][j].vz - direction * work->config->depth);
                setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->bottom / k,
                          work->trail[i][j].vz - direction * work->config->depth);
                setVector(&quad[2], work->trail[i][j - 1].vx, work->trail[i][j - 1].vy + work->config->top / (k + 1),
                          work->trail[i][j - 1].vz - direction * work->config->depth);
                setVector(&quad[3], work->trail[i][j - 1].vx, work->trail[i][j - 1].vy + work->config->bottom / (k + 1),
                          work->trail[i][j - 1].vz - direction * work->config->depth);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                if (otz >= 0)
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 3);
            }
            work->drawn[i]++;
            if (work->hit_flag[i] != 0) {
                if (work->r[i] >= 16) {
                    work->r[i] -= work->config->trail_fade;
                } else {
                    work->r[i] = 0;
                }
                if (work->g[i] >= 16) {
                    work->g[i] -= work->config->trail_fade;
                } else {
                    work->g[i] = 0;
                }
                if (work->b[i] >= 16) {
                    work->b[i] -= work->config->trail_fade;
                } else {
                    work->b[i] = 0;
                }
            }
        }
        if (work->hit_flag[i] != 0 && (work->r[i] || work->g[i] || work->b[i])) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[1] >> 16;
            ft4.clut = work->texture[1];
            setRGB0(&ft4, work->r[i], work->g[i], work->b[i]);
            setVector(&quad[0], -work->config->impact_size, -work->config->impact_size, 0);
            setVector(&quad[1], work->config->impact_size, -work->config->impact_size, 0);
            setVector(&quad[2], -work->config->impact_size, work->config->impact_size, 0);
            setVector(&quad[3], work->config->impact_size, work->config->impact_size, 0);
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->hit[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0)
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);

            setPolyG3(&g3);
            setRGB0(&g3, 0, 0, 0);
            setRGB1(&g3, work->r[i] >> 2, work->g[i] >> 2, work->b[i] >> 2);
            setRGB2(&g3, work->r[i] >> 2, work->g[i] >> 2, work->b[i] >> 2);
            setVector(&quad[0], -work->config->spark_length, 0, 0);
            setVector(&quad[1], work->config->spark_length, 0, 0);
            for (j = 0; j < 16; j++) {
                otz = RotAverage3(&work->sparks[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                  (PSXLONG *)&g3.x2, &p, &flag);
                if (otz >= 0)
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            setVector(&scale, 0x800, 0x800, 0x800);
            m = saved;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setRGB1(&g3, work->r[i], work->g[i], work->b[i]);
            setRGB2(&g3, work->r[i], work->g[i], work->b[i]);
            for (j = 0; j < 16; j++) {
                otz = RotAverage3(&work->sparks[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                  (PSXLONG *)&g3.x2, &p, &flag);
                if (otz >= 0)
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
        }
    }
    work->elapsed += step;
    work->frames++;
    PopMatrix();

    if (work->phase == 3 &&
        !(work->r[work->count - 1] || work->g[work->count - 1] || work->b[work->count - 1])) {
        return 2;
    }
    if (work->elapsed >= work->config->end && work->phase == 1 && work->hit_flag[work->count - 1] == 1) {
        work->phase = 2;
        return 1;
    }
    return work->phase == 1 ? 4 : 0;
}
