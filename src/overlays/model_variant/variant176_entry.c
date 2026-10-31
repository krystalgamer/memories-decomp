#include "../../types.h"
#include "variant176_entry.h"

/* Header 176's entry: a burst of dots that fly out and fall back, three
 * strip rings that grow out of the same point, then a set of sprites that
 * fly outwards, each phase fading over its own duration. The setup builds
 * the rings, the random dot and sprite velocities and the two textures, and
 * prints the work-area size. */

static const VECTOR D_8013C3A4 = {4096, 4096, 4096, 0};
/* The work-size format string; retail's sixteenth byte after the terminator is 0x0C. */
#ifndef VERSION_FRENCH
#ifndef MODEL176_STRING_PAD
#define MODEL176_STRING_PAD "\f"
#endif
static const char D_8013C3B4[16] = "%d (%x) Byte.\n\0" MODEL176_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model176State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 strip;
    GsBOXF dot;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR offset;
    DVECTOR screen[128];
    u16 depths[128], projection[128], flags[128];
    PSXLONG flag, interpolation;
    Model176Config *config;
    GsOT *ot;
    SVECTOR *point, *velocity;
    DVECTOR *projected;
    u16 *depth_cursor, *flag_cursor, *inner_depth, *inner_flag;
    s32 i, j, k, time, radius, size, longest;
    DVECTOR *inner;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C3A4;
    work = (Model176State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C3FC[command];
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->outer_radius * ccos(i * 128) / 4096;
            point->vy = config->outer_radius * csin(i * 128) / 4096;
            point->vz = -direction * config->depth;
        }
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->inner_radius * ccos(i * 128) / 4096;
            point->vy = config->inner_radius * csin(i * 128) / 4096;
            point->vz = 0;
        }
        point = work->dots;
        for (i = 0; i < config->dot_count; point++, i++) {
            setVector(point, 0, 0, 0);
        }
        point = work->dot_velocities;
        for (i = 0; i < config->dot_count; point++, i++) {
            size = rand() % 4096;
            j = rand() % 1024;
            point->vx = config->dot_radius * ccos(size) / 4096 * ccos(j) / 4096;
            point->vy = config->dot_radius * csin(size) / 4096 * ccos(j) / 4096;
            point->vz = -direction * config->dot_radius * csin(j) / 4096;
        }
        point = work->sprite_velocities;
        for (i = 0; i < config->sprite_count; point++, i++) {
            size = rand() % 4096;
            j = rand() % 2048;
            radius = (rand() % config->sprite_radius) / 2 + config->sprite_radius / 2;
            point->vx = radius * ccos(size) / 4096 * csin(j) / 4096;
            point->vy = radius * ccos(j) / 4096;
            point->vz = radius * csin(size) / 4096 * csin(j) / 4096;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C3C4[i]);
        }
        work->completed = 0;
        work->elapsed = 0;
        work->unknown_B1D = 0;
