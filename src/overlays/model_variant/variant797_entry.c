#include "../../types.h"
#include "variant797_entry.h"
#include "../../game/screen_projection.h"

/* Header 797's entry: two glowing flares beside a model part spin sixteen
 * rays each while the light dims; then animated strikes land on the opposing
 * slot and periodic bursts there flash and fade with sixteen double rays. */

static const VECTOR D_8013C6C0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model797State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 flare;
    POLY_G3 ray;
    POLY_FT4 sprite;
    POLY_G3 beam;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG interpolation;
    s32 slot;
    s32 step;
    GsOT *ot;
    void *entry;
    s32 direction;
    POLY_FT4 *fl;
    POLY_G3 *rp;
    s32 i, j;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C6C0;
    work = (Model797State *)context;
    slot = Model_GetActiveSlotIndex();
    fl = &flare;
    rp = &ray;
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C708[command];
        for (i = 0; i < 64; i++) {
            setVector(&work->strikes[i], 0, 0, 0);
            setVector(&work->bursts[i], 0, 0, 0);
            work->strike_frames[i] = 0;
            work->burst_r[i] = work->config->r;
            work->burst_g[i] = work->config->g;
            work->burst_b[i] = work->config->b;
            for (j = 0; j < 16; j++) {
                setVector(&work->rays[i][j], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096);
            }
        }
        setVector(&work->offsets[0], 20, -16, direction * 16);
        setVector(&work->offsets[1], -20, -16, direction * 16);
        for (i = 0; i < 16; i++) {
            setVector(&work->spins[0][i], rand() % 4096, rand() % 4096, rand() % 4096);
            setVector(&work->spins[1][i], rand() % 4096, rand() % 4096, rand() % 4096);
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C6D0[i]);
        }
        work->flare[0] = 0;
        work->flare[1] = 0;
        work->flare[2] = 0;
        work->flare[3] = 0;
        work->level = 2048;
        work->strike_count = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
        work->burst_count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_time) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rotation, -16);
    }
    if (work->strike_count == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 1) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 2;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->start) {
        setPolyFT4(fl);
        setPolyG3(rp);
        entry = Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part);
        GsGetLwUnit(entry, &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        fl->tpage = work->texture[1] >> 16;
        fl->clut = work->texture[1];
        setUV4(fl, 0, 0x80, 0x3F, 0x80, 0, 0xBF, 0x3F, 0xBF);
        setRGB0(fl, work->flare[0], work->flare[0], work->flare[0]);
        setRGB0(rp, 0, 0, 0);
        setRGB1(rp, work->flare[1], work->flare[2], work->flare[3]);
        setRGB2(rp, work->flare[1], work->flare[2], work->flare[3]);
        setSemiTrans(fl, 1);
        setSemiTrans(rp, 1);
        for (i = 0; i < 2; i++) {
            setVector(&vertices[0], -24, -24, 0);
            setVector(&vertices[1], 24, -24, 0);
            setVector(&vertices[2], -24, 24, 0);
            setVector(&vertices[3], 24, 24, 0);
            GsGetLwUnit(entry, &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            addVector(&position, &work->offsets[i]);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            saved = matrix;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&fl->x0, (PSXLONG *)&fl->x1, (PSXLONG *)&fl->x2,
                (PSXLONG *)&fl->x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(fl, ot, (u16)depth);
            }
            setVector(&vertices[0], 0, -48, 0);
            setVector(&vertices[1], -3, 0, 0);
            setVector(&vertices[2], 3, 0, 0);
            for (j = 0; j < 16; j++) {
                matrix = saved;
                RotMatrix(&work->spins[i][j], &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&vertices[0], &vertices[1], &vertices[2],
                    (PSXLONG *)&rp->x0, (PSXLONG *)&rp->x1, (PSXLONG *)&rp->x2, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)rp, ot, (u16)depth, 1);
                }
                (&work->spins[i][j])->vx += 64;
                (&work->spins[i][j])->vy += 64;
                (&work->spins[i][j])->vz += 64;
            }
        }
        if (work->flare[0] < 113) {
            work->flare[0] += 15;
        } else {
            work->flare[0] = 128;
        }
        if (work->flare[1] < 113) {
            work->flare[1] += 15;
        } else {
            work->flare[1] = 128;
        }
        if (work->flare[2] < 81) {
            work->flare[2] += 15;
        } else {
            work->flare[2] = 96;
        }
        if (work->flare[3] < 81) {
            work->flare[3] += 15;
        } else {
            work->flare[3] = 96;
        }
    }
    if (work->elapsed < work->config->hold) {
        PopMatrix();
        work->elapsed += step;
        return 0;
    }
    if (work->elapsed < work->config->emit_end && work->strike_count < 64) {
        Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
        position.vx += rotation.vx / 2 * ((rand() - rand()) % 4096) / 4096;
        position.vy += rotation.vy / 2 * ((rand() - rand()) % 4096) / 4096;
        position.vz += rotation.vz / 2 * ((rand() - rand()) % 4096) / 4096;
        setVector(&work->strikes[work->strike_count], position.vx, position.vy, position.vz);
        work->strike_count++;
        if (work->tick % work->config->burst_period == 0 && work->burst_count < 64) {
            Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
            Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
            position.vx += rotation.vx / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vy += rotation.vy / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vz += rotation.vz / 2 * ((rand() - rand()) % 4096) / 4096;
            setVector(&work->bursts[work->burst_count], position.vx, position.vy, position.vz);
            work->burst_count++;
        }
    }
    for (i = 0; i < work->strike_count; i++) {
        if (work->strike_frames[i] < 8) {
            setPolyFT4(&sprite);
            sprite.tpage = work->texture[0] >> 16;
            sprite.clut = work->texture[0];
            setRGB0(&sprite, 255, 255, 255);
            setVector(&vertices[0], -work->config->strike_size, -work->config->strike_size, 0);
            setVector(&vertices[1], work->config->strike_size, -work->config->strike_size, 0);
            setVector(&vertices[2], -work->config->strike_size, work->config->strike_size, 0);
            setVector(&vertices[3], work->config->strike_size, work->config->strike_size, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&work->strikes[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&sprite.x0, (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2,
                (PSXLONG *)&sprite.x3, &interpolation, &flag);
            if (work->strike_frames[i] < 8) {
                setUV4(&sprite, work->strike_frames[i] % 4 * 64, 0, work->strike_frames[i] % 4 * 64 + 63, 0,
                    work->strike_frames[i] % 4 * 64, 63, work->strike_frames[i] % 4 * 64 + 63, 63);
            } else {
                setUV4(&sprite, work->strike_frames[i] % 4 * 64, 64, work->strike_frames[i] % 4 * 64 + 63, 64,
                    work->strike_frames[i] % 4 * 64, 127, work->strike_frames[i] % 4 * 64 + 63, 127);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)depth, 1);
            }
            work->strike_frames[i]++;
        }
    }
    for (i = 0; i < work->burst_count; i++) {
        if (work->burst_r[i] != 0 || work->burst_g[i] != 0 || work->burst_b[i] != 0) {
            setPolyFT4(&sprite);
            sprite.tpage = work->texture[1] >> 16;
            sprite.clut = work->texture[1];
            setRGB0(&sprite, work->burst_r[i], work->burst_g[i], work->burst_b[i]);
            setVector(&vertices[0], -work->config->burst_size, -work->config->burst_size, 0);
            setVector(&vertices[1], work->config->burst_size, -work->config->burst_size, 0);
            setVector(&vertices[2], -work->config->burst_size, work->config->burst_size, 0);
            setVector(&vertices[3], work->config->burst_size, work->config->burst_size, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&work->bursts[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            saved = matrix;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&sprite.x0, (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2,
                (PSXLONG *)&sprite.x3, &interpolation, &flag);
            setUV4(&sprite, 0, 0x80, 0x3F, 0x80, 0, 0xBF, 0x3F, 0xBF);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)depth, 1);
            }
            setPolyG3(&beam);
            setRGB0(&beam, 0, 0, 0);
            setVector(&vertices[0], -work->config->ray_size, 0, 0);
            setVector(&vertices[1], work->config->ray_size, 0, 0);
            setRGB1(&beam, work->burst_r[i], work->burst_g[i], work->burst_b[i]);
            setRGB2(&beam, work->burst_r[i], work->burst_g[i], work->burst_b[i]);
            for (j = 0; j < 16; j++) {
                matrix = saved;
                GsSetLsMatrix(&base);
                setVector(&scale, 4096, 4096, 4096);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->rays[i][j], &vertices[0], &vertices[1],
                    (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2, &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)depth, 1);
                }
                setVector(&scale, 8192, 8192, 8192);
                matrix = saved;
                GsSetLsMatrix(&base);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->rays[i][j], &vertices[0], &vertices[1],
                    (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2, &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)depth, 3);
                }
            }
            if (work->burst_r[i] >= 16) {
                work->burst_r[i] -= work->config->fade;
            } else {
                work->burst_r[i] = 0;
            }
            if (work->burst_g[i] >= 16) {
                work->burst_g[i] -= work->config->fade;
            } else {
                work->burst_g[i] = 0;
            }
            if (work->burst_b[i] >= 16) {
                work->burst_b[i] -= work->config->fade;
            } else {
                work->burst_b[i] = 0;
            }
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 2) {
        return 2;
    }
    if (work->elapsed >= work->config->emit_end && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->elapsed >= work->config->hold && work->phase == 0) {
        return 4;
    }
    return 0;
}
