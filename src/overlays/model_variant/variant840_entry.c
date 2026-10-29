#include "../../types.h"
#include "variant840_entry.h"

/* Header 840's entry: every second frame until the stop time, one more
 * textured streak leaves each of four model parts from a jittered offset
 * and flies toward a jittered point in front of the opposing slot. On
 * arrival it fades and bursts into a growing quartered flash whose colour
 * decays to black. The ambient light dims while the streaks fly and
 * recovers once the last one has burst. */

static const VECTOR D_8013BEEC = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model840State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, j, k;
    s16 sign;
    s32 otz;
    POLY_FT4 *poly;
    POLY_G4 *gouraud;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013BEEC;
    work = (Model840State *)context;
    poly = &ft4;
    gouraud = &g4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013BF34[command];
        work->burst_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->burst_clut = 0x3D28;
#else
        work->burst_clut = 0x3D20;
#endif
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013BEFC[i]);
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 8; j++) {
                setVector(&work->pos[i][j], (rand() - rand()) % 256 / 8, (rand() - rand()) % 256 / 8, 0);
                setVector(&work->vel[i][j], 0, 0, 0);
                setVector(&work->burst_pos[i][j], 0, 0, 0);
                work->alpha[i][j] = 0x80;
                work->burst_color[i][j].r = 0x60;
                work->burst_color[i][j].g = 0x30;
                work->burst_color[i][j].b = 0x30;
                work->state[i][j] = 0;
                work->scale[i][j] = 0;
            }
        }
        work->level = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
        work->count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 1, &rot, -4);
    } else {
        if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(2, 11, &rot, 16);
        } else {
            Model_QueueCameraOrbitFromPreset(2, 9, &rot, 16);
        }
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 1) {
        if (work->level < 0x800) {
            work->level += step << 6;
        } else {
            work->phase = 2;
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
    setPolyFT4(poly);
    setPolyG4(gouraud);
    for (i = 0; i < 4; i++) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part[i]), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        for (j = 0; j < work->count; j++) {
            if (work->state[i][j] == 0) {
                addVector(&work->pos[i][j], &pos);
                setVector(&work->vel[i][j],
                          ((rand() - rand()) % 256 / 4 - work->pos[i][j].vx) / 8,
                          ((rand() - rand()) % 256 / 4 - 350 - work->pos[i][j].vy) / 8,
                          (direction * 450 - work->pos[i][j].vz) / 8);
                work->state[i][j] = 1;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < work->count; j++) {
            if (work->alpha[i][j] != 0 && work->state[i][j] != 0) {
                poly->tpage = work->texture[0] >> 16;
                poly->clut = work->texture[0];
                setUV4(poly, 0, 0, 47, 0, 0, 127, 47, 127);
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setRGB0(poly, work->alpha[i][j], work->alpha[i][j], work->alpha[i][j]);
                setVector(&quad[0], 0, -12, direction << 7);
                setVector(&quad[1], 0, -12, -direction << 7);
                setVector(&quad[2], 0, 12, direction << 7);
                setVector(&quad[3], 0, 12, -direction << 7);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[2], &quad[0], &quad[3], &quad[1], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                  (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                if (work->alpha[i][j] == 0x80) {
                    setSemiTrans(poly, 0);
                } else {
                    setSemiTrans(poly, 1);
                }
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
                }
                addVector(&work->pos[i][j], &work->vel[i][j]);
                if (work->state[i][j] == 1) {
                    if (direction > 0) {
                        if (work->pos[i][j].vz >= 450) {
                            copyVector(&work->burst_pos[i][j], &work->pos[i][j]);
                            work->state[i][j] = 2;
                        }
                    } else if (work->pos[i][j].vz < -449) {
                        copyVector(&work->burst_pos[i][j], &work->pos[i][j]);
                        work->state[i][j] = 2;
                    }
                }
                if (work->state[i][j] >= 2) {
                    if (work->alpha[i][j] >= 16) {
                        work->alpha[i][j] -= 15;
                    } else {
                        work->alpha[i][j] = 0;
                    }
                }
            }
            if (work->state[i][j] >= 2
                && (work->burst_color[i][j].r != 0 || work->burst_color[i][j].g != 0
                    || work->burst_color[i][j].b != 0)) {
                setVector(&scale, work->scale[i][j] + 0x1000, work->scale[i][j] + 0x1000, 0x1000);
                poly->tpage = work->burst_tpage;
                poly->clut = work->burst_clut;
                GsSetLsMatrix(&light);
                RotTrans(&work->burst_pos[i][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                sign = 1;
                for (k = 0; k < 4; k++) {
                    poly->r0 = work->burst_color[i][j].r;
                    poly->g0 = work->burst_color[i][j].g;
                    sign *= -1;
                    poly->b0 = work->burst_color[i][j].b;
                    setVector(&quad[0], -(sign * 32), k - 2 < 0 ? 32 : -32, 0);
                    setVector(&quad[1], 0, k - 2 >= 0 ? -32 : 32, 0);
                    setVector(&quad[2], -(sign * 32), 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    setUV4(poly, 64, 0, 127, 0, 64, 63, 127, 63);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                      (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                    setUV4(poly, 64, 64, 127, 64, 64, 127, 127, 127);
                    poly->r0 = work->burst_color[i][j].r >> 2;
                    poly->g0 = work->burst_color[i][j].g >> 2;
                    poly->b0 = work->burst_color[i][j].b >> 2;
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                      (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
                    }
                }
                if (work->scale[i][j] < 0x6000) {
                    work->scale[i][j] += 0x1000;
                } else {
                    work->scale[i][j] += 0x100;
                }
                if (work->burst_color[i][j].r > 8) {
                    work->burst_color[i][j].r -= 8;
                } else {
                    work->burst_color[i][j].r = 0;
                }
                if (work->burst_color[i][j].g > 8) {
                    work->burst_color[i][j].g -= 8;
                } else {
                    work->burst_color[i][j].g = 0;
                }
                if (work->burst_color[i][j].b > 8) {
                    work->burst_color[i][j].b -= 8;
                } else {
                    work->burst_color[i][j].b = 0;
                }
            }
        }
    }
    if (work->elapsed < work->config->stop && work->tick % 2 == 0) {
        work->count++;
        if (work->count > 8) {
            work->count = 8;
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->burst_color[0][work->count - 1].r == 0 && work->burst_color[0][work->count - 1].g == 0
        && work->burst_color[0][work->count - 1].b == 0 && work->phase == 2) {
        return 2;
    }
    if (work->state[0][work->count - 1] >= 2 && work->elapsed >= work->config->stop && work->phase == 0) {
        work->state[0][work->count - 1] = 3;
        work->phase = 1;
        return 1;
    }
    if (work->phase == 0 && work->state[0][0] == 2) {
        return 4;
    }
    return 0;
}
