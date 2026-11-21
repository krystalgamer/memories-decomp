#include "../../types.h"
#include "image26952_entry.h"

s32 FrenchModelVariant818_HandleRequest(ModelVariantFlightState *context, s32 command)
{
    MATRIX light, matrix;
    POLY_FT4 quad, burst;
    POLY_G4 trail;
    SVECTOR rotation;
    Image26952Position position;
    VECTOR scale;
    SVECTOR vertices[4];
    long flag, interpolation;
    ModelVariantFlightState *state;
    s32 active, sign, step;
    GsOT *ot;
    s32 i, j, n, theta, depth, distance;

    memset(&rotation, 0, sizeof(rotation));
    memset(&position, 0, sizeof(position));
    scale = gFrenchModelVariant818_UnitScale;
    state = context;
    active = Model_GetActiveSlotIndex();
    sign = active * 2 - 1;
    if (command >= 0) {
        state->config = &gFrenchModelVariant818_aConfigs[command];
        Model_GetSlotDimensionsWithMinimums(active ^ 1, &position.dimensions);
        Model_CopySlotPosition(active ^ 1, &rotation);
        for (i = 0; i < state->config->count; i++) {
            j = 4096 / state->config->count;
            theta = j * i;
            setVector(&state->pos[i],
                state->config->radius * ccos(theta % 4096) / 4096,
                state->config->radius * csin(theta % 4096) / 4096 - 350,
                -sign * 449);
            setVector(&state->target[i],
                rotation.vx + position.dimensions.x / 2 * ((rand() - rand()) % 4096) / 4096,
                rotation.vy + position.dimensions.y / 2 * ((rand() - rand()) % 4096) / 4096,
                sign * 450);
            setVector(&state->step[i],
                (state->target[i].vx - state->pos[i].vx) / state->config->steps,
                (state->target[i].vy - state->pos[i].vy) / state->config->steps,
                (state->target[i].vz - state->pos[i].vz) / state->config->steps);
            setVector(&state->angle[i], 0, 0, theta);
            state->arrived[i] = 0;
            state->level[i] = 128;
            state->tail[i] = 0;
            state->trail_head[i] = 1;
            for (j = 0; j < state->config->spark_count; j++) {
                setVector(&state->sparks[i][j],
                    state->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                    state->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                    state->config->spark_spread * ((rand() - rand()) % 4096) / 4096);
                state->spark_frame[i][j] = 0;
            }
            for (j = 0; j < 32; j++) {
                copyVector(&state->trail[i][j], &state->pos[i]);
            }
        }
        state->texture[0] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant818_aImages[0]);
        state->texture[1] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant818_aImages[1]);
        state->ambient = 2048;
        state->phase = 0;
        state->elapsed = 0;
        state->shown = 0;
        state->moving = 0;
        return 0;
    }

    ot = Graphics_GetDefaultOrderingTable();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (state->elapsed < state->config->launch) {
        Model_QueueCameraOrbitFromPreset(active, 0, &rotation, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(active ^ 1, 1, &rotation, -16);
    }
    if (state->phase == 0) {
        if (state->ambient >= -511) {
            state->ambient -= step * 64;
        } else {
            state->ambient = -512;
        }
    } else if (state->phase == 2) {
        if (state->ambient < 2048) {
            state->ambient += step * 64;
        } else {
            state->phase = 3;
            state->ambient = 2048;
        }
    }
    Model_SetSlotAmbient(2, state->ambient, state->ambient, state->ambient);
    if (state->elapsed < state->config->start) {
        state->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *Model_GetLightSourceMatrix();
    for (i = 0; i < state->shown; i++) {
        MATRIX *render_matrix = &matrix;
        long *render_flag = &flag;
        POLY_FT4 *head_packet = &quad;

        if (state->level[i]) {
            setPolyFT4(&quad);
            quad.tpage = state->texture[0] >> 16;
            quad.clut = state->texture[0];
            setRGB0(&quad, state->level[i], state->level[i], state->level[i]);
            setVector(&vertices[0], 0, -state->config->height, 0);
            setVector(&vertices[1], state->config->width, -state->config->height, 0);
            setVector(&vertices[2], 0, state->config->height, 0);
            setVector(&vertices[3], state->config->width, state->config->height, 0);
            GsSetLsMatrix(&light);
            RotTrans(&state->pos[i], (VECTOR *)matrix.t, render_flag);
            RotMatrix(&state->angle[i], render_matrix);
            MulMatrix2(&light, render_matrix);
            ScaleMatrix(render_matrix, &scale);
            GsSetLsMatrix(render_matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (long *)&quad.x0, (long *)&quad.x1,
                (long *)&quad.x2, (long *)&quad.x3, &interpolation, render_flag);
            setUV4(&quad, 0, 32, 63, 32, 0, 63, 63, 63);
            if (state->arrived[i] == 0) {
                setSemiTrans(&quad, 0);
            } else {
                setSemiTrans(&quad, 1);
            }
            if (depth >= 0) {
                GsSortPoly(head_packet, ot, (u16)depth);
            }
        }
        if (state->arrived[i]) {
            setPolyFT4(&burst);
            burst.tpage = state->texture[1] >> 16;
            burst.clut = state->texture[1];
            setRGB0(&burst, 128, 0, 0);
            setVector(&vertices[0], -state->config->spark_size, -state->config->spark_size, 0);
            setVector(&vertices[1], state->config->spark_size, -state->config->spark_size, 0);
            setVector(&vertices[2], -state->config->spark_size, state->config->spark_size, 0);
            setVector(&vertices[3], state->config->spark_size, state->config->spark_size, 0);
            for (j = 0; j < state->config->spark_count; j++) {
                if (state->spark_frame[i][j] < 8) {
                    copyVector(&position.vector, &state->target[i]);
                    addVector(&position.vector, &state->sparks[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&position.vector, (VECTOR *)matrix.t, render_flag);
                    RotMatrix(&rotation, render_matrix);
                    ScaleMatrix(render_matrix, &scale);
                    GsSetLsMatrix(render_matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (long *)&burst.x0, (long *)&burst.x1,
                        (long *)&burst.x2, (long *)&burst.x3, &interpolation, render_flag);
                    setUV4(&burst,
                        state->spark_frame[i][j] * 32, 0,
                        state->spark_frame[i][j] * 32 + 31, 0,
                        state->spark_frame[i][j] * 32, 31,
                        state->spark_frame[i][j] * 32 + 31, 31);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                    }
                    state->spark_frame[i][j]++;
                }
            }
        }
    }
    if (state->elapsed >= state->config->launch) {
        for (i = 0; i < state->moving; i++) {
            if (state->level[i]) {
                setVector(&state->angle[i],
                    ratan2(state->step[i].vz, state->step[i].vy) - 1024,
                    sign * (ratan2(state->step[i].vx, state->step[i].vz) - 1024),
                    0);
                addVector(&state->pos[i], &state->step[i]);
                copyVector(&state->trail[i][state->trail_head[i]], &state->pos[i]);
                state->trail_head[i] = (state->trail_head[i] + 1) % 32;
                setPolyG4(&trail);
                if (state->trail_head[i] > state->tail[i] && state->level[i]) {
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
                    for (j = state->trail_head[i] - 1, n = 1;
                         last = state->trail_head[i] - 1, j >= last - state->tail[i]; j--, n++) {
                        setRGB0(&trail, state->level[i] / n, state->level[i] / n, state->level[i] / n);
                        setRGB1(&trail, state->level[i] / n, state->level[i] / n, state->level[i] / n);
                        setRGB2(&trail, state->level[i] / (n + 1), state->level[i] / (n + 1), state->level[i] / (n + 1));
                        setRGB3(&trail, state->level[i] / (n + 1), state->level[i] / (n + 1), state->level[i] / (n + 1));
                        setVector(&vertices[0], state->trail[i][j].vx,
                            state->trail[i][j].vy - (u16)(state->config->height / 3) / n,
                            state->trail[i][j].vz);
                        setVector(&vertices[1], state->trail[i][j].vx,
                            state->trail[i][j].vy + (u16)(state->config->height / 3) / n,
                            state->trail[i][j].vz);
                        setVector(&vertices[2], state->trail[i][j - 1].vx,
                            state->trail[i][j - 1].vy - (u16)(state->config->height / 3) / (n + 1),
                            state->trail[i][j - 1].vz);
                        setVector(&vertices[3], state->trail[i][j - 1].vx,
                            state->trail[i][j - 1].vy + (u16)(state->config->height / 3) / (n + 1),
                            state->trail[i][j - 1].vz);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (long *)&trail.x0, (long *)&trail.x1,
                            (long *)&trail.x2, (long *)&trail.x3, &interpolation, &flag);
                        if (depth >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                        }
                    }
                }
                distance = state->pos[i].vz;
                if (distance < 0) {
                    distance = -distance;
                }
                if (distance > 450) {
                    state->arrived[i] = 1;
                    if (state->level[i] >= 16) {
                        state->level[i] -= state->config->fade;
                    } else {
                        state->level[i] = 0;
                    }
                } else {
                    state->tail[i]++;
                }
            }
        }
        state->moving++;
        if (state->moving > state->config->count) {
            state->moving = state->config->count;
        }
    }
    state->shown++;
    if (state->shown > state->config->count) {
        state->shown = state->config->count;
    }
    state->elapsed += step;
    PopMatrix();
    if (state->phase == 3 && state->level[state->config->count - 1] == 0) {
        return 2;
    }
    if (state->arrived[state->config->count - 1] && state->phase == 1) {
        state->phase = 2;
        return 1;
    }
    if (state->arrived[0] && state->phase == 0) {
        state->phase = 1;
        return 4;
    }
    return 0;
}
