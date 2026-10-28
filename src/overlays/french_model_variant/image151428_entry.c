#include "../../types.h"
#include "image151428_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Image151428State *work;
    MATRIX light, matrix;
    GsGLINE line;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    MATRIX impact_matrix;
    PSXLONG flag, interpolation;
    s32 slot, step;
    GsOT *ot;
    s32 i;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C140;
    work = (Image151428State *)context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &D_8013C16C[command];
        for (i = 0; i < 256; i++) {
            setVector(&work->lines[i][0], 0, 0, 0);
            setVector(&work->lines[i][1], 0, 0, 0);
            work->trail_ages[i] = 0;
            work->impact_flags[i] = 0;
            work->impact_frames[i] = 0;
        }
        setVector(&work->velocity, 0, 0, 0);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&position);
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i],
                (position.vx / 2) * ((rand() - rand()) % 4096) / 4096,
                (position.vy / 2) * ((rand() - rand()) % 4096) / 4096,
                (position.vz / 2) * ((rand() - rand()) % 4096) / 4096);
            work->spark_frames[i] = 0;
        }
        for (i = 0; i < 1; i++)
            work->textures[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C150[i]);
        work->ambient = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->live_count = 0;
        work->batch_count = 0;
        work->camera_mode = 1;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->camera_mode == 0) {
        if (work->elapsed < work->config->camera_first_end)
            Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, work->config->camera_first);
        else if (work->elapsed < work->config->camera_second_end)
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rotation, work->config->camera_second);
        else
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rotation, work->config->camera_third);
    }
    if (work->camera_mode == 1) {
        if (work->elapsed < work->config->camera_first_end)
            Model_QueueCameraOrbitFromPreset(slot, 3, &rotation, work->config->camera_first);
        else if (work->elapsed < work->config->camera_second_end)
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rotation, work->config->camera_second);
        else
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rotation, work->config->camera_third);
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
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->phase == 0) {
        Model_CopySlotPosition(slot, (u16 *)&position);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rotation);
        work->velocity.vx = 0;
        work->velocity.vy = (rotation.vy - position.vy) / 6;
        work->velocity.vz = (rotation.vz - position.vz) / 6;
        work->phase = 1;
    }
    if (work->elapsed < work->config->emission_end && work->live_count + 4 < 256) {
        GsGetLw(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
        for (i = 0; i < 4; i++) {
            setVector(&work->lines[work->live_count][0],
                work->config->spread * ((rand() - rand()) % 4096) / 4096,
                0, work->config->spread * ((rand() - rand()) % 4096) / 4096);
            copyVector(&work->lines[work->live_count][1], &work->lines[work->live_count][0]);
            applyVector(&work->lines[work->live_count][0], matrix.t[0], matrix.t[1], matrix.t[2], +=);
            applyVector(&work->lines[work->live_count][1], matrix.t[0], matrix.t[1], matrix.t[2], +=);
            work->live_count++;
        }
        work->batch_count++;
    }
    line.attribute = 0x50000000;
    setVector(&rotation, 0, 0, 0);
    setVector(&position, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    GsSetLsMatrix(&light);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&light, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setPolyFT4(&quad);
    quad.tpage = work->textures[0] >> 16;
    quad.clut = work->textures[0];
    setRGB0(&quad, 32, 32, 32);
    for (i = 0; i < work->live_count; i++) {
        if (work->impact_flags[i] == 0) {
            PSXLONG line_depth;

            GsSetLsMatrix(&matrix);
            setRGB0(&line, work->config->line_red, work->config->line_green, work->config->line_blue);
            setRGB1(&line, 0, 0, 0);
            line_depth = (RotTransPers(&work->lines[i][1], (PSXLONG *)&line.x0, &interpolation, &flag)
                + RotTransPers(&work->lines[i][0], (PSXLONG *)&line.x1, &interpolation, &flag)) / 2;
            if (line_depth >= 0)
                GsSortGLine(&line, ot, line_depth);
            if (work->lines[i][1].vy < 0) {
                addVector(&work->lines[i][1], &work->velocity);
                if (work->trail_ages[i]++ >= 4) {
                    addVector(&work->lines[i][0], &work->velocity);
                }
                if (work->lines[i][1].vy >= 0) {
                    work->impact_flags[i] = 1;
                    work->lines[i][1].vy = 0;
                }
            }
        }
        if (work->impact_flags[i] != 0) {
            GsSetLsMatrix(&light);
            RotTrans(&work->lines[i][1], (VECTOR *)impact_matrix.t, &flag);
            RotMatrix(&rotation, &impact_matrix);
            ScaleMatrix(&impact_matrix, &scale);
            GsSetLsMatrix(&impact_matrix);
            if (work->impact_frames[i] < 16) {
                setVector(&corners[0], -work->config->half_size, -work->config->half_size, 0);
                setVector(&corners[1], work->config->half_size, -work->config->half_size, 0);
                setVector(&corners[2], -work->config->half_size, work->config->half_size, 0);
                setVector(&corners[3], work->config->half_size, work->config->half_size, 0);
                addVector(&corners[0], &work->lines[i][1]);
                addVector(&corners[1], &work->lines[i][1]);
                addVector(&corners[2], &work->lines[i][1]);
                addVector(&corners[3], &work->lines[i][1]);
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                setUV4(&quad,
                    work->impact_frames[i] / 2 % 4 * 32, work->impact_frames[i] / 8 * 32,
                    work->impact_frames[i] / 2 % 4 * 32 + 31, work->impact_frames[i] / 8 * 32,
                    work->impact_frames[i] / 2 % 4 * 32, work->impact_frames[i] / 8 * 32 + 31,
                    work->impact_frames[i] / 2 % 4 * 32 + 31, work->impact_frames[i] / 8 * 32 + 31);
                if (depth >= 0)
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                work->impact_frames[i]++;
            }
        }
    }
    Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
    GsSetLsMatrix(&light);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setRGB0(&quad, work->config->spark_red, work->config->spark_green, work->config->spark_blue);
    for (i = 0; i < work->batch_count; i++) {
        if (work->spark_frames[i] < 16) {
            setVector(&corners[0], -work->config->half_size, -work->config->half_size, 0);
            setVector(&corners[1], work->config->half_size, -work->config->half_size, 0);
            setVector(&corners[2], -work->config->half_size, work->config->half_size, 0);
            setVector(&corners[3], work->config->half_size, work->config->half_size, 0);
            addVector(&corners[0], &work->sparks[i]);
            addVector(&corners[1], &work->sparks[i]);
            addVector(&corners[2], &work->sparks[i]);
            addVector(&corners[3], &work->sparks[i]);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad,
                work->spark_frames[i] / 2 % 4 * 32, work->spark_frames[i] / 8 * 32,
                work->spark_frames[i] / 2 % 4 * 32 + 31, work->spark_frames[i] / 8 * 32,
                work->spark_frames[i] / 2 % 4 * 32, work->spark_frames[i] / 8 * 32 + 31,
                work->spark_frames[i] / 2 % 4 * 32 + 31, work->spark_frames[i] / 8 * 32 + 31);
            if (depth >= 0)
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            work->spark_frames[i]++;
            work->sparks[i].vy -= work->config->rise;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3 && work->impact_frames[work->live_count - 1] >= 16)
        return 2;
    if (work->elapsed > work->config->emission_end) {
        if (work->phase < 2) {
            work->phase = 2;
            return 1;
        }
    } else if (work->phase < 2)
        return 4;
    return 0;
}
