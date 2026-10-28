#include "../../types.h"
#include "variant843_entry.h"
#include "../../game/screen_projection.h"

/* Header 843's entry: while the ambient light dims, orbs and comets leave
 * two model parts for jittered points on the opposing slot, the orbs
 * spinning and flickering as they fly while optional sparks flash from the
 * model. Each arrival opens into a fading four-petal burst; once the last
 * of both has landed the light comes back. */

static const VECTOR D_8013CC80 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model843State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 ft4;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag;
    PSXLONG interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j, k;
    s16 sign;
    s32 depth;
    POLY_FT4 *packet;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CC80;
    work = (Model843State *)context;
    packet = &ft4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CCE4[command];
        work->tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->clut = 0x3D28;
#else
        work->clut = 0x3D20;
#endif
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CC90[i]);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            work->spark_frames[i] = 0;
        }
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 96; j++) {
                setVector(&work->orbs[i][j], work->config->spread * ((rand() - rand()) % 256) / 256,
                          work->config->spread * ((rand() - rand()) % 256) / 256, 0);
                setVector(&work->orb_velocities[i][j], 0, 0, 0);
                setVector(&work->orb_targets[i][j], 0, 0, 0);
                work->orb_spins[i][j] = rand() % 4096;
                work->orb_colors[i][j][0] = work->config->orb_r;
                work->orb_colors[i][j][1] = work->config->orb_g;
                work->orb_colors[i][j][2] = work->config->orb_b;
                work->orb_arrived[i][j] = 0;
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->comets[i][j], (rand() - rand()) % 256 / 4, (rand() - rand()) % 256 / 4, 0);
                setVector(&work->comet_velocities[i][j], 0, 0, 0);
                setVector(&work->comet_targets[i][j], 0, 0, 0);
                work->comet_colors[i][j][0] = work->config->comet_r;
                work->comet_colors[i][j][1] = work->config->comet_g;
                work->comet_colors[i][j][2] = work->config->comet_b;
                work->comet_arrived[i][j] = 0;
            }
        }
        work->orb_count = 0;
        work->comet_count = 0;
        work->level = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
        work->spark_index = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->level > -0x200) {
            work->level -= step << 6;
        } else {
            work->level = -0x200;
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

    setPolyFT4(packet);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->config->sparks == 1) {
        if (work->elapsed < work->config->emit_end) {
            GsSetLsMatrix(&base);
            i = rand() % 4;
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->spark_parts[i]), &matrix);
            setVector(&work->sparks[work->spark_index], matrix.t[0], matrix.t[1], matrix.t[2]);
            work->spark_frames[work->spark_index] = 1;
            work->spark_index = (work->spark_index + 1) % 32;
        }
        packet->tpage = work->texture[1] >> 16;
        packet->clut = work->texture[1];
        setRGB0(packet, 0x40, 0x40, 0x40);
        setVector(&vertices[0], -64, -64, 0);
        setVector(&vertices[1], 64, -64, 0);
        setVector(&vertices[2], -64, 64, 0);
        setVector(&vertices[3], 64, 64, 0);
        for (i = 0; i < 32; i++) {
            if (work->spark_frames[i] != 0 && work->spark_frames[i] < 18) {
                GsSetLsMatrix(&base);
                RotTrans(&work->sparks[i], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&packet->x0,
                                    (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &interpolation,
                                    &flag);
                setUV4(packet, work->spark_frames[i] / 3 * 32 + 32, 0x40, work->spark_frames[i] / 3 * 32 + 63, 0x40,
                       work->spark_frames[i] / 3 * 32 + 32, 0x5F, work->spark_frames[i] / 3 * 32 + 63, 0x5F);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)packet, ot, (u16)depth, 1);
                }
                work->spark_frames[i]++;
            }
        }
    }

    if (work->elapsed < work->config->emit_end) {
        if (work->orb_count < 96) {
            for (i = 0; i < 2; i++) {
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[i]), &matrix);
                setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
                addVector(&work->orbs[i][work->orb_count], &position);
                Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
                Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
                setVector(&work->orb_targets[i][work->orb_count],
                          position.vx + rotation.vx / 2 * ((rand() - rand()) % 256) / 256,
                          position.vy + rotation.vy / 2 * ((rand() - rand()) % 256) / 256,
                          position.vz - direction * (rotation.vz / 2) * (rand() % 256) / 256);
                setVector(&work->orb_velocities[i][work->orb_count],
                          (work->orb_targets[i][work->orb_count].vx - work->orbs[i][work->orb_count].vx) / 32,
                          (work->orb_targets[i][work->orb_count].vy - work->orbs[i][work->orb_count].vy) / 32,
                          (work->orb_targets[i][work->orb_count].vz - work->orbs[i][work->orb_count].vz) / 32);
            }
            work->orb_count++;
        }
        if (work->elapsed < work->config->emit_end && work->comet_count < 32 && !(work->tick & 1)) {
            for (i = 0; i < 2; i++) {
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[i]), &matrix);
                setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
                addVector(&work->comets[i][work->comet_count], &position);
                Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
                Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
                setVector(&work->comet_targets[i][work->comet_count],
                          position.vx + rotation.vx / 2 * ((rand() - rand()) % 256) / 256,
                          position.vy + rotation.vy / 2 * ((rand() - rand()) % 256) / 256,
                          position.vz - direction * (rotation.vz / 2) * (rand() % 256) / 256);
                setVector(&work->comet_velocities[i][work->comet_count],
                          (work->comet_targets[i][work->comet_count].vx - work->comets[i][work->comet_count].vx) / 36,
                          (work->comet_targets[i][work->comet_count].vy - work->comets[i][work->comet_count].vy) / 36,
                          (work->comet_targets[i][work->comet_count].vz - work->comets[i][work->comet_count].vz) / 36);
            }
            work->comet_count++;
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < work->orb_count; j++) {
            if (work->orb_colors[i][j][0] != 0 || work->orb_colors[i][j][1] != 0 || work->orb_colors[i][j][2] != 0) {
                packet->tpage = work->texture[0] >> 16;
                packet->clut = work->texture[0];
                setUV4(packet, 0, 0x40, 0x3F, 0x40, 0, 0x7F, 0x3F, 0x7F);
                if ((work->tick & 1) == j % 2) {
                    setRGB0(packet, work->orb_colors[i][j][0], work->orb_colors[i][j][1], work->orb_colors[i][j][2]);
                } else {
                    setRGB0(packet, work->orb_colors[i][j][0] >> 1, work->orb_colors[i][j][1] >> 1,
                            work->orb_colors[i][j][2] >> 1);
                }
                setVector(&vertices[0], -64, -64, 0);
                setVector(&vertices[1], 64, -64, 0);
                setVector(&vertices[2], -64, 64, 0);
                setVector(&vertices[3], 64, 64, 0);
                setVector(&rotation, 0, 0, work->orb_spins[i][j]);
                GsSetLsMatrix(&base);
                RotTrans(&work->orbs[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&packet->x0,
                                    (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &interpolation,
                                    &flag);
                setSemiTrans(packet, 1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->orbs[i][j], &work->orb_velocities[i][j]);
                work->orb_spins[i][j] = (work->orb_spins[i][j] + 64) % 4096;
                if (work->orb_arrived[i][j] == 0) {
                    if (direction > 0) {
                        if (work->orbs[i][j].vz >= work->orb_targets[i][j].vz) {
                            work->orb_arrived[i][j] = 1;
                        }
                    } else if (work->orbs[i][j].vz <= work->orb_targets[i][j].vz) {
                        work->orb_arrived[i][j] = 1;
                    }
                }
                if (work->orb_arrived[i][j] != 0) {
                    setVector(&rotation, 0, 0, 0);
                    GsSetLsMatrix(&base);
                    RotTrans(&work->orb_targets[i][j], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    packet->tpage = work->tpage;
                    packet->clut = work->clut;
                    setRGB0(packet, work->orb_colors[i][j][0], work->orb_colors[i][j][1], work->orb_colors[i][j][2]);
                    setUV4(packet, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                    sign = 1;
                    for (k = 0; k < 4; k++) {
                        sign *= -1;
                        setVector(&vertices[0], -(sign * 64), k - 2 >= 0 ? -64 : 64, 0);
                        setVector(&vertices[1], 0, k - 2 >= 0 ? -64 : 64, 0);
                        setVector(&vertices[2], -(sign * 64), 0, 0);
                        setVector(&vertices[3], 0, 0, 0);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                                            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                                            (PSXLONG *)&packet->x3, &interpolation, &flag);
                        if (depth >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)packet, ot, (u16)depth, 1);
                        }
                    }
                    if (work->orb_arrived[i][j] != 0) {
                        if (work->orb_colors[i][j][0] >= 16) {
                            work->orb_colors[i][j][0] -= 15;
                        } else {
                            work->orb_colors[i][j][0] = 0;
                        }
                        if (work->orb_colors[i][j][1] >= 16) {
                            work->orb_colors[i][j][1] -= 15;
                        } else {
                            work->orb_colors[i][j][1] = 0;
                        }
                        if (work->orb_colors[i][j][2] >= 16) {
                            work->orb_colors[i][j][2] -= 15;
                        } else {
                            work->orb_colors[i][j][2] = 0;
                        }
                    }
                }
            }
        }

        setVector(&rotation, 0, 0, 0);
        packet->tpage = work->tpage;
        packet->clut = work->clut;
        setUV4(packet, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
        for (j = 0; j < work->comet_count; j++) {
            if (work->comet_colors[i][j][0] != 0 || work->comet_colors[i][j][1] != 0
                || work->comet_colors[i][j][2] != 0) {
                if (work->comet_arrived[i][j] == 0) {
                    if ((work->tick & 1) == j % 2) {
                        setRGB0(packet, work->comet_colors[i][j][0], work->comet_colors[i][j][1],
                                work->comet_colors[i][j][2]);
                    } else {
                        setRGB0(packet, work->comet_colors[i][j][0] >> 1, work->comet_colors[i][j][1] >> 1,
                                work->comet_colors[i][j][2] >> 1);
                    }
                    GsSetLsMatrix(&base);
                    RotTrans(&work->comets[i][j], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    sign = 1;
                    for (k = 0; k < 4; k++) {
                        sign *= -1;
                        setVector(&vertices[0], -(sign * 16), k - 2 >= 0 ? -16 : 16, 0);
                        setVector(&vertices[1], 0, k - 2 >= 0 ? -16 : 16, 0);
                        setVector(&vertices[2], -(sign * 16), 0, 0);
                        setVector(&vertices[3], 0, 0, 0);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                                            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                                            (PSXLONG *)&packet->x3, &interpolation, &flag);
                        setSemiTrans(packet, 1);
                        if (depth >= 0 && flag >= 0) {
                            GsSortPoly(packet, ot, (u16)depth);
                        }
                    }
                    addVector(&work->comets[i][j], &work->comet_velocities[i][j]);
                    if (work->comet_arrived[i][j] == 0) {
                        if (direction > 0) {
                            if (work->comets[i][j].vz >= work->comet_targets[i][j].vz) {
                                work->comet_arrived[i][j] = 1;
                            }
                        } else if (work->comets[i][j].vz <= work->comet_targets[i][j].vz) {
                            work->comet_arrived[i][j] = 1;
                        }
                    }
                }
                if (work->comet_arrived[i][j] != 0) {
                    setVector(&rotation, 0, 0, 0);
                    GsSetLsMatrix(&base);
                    RotTrans(&work->comet_targets[i][j], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    packet->tpage = work->tpage;
                    packet->clut = work->clut;
                    setRGB0(packet, work->comet_colors[i][j][0], work->comet_colors[i][j][1],
                            work->comet_colors[i][j][2]);
                    setUV4(packet, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                    sign = 1;
                    for (k = 0; k < 4; k++) {
                        sign *= -1;
                        setVector(&vertices[0], -(sign * 128), k - 2 >= 0 ? -128 : 128, 0);
                        setVector(&vertices[1], 0, k - 2 >= 0 ? -128 : 128, 0);
                        setVector(&vertices[2], -(sign * 128), 0, 0);
                        setVector(&vertices[3], 0, 0, 0);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                                            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                                            (PSXLONG *)&packet->x3, &interpolation, &flag);
                        if (depth >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)packet, ot, (u16)depth, 1);
                        }
                    }
                    if (work->comet_colors[i][j][0] >= 16) {
                        work->comet_colors[i][j][0] -= 15;
                    } else {
                        work->comet_colors[i][j][0] = 0;
                    }
                    if (work->comet_colors[i][j][1] >= 16) {
                        work->comet_colors[i][j][1] -= 15;
                    } else {
                        work->comet_colors[i][j][1] = 0;
                    }
                    if (work->comet_colors[i][j][2] >= 16) {
                        work->comet_colors[i][j][2] -= 15;
                    } else {
                        work->comet_colors[i][j][2] = 0;
                    }
                }
            }
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->orb_colors[0][work->orb_count - 1][0] == 0 && work->orb_colors[0][work->orb_count - 1][1] == 0
        && work->orb_colors[0][work->orb_count - 1][2] == 0 && work->comet_colors[0][work->comet_count - 1][0] == 0
        && work->comet_colors[0][work->comet_count - 1][1] == 0
        && work->comet_colors[0][work->comet_count - 1][2] == 0 && work->phase == 2) {
        return 2;
    }
    if (work->config->emit_end < work->elapsed && work->orb_arrived[0][work->orb_count - 1] != 0
        && work->comet_arrived[0][work->comet_count - 1] != 0 && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->orb_arrived[0][0] != 0 && work->phase == 0) {
        return 4;
    }
    return 0;
}
