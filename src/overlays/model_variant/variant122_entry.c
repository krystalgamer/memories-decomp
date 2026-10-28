#include "../../types.h"
#include "variant122_entry.h"

/* The French images flash all 256 PAL lines. */
#ifdef VERSION_FRENCH
#define FLASH_HEIGHT 256
#else
#define FLASH_HEIGHT 240
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model122State *work;
    Model122Command *cmd;
    MATRIX view, matrix;
    POLY_FT4 quad;
    POLY_F4 tile;
    GsGLINE line;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    SVECTOR point;
    PSXLONG flag, interpolation;
    s32 side;
    GsOT *ot;
    s32 i, j;
    SVECTOR *p, *q;
    s16 *life;
    s32 t, size;
    s32 depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C840;
    work = (Model122State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    /* A nested scope gives step the last spill slot, as in the original. */
    {
        u8 step;
        step = Model_GetFrameStep();
        if (command >= 0) {
            cmd = work->command = &D_8013C888[command];
            p = work->particles;
            for (i = 0; i < 64; p++, i++) {
                j = rand() % 4096;
                size = rand() % 1536 + 0x100;
                p->vx = cmd->radius * ccos(j) / 4096 * ccos(size) / 4096;
                p->vy = cmd->radius * csin(j) / 4096 * ccos(size) / 4096;
                p->vz = cmd->radius * csin(size) / 4096;
            }
            for (i = 0; i < 64; i++) {
                work->life[i] = cmd->life * i / 64;
            }
            p = work->spray;
            for (i = 0; i < 32; i++, p++) {
                p->vx = rand() % (cmd->spray_spread * 2) - cmd->spray_spread;
                p->vy = rand() % (cmd->spray_spread * 2) - cmd->spray_spread;
                p->vz = side * rand() % cmd->spray_spread;
            }
            p = work->dust;
            for (i = 0; i < 16; i++, p++) {
                p->vx = rand() % (cmd->dust_spread * 2) - cmd->dust_spread;
                p->vy = rand() % (cmd->dust_spread * 2) - cmd->dust_spread;
                p->vz = rand() % (cmd->dust_spread * 2) - cmd->dust_spread;
            }
            work->done = 0;
            for (i = 0; i < 2; i++) {
                work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C850[i]);
            }
            work->elapsed = 0;
            return 0;
        }
        cmd = work->command;
        ot = func_80058F10();
        SetPolyFT4(&quad);
        SetPolyF4(&tile);
        line.attribute = 0x50000000;
        box.attribute = 0x50000000;
        box.w = 1;
        box.h = 1;
        view = *(MATRIX *)Model_GetLightSourceMatrix();
        GsSetLsMatrix(&view);
        if (work->elapsed < cmd->start) {
            work->elapsed += Model_GetFrameStep();
            return 0;
        }
        PushMatrix();
        t = work->elapsed - cmd->start;
        j = cmd->release - cmd->start;
        line.r0 = 0;
        line.g0 = 0;
        line.b0 = 0;
        if (t >= 0 && t < j) {
            GsSetLsMatrix(&view);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), cmd->slot), &matrix);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            position.vy += cmd->height;
            rotation.vx = 0;
            rotation.vy = (Model_GetActiveSlotIndex() ^ 1) << 11;
            rotation.vz = 0;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            p = work->particles;
            life = work->life;
            for (i = 0; i < 64; p++, life++, i++) {
                if (*life >= 0) {
                    red = cmd->glow_red * (cmd->life - *life) / cmd->life;
                    green = cmd->glow_green * (cmd->life - *life) / cmd->life;
                    blue = cmd->glow_blue * (cmd->life - *life) / cmd->life;
                    line.r1 = red;
                    line.g1 = green;
                    line.b1 = blue;
                    corners[0].vx = p->vx * *life / cmd->life;
                    corners[0].vy = p->vy * *life / cmd->life;
                    corners[0].vz = p->vz * *life / cmd->life;
                    corners[1].vx = corners[0].vx / 2;
                    corners[1].vy = corners[0].vy / 2;
                    corners[1].vz = corners[0].vz / 2;
                    depth = RotTransPers(&corners[0], (PSXLONG *)&line.x0, &interpolation, &flag);
                    depth += RotTransPers(&corners[1], (PSXLONG *)&line.x1, &interpolation, &flag);
                    depth /= 2;
                    if (depth >= 0 && flag >= 0) {
                        GsSortGLine(&line, ot, depth);
                    }
                    *life -= step;
                    if (*life < 0 && t + cmd->life < j) {
                        *life += cmd->life;
                    }
                }
            }
            setVector(&corners[0], -cmd->fan_size, -cmd->fan_size, 0);
            setVector(&corners[1], cmd->fan_size, -cmd->fan_size, 0);
            setVector(&corners[2], -cmd->fan_size, cmd->fan_size, 0);
            setVector(&corners[3], cmd->fan_size, cmd->fan_size, 0);
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            SetSemiTrans(&quad, 1);
            size = (t << 12) / j;
            quad.u0 = 0;
            quad.v0 = 0;
            quad.u1 = 0x3F;
            quad.v1 = 0;
            quad.u2 = 0;
            quad.v2 = 0x3F;
            quad.u3 = 0x3F;
            quad.v3 = 0x3F;
            quad.r0 = cmd->fan_red;
            quad.g0 = cmd->fan_green;
            quad.b0 = cmd->fan_blue;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
        t = work->elapsed - cmd->release;
        if (t >= 0 && t < cmd->flash_time) {
            tile.x0 = 0;
            tile.y0 = 0;
            tile.x1 = 320;
            tile.y1 = 0;
            tile.x2 = 0;
            tile.y2 = FLASH_HEIGHT;
            tile.x3 = 320;
            tile.y3 = FLASH_HEIGHT;
            red = cmd->flash_red * (cmd->flash_time - t) / cmd->flash_time;
            green = cmd->flash_green * (cmd->flash_time - t) / cmd->flash_time;
            blue = cmd->flash_blue * (cmd->flash_time - t) / cmd->flash_time;
            tile.r0 = red;
            tile.g0 = green;
            tile.b0 = blue;
            Graphics_SubmitDrawModePacket((u32 *)&tile, ot, 1, 1);
        }
        t = work->elapsed - cmd->release;
        line.r1 = 0;
        line.g1 = 0;
        line.b1 = 0;
        if (t >= 0 && t < cmd->flash_time) {
            GsSetLsMatrix(&view);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), cmd->slot), &matrix);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            position.vy += cmd->height;
            rotation.vx = 0;
            rotation.vy = (Model_GetActiveSlotIndex() ^ 1) << 11;
            rotation.vz = 0;
            j = (t << 12) / cmd->flash_time;
            scale.vx = j;
            scale.vy = j;
            scale.vz = j;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            red = cmd->glow_red * (cmd->flash_time - t) / cmd->flash_time / 2;
            green = cmd->glow_green * (cmd->flash_time - t) / cmd->flash_time / 2;
            blue = cmd->glow_blue * (cmd->flash_time - t) / cmd->flash_time / 2;
            line.r0 = red;
            line.g0 = green;
            line.b0 = blue;
            p = work->particles;
            for (i = 0; i < 64; p++, i++) {
                corners[0].vx = p->vx;
                corners[0].vy = p->vy;
                corners[0].vz = p->vz;
                corners[1].vx = corners[0].vx / 2;
                corners[1].vy = corners[0].vy / 2;
                corners[1].vz = corners[0].vz / 2;
                depth = RotTransPers(&corners[0], (PSXLONG *)&line.x0, &interpolation, &flag);
                depth += RotTransPers(&corners[1], (PSXLONG *)&line.x1, &interpolation, &flag);
                depth /= 2;
                if (depth >= 0 && flag >= 0) {
                    GsSortGLine(&line, ot, depth);
                }
            }
            setVector(&corners[0], -cmd->fan_size, -cmd->fan_size, 0);
            setVector(&corners[1], cmd->fan_size, -cmd->fan_size, 0);
            setVector(&corners[2], -cmd->fan_size, cmd->fan_size, 0);
            setVector(&corners[3], cmd->fan_size, cmd->fan_size, 0);
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            SetSemiTrans(&quad, 1);
            quad.u0 = 0;
            quad.v0 = 0;
            quad.u1 = 0x3F;
            quad.v1 = 0;
            quad.u2 = 0;
            quad.v2 = 0x3F;
            quad.u3 = 0x3F;
            quad.v3 = 0x3F;
            red = cmd->fan_red * (cmd->flash_time - t) / cmd->flash_time;
            green = cmd->fan_green * (cmd->flash_time - t) / cmd->flash_time;
            blue = cmd->fan_blue * (cmd->flash_time - t) / cmd->flash_time;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            size = (t << 13) / cmd->flash_time + 0x1000;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
        setVector(&corners[0], -cmd->spray_size, -cmd->spray_size, 0);
        setVector(&corners[1], cmd->spray_size, -cmd->spray_size, 0);
        setVector(&corners[2], -cmd->spray_size, cmd->spray_size, 0);
        setVector(&corners[3], cmd->spray_size, cmd->spray_size, 0);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        p = work->spray;
        for (i = 0; i < 32; i++, p++) {
            setVector(&position, 0, -350, side * 450);
            addVector(&position, p);
            t = work->elapsed - cmd->spray_start - i * cmd->spray_interval;
            if (t >= 0 && t < cmd->spray_time) {
                red = cmd->spray_red * (cmd->spray_time - t) / cmd->spray_time;
                green = cmd->spray_green * (cmd->spray_time - t) / cmd->spray_time;
                blue = cmd->spray_blue * (cmd->spray_time - t) / cmd->spray_time;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
                GsSetLsMatrix(&view);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                    &interpolation, &flag);
                j = t * 3 / cmd->spray_time;
                quad.v0 = 0;
                quad.v1 = 0;
                quad.v2 = 0x1F;
                quad.v3 = 0x1F;
                quad.u0 = (j << 5) + 0x80;
                quad.u1 = (j << 5) + 0x9F;
                quad.u2 = (j << 5) + 0x80;
                quad.u3 = (j << 5) + 0x9F;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
            }
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            t = work->elapsed - cmd->spray_start - i * cmd->spray_interval;
            q = work->dust;
            if (t >= 0 && t < cmd->dust_time) {
                for (j = 0; j < 16; q++, j++) {
                    box.r = cmd->dust_level * (cmd->dust_time - t) / cmd->dust_time;
                    box.g = cmd->dust_level * (cmd->dust_time - t) / cmd->dust_time;
                    box.b = cmd->dust_level * (cmd->dust_time - t) / cmd->dust_time;
                    point.vx = q->vx;
                    point.vy = q->vy;
                    point.vz = q->vz;
                    point.vx *= t;
                    point.vy *= t;
                    point.vz *= t;
                    depth = RotTransPers(&point, (PSXLONG *)&box.x, &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        GsSortBoxFill(&box, ot, depth);
                    }
                }
            }
        }
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        t = work->elapsed - cmd->spray_start;
        if (t < 0) {
            return 0;
        }
        if (cmd->spray_interval * 32 + cmd->dust_time < t) {
            return 2;
        }
        if (cmd->spray_interval * 32 + cmd->spray_time >= t) {
            return 4;
        }
        if (work->done != 0) {
            return 0;
        }
        work->done = 1;
        return 1;
    }
}
