#include "../../types.h"
#include "variant132_entry.h"
#include "../../game/graphics_constants.h"

/* Header 132's entry: an anchor swirls in towards the active slot, travels
 * to the opposite slot and hops about there; ten textured orb rings rise
 * from it, eight sparks burst from the target and a full-screen flash fades
 * out at the end. The return value reports the hit (1) once, the end (2),
 * 4 while sparks are still spawning, or nothing (0). */

static const VECTOR D_8013C378 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR anchor;
    SVECTOR vertices[4];
    DVECTOR projected[34];
    u16 depths[34], interpolation[34], flags[34];
    PSXLONG flag, parameter;
    Model132State *work;
    GsOT *ot;
    s32 step;
    s32 i;
    s32 time;
    Model132Config *config;
    s32 j, angle, progress, span, frame;
    PSXLONG depth;
    SVECTOR *point;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C378;
    work = (Model132State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C3C0[command % 100];
        point = work->ring;
        for (i = 0; i < 17; point++, i++) {
            point->vx = config->radius * ccos(i * 256) / 4096;
            point->vy = 0;
            point->vz = config->radius * csin(i * 256) / 4096;
        }
        point = work->orbs;
        for (i = 0; i < 10; point++, i++) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
            point->vy = -config->height * (10 - i) / 10;
            point->pad = 0;
        }
        point = work->sparks;
        for (i = 0; i < 8; point++, i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point->pad = 0;
        }
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C388[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &D_8013C388[1]);
        work->elapsed = 0;
        work->started = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    GsSetLsMatrix(&base);
    time = work->elapsed - config->delay;
    if (time < config->approach) {
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&anchor);
        anchor.vx += config->swirl * (config->approach - time) / config->approach
            * ccos((time << 12) / config->approach) / 4096;
        anchor.vy = 0;
        anchor.vz += config->swirl * (config->approach - time) / config->approach
            * csin((time << 12) / config->approach) / 4096;
    } else if (time < config->travel + config->approach) {
        progress = time - config->approach;
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&vertices[0]);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[1]);
        angle = csin((progress << 13) / config->travel);
        anchor.vx = vertices[0].vx + (vertices[1].vx - vertices[0].vx) * progress / config->travel
            + (config->spread - config->swirl) * angle / 4096;
        anchor.vy = 0;
        anchor.vz = vertices[0].vz + (vertices[1].vz - vertices[0].vz) * progress / config->travel;
    } else {
        progress = time - config->approach - config->travel;
        span = config->end - config->delay - config->approach - config->travel;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&anchor);
        anchor.vx += config->spread * csin((progress << 13) / span) / 4096;
        anchor.vy = 0;
        anchor.vz += config->spread * csin((progress << 14) / span) / 8192;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 31, 0, 0, 127, 31, 127);
    setVector(&rotation, 0, 0, 0);
    time = work->elapsed - config->delay;
    if (time >= 0) {
        point = work->orbs;
        for (i = 0; i < 10; point++, i++) {
            if ((s16)(config->life / 10) * i < time % config->life && point->pad <= 0
                && work->elapsed < config->end) {
                point->vx = anchor.vx;
                point->vy = anchor.vy;
                point->vz = anchor.vz;
                point->pad += config->life;
                point->vy = -config->height * (config->life - point->pad) / config->life;
            }
            if (point->pad > 0) {
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                setVector(&scale, 0x1400, 0x1400, 0x1400);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(work->ring, projected, depths, interpolation, flags, 17);
                position.vy += config->rise;
                position.vy = position.vy < -config->height ? -config->height
                    : position.vy > 0 ? 0 : position.vy;
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(work->ring, &projected[17], &depths[17], &interpolation[17], &flags[17], 17);
                red = config->red * point->pad / config->life;
                green = config->green * point->pad / config->life;
                blue = config->blue * point->pad / config->life;
                setRGB0(&quad, red, green, blue);
                for (j = 0; j < 16; j++) {
                    setXY4(&quad, projected[j + 17].vx, projected[j + 17].vy,
                        projected[j + 18].vx, projected[j + 18].vy,
                        projected[j].vx, projected[j].vy,
                        projected[j + 1].vx, projected[j + 1].vy);
                    depth = AverageZ4(depths[j], depths[j + 1], depths[j + 17], depths[j + 18]);
                    flag = (flags[j] | flags[j + 1] | flags[j + 17] | flags[j + 18]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        GsSortPoly(&quad, ot, depth);
                    }
                }
                point->vy -= config->height / config->life * step;
                point->pad -= step;
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setUV4(&quad, 0, 0, 31, 0, 0, 31, 31, 31);
    setRGB0(&quad, config->spark_red, config->spark_green, config->spark_blue);
    setVector(&rotation, 0, 0, 0);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&scale, 4096, 4096, 4096);
    time = work->elapsed - config->delay - (config->approach + config->travel);
    if (time >= 0) {
        point = work->sparks;
        for (i = 0; i < 8; point++, i++) {
            if (time % 16 > i * 2 && point->pad <= 0
                && work->elapsed < config->end) {
                frame = point->pad;
                Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
                point->pad = frame + 16;
                point->vx += rand() % config->spread * 2 - config->spread;
                point->vy = rand() % config->spread * 2 - (s16)(config->spread + 350);
                point->vz += rand() % config->spread * 2 - config->spread;
            }
            if (point->pad > 0) {
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
                frame = (16 - point->pad) / 2;
                setUV4(&quad, frame % 4 * 32, frame / 4 * 32 + 128,
                    frame % 4 * 32 + 31, frame / 4 * 32 + 128,
                    frame % 4 * 32, frame / 4 * 32 + 159,
                    frame % 4 * 32 + 31, frame / 4 * 32 + 159);
                GsSortPoly(&quad, ot, depth);
                point->vy -= config->half_size / 4 * step;
                point->pad -= step;
            }
        }
    }
    time = work->elapsed - config->end;
    if (time >= 0 && time < config->life) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = config->flash_red * (config->life - time) / config->life;
        green = config->flash_green * (config->life - time) / config->life;
        blue = config->flash_blue * (config->life - time) / config->life;
        setRGB0(&flash, red, green, blue);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    if (work->elapsed < config->end + config->life) {
        if (work->elapsed < config->delay + config->approach + config->travel) {
            return 0;
        }
        if (work->elapsed < config->end) {
            return 4;
        }
        if (work->started) {
            return 0;
        }
        work->started++;
        return 1;
    }
    return 2;
}
