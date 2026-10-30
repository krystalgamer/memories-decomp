#include "../../types.h"
#include "image26952_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX light, matrix;
    POLY_FT4 quad, burst;
    POLY_G4 trail;
    SVECTOR rotation;
    Image26952Position position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Image26952State *state;
    s32 active, sign, step;
    GsOT *ot;
    s32 i, j, n, theta, depth, distance;

    memset(&rotation, 0, sizeof(rotation));
    memset(&position, 0, sizeof(position));
    scale = D_8013C544;
    state = (Image26952State *)context;
    active = Model_GetActiveSlotIndex();
    sign = active * 2 - 1;
    if (command >= 0) {
        state->config = &D_8013C58C[command];
        Model_GetSlotDimensionsWithMinimums(active ^ 1, &position.dimensions);
        Model_CopySlotPosition(active ^ 1, (u16 *)&rotation);
        for (i = 0; i < state->config->count; i++) {
            j = 4096 / state->config->count;
            theta = j * i;
            setVector(&state->positions[i],
                state->config->radius * ccos(theta % 4096) / 4096,
                state->config->radius * csin(theta % 4096) / 4096 - 350,
                -sign * 449);
            setVector(&state->targets[i],
                rotation.vx + position.dimensions.x / 2 * ((rand() - rand()) % 4096) / 4096,
                rotation.vy + position.dimensions.y / 2 * ((rand() - rand()) % 4096) / 4096,
                sign * 450);
            setVector(&state->velocities[i],
                (state->targets[i].vx - state->positions[i].vx) / state->config->travel_duration,
                (state->targets[i].vy - state->positions[i].vy) / state->config->travel_duration,
                (state->targets[i].vz - state->positions[i].vz) / state->config->travel_duration);
            setVector(&state->rotations[i], 0, 0, theta);
            state->hit[i] = 0;
            state->color[i] = 128;
            state->length[i] = 0;
            state->head[i] = 1;
            for (j = 0; j < state->config->spark_count; j++) {
                setVector(&state->sparks[i][j],
                    state->config->spread * ((rand() - rand()) % 4096) / 4096,
                    state->config->spread * ((rand() - rand()) % 4096) / 4096,
                    state->config->spread * ((rand() - rand()) % 4096) / 4096);
                state->spark_frames[i][j] = 0;
            }
            for (j = 0; j < 32; j++) {
                copyVector(&state->history[i][j], &state->positions[i]);
            }
        }
        state->textures[0] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &D_8013C554[0]);
        state->textures[1] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &D_8013C554[1]);
        state->ambient = 2048;
        state->stage = 0;
        state->elapsed = 0;
        state->visible = 0;
        state->launched = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (state->elapsed < state->config->camera_delay) {
        Model_QueueCameraOrbitFromPreset(active, 0, &rotation, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(active ^ 1, 1, &rotation, -16);
    }
    if (state->stage == 0) {
        if (state->ambient >= -511) {
            state->ambient -= step * 64;
        } else {
            state->ambient = -512;
        }
    } else if (state->stage == 2) {
        if (state->ambient < 2048) {
            state->ambient += step * 64;
        } else {
            state->stage = 3;
            state->ambient = 2048;
        }
    }
    Model_SetSlotAmbient(2, state->ambient, state->ambient, state->ambient);
    if (state->elapsed < state->config->start) {
        state->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < state->visible; i++) {
        MATRIX *render_matrix = &matrix;
        PSXLONG *render_flag = &flag;
        POLY_FT4 *head_packet = &quad;

        if (state->color[i]) {
            setPolyFT4(&quad);
            quad.tpage = state->textures[0] >> 16;
            quad.clut = state->textures[0];
            setRGB0(&quad, state->color[i], state->color[i], state->color[i]);
            setVector(&vertices[0], 0, -state->config->height, 0);
            setVector(&vertices[1], state->config->width, -state->config->height, 0);
            setVector(&vertices[2], 0, state->config->height, 0);
            setVector(&vertices[3], state->config->width, state->config->height, 0);
            GsSetLsMatrix(&light);
            RotTrans(&state->positions[i], (VECTOR *)matrix.t, render_flag);
            RotMatrix(&state->rotations[i], render_matrix);
            MulMatrix2(&light, render_matrix);
            ScaleMatrix(render_matrix, &scale);
            GsSetLsMatrix(render_matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, render_flag);
            setUV4(&quad, 0, 32, 63, 32, 0, 63, 63, 63);
            if (state->hit[i] == 0) {
                setSemiTrans(&quad, 0);
            } else {
                setSemiTrans(&quad, 1);
            }
            if (depth >= 0) {
                GsSortPoly(head_packet, ot, (u16)depth);
            }
        }
        if (state->hit[i]) {
            setPolyFT4(&burst);
            burst.tpage = state->textures[1] >> 16;
            burst.clut = state->textures[1];
            setRGB0(&burst, 128, 0, 0);
            setVector(&vertices[0], -state->config->spark_size, -state->config->spark_size, 0);
            setVector(&vertices[1], state->config->spark_size, -state->config->spark_size, 0);
            setVector(&vertices[2], -state->config->spark_size, state->config->spark_size, 0);
            setVector(&vertices[3], state->config->spark_size, state->config->spark_size, 0);
            for (j = 0; j < state->config->spark_count; j++) {
                if (state->spark_frames[i][j] < 8) {
                    copyVector(&position.vector, &state->targets[i]);
                    addVector(&position.vector, &state->sparks[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&position.vector, (VECTOR *)matrix.t, render_flag);
                    RotMatrix(&rotation, render_matrix);
                    ScaleMatrix(render_matrix, &scale);
                    GsSetLsMatrix(render_matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1,
                        (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3, &interpolation, render_flag);
                    setUV4(&burst,
                        state->spark_frames[i][j] * 32, 0,
                        state->spark_frames[i][j] * 32 + 31, 0,
                        state->spark_frames[i][j] * 32, 31,
                        state->spark_frames[i][j] * 32 + 31, 31);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                    }
                    state->spark_frames[i][j]++;
                }
            }
        }
    }
    if (state->elapsed >= state->config->camera_delay) {
        for (i = 0; i < state->launched; i++) {
            if (state->color[i]) {
                setVector(&state->rotations[i],
                    ratan2(state->velocities[i].vz, state->velocities[i].vy) - 1024,
                    sign * (ratan2(state->velocities[i].vx, state->velocities[i].vz) - 1024),
                    0);
                addVector(&state->positions[i], &state->velocities[i]);
                copyVector(&state->history[i][state->head[i]], &state->positions[i]);
                state->head[i] = (state->head[i] + 1) % 32;
                setPolyG4(&trail);
                if (state->head[i] > state->length[i] && state->color[i]) {
                    s32 last;

                    setVector(&position.vector, 0, 0, 0);
                    setVector(&rotation, 0, 0, 0);
                    setVector(&scale, 4096, 4096, 4096);
                    GsSetLsMatrix(&light);
                    RotTrans(&position.vector, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    MulMatrix2(&light, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    for (j = state->head[i] - 1, n = 1;
                         last = state->head[i] - 1, j >= last - state->length[i]; j--, n++) {
                        setRGB0(&trail, state->color[i] / n, state->color[i] / n, state->color[i] / n);
                        setRGB1(&trail, state->color[i] / n, state->color[i] / n, state->color[i] / n);
                        setRGB2(&trail, state->color[i] / (n + 1), state->color[i] / (n + 1), state->color[i] / (n + 1));
                        setRGB3(&trail, state->color[i] / (n + 1), state->color[i] / (n + 1), state->color[i] / (n + 1));
                        setVector(&vertices[0], state->history[i][j].vx,
                            state->history[i][j].vy - (u16)(state->config->height / 3) / n,
                            state->history[i][j].vz);
                        setVector(&vertices[1], state->history[i][j].vx,
                            state->history[i][j].vy + (u16)(state->config->height / 3) / n,
                            state->history[i][j].vz);
                        setVector(&vertices[2], state->history[i][j - 1].vx,
                            state->history[i][j - 1].vy - (u16)(state->config->height / 3) / (n + 1),
                            state->history[i][j - 1].vz);
                        setVector(&vertices[3], state->history[i][j - 1].vx,
                            state->history[i][j - 1].vy + (u16)(state->config->height / 3) / (n + 1),
                            state->history[i][j - 1].vz);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (PSXLONG *)&trail.x0, (PSXLONG *)&trail.x1,
                            (PSXLONG *)&trail.x2, (PSXLONG *)&trail.x3, &interpolation, &flag);
                        if (depth >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                        }
                    }
                }
                distance = state->positions[i].vz;
                if (distance < 0) {
                    distance = -distance;
                }
                if (distance > 450) {
                    state->hit[i] = 1;
                    if (state->color[i] >= 16) {
                        state->color[i] -= state->config->fade;
                    } else {
                        state->color[i] = 0;
                    }
                } else {
                    state->length[i]++;
                }
            }
        }
        state->launched++;
        if (state->launched > state->config->count) {
            state->launched = state->config->count;
        }
    }
    state->visible++;
    if (state->visible > state->config->count) {
        state->visible = state->config->count;
    }
    state->elapsed += step;
    PopMatrix();
    if (state->stage == 3 && state->color[state->config->count - 1] == 0) {
        return 2;
    }
    if (state->hit[state->config->count - 1] && state->stage == 1) {
        state->stage = 2;
        return 1;
    }
    if (state->hit[0] && state->stage == 0) {
        state->stage = 1;
        return 4;
    }
    return 0;
}
