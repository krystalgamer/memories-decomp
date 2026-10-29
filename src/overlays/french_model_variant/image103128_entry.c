#include "../../types.h"
#include "image103128_entry.h"

/* The North American header-840/937 images load the burst palette from CLUT
 * row 0x3D20 instead of the PAL row 0x3D28. */
#ifndef IMAGE103128_BURST_CLUT
#define IMAGE103128_BURST_CLUT 0x3D28
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Image103128State *state;
    MATRIX light;
    MATRIX matrix;
    POLY_FT4 quad;
    POLY_G4 gradient;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR corners[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot, side, step;
    GsOT *ot;
    s32 i, j, k;
    s32 depth;
    u16 sign;
    s32 jitter;
    s32 height;
    POLY_FT4 *packet;
    POLY_G4 *gradient_packet;

    memset(&rotation, 0, 8);
    memset(&translation, 0, 8);
    scale = D_8013BEEC;
    state = (Image103128State *)context;
    packet = &quad;
    gradient_packet = &gradient;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        state->config = &D_8013BF34[command];
        state->tpage = 174;
        state->clut = IMAGE103128_BURST_CLUT;
        for (i = 0; i < 2; i++) {
            state->textures[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &D_8013BEFC[i]);
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 8; j++) {
                setVector(&state->positions[i][j], (rand() - rand()) % 256 / 8,
                          (rand() - rand()) % 256 / 8, 0);
                setVector(&state->velocities[i][j], 0, 0, 0);
                setVector(&state->impacts[i][j], 0, 0, 0);
                state->alpha[i][j] = 128;
                state->colors[i][j].r = 96;
                state->colors[i][j].g = 48;
                state->colors[i][j].b = 48;
                state->control.fields.states[i][j] = 0;
                state->sizes[i][j] = 0;
            }
        }
        state->ambient = 2048;
        state->control.fields.phase = 0;
        state->elapsed = 0;
        state->tick = 0;
        state->control.fields.count = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (state->elapsed < state->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 1, &rotation, -4);
    } else if (side > 0) {
        Model_QueueCameraOrbitFromPreset(2, 11, &rotation, 16);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 9, &rotation, 16);
    }
    if (state->control.fields.phase == 0) {
        if (state->ambient > -512) {
            state->ambient -= step << 6;
        } else {
            state->ambient = -512;
        }
    } else if (state->control.fields.phase == 1) {
        if (state->ambient < 2048) {
            state->ambient += step << 6;
        } else {
            state->control.fields.phase = 2;
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
    setPolyFT4(packet);
    setPolyG4(gradient_packet);
    for (i = 0; i < 4; i++) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), state->config->parts[i]), &matrix);
        setVector(&translation, matrix.t[0], matrix.t[1], matrix.t[2]);
        for (j = 0; j < state->control.fields.count; j++) {
            if (state->control.fields.states[i][j] == 0) {
                addVector(&state->positions[i][j], &translation);
                setVector(&state->velocities[i][j],
                    ((rand() - rand()) % 256 / 4 - state->positions[i][j].vx) / 8,
                    (jitter = (rand() - rand()) % 256 / 4,
                     height = state->positions[i][j].vy + 350,
                     (jitter - height) / 8),
                    (side * 450 - state->positions[i][j].vz) / 8);
                state->control.fields.states[i][j] = 1;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < state->control.fields.count; j++) {
            if (state->alpha[i][j] && state->control.fields.states[i][j]) {
                packet->tpage = state->textures[0] >> 16;
                packet->clut = state->textures[0];
                setUV4(packet, 0, 0, 47, 0, 0, 127, 47, 127);
                setVector(&translation, 0, 0, 0);
                setVector(&rotation, 0, 0, 0);
                setVector(&scale, 4096, 4096, 4096);
                setRGB0(packet, state->alpha[i][j], state->alpha[i][j], state->alpha[i][j]);
                setVector(&corners[0], 0, -12, side << 7);
                setVector(&corners[1], 0, -12, -side << 7);
                setVector(&corners[2], 0, 12, side << 7);
                setVector(&corners[3], 0, 12, -side << 7);
                GsSetLsMatrix(&light);
                RotTrans(&state->positions[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&light, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corners[2], &corners[0], &corners[3], &corners[1],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                    (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
                if (state->alpha[i][j] == 128) {
                    setSemiTrans(packet, 0);
                } else {
                    setSemiTrans(packet, 1);
                }
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, depth & 0xFFFF);
                }
                addVector(&state->positions[i][j], &state->velocities[i][j]);
                if (state->control.fields.states[i][j] == 1) {
                    if (side > 0) {
                        if (state->positions[i][j].vz >= 450) {
                            copyVector(&state->impacts[i][j], &state->positions[i][j]);
                            state->control.fields.states[i][j] = 2;
                        }
                    } else {
                        if (state->positions[i][j].vz <= -450) {
                            copyVector(&state->impacts[i][j], &state->positions[i][j]);
                            state->control.fields.states[i][j] = 2;
                        }
                    }
                }
                if (state->control.fields.states[i][j] >= 2) {
                    if (state->alpha[i][j] >= 16) {
                        state->alpha[i][j] -= 15;
                    } else {
                        state->alpha[i][j] = 0;
                    }
                }
            }
            if (state->control.fields.states[i][j] >= 2 &&
                (state->colors[i][j].r || state->colors[i][j].g || state->colors[i][j].b)) {
                scale.vx = state->sizes[i][j] + 4096;
                scale.vy = state->sizes[i][j] + 4096;
                scale.vz = 4096;
                packet->tpage = state->tpage;
                packet->clut = state->clut;
                GsSetLsMatrix(&light);
                RotTrans(&state->impacts[i][j], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                saved = matrix;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                sign = 1;
                for (k = 0; k < 4; k++) {
                    s32 next_side;
                    s32 first_x;
                    setRGB0(packet, state->colors[i][j].r, state->colors[i][j].g, state->colors[i][j].b);
                    next_side = -(s16)sign;
                    first_x = -(s16)next_side * 32;
                    sign = next_side;
                    setVector(&corners[0], first_x, k - 2 < 0 ? 32 : -32, 0);
                    setVector(&corners[1], 0, k - 2 >= 0 ? -32 : 32, 0);
                    setVector(&corners[2], -((s16)sign * 32), 0, 0);
                    setVector(&corners[3], 0, 0, 0);
                    setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
                    depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                        (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)packet, ot, depth & 0xFFFF, 1);
                    }
                    setUV4(packet, 64, 64, 127, 64, 64, 127, 127, 127);
                    setRGB0(packet, state->colors[i][j].r >> 2,
                            state->colors[i][j].g >> 2, state->colors[i][j].b >> 2);
                    depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                        (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &p, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)packet, ot, depth & 0xFFFF, 1);
                    }
                }
                if (state->sizes[i][j] < 24576) {
                    state->sizes[i][j] += 4096;
                } else {
                    state->sizes[i][j] += 256;
                }
                if (state->colors[i][j].r >= 9) {
                    state->colors[i][j].r -= 8;
                } else {
                    state->colors[i][j].r = 0;
                }
                if (state->colors[i][j].g >= 9) {
                    state->colors[i][j].g -= 8;
                } else {
                    state->colors[i][j].g = 0;
                }
                if (state->colors[i][j].b >= 9) {
                    state->colors[i][j].b -= 8;
                } else {
                    state->colors[i][j].b = 0;
                }
            }
        }
    }
    if (state->elapsed < state->config->end && (state->tick & 1) == 0) {
        state->control.fields.count++;
        if (state->control.fields.count > 8) {
            state->control.fields.count = 8;
        }
    }
    state->elapsed += step;
    state->tick++;
    PopMatrix();
    if (!state->colors[0][state->control.fields.count - 1].r &&
        !state->colors[0][state->control.fields.count - 1].g &&
        !state->colors[0][state->control.fields.count - 1].b &&
        state->control.fields.phase == 2) {
        return 2;
    }
    if (state->control.fields.states[0][state->control.fields.count - 1] >= 2 &&
        state->elapsed >= state->config->end &&
        state->control.fields.phase == 0) {
        state->control.fields.states[0][state->control.fields.count - 1] = 3;
        state->control.fields.phase = 1;
        return 1;
    }
    return state->control.word == 0x20000 ? 4 : 0;
}
