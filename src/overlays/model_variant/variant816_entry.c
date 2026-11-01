#include "../../types.h"
#include "variant816_entry.h"
#include "../../game/screen_projection.h"

/* Header 816's entry: while the ambient light dims, orbs, comets and
 * spinners leave a model part and fly to jittered points on the opposing
 * slot. Each orb bursts into a fading glow, each comet bursts into a glow and
 * eight tumbling textured shards, and each spinner breaks into eight falling
 * debris pieces; once the last orb and comet have landed the light comes
 * back. */

static const VECTOR gModelVariant816_D2B6C = {4096, 4096, 4096, 0};

s32 ModelVariant816_HandleRequest(u8 *context, s32 command)
{
    Model816State *work;
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
    s32 i, j, k, frame;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant816_D2B6C;
    work = (Model816State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant816_D2C08[command];
        for (i = 0; i < 1; i++) {
            for (j = 0; j < 64; j++) {
                setVector(&work->orbs[i][j], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096, 0);
                setVector(&work->orb_velocities[i][j], 0, 0, 0);
                setVector(&work->orb_spins[i][j], 0, 0, rand() % 4096);
                work->orb_arrived[i][j] = 0;
                work->orb_r[i][j] = work->config->burst_r;
                work->orb_g[i][j] = work->config->burst_g;
                work->orb_b[i][j] = work->config->burst_b;
                work->orb_targets[i][j] = 0;
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->spinners[i][j], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096, 0);
                setVector(&work->spinner_velocities[i][j], 0, 0, 0);
                setVector(&work->spinner_spins[i][j], 0, 0, 0);
                work->spinner_arrived[i][j] = 0;
                work->spinner_targets[i][j] = 0;
                work->debris_level[i][j] = 0x80;
                for (k = 0; k < 8; k++) {
                    setVector(&work->debris[i][j][k], 0, 0, 0);
                    setVector(&work->debris_velocities[i][j][k],
                        work->config->debris_spread * ((rand() - rand()) % 4096) / 4096,
                        work->config->debris_spread * ((rand() - rand()) % 4096) / 4096,
                        work->config->debris_spread * ((rand() - rand()) % 4096) / 4096);
                    setVector(&work->debris_spins[i][j][k], 0, 0, rand() % 4096);
                }
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->comets[i][j], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096, 0);
                setVector(&work->comet_velocities[i][j], 0, 0, 0);
                setVector(&work->comet_spins[i][j], 0, 0, rand() % 4096);
                for (k = 0; k < 8; k++) {
                    setVector(&work->shards[i][j][k], 0, 0, 0);
                    setVector(&work->shard_velocities[i][j][k],
                        work->config->shard_speed * ((rand() - rand()) % 4096) / 4096,
                        work->config->shard_speed * ((rand() - rand()) % 4096) / 4096,
                        work->config->shard_speed * ((rand() - rand()) % 4096) / 4096);
                }
                work->comet_arrived[i][j] = 0;
                work->comet_r[i][j] = work->config->burst_r;
                work->comet_g[i][j] = work->config->burst_g;
                work->comet_b[i][j] = work->config->burst_b;
                work->comet_targets[i][j] = 0;
            }
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant816_D2B7C[i]);
        }
        work->orb_count = 0;
        work->comet_count = 0;
        work->spinner_count = 0;
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 5;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 2) {
        if (work->level < 2048) {
            work->level += step << 5;
        } else {
            work->phase = 3;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->emit_end && work->orb_count < 64
        && work->tick % work->config->orb_period == 0) {
        for (i = 0; i < 1; i++) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[i]), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            addVector(&work->orbs[i][work->orb_count], &position);
            Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
            Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
            position.vx += rotation.vx / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vy += rotation.vy / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vz += rotation.vz / 2 * ((rand() - rand()) % 4096) / 4096;
            work->orb_targets[i][work->orb_count] = position.vz;
            setVector(&work->orb_velocities[i][work->orb_count], (position.vx - work->orbs[i][work->orb_count].vx) / work->config->flight,
                (position.vy - work->orbs[i][work->orb_count].vy) / work->config->flight,
                (position.vz - work->orbs[i][work->orb_count].vz) / work->config->flight);
            if (ABS(position.vz) < ABS(work->orbs[i][work->orb_count].vz)) {
                work->orbs[i][work->orb_count].vz = -direction * (ABS(position.vz) - 1);
            }
        }
        work->orb_count++;
    }
    if (work->elapsed < work->config->emit_end && work->comet_count < 16
        && work->tick % work->config->comet_period == 0) {
        for (i = 0; i < 1; i++) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[i]), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            addVector(&work->comets[i][work->comet_count], &position);
            Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
            Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
            position.vx += rotation.vx / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vy += rotation.vy / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vz += rotation.vz / 2 * ((rand() - rand()) % 4096) / 4096;
            for (k = 0; k < 8; k++) {
                setVector(&work->shards[i][work->comet_count][k], position.vx, position.vy, position.vz);
            }
            work->comet_targets[i][work->comet_count] = position.vz;
            setVector(&work->comet_velocities[i][work->comet_count], (position.vx - work->comets[i][work->comet_count].vx) / work->config->flight,
                (position.vy - work->comets[i][work->comet_count].vy) / work->config->flight,
                (position.vz - work->comets[i][work->comet_count].vz) / work->config->flight);
            if (ABS(position.vz) < ABS(work->comets[i][work->comet_count].vz)) {
                work->comets[i][work->comet_count].vz = -direction * (ABS(position.vz) - 1);
            }
        }
        work->comet_count++;
    }
    if (work->elapsed < work->config->emit_end && work->spinner_count < 32
        && work->tick % work->config->spinner_period == 0) {
        for (i = 0; i < 1; i++) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[i]), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            addVector(&work->spinners[i][work->spinner_count], &position);
            Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
            Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
            position.vx += rotation.vx / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vy += rotation.vy / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vz += rotation.vz / 2 * ((rand() - rand()) % 4096) / 4096;
            work->spinner_targets[i][work->spinner_count] = position.vz;
            for (k = 0; k < 8; k++) {
                setVector(&work->debris[i][work->spinner_count][k], position.vx, position.vy, position.vz);
            }
            setVector(&work->spinner_velocities[i][work->spinner_count], (position.vx - work->spinners[i][work->spinner_count].vx) / work->config->flight,
                (position.vy - work->spinners[i][work->spinner_count].vy) / work->config->flight,
                (position.vz - work->spinners[i][work->spinner_count].vz) / work->config->flight);
            if (ABS(position.vz) < ABS(work->spinners[i][work->spinner_count].vz)) {
                work->spinners[i][work->spinner_count].vz = -direction * (ABS(position.vz) - 1);
            }
        }
        work->spinner_count++;
    }
    setVector(&position, 0, 0, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    for (i = 0; i < 1; i++) {
        for (j = 0; j < work->orb_count; j++) {
            if (work->orb_arrived[i][j] == 0) {
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[3] >> 16;
                ft4.clut = work->texture[3];
                setRGB0(&ft4, work->config->orb_r, work->config->orb_g, work->config->orb_b);
                setVector(&vertices[0], -work->config->orb_size, -work->config->orb_size, 0);
                setVector(&vertices[1], work->config->orb_size, -work->config->orb_size, 0);
                setVector(&vertices[2], -work->config->orb_size, work->config->orb_size, 0);
                setVector(&vertices[3], work->config->orb_size, work->config->orb_size, 0);
                GsSetLsMatrix(&base);
                RotTrans(&work->orbs[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&work->orb_spins[i][j], &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                    (PSXLONG *)&ft4.x3, &interpolation, &flag);
                setUV4(&ft4, 0x98, 0, 0xB7, 0, 0x98, 0x1F, 0xB7, 0x1F);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)depth, 1);
                }
                addVector(&work->orbs[i][j], &work->orb_velocities[i][j]);
                if (direction > 0) {
                    if (work->orbs[i][j].vz > work->orb_targets[i][j]) {
                        work->orb_arrived[i][j] = 1;
                    }
                } else if (work->orbs[i][j].vz < work->orb_targets[i][j]) {
                    work->orb_arrived[i][j] = 1;
                }
            } else if (work->orb_r[i][j] != 0 || work->orb_g[i][j] != 0 || work->orb_b[i][j] != 0) {
                if (work->phase == 0) {
                    work->phase = 1;
                }
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setRGB0(&ft4, work->orb_r[i][j], work->orb_g[i][j], work->orb_b[i][j]);
                setVector(&vertices[0], -work->config->orb_burst_size, -work->config->orb_burst_size, 0);
                setVector(&vertices[1], work->config->orb_burst_size, -work->config->orb_burst_size, 0);
                setVector(&vertices[2], -work->config->orb_burst_size, work->config->orb_burst_size, 0);
                setVector(&vertices[3], work->config->orb_burst_size, work->config->orb_burst_size, 0);
                GsSetLsMatrix(&base);
                RotTrans(&work->orbs[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                    (PSXLONG *)&ft4.x3, &interpolation, &flag);
                setUV4(&ft4, 0, 0x20, 0x3F, 0x20, 0, 0x5F, 0x3F, 0x5F);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)depth, 1);
                }
                if (work->orb_r[i][j] >= 32) work->orb_r[i][j] -= work->config->orb_fade;
                else work->orb_r[i][j] = 0;
                if (work->orb_g[i][j] >= 32) work->orb_g[i][j] -= work->config->orb_fade;
                else work->orb_g[i][j] = 0;
                if (work->orb_b[i][j] >= 32) work->orb_b[i][j] -= work->config->orb_fade;
                else work->orb_b[i][j] = 0;
            }
        }
        for (j = 0; j < work->comet_count; j++) {
            if (work->comet_arrived[i][j] == 0) {
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[0] >> 16;
                ft4.clut = work->texture[0];
                setRGB0(&ft4, work->config->comet_r, work->config->comet_g, work->config->comet_b);
                setVector(&vertices[0], -work->config->comet_size, -work->config->comet_size, 0);
                setVector(&vertices[1], work->config->comet_size, -work->config->comet_size, 0);
                setVector(&vertices[2], -work->config->comet_size, work->config->comet_size, 0);
                setVector(&vertices[3], work->config->comet_size, work->config->comet_size, 0);
                GsSetLsMatrix(&base);
                RotTrans(&work->comets[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&work->comet_spins[i][j], &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                    (PSXLONG *)&ft4.x3, &interpolation, &flag);
                frame = rand() % 3;
                setUV4(&ft4, frame * 32, 0, frame * 32 + 31, 0, frame * 32, 0x1F, frame * 32 + 31, 0x1F);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)depth, 1);
                }
                addVector(&work->comets[i][j], &work->comet_velocities[i][j]);
                if (direction > 0) {
                    if (work->comets[i][j].vz > work->comet_targets[i][j]) {
                        work->comet_arrived[i][j] = 1;
                    }
                } else if (work->comets[i][j].vz < work->comet_targets[i][j]) {
                    work->comet_arrived[i][j] = 1;
                }
            } else if (work->comet_r[i][j] != 0 || work->comet_g[i][j] != 0 || work->comet_b[i][j] != 0) {
                if (work->phase == 0) {
                    work->phase = 1;
                }
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setRGB0(&ft4, work->comet_r[i][j], work->comet_g[i][j], work->comet_b[i][j]);
                setVector(&vertices[0], -work->config->comet_burst_size, -work->config->comet_burst_size, 0);
                setVector(&vertices[1], work->config->comet_burst_size, -work->config->comet_burst_size, 0);
                setVector(&vertices[2], -work->config->comet_burst_size, work->config->comet_burst_size, 0);
                setVector(&vertices[3], work->config->comet_burst_size, work->config->comet_burst_size, 0);
                GsSetLsMatrix(&base);
                RotTrans(&work->comets[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                    (PSXLONG *)&ft4.x3, &interpolation, &flag);
                setUV4(&ft4, 0, 0x20, 0x3F, 0x20, 0, 0x5F, 0x3F, 0x5F);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)depth, 1);
                }
                ft4.tpage = work->texture[4] >> 16;
                ft4.clut = work->texture[4];
                setVector(&vertices[0], -work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[1], work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[2], -work->config->shard_size, work->config->shard_size, 0);
                setVector(&vertices[3], work->config->shard_size, work->config->shard_size, 0);
                for (k = 0; k < 8; k++) {
                    GsSetLsMatrix(&base);
                    RotTrans(&work->shards[i][j][k], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                        (PSXLONG *)&ft4.x3, &interpolation, &flag);
                    if (!(rand() & 1)) {
                        setUV4(&ft4, 0x60, 0, 0x7F, 0, 0x60, 0x1F, 0x7F, 0x1F);
                    } else {
                        setUV4(&ft4, 0x80, 0x20, 0x9F, 0x20, 0x80, 0x3F, 0x9F, 0x3F);
                    }
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)depth, 1);
                    }
                    addVector(&work->shards[i][j][k], &work->shard_velocities[i][j][k]);
                }
                if (work->comet_r[i][j] >= 16) work->comet_r[i][j] -= work->config->comet_fade;
                else work->comet_r[i][j] = 0;
                if (work->comet_g[i][j] >= 16) work->comet_g[i][j] -= work->config->comet_fade;
                else work->comet_g[i][j] = 0;
                if (work->comet_b[i][j] >= 16) work->comet_b[i][j] -= work->config->comet_fade;
                else work->comet_b[i][j] = 0;
            }
        }
        for (j = 0; j < work->spinner_count; j++) {
            if (work->spinner_arrived[i][j] == 0) {
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[2] >> 16;
                ft4.clut = work->texture[2];
                setRGB0(&ft4, 0x80, 0x80, 0x80);
                setVector(&vertices[0], -work->config->spinner_width, -work->config->spinner_height, 0);
                setVector(&vertices[1], work->config->spinner_width, -work->config->spinner_height, 0);
                setVector(&vertices[2], -work->config->spinner_width, work->config->spinner_height, 0);
                /* Retail takes this corner's height from the comet size. */
                setVector(&vertices[3], work->config->spinner_width, work->config->comet_size, 0);
                GsSetLsMatrix(&base);
                RotTrans(&work->spinners[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&work->spinner_spins[i][j], &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                    (PSXLONG *)&ft4.x3, &interpolation, &flag);
                setUV4(&ft4, 0, 0x60, 0x1F, 0x60, 0, 0x9F, 0x1F, 0x9F);
                setSemiTrans(&ft4, 0);
                if (depth >= 0) {
                    GsSortPoly(&ft4, ot, (u16)depth);
                }
                work->spinner_spins[i][j].vz = (work->spinner_spins[i][j].vz + work->config->spin) % 4096;
                addVector(&work->spinners[i][j], &work->spinner_velocities[i][j]);
                if (direction > 0) {
                    if (work->spinners[i][j].vz > work->spinner_targets[i][j]) {
                        work->spinner_arrived[i][j] = 1;
                    }
                } else if (work->spinners[i][j].vz < work->spinner_targets[i][j]) {
                    work->spinner_arrived[i][j] = 1;
                }
            } else if (work->debris_level[i][j] != 0) {
                if (work->phase == 0) {
                    work->phase = 1;
                }
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[2] >> 16;
                ft4.clut = work->texture[2];
                setRGB0(&ft4, work->debris_level[i][j], work->debris_level[i][j], work->debris_level[i][j]);
                setVector(&vertices[0], -work->config->debris_width, -work->config->debris_height, 0);
                setVector(&vertices[1], work->config->debris_width, -work->config->debris_height, 0);
                setVector(&vertices[2], -work->config->debris_width, work->config->debris_height, 0);
                setVector(&vertices[3], work->config->debris_width, work->config->debris_height, 0);
                for (k = 0; k < 8; k++) {
                    if (work->debris[i][j][k].vy < 0) {
                        GsSetLsMatrix(&base);
                        RotTrans(&work->debris[i][j][k], (VECTOR *)matrix.t, &flag);
                        RotMatrix(&work->debris_spins[i][j][k], &matrix);
                        ScaleMatrix(&matrix, &scale);
                        GsSetLsMatrix(&matrix);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2,
                            (PSXLONG *)&ft4.x3, &interpolation, &flag);
                        setUV4(&ft4, 0, 0x60, 0x1F, 0x60, 0, 0x9F, 0x1F, 0x9F);
                        setSemiTrans(&ft4, 1);
                        if (depth >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)depth, 1);
                        }
                        addVector(&work->debris[i][j][k], &work->debris_velocities[i][j][k]);
                        work->debris_velocities[i][j][k].vy += work->config->debris_fall;
                        work->debris_spins[i][j][k].vz = (work->debris_spins[i][j][k].vz + work->config->spin / 2) % 4096;
                    }
                }
                if (work->debris_level[i][j] >= 10) work->debris_level[i][j] -= work->config->spinner_fade;
                else work->debris_level[i][j] = 0;
            }
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 3) {
        return 2;
    }
    if (work->elapsed >= work->config->emit_end && work->phase == 1 && work->orb_arrived[0][work->orb_count - 1] == 1
        && work->comet_arrived[0][work->comet_count - 1] == 1) {
        work->phase = 2;
        return 1;
    }
    return (work->phase == 1) * 4;
}
