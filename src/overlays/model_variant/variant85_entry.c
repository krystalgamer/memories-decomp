#include "../../types.h"
#include "variant85_entry.h"

/* Header 85's entry: up to 32 spinning triangles open around a model part
 * while the slots play backwards, a flickering sprite follows the part, and
 * a burst sprite grows in front of the slot with a textured funnel behind
 * it. The return value reports the hit (1) once, the end of the burst (2) or
 * neither (0). */

static const VECTOR D_8013BF44 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_G3 tri;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    /* Only three vertices are declared: the fourth sprite corner is written
     * into the start of screen, which retail's frame layout requires. */
    SVECTOR vertices[3];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    PSXLONG flag, interpolation;
    Model85State *work;
    Model85Config *config;
    GsOT *ot;
    s32 side;
    s32 i, time, size;
    PSXLONG depth;
    u32 clip;
    u8 red, green, blue;
    SVECTOR *angle, *speed;
    DVECTOR *projected, *far;
    u16 *near_depth, *far_depth, *flag_p;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BF44;
    work = (Model85State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BFA8[command];
        angle = work->angles;
        speed = work->speeds;
        for (i = 0; i < config->count; angle++, speed++, i++) {
            angle->vx = rand() % 4096;
            angle->vy = rand() % 4096;
            angle->vz = rand() % 4096;
            speed->vx = rand() % 32;
            speed->vy = rand() % 32;
            speed->vz = rand() % 32;
            speed->vz = 32 - speed->vx;
        }
        angle = work->rings;
        for (i = 0; i < 33; angle++, i++) {
            angle->vx = config->outer_radius * ccos(i * 128) / 4096;
            angle->vy = config->outer_radius * csin(i * 128) / 4096;
            angle->vz = -side * config->inner_radius;
        }
        for (i = 0; i < 33; angle++, i++) {
            angle->vx = config->inner_radius * ccos(i * 128) / 4096;
            angle->vy = config->inner_radius * csin(i * 128) / 4096;
            angle->vz = 0;
        }
        work->hit = 0;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013BF54[i]);
        }
        work->elapsed = -config->delay;
        work->frame = 0;
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
    SetPolyG3(&tri);
    setRGB0(&tri, 0, 0, 0);
    setRGB1(&tri, 0, 0, 0);
    SetPolyFT4(&quad);
    setRGB0(&quad, config->sprite_red, config->sprite_green, config->sprite_blue);
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    position.vx = matrix.t[0];
    position.vy = matrix.t[1];
    position.vz = matrix.t[2];
    if (work->elapsed < config->spin_end) {
        Model_SetSlotPlaybackDirection(0, 0);
        Model_SetSlotPlaybackDirection(1, 0);
        Model_SetSlotAmbient(2, -1024, -1024, -1024);
        setVector(&vertices[2], 0, 0, 0);
        angle = work->angles;
        speed = work->speeds;
        for (i = 0; i < config->count; angle++, speed++, i++) {
            GsSetLsMatrix(&base);
            rotation.vx = speed->vx;
            rotation.vy = speed->vy;
            rotation.vz = speed->vz;
            rotation.vx = rotation.vx * work->elapsed;
            rotation.vy = rotation.vy * work->elapsed;
            rotation.vz = rotation.vz * work->elapsed;
            rotation.vx += angle->vx;
            rotation.vy += angle->vy;
            rotation.vz += angle->vz;
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&vertices[0], config->width, config->height, 0);
            setVector(&vertices[1], -config->width, config->height, 0);
            depth = RotTransPers3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&tri.x0,
                (PSXLONG *)&tri.x1, (PSXLONG *)&tri.x2, &interpolation, &flag);
            setRGB2(&tri, config->red, config->green, config->blue);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&tri, ot, (u16)depth, 1);
            }
            setVector(&vertices[0], config->width / 2, config->height / 2, 0);
            setVector(&vertices[1], -config->width / 2, config->height / 2, 0);
            depth = RotTransPers3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&tri.x0,
                (PSXLONG *)&tri.x1, (PSXLONG *)&tri.x2, &interpolation, &flag);
            setRGB2(&tri, 255, 255, 255);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&tri, ot, (u16)depth, 1);
            }
        }
    } else {
        Model_SetSlotPlaybackDirection(0, 1);
        Model_SetSlotPlaybackDirection(1, 1);
    }
    if (work->elapsed < config->burst_start) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->size / 4, -config->size / 4, 0);
        setVector(&vertices[1], config->size / 4, -config->size / 4, 0);
        setVector(&vertices[2], -config->size / 4, config->size / 4, 0);
        setVector(&vertices[3], config->size / 4, config->size / 4, 0);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        size = ((work->frame & 1) << 9) + 4096;
        setRGB0(&quad, 128, 128, 128);
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag) - 30;
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
    }
    time = work->elapsed - config->burst_start;
    if (time >= 0 && time < config->duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->size, -config->size, 0);
        setVector(&vertices[1], config->size, -config->size, 0);
        setVector(&vertices[2], -config->size, config->size, 0);
        setVector(&vertices[3], config->size, config->size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&position, 0, -350, side * 450);
        size = (time << 12) / config->duration + 2048;
        setVector(&scale, size, size, size);
        red = config->sprite_red * (config->duration - time) / config->duration;
        green = config->sprite_green * (config->duration - time) / config->duration;
        blue = config->sprite_blue * (config->duration - time) / config->duration;
        setRGB0(&quad, red, green, blue);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->rings, screen, depths, projection, flags, 66);
        projected = screen;
        far = &screen[33];
        near_depth = depths;
        far_depth = &depths[33];
        flag_p = flags;
        for (i = 0; i < 32; projected++, far++, near_depth++, far_depth++, flag_p++, i++) {
            quad.x0 = projected[0].vx;
            quad.y0 = projected[0].vy;
            quad.x1 = projected[1].vx;
            quad.y1 = projected[1].vy;
            quad.x2 = far[0].vx;
            quad.y2 = far[0].vy;
            quad.x3 = far[1].vx;
            quad.y3 = far[1].vy;
            depth = AverageZ4(near_depth[0], near_depth[1], far_depth[0], far_depth[1]);
            clip = flag_p[0];
            flag = (flag_p[1] | clip) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    work->frame++;
    if (work->elapsed < config->spin_end) {
        return 0;
    }
    if (work->elapsed < config->burst_start) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 0;
    }
    if (config->burst_start + config->duration < work->elapsed) {
        return 2;
    }
    if (work->hit) {
        return 0;
    }
    work->hit++;
    return 1;
}
