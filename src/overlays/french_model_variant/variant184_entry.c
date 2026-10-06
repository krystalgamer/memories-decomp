#include "../../types.h"
#include "variant184_entry.h"

static const VECTOR D_8013BEF4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model184State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flat;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG flag, interpolation;
    Model184Config *config;
    GsOT *ot;
    SVECTOR *point, *target;
    s32 step, particle_index, i, j, k, radius, time;
    s32 red, green, blue;
    PSXLONG depth;
    DVECTOR *screen0, *screen1;
    u16 *depth0, *depth1, *flags0;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BEF4;
    work = (Model184State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->targets;
        work->config = &D_8013BF58[command % 100];
        config = work->config;
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->origin);
        work->origin.vy = -350;
        for (i = 0; i < config->count; i++, point++) {
            j = rand() % 4096;
            k = rand() % 2048;
            radius = rand() % config->spread;
            setVector(point, radius * csin(j) / 4096 * csin(k) / 4096,
                radius * ccos(k) / 4096, radius * ccos(j) / 4096 * csin(k) / 4096);
            point->vx += work->origin.vx;
            point->vy += work->origin.vy;
            point->vz += work->origin.vz;
        }
        point = work->rings;
        for (i = 0; i < 17; point++, i++) {
            setVector(point, config->outer_radius * ccos(i * 4096 / 16) / 4096,
                config->outer_radius * csin(i * 4096 / 16) / 4096, 0);
        }
        for (i = 0; i < 17; point++, i++) {
            setVector(point, config->inner_radius * ccos(i * 4096 / 16) / 4096,
                config->inner_radius * csin(i * 4096 / 16) / 4096, 0);
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BF04[i]);
        }
        work->elapsed = 0;
        work->updates = 0;
        work->completed = 0;
        work->command_group = command / 100;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flat);
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->moving_half_size, -config->moving_half_size, 0);
    setVector(&vertices[1], config->moving_half_size, -config->moving_half_size, 0);
    setVector(&vertices[2], -config->moving_half_size, config->moving_half_size, 0);
    setVector(&vertices[3], config->moving_half_size, config->moving_half_size, 0);
    setVector(&rotation, 0, 0, 0);
    point = work->positions;
    target = work->targets;
    for (i = 0; i < config->count; point++, target++, i++) {
        time = work->elapsed - config->delay - i * config->interval;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->pad = 0;
        } else if (time < config->travel_duration) {
            s32 travel_red, travel_green, travel_blue;
            copyVector(&position, point);
            position.vy -= config->lift * csin(time * 2048 / config->travel_duration) / 4096;
            j = 1024 + time * 3072 / config->travel_duration;
            limitRange(j, 0, 4096);
            setVector(&scale, j, j, j);
            travel_red = config->red;
            travel_green = config->green;
            travel_blue = config->blue;
            setRGB0(&quad, travel_red, travel_green, travel_blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, i % 3 * 32, (i % 9) / 3 * 32,
                i % 3 * 32 + 31, (i % 9) / 3 * 32,
                i % 3 * 32, (i % 9) / 3 * 32 + 31,
                i % 3 * 32 + 31, (i % 9) / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (target->vx - point->vx) / (config->travel_duration - time) * step;
            point->vy += (target->vy - point->vy) / (config->travel_duration - time) * step;
            point->vz += (target->vz - point->vz) / (config->travel_duration - time) * step;
        }
    }
    point = work->positions;
    for (particle_index = 0; particle_index < config->count; point++, particle_index++) {
        time = work->elapsed - config->delay - particle_index * config->interval - config->travel_duration;
        if (time >= 0 && time < config->burst_duration) {
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            SetSemiTrans(&quad, 1);
            setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
            setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
            setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
            setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
            setVector(&rotation, 0, 0, 0);
            copyVector(&position, point);
            j = 4096 + time * 4096 / config->burst_duration;
            setVector(&scale, j, j, j);
            red = config->red * (config->burst_duration - time) / config->burst_duration;
            green = config->green * (config->burst_duration - time) / config->burst_duration;
            blue = config->blue * (config->burst_duration - time) / config->burst_duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            quad.tpage = work->texture[1] >> 16;
            quad.clut = work->texture[1];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
            RotTransPersN(work->rings, screen, depths, projection, flags, 34);
            screen0 = screen;
            screen1 = screen + 17;
            depth0 = depths;
            depth1 = depths + 17;
            flags0 = flags;
            for (i = 0; i < 16; screen0++, screen1++, depth0++, depth1++, flags0++, i++) {
                u16 first_flags;
                setXY4(&quad, screen0[0].vx, screen0[0].vy, screen0[1].vx, screen0[1].vy,
                    screen1[0].vx, screen1[0].vy, screen1[1].vx, screen1[1].vy);
                depth = AverageZ4(depth0[0], depth0[1], depth1[0], depth1[1]);
                first_flags = flags0[0];
                flag = (flags0[1] | first_flags) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->updates++;
    time = work->elapsed - config->delay - config->travel_duration;
    if (time < 0) {
        return 0;
    } else {
        time -= config->interval * config->count;
        if (time > config->burst_duration) {
            if (work->completed) {
                return 2;
            } else {
                work->completed++;
                return 1;
            }
        } else {
            return 4;
        }
    }
}
