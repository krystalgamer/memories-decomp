#include "../../types.h"
#include "variant103_entry.h"

/* Header 103's entry: a fan of triangles grows out of the followed slot
 * part, holds and then swells and fades while the screen flashes white,
 * four pairs of blades spin open from the centre, and eight rotated arcs of
 * wobbling textured strips spread around it. */

static const VECTOR D_8013C234 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model103State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_G3 fan;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[18];
    u16 depths[18], projection[18], flags[18];
    PSXLONG flag, interpolation;
    Model103Config *config;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j, k, time, value, angle, size;
    u8 red, green, blue, shade;
    PSXLONG depth;
    s32 stretch, next;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C234;
    work = (Model103State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C260[command];
        point = work->fan;
        point->vx = config->radius * 2 / 5;
        point->vy = config->radius * 2 / 5;
        point->vz = 0;
        point++;
        for (i = 0; i < 17; point++, i++) {
            point->vx = config->radius * ccos(i * 256) / 4096;
            point->vy = config->radius * csin(i * 256) / 4096;
            point->vz = 0;
        }
        point = work->ring;
        for (i = 0; i < 8; point++, i++) {
            angle = (i * 2048) / 7;
            point->vx = 0;
            point->vy = -config->radius * ccos(angle) / 4096;
            point->vz = config->radius * csin(angle) / 4096;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C244[i]);
        }
        work->elapsed = -config->delay;
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
    SetPolyG3(&fan);
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    time = work->elapsed - config->grow_duration - config->hold_duration;
    if (time >= 0 && time < config->fade_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        shade = (config->fade_duration - time) * 255 / config->fade_duration;
        setRGB0(&flash, shade, shade, shade);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed;
    if (time >= 0 && time < config->grow_duration + config->hold_duration + config->fade_duration) {
        if (time < config->grow_duration + config->hold_duration) {
            setRGB0(&fan, config->red, config->green, config->blue);
            setRGB1(&fan, 0, 0, 0);
            setRGB2(&fan, 0, 0, 0);
            SetSemiTrans(&fan, 0);
        } else {
            value = config->fade_duration - (time - (config->grow_duration + config->hold_duration));
            shade = value * 255 / config->fade_duration;
            setRGB0(&fan, shade, shade, shade);
            red = config->red * value / config->fade_duration;
            green = config->green * value / config->fade_duration;
            blue = config->blue * value / config->fade_duration;
            setRGB1(&fan, red, green, blue);
            setRGB2(&fan, red, green, blue);
            SetSemiTrans(&fan, 1);
        }
        GsSetLsMatrix(&base);
        if (time < config->grow_duration + config->hold_duration) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            work->position.vx = matrix.t[0];
            work->position.vy = matrix.t[1];
            work->position.vz = matrix.t[2];
        } else {
            position.vx = work->position.vx;
            position.vy = work->position.vy;
            position.vz = work->position.vz;
        }
        setVector(&rotation, 0, 0, 0);
        if (time < config->grow_duration) {
            value = (time << 12) / config->grow_duration;
            setVector(&scale, value, value, value);
        } else if (time >= config->grow_duration + config->hold_duration) {
            value = ((time - config->grow_duration - config->hold_duration) << 13) / config->fade_duration + 4096;
            setVector(&scale, value, value, value);
        } else {
            setVector(&scale, 4096, 4096, 4096);
        }
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->fan, screen, depths, projection, flags, 18);
        for (i = 0; i < 16; i++) {
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 1].vx, screen[i + 1].vy,
                screen[i + 2].vx, screen[i + 2].vy);
            depth = AverageZ3(depths[0], depths[i + 1], depths[i + 2]);
            flag = (flags[0] | flags[i + 1] | flags[i + 2]) & 0x20;
            if (depth >= 0 && flag == 0) {
                if (time < config->grow_duration + config->hold_duration) {
                    GsSortPoly(&fan, ot, (u16)depth);
                } else {
                    Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 2);
                }
            }
        }
    }
    time = work->elapsed - (config->grow_duration + config->hold_duration);
    if (time >= 0 && time < config->fade_duration) {
        red = config->blade_red * (config->fade_duration - time) / config->fade_duration;
        green = config->blade_green * (config->fade_duration - time) / config->fade_duration;
        blue = config->blade_blue * (config->fade_duration - time) / config->fade_duration;
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        setRGB0(&fan, red, green, blue);
        SetSemiTrans(&fan, 1);
        setVector(&vertices[0], 0, 0, 0);
        setVector(&vertices[1], config->blade_length, -config->blade_width / 2, 0);
        setVector(&vertices[2], config->blade_length, config->blade_width / 2, 0);
        position.vx = work->position.vx;
        position.vy = work->position.vy;
        position.vz = work->position.vz;
        value = (time << 12) / config->fade_duration;
        setVector(&scale, value, value, value);
        for (i = 0; i < 4; i++) {
            value = (i << 10) + (time << 10) / config->fade_duration;
            setVector(&rotation, 0, 0, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotTransPers3(&vertices[0], &vertices[1], &vertices[2],
                (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 2);
            }
            setVector(&rotation, 0, 2048, value + 512);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotTransPers3(&vertices[0], &vertices[1], &vertices[2],
                (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 2);
            }
        }
    }
    time = work->elapsed - (config->grow_duration + config->hold_duration);
    if (time >= 0 && time < config->strip_duration + config->strip_fade_duration) {
        if (time < config->strip_duration) {
            setRGB0(&quad, config->strip_red, config->strip_green, config->strip_blue);
        } else {
            red = config->strip_red * (config->strip_fade_duration - time + config->strip_duration) / config->strip_fade_duration;
            green = config->strip_green * (config->strip_fade_duration - time + config->strip_duration) / config->strip_fade_duration;
            blue = config->strip_blue * (config->strip_fade_duration - time + config->strip_duration) / config->strip_fade_duration;
            setRGB0(&quad, red, green, blue);
        }
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 0, 31, 0, 0, 63, 31, 63);
        position.vx = work->position.vx;
        position.vy = work->position.vy;
        position.vz = work->position.vz;
        value = (time << 13) / config->strip_duration;
        stretch = value + csin((time << 12) / 20 % 4096) / 4;
        if (value < 4096) {
            size = 4096;
        } else if (value < 8193) {
            size = value;
        } else {
            size = 8192;
        }
        setVector(&scale, stretch, size, stretch);
        /* Retail adds the second strip corner's phase step from a register
         * rather than folding the constant into the index sum. */
        next = 1;
        for (j = 0; j < 8; j++) {
            setVector(&rotation, 0, j << 9, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->ring;
            value = time / 4;
            value = value < 0 ? 0 : (value >= 8 ? 7 : value);
            for (k = 0; k < value; point++, k++) {
                setVector(&vertices[0], point[0].vx, point[0].vy, point[0].vz);
                vertices[0].vx -= (s16)(config->radius / 7) * csin((k + j + time * 2) * 4608 / 7) / 4096;
                setVector(&vertices[1], vertices[0].vx, vertices[0].vy, vertices[0].vz);
                vertices[1].vx += (s16)(config->radius / 7) * 2;
                setVector(&vertices[2], point[1].vx, point[1].vy, point[1].vz);
                vertices[2].vx -= (s16)(config->radius / 7) * csin((k + j + (time * 2 + next)) * 4608 / 7) / 4096;
                setVector(&vertices[3], vertices[2].vx, vertices[2].vy, vertices[2].vz);
                vertices[3].vx += (s16)(config->radius / 7) * 2;
                depth = RotTransPers4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed;
    if (time < config->grow_duration + config->hold_duration) {
        return 0;
    }
    if (config->grow_duration + config->hold_duration + config->strip_duration + config->strip_fade_duration < time) {
        return 2;
    }
    return 1;
}
