#include "../../types.h"
#include "variant180_entry.h"
#include "../../game/screen_projection.h"

/* The entry draws expanding, fading rings at this slot, a screen flash, and
 * then pairs of rays that grow upwards around the other slot. A negative
 * command draws a frame; any other command sets the work area up. */

static const VECTOR D_8013BC4C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model180State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    PSXLONG flag, interpolation;
    Model180Config *config;
    GsOT *ot;
    SVECTOR *point;
    s32 direction, i, j, angle, factor, time, offset, scale_value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BC4C;
    work = (Model180State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BC94[command];
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&work->origin);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->opponent);
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            angle = i << 7;
            point->vx = config->ring_radius * ccos(angle) / 4096;
            point->vy = -config->ring_width;
            point->vz = config->ring_radius * csin(angle) / 4096;
        }
        factor = config->ring_radius - config->ring_width;
        for (i = 0; i < 33; point++, i++) {
            angle = i << 7;
            point->vx = factor * ccos(angle) / 4096;
            point->vy = 0;
            point->vz = factor * csin(angle) / 4096;
        }
        point = work->rays;
        for (i = 0; i < 16; i++) {
            point->vx = config->spread * (i << 1) / 16;
            point->vy = 0;
            point->vz = direction * config->spread * i / 16;
            point++;
            point->vx = -config->spread * (i << 1) / 16;
            point->vy = 0;
            point->vz = direction * config->spread * i / 16;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013BC5C[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
    position.vy = -350;
    factor = work->elapsed - config->ring_start;
    for (offset = 0; offset < config->ring_span; offset += config->ring_spacing) {
        if (factor >= offset && factor < offset + config->ring_duration) {
            s32 red, green, blue, remaining, t;
            t = factor - offset;
            scale_value = (t << 12) / config->ring_duration;
            setVector(&scale, scale_value, scale_value, scale_value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 66);
            remaining = config->ring_duration - t;
            red = config->ring_r * remaining / config->ring_duration;
            green = config->ring_g * remaining / config->ring_duration;
            blue = config->ring_b * remaining / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            for (i = 0; i < 32; i++) {
                scale_value = i + 33;
                setXY4(&quad, screen[i].vx, screen[i].vy,
                       screen[i + 1].vx, screen[i + 1].vy,
                       screen[scale_value].vx, screen[scale_value].vy,
                       screen[scale_value + 1].vx, screen[scale_value + 1].vy);
                depth = AverageZ4(depths[i], depths[i + 1], depths[i + 33], depths[i + 34]);
                flag = (flags[i] | flags[i + 1] | flags[i + 33] | flags[i + 34]) & 0x20;
                setUV4(&quad, i * 8, 0, i * 8 + 7, 0, i * 8, 31, i * 8 + 7, 31);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    offset = work->elapsed - config->flash_start;
    if (offset >= 0 && offset < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, 240, 320, 240);
        scale_value = (config->flash_duration - offset) * 255;
        scale_value /= config->flash_duration;
        setRGB0(&flash, scale_value, scale_value, scale_value);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    setVector(&vertices[0], -config->ray_half_width, -config->ray_height, 0);
    setVector(&vertices[1], config->ray_half_width, -config->ray_height, 0);
    setVector(&vertices[2], -config->ray_half_width, config->ray_height, 0);
    setVector(&vertices[3], config->ray_half_width, config->ray_height, 0);
    setVector(&rotation, 0, 0, 0);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
    point = work->rays;
    for (i = 0; i < 16; i++) {
        time = work->elapsed - config->ray_start - config->ray_spacing * i;
        if (time >= 0 && time < config->ray_duration) {
            for (j = 0; j < 2; point++, j++) {
                s32 red, green, blue, remaining;
                setVector(&position, point->vx, point->vy, point->vz);
                position.vx += work->opponent.vx;
                position.vy += work->opponent.vy;
                position.vz += work->opponent.vz;
                position.vy = -350;
                factor = (time << 12) / config->ray_duration;
                setVector(&scale, 4096, factor, 4096);
                remaining = config->ray_duration - time;
                red = config->ray_r * remaining / config->ray_duration;
                green = config->ray_g * remaining / config->ray_duration;
                blue = config->ray_b * remaining / config->ray_duration;
                setRGB0(&quad, red, green, blue);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        } else {
            point += 2;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->ray_start;
    if (time >= 0) {
        time -= config->ray_spacing * 16;
        if (time < config->ray_duration) {
            return 4;
        }
        if (work->completed != 0) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 0;
}
