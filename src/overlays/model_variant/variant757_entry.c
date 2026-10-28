#include "../../types.h"
#include "variant757_entry.h"

/* Header 757's entry: the ambient light dims while up to thirty-two animated
 * sparks spray from a model part and, every period, another of up to 52
 * shards leaves it for a jittered point near the opposing slot, cycling
 * through three images and growing as it flies. On arrival each shard
 * bursts into sixteen tinted fragments that drift apart; once the last shard
 * has burst after spawning ends, the light comes back. */

static const VECTOR D_8013C4E8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model757State *work;
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
    scale = D_8013C4E8;
    work = (Model757State *)context;
    packet = &ft4;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C584[command];
        work->unused_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->unused_clut = 0x3D28;
#else
        work->unused_clut = 0x3D20;
#endif
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C4F8[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C4F8[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C4F8[2]);
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C4F8[3]);
        work->texture[4] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &D_8013C4F8[3]);
        work->texture[5] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C4F8[4]);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        for (i = 0; i < 52; i++) {
            setVector(&work->positions[i], 0, 0, 0);
            setVector(&work->targets[i], 0, 0, 0);
            setVector(&work->velocities[i], 0, 0, 0);
            work->states[i] = 0;
            work->sizes[i] = work->config->size + work->config->size / 2 * (rand() % 256) / 256;
            work->images[i] = rand() % 3;
            work->frames[i] = 0;
            for (j = 0; j < 16; j++) {
                setVector(&work->fragments[i][j], (rand() - rand()) % 256 / 4, (rand() - rand()) % 256 / 4,
                          (rand() - rand()) % 256 / 4);
                setVector(&work->fragment_velocities[i][j], (rand() - rand()) % 256 * 12 / 256,
                          rand() % 256 * 12 / 256, (rand() - rand()) % 256 * 12 / 256);
            }
        }
        for (i = 0; i < 32; i++) {
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
    packet->tpage = work->texture[3] >> 16;
    packet->clut = work->texture[3];
    setRGB0(packet, work->config->r, work->config->g, work->config->b);
    for (i = 0; i < work->spark_count; i++) {
        if (work->spark_frames[i] < 16) {
            if (work->spark_flags[i] == 0) {
                addVector(&work->sparks[i], &pos);
                work->spark_flags[i] = 1;
            }
            setUV4(packet, work->spark_frames[i] / 2 % 4 * 64, work->spark_frames[i] / 8 * 64 + 64,
                   work->spark_frames[i] / 2 % 4 * 64 + 63, work->spark_frames[i] / 8 * 64 + 64,
                   work->spark_frames[i] / 2 % 4 * 64, work->spark_frames[i] / 8 * 64 + 127,
                   work->spark_frames[i] / 2 % 4 * 64 + 63, work->spark_frames[i] / 8 * 64 + 127);
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
            /* Spelling the scale as "* 3 << 2" keeps GCC from folding 12
             * into the direction before the multiply. */
            setVector(&work->spark_velocities[i], (rand() - rand()) % 256 * 12 / 256,
                      (rand() - rand()) % 256 * 12 / 256, direction * (rand() % 256 * 3 << 2) / 256);
            work->spark_frames[i] = 0;
        }
    }
    work->spark_count++;
    if (work->spark_count > 32) {
        work->spark_count = 32;
    }

    for (i = 0; i < work->count; i++) {
        if (work->states[i] == 0) {
            work->states[i] = 1;
            setVector(&work->positions[i], pos.vx, pos.vy, pos.vz);
            Model_CopySlotPosition(slot ^ 1, (u16 *)&rot);
            setVector(&work->targets[i], rot.vx + (rand() - rand()) % 256 / 2,
                      rot.vy + (rand() - rand()) % 256 / 2,
                      rot.vz - direction * (rand() % 256 << 5) / 256);
            setVector(&work->velocities[i], (work->targets[i].vx - work->positions[i].vx) / work->config->flight,
                      (work->targets[i].vy - work->positions[i].vy) / work->config->flight,
                      (work->targets[i].vz - work->positions[i].vz) / work->config->flight);
        } else if (work->states[i] == 1) {
            packet->tpage = work->texture[work->images[i]] >> 16;
            packet->clut = work->texture[work->images[i]];
            setRGB0(packet, 0x80, 0x80, 0x80);
            setUV4(packet, work->images[i] * 40, 0, work->images[i] * 40 + 39, 0,
                   work->images[i] * 40, 39, work->images[i] * 40 + 39, 39);
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
                if (work->positions[i].vz >= work->targets[i].vz) {
                    for (j = 0; j < 16; j++) {
                        addVector(&work->fragments[i][j], &work->positions[i]);
                    }
                    work->states[i] = 2;
                } else {
                    work->images[i] = (work->images[i] + 1) % 3;
                    addVector(&work->positions[i], &work->velocities[i]);
                    if (!(work->tick & 1)) {
                        work->sizes[i] += work->config->growth;
                    }
                }
            } else {
                if (work->positions[i].vz > work->targets[i].vz) {
                    work->images[i] = (work->images[i] + 1) % 3;
                    addVector(&work->positions[i], &work->velocities[i]);
                    if (!(work->tick & 1)) {
                        work->sizes[i] += work->config->growth;
                    }
                } else {
                    for (j = 0; j < 16; j++) {
                        addVector(&work->fragments[i][j], &work->positions[i]);
                    }
                    work->states[i] = 2;
                }
            }
        } else if (work->states[i] == 2) {
            if (work->frames[i] < 16) {
                packet->tpage = work->texture[4] >> 16;
                packet->clut = work->texture[4];
                setRGB0(packet, work->config->fragment_r, work->config->fragment_g, work->config->fragment_b);
                setUV4(packet, work->frames[i] / 2 % 4 * 64, work->frames[i] / 8 * 64 + 64,
                       work->frames[i] / 2 % 4 * 64 + 63, work->frames[i] / 8 * 64 + 64,
                       work->frames[i] / 2 % 4 * 64, work->frames[i] / 8 * 64 + 127,
                       work->frames[i] / 2 % 4 * 64 + 63, work->frames[i] / 8 * 64 + 127);
                setVector(&quad[0], -work->sizes[i] * 2, -work->sizes[i] * 2, 0);
                setVector(&quad[1], work->sizes[i] * 2, -work->sizes[i] * 2, 0);
                setVector(&quad[2], -work->sizes[i] * 2, work->sizes[i] * 2, 0);
                setVector(&quad[3], work->sizes[i] * 2, work->sizes[i] * 2, 0);
                for (j = 0; j < 16; j++) {
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
    if (work->elapsed < work->config->spawn_end && work->tick % work->config->period == 0) {
        work->count++;
        if (work->count > 52) {
            work->count = 52;
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
