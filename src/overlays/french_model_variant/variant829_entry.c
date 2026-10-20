#include "../../types.h"
#include "variant829_entry.h"

static const VECTOR D_8013BF88 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model829State *work;
    s32 slot, direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR rotation;
    Model829Position position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 step;
    GsOT *ot;
    s32 i, j, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position.point, 0, 8);
    scale = D_8013BF88;
    work = (Model829State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C008[command];
        func_80057E20(slot ^ 1, &position.adjustment);
        for (i = 0; i < 4; i++) {
            s32 sample;

            work->cloud_counts[i] = 0;
            sample = rand();
            sample -= rand();
            setVector(&work->positions[i],
                work->config->wave_amplitude * ((sample % 4096) * 2 + sample % 4096) / 4096,
                -350, -direction * 354);
            setVector(&work->velocities[i],
                (position.adjustment.x / 2 * ((rand() - rand()) % 4096) / 4096 -
                    work->positions[i].vx) / work->config->travel_divisor,
                0, (direction * 450 - work->positions[i].vz) / work->config->travel_divisor);
            work->red[i] = 1;
            work->green[i] = 1;
            work->blue[i] = 1;
            work->status.fields.states[i] = 0;
            for (j = 0; j < 32; j++) {
                setVector(&work->cloud_offsets[i][j],
                    work->config->cloud_spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->cloud_spread * ((rand() - rand()) % 4096) / 4096,
                    (work->config->cloud_spread / 4) * ((rand() - rand()) % 4096) / 4096);
                work->cloud_gray[i][j] = 128;
            }
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BF98[i]);
        }
        work->brightness = 2048;
        work->status.fields.phase = 0;
        work->elapsed = 0;
        work->updates = 0;
        work->phase_frames = 0;
        work->status.fields.active = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (!work->phase_frames) {
        func_8005F5C8(slot, 0, &rotation, -16);
    } else {
        func_8005F5C8(slot ^ 1, 2, &rotation, -32);
    }
    if (work->status.fields.phase == 0) {
        if (work->brightness > -512) {
            work->brightness -= 64 * step;
        } else {
            work->brightness = -512;
        }
    } else if (work->status.fields.phase == 1) {
        if (work->brightness < 2048) {
            work->brightness += 64 * step;
        } else {
            work->status.fields.phase = 2;
            work->brightness = 2048;
        }
    }
    func_800595C8(2, work->brightness, work->brightness, work->brightness);
    if (work->elapsed < work->config->startup_delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < work->status.fields.active; i++) {
        if ((work->red[i] || work->green[i] || work->blue[i]) && !work->status.fields.states[i]) {
            setPolyFT4(&quad);
            setRGB0(&quad, work->red[i], work->green[i], work->blue[i]);
            if (i == 0) {
                quad.tpage = work->texture[1] >> 16;
                quad.clut = work->texture[1];
                setVector(&vertices[0], -work->config->square_half_size, -work->config->square_half_size, 0);
                setVector(&vertices[1], work->config->square_half_size, -work->config->square_half_size, 0);
                setVector(&vertices[2], -work->config->square_half_size, work->config->square_half_size, 0);
                setVector(&vertices[3], work->config->square_half_size, work->config->square_half_size, 0);
                setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
            } else if (i == 1) {
                j = rand() % 9;
                quad.tpage = work->texture[0] >> 16;
                quad.clut = work->texture[0];
                setVector(&vertices[0], -work->config->animated_half_width, -work->config->animated_half_height, 0);
                setVector(&vertices[1], work->config->animated_half_width, -work->config->animated_half_height, 0);
                setVector(&vertices[2], -work->config->animated_half_width, work->config->animated_half_height, 0);
                setVector(&vertices[3], work->config->animated_half_width, work->config->animated_half_height, 0);
                setUV4(&quad, j * 32, 128, j * 32 + 31, 128,
                    j * 32, 191, j * 32 + 31, 191);
            } else if (i == 2) {
                quad.tpage = work->texture[3] >> 16;
                quad.clut = work->texture[3];
                setVector(&vertices[0], -work->config->rectangle_half_width, -work->config->rectangle_half_height, 0);
                setVector(&vertices[1], work->config->rectangle_half_width, -work->config->rectangle_half_height, 0);
                setVector(&vertices[2], -work->config->rectangle_half_width, work->config->rectangle_half_height, 0);
                setVector(&vertices[3], work->config->rectangle_half_width, work->config->rectangle_half_height, 0);
                setUV4(&quad, 64, 0, 143, 0, 64, 63, 143, 63);
            } else if (i == 3) {
                j = rand() % 9;
                quad.tpage = work->texture[0] >> 16;
                quad.clut = work->texture[0];
                setVector(&vertices[0], -work->config->animated_half_width, -work->config->animated_half_height, 0);
                setVector(&vertices[1], work->config->animated_half_width, -work->config->animated_half_height, 0);
                setVector(&vertices[2], -work->config->animated_half_width, work->config->animated_half_height, 0);
                setVector(&vertices[3], work->config->animated_half_width, work->config->animated_half_height, 0);
                setUV4(&quad, j * 32, 128, j * 32 + 31, 128,
                    j * 32, 191, j * 32 + 31, 191);
            }
            GsSetLsMatrix(&base);
            RotTrans(&work->positions[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0) {
                func_8005B260((u32 *)&quad, ot, (u16)depth, 1);
            }
            if (work->status.fields.active == 4) {
                if (ABS(work->positions[i].vz) < 450) {
                    addVector(&work->positions[i], &work->velocities[i]);
                    work->positions[i].vy = -350 + work->config->wave_amplitude *
                        csin(((i * 16 + 64) * work->phase_frames) & 0xFF0) / 4096;
                } else {
                    work->status.fields.states[i] = 1;
                }
            }
            if (work->red[i] <= 120) {
                work->red[i] += work->config->fade_step;
            } else {
                work->red[i] = 128;
            }
            if (work->green[i] <= 120) {
                work->green[i] += work->config->fade_step;
            } else {
                work->green[i] = 128;
            }
            if (work->blue[i] <= 120) {
                work->blue[i] += work->config->fade_step;
            } else {
                work->blue[i] = 128;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (work->status.fields.states[i]) {
            work->cloud_counts[i] += step;
            if (work->cloud_counts[i] > 32) {
                work->cloud_counts[i] = 32;
            }
            setPolyFT4(&quad);
            quad.tpage = work->texture[2] >> 16;
            quad.clut = work->texture[2];
            setVector(&vertices[0], -work->config->cloud_half_width, -work->config->cloud_half_height, 0);
            setVector(&vertices[1], work->config->cloud_half_width, -work->config->cloud_half_height, 0);
            setVector(&vertices[2], -work->config->cloud_half_width, work->config->cloud_half_height, 0);
            setVector(&vertices[3], work->config->cloud_half_width, work->config->cloud_half_height, 0);
            for (j = 0; j < work->cloud_counts[i]; j++) {
                if (work->cloud_gray[i][j]) {
                    setRGB0(&quad, work->cloud_gray[i][j], work->cloud_gray[i][j], work->cloud_gray[i][j]);
                    copyVector(&position.point, &work->positions[i]);
                    addVector(&position.point, &work->cloud_offsets[i][j]);
                    GsSetLsMatrix(&base);
                    RotTrans(&position.point, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                        (PSXLONG *)&quad.x3, &interpolation, &flag);
                    value = rand() % 9;
                    setUV4(&quad, value * 32, 64, value * 32 + 31, 64,
                        value * 32, 127, value * 32 + 31, 127);
                    if (depth >= 0) {
                        func_8005B260((u32 *)&quad, ot, (u16)depth, 1);
                    }
                    work->cloud_offsets[i][j].vy -= 8;
                    if (work->cloud_gray[i][j] >= 16) {
                        work->cloud_gray[i][j] -= work->config->fade_step;
                    } else {
                        work->cloud_gray[i][j] = 0;
                        if (j == 31) {
                            work->status.fields.states[i] = 2;
                        }
                    }
                }
            }
        }
    }
    if (!(work->updates & 31)) {
        work->status.fields.active++;
        if (work->status.fields.active > 4) {
            work->status.fields.active = 4;
        }
    }
    if (work->status.fields.active == 4) {
        work->phase_frames++;
    }
    work->elapsed += step;
    work->updates++;
    PopMatrix();
    if (work->status.first_pair == 0x20002) {
        return 2;
    } else if (work->status.first_pair == 0x10000) {
        work->status.fields.phase = 1;
        return 1;
    }
    return 0;
}
