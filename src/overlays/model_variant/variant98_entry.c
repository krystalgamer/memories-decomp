#include "../../types.h"
#include "variant98_entry.h"

/* Header 98's entry: a blinking six-blade fan grows on a joint, a textured
 * 9x9 grid spins under the opponent and rising sparks and spinning rings
 * stream from it. Any non-negative command sets the work area up; a negative
 * command draws a frame and returns 0 before the sparks start, 4 while they
 * are emitted, 1 on the first frame after that (0 later) and 2 at the end. */

static const VECTOR gModelVariant98_D15D4 = {4096, 4096, 4096, 0};

s32 ModelVariant98_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT3 fan;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corner[4];
    DVECTOR screen[81];
    u16 depths[81], projection[81], flags[81];
    PSXLONG flag, interpolation;
    Model98State *work;
    GsOT *ot;
    Model98Config *config;
    s32 side;
    /* The scratch variables are shared between stages as in the original;
     * splitting them changes the register allocation. */
    s32 i, j, k, phase, time, size, otz;
    SVECTOR *point;
    u8 red, green, blue;
    s32 fade;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant98_D15D4;
    work = (Model98State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &gModelVariant98_D168C[command];
        point = &work->center;
        work->center.vx = 0;
        point->vy = 0;
        point->vz = 0;
        point = work->blades;
        for (i = 0; i < 6; i++) {
            phase = (i << 12) / 6;
            point->vx = config->radius * ccos(phase) / 4096;
            point->vy = config->radius * csin(phase) / 4096;
            point->vz = side * config->depth;
            point++;
            point->vx = config->radius * ccos(phase + config->spread) / 4096;
            point->vy = config->radius * csin(phase + config->spread) / 4096;
            point->vz = side * config->depth;
            point++;
        }
        point = work->grid;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                point->vx = config->grid_size * j / 8 - config->grid_size / 2;
                point->vy = 0;
                point->vz = config->grid_size * i / 8 - config->grid_size / 2;
                point++;
            }
        }
        point = work->sparks;
        for (i = 0; i < 144; i++) {
            otz = rand() % (config->grid_size / 2);
            k = rand() % 4096;
            point->vx = otz * ccos(k) / 4096;
            point->vy = 0;
            point->vz = otz * csin(k) / 4096;
            point++;
        }
        /* The original fills 144 points from the ring offset onward, past
         * the end of the state, through the workspace's scatter view. */
        point = ((Model98Workspace *)work)->scatter.points;
        for (i = 0; i < 144; i++) {
            otz = rand() % (config->grid_size / 2 - config->ring_size);
            k = rand() % 4096;
            point->vx = otz * ccos(k) / 4096;
            point->vy = 0;
            point->vz = otz * csin(k) / 4096;
            point++;
        }
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->target);
        work->target.vy = -10;
        for (i = 0; i < 6; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant98_D15E4[i]);
        }
        work->elapsed = -config->delay;
        work->toggle = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    PushMatrix();
    SetPolyFT3(&fan);
    fan.tpage = work->texture[5] >> 16;
    fan.clut = work->texture[5];
    SetPolyFT4(&quad);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetPolyF4(&flash);
    phase = work->elapsed;
    if (phase >= 0 && phase < config->grow + config->hold) {
        size = 4096;
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        rotation.vx = 0;
        rotation.vy = 0;
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        rotation.vz = phase * config->spread / 3;
        if (phase < config->grow) {
            size = (phase << 12) / config->grow;
        } else if (phase >= config->hold) {
            size = ((config->grow + config->hold - phase) << 12) / config->grow;
        }
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        if (work->toggle) {
            fan.r0 = config->fan_color[0];
            fan.g0 = config->fan_color[1];
            fan.b0 = config->fan_color[2];
        } else {
            fan.r0 = config->fan_color[0] >> 1;
            fan.g0 = config->fan_color[1] >> 1;
            fan.b0 = config->fan_color[2] >> 1;
        }
        point = work->blades;
        for (i = 0; i < 6; i++) {
            otz = RotAverage3(&work->center, point, point + 1, (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1,
                              (PSXLONG *)&fan.x2, &interpolation, &flag);
            setUV3(&fan, 64, 134, 192, 128, 192, 143);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)otz, 1);
            }
            point += 2;
        }
    }
    time = work->elapsed - config->grid_start;
    if (time >= 0) {
        position.vx = work->target.vx;
        position.vy = work->target.vy;
        position.vz = work->target.vz;
        rotation.vx = 0;
        rotation.vy = time * 96 % 4096;
        rotation.vz = 0;
        size = 4096;
        if (time < config->grid_grow) {
            size = (time << 12) / config->grid_grow;
        }
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        if (time < config->grid_grow) {
            red = config->grid_color[0] * time / config->grid_grow;
            green = config->grid_color[1] * time / config->grid_grow;
            blue = config->grid_color[2] * time / config->grid_grow;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            SetSemiTrans(&quad, 1);
        } else {
            size = config->grid_grow + config->spark_spacing * 144;
            if (time >= size) {
                fade = config->spark_life + size - time;
                red = config->grid_color[0] * fade / config->spark_life;
                green = config->grid_color[1] * fade / config->spark_life;
                blue = config->grid_color[2] * fade / config->spark_life;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
                SetSemiTrans(&quad, 1);
            } else {
                quad.r0 = config->grid_color[0];
                quad.g0 = config->grid_color[1];
                quad.b0 = config->grid_color[2];
                SetSemiTrans(&quad, 0);
            }
        }
        RotTransPersN(work->grid, screen, depths, projection, flags, 81);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                size = i * 9 + j;
                k = (i + 1) * 9 + j;
                quad.x0 = screen[size].vx;
                quad.y0 = screen[size].vy;
                quad.x1 = screen[size + 1].vx;
                quad.y1 = screen[size + 1].vy;
                quad.x2 = screen[k].vx;
                quad.y2 = screen[k].vy;
                quad.x3 = screen[k + 1].vx;
                quad.y3 = screen[k + 1].vy;
                otz = AverageZ4(depths[size], depths[size + 1], depths[k],
                                depths[k + 1]);
                flag = flags[k + 1] & 0x20;
                quad.clut = work->texture[0];
                setUV4(&quad, j * 8, i * 8 + 64, j * 8 + 7, i * 8 + 64, j * 8, i * 8 + 71, j * 8 + 7,
                       i * 8 + 71);
                if (otz >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, otz);
                }
            }
        }
    }
    time = work->elapsed - (config->grid_start + config->grid_grow);
    if (time >= 0 && time < config->spark_spacing * 144 + config->spark_life) {
        corner[0].vx = -config->spark_width / 2;
        corner[0].vy = 0;
        corner[0].vz = 0;
        corner[1].vx = config->spark_width / 2;
        corner[1].vy = 0;
        corner[1].vz = 0;
        quad.r0 = config->spark_color[0];
        quad.g0 = config->spark_color[1];
        quad.b0 = config->spark_color[2];
        quad.clut = work->texture[5];
        setUV4(&quad, 64, 128, 191, 128, 64, 136, 191, 136);
        point = work->sparks;
        for (i = 0; i < 144; i++) {
            if (i * config->spark_spacing < time && time < i * config->spark_spacing + config->spark_life) {
                position.vx = work->target.vx;
                position.vy = work->target.vy;
                position.vz = work->target.vz;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                setVector(&rotation, 0, 0, 0);
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotTransPers(&corner[0], (PSXLONG *)&quad.x0, &interpolation, &flag);
                otz += RotTransPers(&corner[1], (PSXLONG *)&quad.x2, &interpolation, &flag);
                position.vy += config->rise;
                if (position.vy > 0) {
                    position.vy = 0;
                }
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz += RotTransPers(&corner[0], (PSXLONG *)&quad.x1, &interpolation, &flag);
                otz += RotTransPers(&corner[1], (PSXLONG *)&quad.x3, &interpolation, &flag);
                otz /= 4;
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)otz, 1);
                }
                point->vy -= 1200 / config->spark_life * Model_GetFrameStep();
            }
            point++;
        }
    }
    phase = work->elapsed - (config->grid_start + config->grid_grow);
    if (phase >= 0 && phase < config->ring_spacing * 16 + config->ring_life) {
        corner[0].vx = -config->ring_size / 2;
        corner[0].vy = 0;
        corner[0].vz = 0;
        corner[1].vx = config->ring_size / 2;
        corner[1].vy = 0;
        corner[1].vz = 0;
        corner[2].vx = -config->ring_size / 2;
        corner[2].vy = config->ring_size;
        corner[2].vz = 0;
        corner[3].vx = config->ring_size / 2;
        corner[3].vy = config->ring_size;
        corner[3].vz = 0;
        SetSemiTrans(&quad, 1);
        point = work->rings;
        for (i = 0; i < 16; i++) {
            if (i * config->ring_spacing < phase && phase < i * config->ring_spacing + config->ring_life) {
                position.vx = work->target.vx;
                position.vy = work->target.vy;
                position.vz = work->target.vz;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                setVector(&rotation, 0, phase << 7, 0);
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3], (PSXLONG *)&quad.x0,
                                  (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                  &interpolation, &flag);
                if (point->vy < -config->ring_size) {
                    quad.r0 = config->ring_color[0];
                    quad.g0 = config->ring_color[1];
                    quad.b0 = config->ring_color[2];
                } else {
                    red = -config->ring_color[0] * point->vy / config->ring_size;
                    green = -config->ring_color[1] * point->vy / config->ring_size;
                    blue = -config->ring_color[2] * point->vy / config->ring_size;
                    quad.r0 = red;
                    quad.g0 = green;
                    quad.b0 = blue;
                }
                if (phase % 32 >= 8 && phase % 32 < 24) {
                    quad.clut = work->texture[0];
                    setUV4(&quad, 0, 64, 63, 64, 0, 127, 63, 127);
                } else {
                    quad.clut = work->texture[4];
                    setUV4(&quad, 0, 128, 63, 128, 0, 191, 63, 191);
                }
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, otz);
                }
                point->vy -= 1200 / config->ring_life * Model_GetFrameStep();
            }
            point++;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    work->toggle ^= 1;
    k = work->elapsed - (config->grid_start + config->grid_grow);
    if (k < 0) {
        return 0;
    }
    if (k > config->spark_spacing * 144 + config->spark_life) {
        return 2;
    }
    if (k <= config->spark_spacing * 144) {
        return 4;
    }
    if (!work->completed) {
        work->completed++;
        return 1;
    }
    return 0;
}
