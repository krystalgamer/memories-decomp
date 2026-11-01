#include "../../types.h"
#include "variant839_entry.h"

/* Header 839's entry: the ambient light dims while sixteen animated sparks
 * spray from a model part and, every other frame, another of up to 128
 * bubbles leaves it, drifting forward and slowly sinking. Once a bubble
 * passes the far depth it bursts into a six-frame flash and four fragments
 * that drift apart; once the last bubble has burst after spawning ends, the
 * light comes back. */

static const VECTOR gModelVariant839_D14D4 = {4096, 4096, 4096, 0};

s32 ModelVariant839_HandleRequest(u8 *context, s32 command)
{
    Model839State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j;
    s32 otz;
    POLY_FT4 *packet;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant839_D14D4;
    work = (Model839State *)context;
    packet = &ft4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant839_D1538[command];
        work->unused_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->unused_clut = 0x3D28;
#else
        work->unused_clut = 0x3D20;
#endif
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant839_D14E4[i]);
        }
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        for (i = 0; i < 128; i++) {
            setVector(&work->positions[i], 0, 0, 0);
            setVector(&work->velocities[i], work->config->speed * ((rand() - rand()) % 1024) / 4096,
                      -work->config->rise + work->config->rise / 2 * ((rand() - rand()) % 4096) / 4096,
                      direction * work->config->speed);
            work->states[i] = 0;
            work->sizes[i] = work->config->size + work->config->size / 2 * (rand() % 256) / 256;
            work->frames[i] = 0;
            for (j = 0; j < 4; j++) {
                setVector(&work->fragments[i][j], (rand() - rand()) % 256 / 4, (rand() - rand()) % 256 / 4,
                          (rand() - rand()) % 256 / 4);
                setVector(&work->fragment_velocities[i][j], (rand() - rand()) % 256 / 32, rand() % 256 / 32,
                          (rand() - rand()) % 256 / 32);
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->spark_velocities[i], (rand() - rand()) % 256 / 32, (rand() - rand()) % 256 / 32,
                      direction * (rand() % 256 << 3) / 256);
            work->spark_frames[i] = 0;
            work->spark_flags[i] = 0;
        }
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->count = 0;
        work->spark_count = 0;
        work->tick = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 32;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step * 32;
        } else {
            work->phase = 2;
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
    setPolyFT4(packet);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
    pos.vx = m.t[0];
    pos.vy = m.t[1];
    pos.vz = m.t[2];
    packet->tpage = work->texture[2] >> 16;
    packet->clut = work->texture[2];
    setRGB0(packet, 0x40, 0x40, 0x40);
    for (i = 0; i < work->spark_count; i++) {
        if (work->spark_frames[i] < 24) {
            if (work->spark_flags[i] == 0) {
                addVector(&work->sparks[i], &pos);
                work->spark_flags[i] = 1;
            }
            setUV4(packet, work->spark_frames[i] / 3 % 4 * 32 + 128, work->spark_frames[i] / 12 * 32 + 64,
                   work->spark_frames[i] / 3 % 4 * 32 + 159, work->spark_frames[i] / 12 * 32 + 64,
                   work->spark_frames[i] / 3 % 4 * 32 + 128, work->spark_frames[i] / 12 * 32 + 95,
                   work->spark_frames[i] / 3 % 4 * 32 + 159, work->spark_frames[i] / 12 * 32 + 95);
            setVector(&quad[0], -64, -64, 0);
            setVector(&quad[1], 64, -64, 0);
            setVector(&quad[2], -64, 64, 0);
            setVector(&quad[3], 64, 64, 0);
            GsSetLsMatrix(&light);
            RotTrans(&work->sparks[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                              (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
            setSemiTrans(packet, 1);
            if (flag >= 0 && otz >= 0) {
                GsSortPoly(packet, ot, (u16)otz);
            }
            addVector(&work->sparks[i], &work->spark_velocities[i]);
            work->spark_frames[i]++;
        } else if (work->elapsed < work->config->spawn_end) {
            setVector(&work->sparks[i], pos.vx, pos.vy, pos.vz);
            setVector(&work->spark_velocities[i], (rand() - rand()) % 256 / 32, (rand() - rand()) % 256 / 32,
                      direction * (rand() % 256 << 3) / 256);
            work->spark_frames[i] = 0;
        }
    }
    work->spark_count++;
    if (work->spark_count > 16) {
        work->spark_count = 16;
    }

    for (i = 0; i < work->count; i++) {
        if (work->states[i] == 0) {
            work->states[i] = 1;
            setVector(&work->positions[i], pos.vx, pos.vy, pos.vz);
        } else if (work->states[i] == 1) {
            packet->tpage = work->texture[1] >> 16;
            packet->clut = work->texture[1];
            setRGB0(packet, 0x80, 0x80, 0x80);
            setUV4(packet, 0, 0x80, 0x1F, 0x80, 0, 0x9F, 0x1F, 0x9F);
            setVector(&quad[0], -work->sizes[i], -work->sizes[i], 0);
            setVector(&quad[1], work->sizes[i], -work->sizes[i], 0);
            setVector(&quad[2], -work->sizes[i], work->sizes[i], 0);
            setVector(&quad[3], work->sizes[i], work->sizes[i], 0);
            GsSetLsMatrix(&light);
            RotTrans(&work->positions[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                              (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
            setSemiTrans(packet, 1);
            if (flag >= 0 && otz >= 0) {
                GsSortPoly(packet, ot, (u16)otz);
            }
            if (direction > 0) {
                if (work->positions[i].vz >= 450) {
                    for (j = 0; j < 4; j++) {
                        addVector(&work->fragments[i][j], &work->positions[i]);
                    }
                    work->states[i] = 2;
                } else {
                    addVector(&work->positions[i], &work->velocities[i]);
                    if (!(work->tick & 3)) {
                        work->velocities[i].vy++;
                    }
                }
            } else {
                if (work->positions[i].vz > -450) {
                    addVector(&work->positions[i], &work->velocities[i]);
                    if (!(work->tick & 3)) {
                        work->velocities[i].vy++;
                    }
                } else {
                    for (j = 0; j < 4; j++) {
                        addVector(&work->fragments[i][j], &work->positions[i]);
                    }
                    work->states[i] = 2;
                }
            }
        } else if (work->states[i] == 2) {
            if (work->frames[i] < 6) {
                packet->tpage = work->texture[0] >> 16;
                packet->clut = work->texture[0];
                setRGB0(packet, 0x80, 0x80, 0x80);
                setUV4(packet, work->frames[i] % 4 * 64, work->frames[i] / 4 * 64,
                       work->frames[i] % 4 * 64 + 63, work->frames[i] / 4 * 64,
                       work->frames[i] % 4 * 64, work->frames[i] / 4 * 64 + 63,
                       work->frames[i] % 4 * 64 + 63, work->frames[i] / 4 * 64 + 63);
                setVector(&quad[0], -work->sizes[i] * 3, -work->sizes[i] * 3, 0);
                setVector(&quad[1], work->sizes[i] * 3, -work->sizes[i] * 3, 0);
                setVector(&quad[2], -work->sizes[i] * 3, work->sizes[i] * 3, 0);
                setVector(&quad[3], work->sizes[i] * 3, work->sizes[i] * 3, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->positions[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                                  (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
                setSemiTrans(packet, 1);
                if (flag >= 0 && otz >= 0) {
                    GsSortPoly(packet, ot, (u16)otz);
                }
            }
            if (work->frames[i] < 16) {
                packet->tpage = work->texture[2] >> 16;
                packet->clut = work->texture[2];
                setRGB0(packet, 0x20, 0x20, 0x40);
                setUV4(packet, work->frames[i] / 2 % 4 * 32 + 128, work->frames[i] / 8 * 32 + 64,
                       work->frames[i] / 2 % 4 * 32 + 159, work->frames[i] / 8 * 32 + 64,
                       work->frames[i] / 2 % 4 * 32 + 128, work->frames[i] / 8 * 32 + 95,
                       work->frames[i] / 2 % 4 * 32 + 159, work->frames[i] / 8 * 32 + 95);
                setVector(&quad[0], -work->sizes[i] * 3, -work->sizes[i] * 3, 0);
                setVector(&quad[1], work->sizes[i] * 3, -work->sizes[i] * 3, 0);
                setVector(&quad[2], -work->sizes[i] * 3, work->sizes[i] * 3, 0);
                setVector(&quad[3], work->sizes[i] * 3, work->sizes[i] * 3, 0);
                for (j = 0; j < 4; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->fragments[i][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&packet->x0,
                                      (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
                    setSemiTrans(packet, 1);
                    if (flag >= 0 && otz >= 0) {
                        GsSortPoly(packet, ot, (u16)otz);
                    }
                    addVector(&work->fragments[i][j], &work->fragment_velocities[i][j]);
                }
            }
            work->frames[i]++;
        }
    }
    if (work->elapsed < work->config->spawn_end && !(work->tick & 1)) {
        work->count++;
        if (work->count > 128) {
            work->count = 128;
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 2) {
        return 2;
    }
    if (work->states[work->count - 1] >= 2 && work->config->spawn_end < work->elapsed && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->states[0] >= 2 && work->phase == 0) {
        return 4;
    }
    return 0;
}
