#include "../../types.h"
#include "variant101_entry.h"

const VECTOR D_8013BAE4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model101State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    GsBOXF box;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    DVECTOR projected[66];
    u16 depths[66];
    u16 interpolation[96];
    u16 flags[96];
    s32 active;
    s32 flag;
    s32 direction;
    GsOT *ot;
    Model101Config *config;
    SVECTOR *point;
    SVECTOR *velocity;
    DVECTOR *screen;
    s32 i;
    s32 angle_a, angle_b;
    s32 depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BAE4;
    work = (Model101State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &((Model101Config *)D_8013BB10)[command];
        work->texture = func_80059A50(Model_GetActiveSlotIndex(), 1, (GsIMAGE *)D_8013BAF4);
        point = work->positions;
        velocity = work->velocities;
        screen = work->projected;
        for (i = 0; i < config->count; point++, velocity++, screen++, i++) {
            setVector(point, 0, 0, 0);
            screen->vx = screen->vy = 0;
            work->depths[i] = 0;
            angle_a = rand() % 4096;
            angle_b = rand() % 1024;
            velocity->vx = config->speed * ccos(angle_a) / 4096 * ccos(angle_b) / 4096;
            velocity->vy = config->speed * csin(angle_a) / 4096 * ccos(angle_b) / 4096;
            velocity->vz = -direction * config->speed * csin(angle_b) / 4096;
        }
        point = work->ring;
        velocity = &work->ring[33];
        for (i = 0; i < 33; point++, velocity++, i++) {
            point->vx = config->radius * ccos(i * 4096 / 32) / 4096;
            point->vy = config->radius * csin(i * 4096 / 32) / 4096;
            point->vz = 0;
            velocity->vx = config->radius * ccos(i * 4096 / 32) / 6144;
            velocity->vy = config->radius * csin(i * 4096 / 32) / 6144;
            velocity->vz = direction * config->radius / 5;
        }
        work->elapsed = -config->delay;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    box.w = box.h = 2;
    box.r = config->particle_r;
    box.g = config->particle_g;
    box.b = config->particle_b;
    SetPolyFT4(&quad);
    quad.tpage = work->texture >> 16;
    quad.clut = work->texture;
    setUV4(&quad, 0, 0, 31, 0, 0, 63, 31, 63);
    setVector(&position, 0, -350, direction * 450);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&active);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    box.attribute = 0x40000000;
    for (point = work->positions, screen = work->projected, i = 0;
         i < config->count; point++, screen++, i++) {
        if (point->vy < 350) {
            box.x = screen->vx;
            box.y = screen->vy;
            if (!(flags[i] & 0x20)) {
                GsSortBoxFill(&box, ot, work->depths[i] >> 2);
            }
        }
    }
    point = work->positions;
    screen = work->projected;
    RotTransPersN(point, screen, work->depths, interpolation, flags, config->count);
    box.attribute = 0;
    active = 0;
    for (i = 0; i < config->count; point++, screen++, i++) {
        if (point->vy < 350) {
            box.x = screen->vx;
            box.y = screen->vy;
            if (!(flags[i] & 0x20)) {
                GsSortBoxFill(&box, ot, work->depths[i] >> 2);
            }
            active = 1;
        }
    }
    gteMIMefunc(work->positions, work->velocities, config->count, 2048);
    point = work->positions;
    {
        SVECTOR *gravity_velocity = work->velocities;
        for (i = 0; i < config->count; point++, gravity_velocity++, i++) {
            gravity_velocity->vy += (350 - point->vy) / config->speed / 2;
        }
    }
    if (work->elapsed < config->duration) {
        s32 red, green, blue;
        setVector(&position, 0, -350, direction * 450);
        i = (work->elapsed << 12) / config->duration;
        scale.vz = scale.vy = scale.vx = i;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, projected, depths, interpolation, flags, 66);
        red = config->ring_r * (config->duration - work->elapsed) / config->duration;
        green = config->ring_g * (config->duration - work->elapsed) / config->duration;
        blue = config->ring_b * (config->duration - work->elapsed) / config->duration;
        setRGB0(&quad, red, green, blue);
        SetSemiTrans(&quad, 1);
        for (i = 0; i < 32; i++) {
            setXY4(&quad, projected[i].vx, projected[i].vy,
                projected[i + 1].vx, projected[i + 1].vy,
                projected[i + 33].vx, projected[i + 33].vy,
                projected[i + 34].vx, projected[i + 34].vy);
            depth = AverageZ4(depths[i], depths[i + 1], depths[i + 33], depths[i + 34]);
            flag = (flags[i] | flags[i + 1] | flags[i + 33] | flags[i + 34]) & 0x20;
            if (depth >= 0 && !flag) {
                GsSortPoly(&quad, ot, depth & 0xFFFF);
            }
        }
        active = 1;
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->completed) {
        return active ? 0 : 2;
    } else {
        work->completed++;
        return 1;
    }
}