#ifndef VERSION_FRENCH
        printf(D_8013C3B4, sizeof(Model176State), sizeof(Model176State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    GsSetLsMatrix(&base);
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&strip);
    SetSemiTrans(&quad, 1);
    SetSemiTrans(&flash, 1);
    SetSemiTrans(&strip, 1);
    dot.attribute = 0x50000000;
    dot.w = 1;
    dot.h = 1;
    Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&offset);
    offset.vy = -350;
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->dot_duration) {
        if (config->part == 0xFF) {
            setVector(&position, 0, -350, direction * 450);
        } else {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->dots, screen, depths, projection, flags, config->dot_count);
        dot.r = config->dot_red * (config->dot_duration - time) / config->dot_duration;
        dot.g = config->dot_green * (config->dot_duration - time) / config->dot_duration;
        dot.b = config->dot_blue * (config->dot_duration - time) / config->dot_duration;
        projected = screen;
        depth_cursor = depths;
        flag_cursor = flags;
        point = work->dots;
        velocity = work->dot_velocities;
        for (i = 0; i < config->dot_count; projected++, depth_cursor++, flag_cursor++, point++, velocity++, i++) {
            dot.x = projected->vx;
            dot.y = projected->vy;
            depth = *depth_cursor >> 2;
            flag = *flag_cursor & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortBoxFill(&dot, ot, depth);
            }
            point->vx += velocity->vx / 2;
            point->vy += velocity->vy / 2;
            point->vz += velocity->vz / 2;
            velocity->vy += (350 - point->vy) / config->dot_radius / 2;
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->strip_duration) {
        strip.tpage = work->texture[1] >> 16;
        strip.clut = work->texture[1];
        setUV4(&strip, 0, 0, 31, 0, 0, 127, 31, 127);
        if (config->part == 0xFF) {
            setVector(&position, 0, -350, direction * 450);
        } else {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        setVector(&rotation, 0, 0, 0);
        for (k = 0; k < 3; k++) {
            s32 red, green, blue;

            size = (config->strip_duration - time) * 128 / config->strip_duration;
            red = config->strip_red * (config->strip_duration - time) / config->strip_duration;
            green = config->strip_green * (config->strip_duration - time) / config->strip_duration;
            blue = config->strip_blue * (config->strip_duration - time) / config->strip_duration;
            setRGB2(&strip, size, size, size);
            setRGB3(&strip, size, size, size);
            strip.r0 = red;
            strip.r1 = red;
            strip.g0 = green;
            strip.g1 = green;
            strip.b0 = blue;
            strip.b1 = blue;
            size = time * 3072 / config->strip_duration + 1024;
            setVector(&scale, size / (k + 1), size / (k + 1), size / (3 - k));
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 66);
            projected = screen;
            inner = &screen[33];
            depth_cursor = depths;
            inner_depth = &depths[33];
            flag_cursor = flags;
            inner_flag = &flags[33];
            for (i = 0; i < 32; projected++, inner++, depth_cursor++, inner_depth++, flag_cursor++, inner_flag++, i++) {
                setXY4(&strip, projected[0].vx, projected[0].vy, projected[1].vx, projected[1].vy,
                    inner[0].vx, inner[0].vy, inner[1].vx, inner[1].vy);
                depth = AverageZ4(depth_cursor[0], depth_cursor[1], inner_depth[0], inner_depth[1]);
                flag = (flag_cursor[0] | flag_cursor[1] | inner_flag[0] | inner_flag[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&strip, ot, (u16)depth);
                }
            }
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->sprite_duration) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 0, 128, 31, 128, 0, 159, 31, 159);
        setVector(&vertices[0], -config->sprite_size, -config->sprite_size, 0);
        setVector(&vertices[1], config->sprite_size, -config->sprite_size, 0);
        setVector(&vertices[2], -config->sprite_size, config->sprite_size, 0);
        setVector(&vertices[3], config->sprite_size, config->sprite_size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        {
        s32 red, green, blue;

        red = config->sprite_red * (config->sprite_duration - time) / config->sprite_duration;
        green = config->sprite_green * (config->sprite_duration - time) / config->sprite_duration;
        blue = config->sprite_blue * (config->sprite_duration - time) / config->sprite_duration;
        setRGB0(&quad, red, green, blue);
        }
        point = work->sprite_velocities;
        for (i = 0; i < config->sprite_count; point++, i++) {
            if (config->part == 0xFF) {
                setVector(&position, 0, -350, direction * 450);
            } else {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
                setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            }
            position.vx += point->vx * time / config->sprite_duration;
            position.vy += point->vy * time / config->sprite_duration;
            position.vz += point->vz * time / config->sprite_duration;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->delay;
    if (time < 0) {
        return 0;
    }
    size = config->dot_duration < config->strip_duration ? config->strip_duration : config->dot_duration;
    longest = size;
    if (longest < config->sprite_duration) {
        longest = config->sprite_duration;
    }
    if (time < longest) {
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    } else {
        return 2;
    }
    return 0;
}
