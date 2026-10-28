#include "../../types.h"
#include "image16740_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX light;
    MATRIX matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR corners[4];
    DVECTOR screens[66];
    u16 depths[66];
    u16 interpolation[66];
    u16 flags[66];
    PSXLONG flag;
    PSXLONG p;
    PSXLONG depth;
    Image16740State *state;
    GsOT *ot;
    Image16740Config *config;
    SVECTOR *point;
    s32 i, j;
    s32 side;
    s32 elapsed;
    s32 age;
    s32 factor;
    s32 numerator;
    s32 value;

    memset(&rotation, 0, sizeof(rotation));
    memset(&translation, 0, sizeof(translation));
    scale = D_8013BC44;
    state = (Image16740State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = state->config = &D_8013BC8C[command];
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&state->translation);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&state->origin);
        point = state->ring;
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->radius * ccos(i * 128) / 4096;
            point->vy = -config->thickness;
            point->vz = config->radius * csin(i * 128) / 4096;
        }
        elapsed = config->radius - config->thickness;
        for (i = 0; i < 33; point++, i++) {
            point->vx = elapsed * ccos(i * 128) / 4096;
            point->vy = 0;
            point->vz = elapsed * csin(i * 128) / 4096;
        }
        point = state->beams;
        for (i = 0; i < 16; i++) {
            value = i * 2;
            point->vx = config->spread * value / 16;
            point->vy = 0;
            point->vz = side * config->spread * i / 16;
            point++;
            point->vx = -config->spread * value / 16;
            point->vy = 0;
            point->vz = side * config->spread * i / 16;
            point++;
        }
        for (i = 0; i < 2; i++) {
            state->textures[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &D_8013BC54[i]);
        }
        state->time = 0;
        state->completed = 0;
        return 0;
    }

    config = state->config;
    ot = func_80058F10();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = state->textures[0] >> 16;
    quad.clut = state->textures[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&translation, state->translation.vx,
              state->translation.vy, state->translation.vz);
    translation.vy = config->ring_height;
    elapsed = state->time - config->ring_begin;
    for (j = 0; j < config->ring_span; j += config->ring_spacing) {
        if (elapsed >= j && elapsed < j + config->ring_duration) {
            s32 red, green, blue;
            age = elapsed - j;
            factor = (age << 12) / config->ring_duration;
            setVector(&scale, factor, factor, factor);
            GsSetLsMatrix(&light);
            RotTrans(&translation, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(state->ring, screens, depths, interpolation, flags, 66);
            age = config->ring_duration - age;
            numerator = config->ring_color[0] * age;
            red = numerator / config->ring_duration;
            numerator = config->ring_color[1] * age;
            green = numerator / config->ring_duration;
            numerator = config->ring_color[2] * age;
            blue = numerator / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            for (i = 0; i < 32; i++) {
                factor = i + 33;
                setXY4(&quad, screens[i].vx, screens[i].vy,
                       screens[i + 1].vx, screens[i + 1].vy,
                       screens[factor].vx, screens[factor].vy,
                       screens[factor + 1].vx, screens[factor + 1].vy);
                depth = AverageZ4(depths[i], depths[i + 1],
                                  depths[i + 33], depths[i + 34]);
                flag = (flags[i] | flags[i + 1] | flags[i + 33] | flags[i + 34]) & 0x20;
                setUV4(&quad, i * 8, 0, i * 8 + 7, 0,
                       i * 8, 31, i * 8 + 7, 31);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, depth & 0xFFFF);
                }
            }
        }
    }
    {
        j = state->time - config->flash_begin;
        if (j >= 0 && j < config->flash_duration) {
            setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
            value = config->flash_duration - j;
            factor = value * 255;
            factor /= config->flash_duration;
            setRGB0(&flash, factor, factor, factor);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        }
    }

    setVector(&corners[0], -config->half_width, -config->height, 0);
    setVector(&corners[1], config->half_width, -config->height, 0);
    setVector(&corners[2], -config->half_width, 0, 0);
    setVector(&corners[3], config->half_width, 0, 0);
    setVector(&rotation, 0, 0, 0);
    quad.tpage = state->textures[1] >> 16;
    quad.clut = state->textures[1];
    setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
    point = state->beams;
    for (i = 0; i < 16; i++) {
        age = state->time - config->beam_begin - config->beam_spacing * i;
        if (age >= 0 && age < config->beam_duration) {
            s32 k;
            for (k = 0; k < 2; point++, k++) {
                s32 red, green, blue;
                setVector(&translation, point->vx, point->vy, point->vz);
                translation.vx += state->origin.vx;
                translation.vy += state->origin.vy;
                translation.vz += state->origin.vz;
                translation.vy = 0;
                elapsed = (age << 12) / config->beam_duration;
                scale.vx = 4096;
                scale.vz = 4096;
                scale.vy = elapsed;
                value = config->beam_duration - age;
                numerator = config->beam_color[0] * value;
                red = numerator / config->beam_duration;
                numerator = config->beam_color[1] * value;
                green = numerator / config->beam_duration;
                numerator = config->beam_color[2] * value;
                blue = numerator / config->beam_duration;
                setRGB0(&quad, red, green, blue);
                GsSetLsMatrix(&light);
                RotTrans(&translation, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth & 0xFFFF);
                }
            }
        } else {
            point += 2;
        }
    }
    PopMatrix();
    state->time += Model_GetFrameStep();
    age = state->time - config->beam_begin;
    if (age < 0) {
        return 0;
    }
    age -= config->beam_spacing * 16;
    if (age >= config->beam_duration) {
        if (state->completed) {
            return 2;
        } else {
            state->completed = 1;
            return 1;
        }
    }
    return 4;
}
