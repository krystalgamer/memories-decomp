#include "../../types.h"
#include "image119708_entry.h"

#define IMAGE119708_DRAW_PARTICLES() \
    for (i = 0; i < work->live_count; i++) { \
        if (work->ages[i] < 24) { \
            setPolyFT4(&quad); \
            setRGB0(&quad, 128, 128, 128); \
            quad.tpage = work->textures[0] >> 16; \
            quad.clut = work->textures[0]; \
            setVector(&corners[0], -work->config->half_size, -work->config->half_size, 0); \
            setVector(&corners[1], work->config->half_size, -work->config->half_size, 0); \
            setVector(&corners[2], -work->config->half_size, work->config->half_size, 0); \
            setVector(&corners[3], work->config->half_size, work->config->half_size, 0); \
            setVector(&position, 0, 0, 0); \
            setVector(&rotation, 0, 0, 0); \
            setVector(&scale, 4096, 4096, 4096); \
            GsSetLsMatrix(&light); \
            RotTrans(&work->particles[i], (VECTOR *)matrix.t, &flag); \
            RotMatrix(&rotation, &matrix); \
            ScaleMatrix(&matrix, &scale); \
            GsSetLsMatrix(&matrix); \
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3], \
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, \
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag); \
            setUV4(&quad, \
                (u16)(work->ages[i] / 3) * 32, 0, \
                (u16)(work->ages[i] / 3) * 32 + 31, 0, \
                (u16)(work->ages[i] / 3) * 32, 31, \
                (u16)(work->ages[i] / 3) * 32 + 31, 31); \
            if (depth >= 0) \
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1); \
            work->ages[i]++; \
            work->particles[i].vy -= work->speeds[i]; \
        } \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Image119708State *work;
    MATRIX light, matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    PSXLONG flag, interpolation, depth;
    GsOT *ot;
    GsCOORDUNIT *coordinate;
    s32 i, value, slot, direction, step;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C2CC;
    work = (Image119708State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C2F8[command];
        Model_CopySlotPosition(slot, (u16 *)&position);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rotation);
        setVector(&work->offset, 0, 0, 0);
        work->velocity.vx = 0;
        work->velocity.vy = 350 / work->config->rise_frames;
        work->velocity.vz = (rotation.vz - position.vz) / work->config->rise_frames;
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&position);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->particles[i],
                rotation.vx + work->config->spread * ((rand() - rand()) % 4096) / 4096,
                rotation.vy - work->config->spread * (rand() % 4096) / 4096,
                rotation.vz + work->config->spread * ((rand() - rand()) % 4096) / 4096);
            value = rand() - rand();
            work->speeds[i] = work->config->fall_speed + value % (work->config->fall_speed / 2);
            work->ages[i] = 0;
        }
        for (i = 0; i < 1; i++)
            work->textures[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C2DC[i]);
        work->scale = 4096;
        work->ambient = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->live_count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->pause_end)
        Model_QueueCameraOrbitFromPreset(slot, 13, &rotation, -8);
    else {
        if (direction > 0)
            Model_QueueCameraOrbitFromPreset(2, 9, &rotation, 16);
        else
            Model_QueueCameraOrbitFromPreset(2, 11, &rotation, 16);
    }
    if (work->phase == 0) {
        if (work->ambient > -512)
            work->ambient -= step * 64;
        else
            work->ambient = -512;
    } else if (work->phase == 2) {
        if (work->ambient < 2048)
            work->ambient += step * 64;
        else {
            work->phase = 3;
            work->ambient = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    setVector(&scale, work->scale, work->scale, work->scale);
    Model_SetSlotScale(slot, (ModelSlotS32Quad *)&scale);
    if (work->elapsed < work->config->rise_end) {
        setVector(&scale, 4096, 4096, 4096);
        work->elapsed += step;
        coordinate = (GsCOORDUNIT *)Model_GetCurrentDataEntry(slot);
        TransMatrix(&coordinate->matrix, &work->offset);
        Model_UpdateCoordinateUnitMatrix(coordinate, &scale);
        addVector(&work->offset, &work->velocity);
        PopMatrix();
        return 0;
    } else if (work->elapsed < work->config->pause_end) {
        work->elapsed += step;
        PopMatrix();
        return 0;
    } else if (work->elapsed < work->config->grow_end) {
        setVector(&scale, 4096, 4096, 4096);
        IMAGE119708_DRAW_PARTICLES();
        work->elapsed += step;
        if (work->scale < 8191)
            work->scale += work->config->scale_step;
        else {
            work->live_count++;
            if (work->live_count > work->config->count)
                work->live_count = work->config->count;
        }
        PopMatrix();
        return 4;
    } else if (work->phase == 0) {
        setVector(&scale, 4096, 4096, 4096);
        IMAGE119708_DRAW_PARTICLES();
        if (work->scale > 4096) {
            work->live_count++;
            if (work->live_count > work->config->count)
                work->live_count = work->config->count;
            work->scale -= work->config->scale_step;
        }
        work->phase = 1;
        work->elapsed += step;
        PopMatrix();
        return 1;
    } else if (work->elapsed < work->config->hold_end) {
        setVector(&scale, 4096, 4096, 4096);
        IMAGE119708_DRAW_PARTICLES();
        if (work->scale > 4096) {
            work->live_count++;
            if (work->live_count > work->config->count)
                work->live_count = work->config->count;
            work->scale -= work->config->scale_step;
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    } else if (work->elapsed < work->config->return_end) {
        IMAGE119708_DRAW_PARTICLES();
        coordinate = (GsCOORDUNIT *)Model_GetCurrentDataEntry(slot);
        TransMatrix(&coordinate->matrix, &work->offset);
        Model_UpdateCoordinateUnitMatrix(coordinate, &scale);
        work->phase = 2;
        applyVector(&work->offset, work->velocity.vx, work->velocity.vy, work->velocity.vz, -=);
        work->elapsed += step;
        PopMatrix();
        return 0;
    } else {
        work->elapsed += step;
        PopMatrix();
        return 2;
    }
}

#undef IMAGE119708_DRAW_PARTICLES
