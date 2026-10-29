#include "../../types.h"
#include "variant773_entry.h"

static const VECTOR D_8013D508 = {4096, 4096, 4096, 0};

/* Header-773 MODEL entry: twelve streaking orbs with trails, ribbons and impact flashes. */
s32 func_8013B004(u8 *context, s32 command)
{
    Model773State *work;
    MATRIX view, matrix;
    POLY_FT4 orb;
    POLY_FT4 flash;
    POLY_FT4 burst;
    POLY_G4 ribbon;
    GsBOXF box;
    SVECTOR rotation, offset;
    VECTOR scale;
    SVECTOR corners[4];
    DVECTOR screen[144];
    u16 depths[144], interpolations[144], flags[144];
    PSXLONG flag, interpolation;
    s32 side, step;
    GsOT *ot;
    s32 i, j, k;
    s32 a, radius, depth;
    SVECTOR *pos, *vel, *off, *dir, *point;

    memset(&rotation, 0, 8);
    memset(&offset, 0, 8);
    scale = D_8013D508;
    work = (Model773State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->command = &D_8013D56C[command];
        pos = work->pos;
        vel = work->vel;
        /* vy and vz go through the word view so they keep their own
         * strength-reduced pointers, as in the original. */
        for (i = 0; i < 12; i++) {
            j = rand() % 4096 - rand() % 4096;
            k = rand() % 4096 - rand() % 4096;
            pos->vx = work->command->spread_x * j / 4096;
            ((Model773Point *)pos)->c[1] = work->command->spread_x * k / 4096;
            ((Model773Point *)pos)->c[2] = 0;
            vel->vx = work->command->spread_y * j / 4096;
            ((Model773Point *)vel)->c[1] = work->command->spread_y * k / 4096 - 350;
            ((Model773Point *)vel)->c[2] = side * 450;
            pos++;
            vel++;
        }
        pos = work->offsets.flat;
        vel = work->directions.flat;
        for (i = 0; i < 144; i++) {
            a = rand() % 4096;
            depth = rand() % 4096;
            radius = work->command->trail_radius / work->command->trail_period;
            pos->vx = 0;
            ((Model773Point *)pos)->c[1] = 0;
            ((Model773Point *)pos)->c[2] = 0;
            vel->vx = radius * ccos(a) / 4096 * ccos(depth) / 4096;
            ((Model773Point *)vel)->c[1] = radius * ccos(a) / 4096 * csin(depth) / 4096;
            ((Model773Point *)vel)->c[2] = radius * csin(a) / 4096;
            pos++;
            vel++;
        }
        for (i = 0; i < 12; i++) {
            for (j = 0; j < 32; j++) {
                setVector(&work->ribbon[i][j], 0, 0, 0);
            }
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D518[i]);
        }
        work->state = 0;
        work->done = 0;
        for (i = 0; i < 12; i++) {
            work->landed[i] = 0;
            work->flash[i] = 0;
            work->burst[i] = 0;
            work->head[i] = 0;
            work->red[i] = work->command->ribbon_red;
            work->green[i] = work->command->ribbon_green;
            work->blue[i] = work->command->ribbon_blue;
        }
        work->timer = 0;
        work->ambient = 0x800;
        work->count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->state == 0) {
        work->ambient = (work->ambient < -511) ? -512 : work->ambient - step * 64;
    } else if (work->state >= 5) {
        work->ambient = (work->ambient < 0x800) ? work->ambient + step * 64 : 0x800;
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->timer < work->command->start) {
        work->timer += step;
        return 0;
    }
    PushMatrix();
    view = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->timer >= work->command->start && work->timer < work->command->phase2 && work->state == 0) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
        offset.vx = matrix.t[0];
        offset.vy = matrix.t[1];
        offset.vz = matrix.t[2];
        for (i = 0; i < work->command->batch_count; i++) {
            addVector(&work->pos[i], &offset);
            applyVector(&work->vel[i], work->pos[i].vx, work->pos[i].vy, work->pos[i].vz, -=);
            applyVector(&work->vel[i], work->command->travel_steps, work->command->travel_steps,
                        work->command->travel_steps, /=);
        }
        work->count += work->command->batch_count;
        work->state = 1;
    }
    if (work->timer >= work->command->phase2 && work->timer < work->command->phase3 && work->state == 1) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
        offset.vx = matrix.t[0];
        offset.vy = matrix.t[1];
        offset.vz = matrix.t[2];
        for (i = work->count; i < work->command->batch_count + work->count; i++) {
            addVector(&work->pos[i], &offset);
            applyVector(&work->vel[i], work->pos[i].vx, work->pos[i].vy, work->pos[i].vz, -=);
            applyVector(&work->vel[i], work->command->travel_steps, work->command->travel_steps,
                        work->command->travel_steps, /=);
        }
        work->count += work->command->batch_count;
        work->state = 2;
    }
    if (work->timer >= work->command->phase3 && work->timer < work->command->phase4 && work->state == 2) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
        offset.vx = matrix.t[0];
        offset.vy = matrix.t[1];
        offset.vz = matrix.t[2];
        for (i = work->count; i < work->command->batch_count + work->count; i++) {
            addVector(&work->pos[i], &offset);
            applyVector(&work->vel[i], work->pos[i].vx, work->pos[i].vy, work->pos[i].vz, -=);
            applyVector(&work->vel[i], work->command->travel_steps, work->command->travel_steps,
                        work->command->travel_steps, /=);
        }
        work->count += work->command->batch_count;
        work->state = 3;
    }
    if (work->timer >= work->command->phase4 && work->timer < work->command->phase_end && work->state == 3) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
        offset.vx = matrix.t[0];
        offset.vy = matrix.t[1];
        offset.vz = matrix.t[2];
        for (i = work->count; i < work->command->batch_count + work->count; i++) {
            addVector(&work->pos[i], &offset);
            applyVector(&work->vel[i], work->pos[i].vx, work->pos[i].vy, work->pos[i].vz, -=);
            applyVector(&work->vel[i], work->command->travel_steps, work->command->travel_steps,
                        work->command->travel_steps, /=);
        }
        work->count += work->command->batch_count;
        work->state = 4;
    }
    if (work->state > 0) {
        pos = work->pos;
        vel = work->vel;
        for (i = 0; i < work->count; i++) {
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            off = work->offsets.grid[i];
            dir = work->directions.grid[i];
            box.attribute = 0x50000000;
            box.w = 1;
            box.h = 1;
            GsSetLsMatrix(&view);
            RotTrans(pos, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(off, screen, depths, interpolations, flags, 12);
            for (j = 0; j < 12; j++) {
                box.x = screen[j].vx;
                box.y = screen[j].vy;
                a = work->command->trail_period - (j + work->timer) % work->command->trail_period;
                box.r = work->command->trail_red * a / work->command->trail_period;
                box.g = work->command->trail_green * a / work->command->trail_period;
                box.b = work->command->trail_blue * a / work->command->trail_period;
                if (depths[j] != 0) {
                    GsSortBoxFill(&box, ot, depths[j] / 4);
                }
                a = (work->timer + j) % work->command->trail_period;
                off->vx = dir->vx;
                ((Model773Point *)off)->c[1] = ((Model773Point *)dir)->c[1];
                ((Model773Point *)off)->c[2] = ((Model773Point *)dir)->c[2];
                off->vx *= a;
                ((Model773Point *)off)->c[1] *= a;
                ((Model773Point *)off)->c[2] *= a;
                off++;
                dir++;
            }
            if (work->head[i] >= 5 &&
                (work->red[i] != 0 || work->green[i] != 0 || work->blue[i] != 0)) {
                setlen(&ribbon, 8);
                setcode(&ribbon, 0x38);
                GsSetLsMatrix(&view);
                for (j = work->head[i] - 1, k = 1; j >= work->head[i] - 4; k++, j--) {
                    ribbon.r0 = work->red[i] / k;
                    ribbon.g0 = work->green[i] / k;
                    ribbon.b0 = work->blue[i] / k;
                    ribbon.r1 = work->red[i] / k;
                    ribbon.g1 = work->green[i] / k;
                    ribbon.b1 = work->blue[i] / k;
                    ribbon.r2 = work->red[i] / k;
                    ribbon.g2 = work->green[i] / k;
                    ribbon.b2 = work->blue[i] / k;
                    ribbon.r3 = work->red[i] / k;
                    ribbon.g3 = work->green[i] / k;
                    ribbon.b3 = work->blue[i] / k;
                    corners[0].vx = work->ribbon[i][j].vx;
                    corners[0].vy = work->ribbon[i][j].vy - work->command->ribbon_width / 2;
                    corners[0].vz = work->ribbon[i][j].vz;
                    corners[1].vx = work->ribbon[i][j].vx;
                    corners[1].vy = work->ribbon[i][j].vy + work->command->ribbon_width / 2;
                    corners[1].vz = work->ribbon[i][j].vz;
                    corners[2].vx = work->ribbon[i][j - 1].vx;
                    corners[2].vy = work->ribbon[i][j - 1].vy - work->command->ribbon_width / 2;
                    corners[2].vz = work->ribbon[i][j - 1].vz;
                    corners[3].vx = work->ribbon[i][j - 1].vx;
                    corners[3].vy = work->ribbon[i][j - 1].vy + work->command->ribbon_width / 2;
                    corners[3].vz = work->ribbon[i][j - 1].vz;
                    depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                        (PSXLONG *)&ribbon.x0, (PSXLONG *)&ribbon.x1,
                                        (PSXLONG *)&ribbon.x2, (PSXLONG *)&ribbon.x3, &interpolation, &flag);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ribbon, ot, (u16)depth, 1);
                    }
                }
                if (work->landed[i] > 0) {
                    if (work->red[i] >= 16) {
                        work->red[i] -= step * 5;
                    } else {
                        work->red[i] = 0;
                    }
                    if (work->green[i] >= 16) {
                        work->green[i] -= step * 5;
                    } else {
                        work->green[i] = 0;
                    }
                    work->blue[i] -= step * 10;
                    if (work->blue[i] < 15 || work->blue[i] > 128) {
                        work->blue[i] = 0;
                    }
                }
            }
            setlen(&orb, 9);
            setcode(&orb, 0x2C);
            orb.tpage = work->texture[0] >> 16;
            orb.clut = work->texture[0];
            orb.r0 = 0x80;
            orb.g0 = 0x80;
            orb.b0 = 0x80;
            corners[0].vx = 0;
            corners[0].vy = -work->command->ribbon_width;
            corners[0].vz = -work->command->sprite_height * 2;
            corners[1].vx = 0;
            corners[1].vy = -work->command->ribbon_width;
            corners[1].vz = 0;
            corners[2].vx = 0;
            corners[2].vy = 0;
            corners[2].vz = -work->command->sprite_height * 2;
            corners[3].vx = 0;
            corners[3].vy = 0;
            corners[3].vz = 0;
            GsSetLsMatrix(&view);
            RotTrans(pos, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            orb.u0 = 0x37;
            orb.v0 = 0;
            orb.u1 = 0;
            orb.v1 = 0;
            orb.u2 = 0x37;
            orb.v2 = 0x10;
            orb.u3 = 0;
            orb.v3 = 0x10;
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
                                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&orb, ot, (u16)depth, 0);
            }
            orb.u0 = 0x37;
            orb.v0 = 0x1F;
            orb.u1 = 0;
            orb.v1 = 0x1F;
            orb.u2 = 0x37;
            orb.v2 = 0x10;
            orb.u3 = 0;
            orb.v3 = 0x10;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0x555;
            GsSetLsMatrix(&view);
            RotTrans(pos, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
                                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&orb, ot, (u16)depth, 0);
            }
            orb.u0 = 0x37;
            orb.v0 = 0x1F;
            orb.u1 = 0;
            orb.v1 = 0x1F;
            orb.u2 = 0x37;
            orb.v2 = 0x10;
            orb.u3 = 0;
            orb.v3 = 0x10;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0xAAB;
            GsSetLsMatrix(&view);
            RotTrans(pos, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
                                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&orb, ot, (u16)depth, 0);
            }
            if (side * work->pos[i].vz < 450 - work->command->sprite_height) {
                (work->ribbon[i] + work->head[i])->vx = pos->vx;
                (work->ribbon[i] + work->head[i])->vy = ((Model773Point *)pos)->c[1];
                (work->ribbon[i] + work->head[i])->vz = ((Model773Point *)pos)->c[2];
                work->head[i] = (work->head[i] + 1) % 32;
                pos->vx += vel->vx;
                ((Model773Point *)pos)->c[1] += vel->vy;
                ((Model773Point *)pos)->c[2] += vel->vz;
                if (side * work->pos[i].vz >= 450 - work->command->sprite_height) {
                    work->landed[i] = 1;
                    work->pos[i].vz = side * (450 - work->command->sprite_height);
                }
            }
            if (work->landed[i] > 0 && work->burst[i] < 4) {
                setlen(&burst, 9);
                setcode(&burst, 0x2C);
                burst.tpage = work->texture[1] >> 16;
                burst.clut = work->texture[1];
                burst.r0 = 0x80;
                burst.g0 = 0x80;
                burst.b0 = 0x80;
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                GsSetLsMatrix(&view);
                RotTrans(pos, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                corners[0].vx = -work->command->burst_size;
                corners[0].vy = -work->command->burst_size * 2;
                corners[0].vz = 0;
                corners[1].vx = work->command->burst_size;
                corners[1].vy = -work->command->burst_size * 2;
                corners[1].vz = 0;
                corners[2].vx = -work->command->burst_size;
                corners[2].vy = work->command->burst_size * 2;
                corners[2].vz = 0;
                corners[3].vx = work->command->burst_size;
                corners[3].vy = work->command->burst_size * 2;
                corners[3].vz = 0;
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                    (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2,
                                    (PSXLONG *)&burst.x3, &interpolation, &flag);
                burst.u0 = (work->burst[i] % 4) * 32 + 0x38;
                burst.v0 = 0;
                burst.u1 = (work->burst[i] % 4) * 32 + 0x57;
                burst.v1 = 0;
                burst.u2 = (work->burst[i] % 4) * 32 + 0x38;
                burst.v2 = 0x3F;
                burst.u3 = (work->burst[i] % 4) * 32 + 0x57;
                burst.v3 = 0x3F;
                SetSemiTrans(&burst, 1);
                if (depth >= 0) {
                    GsSortPoly(&burst, ot, depth);
                }
                work->burst[i]++;
                if (work->burst[i] == 4) {
                    work->done++;
                }
            }
            pos++;
            vel++;
        }
    }
    if (work->done >= 12 && work->timer >= work->command->flash_start && work->flash[0] < 8) {
        setlen(&flash, 9);
        setcode(&flash, 0x2C);
        flash.tpage = work->texture[2] >> 16;
        flash.clut = work->texture[2];
        flash.r0 = 0x80;
        flash.g0 = 0x80;
        flash.b0 = 0x80;
        corners[0].vx = -work->command->flash_size;
        corners[0].vy = -work->command->flash_size;
        corners[0].vz = 0;
        corners[1].vx = work->command->flash_size;
        corners[1].vy = -work->command->flash_size;
        corners[1].vz = 0;
        corners[2].vx = -work->command->flash_size;
        corners[2].vy = work->command->flash_size;
        corners[2].vz = 0;
        corners[3].vx = work->command->flash_size;
        corners[3].vy = work->command->flash_size;
        corners[3].vz = 0;
        point = work->pos;
        for (i = 11; i >= work->count; i--) {
            if (work->flash[i] < 8) {
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                GsSetLsMatrix(&view);
                RotTrans(&point[i], (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                    (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                                    (PSXLONG *)&flash.x3, &interpolation, &flag);
                if (work->flash[i] < 4) {
                    flash.u0 = (work->flash[i] % 4) * 64;
                    flash.v0 = 0x40;
                    flash.u1 = (work->flash[i] % 4) * 64 + 0x3F;
                    flash.v1 = 0x40;
                    flash.u2 = (work->flash[i] % 4) * 64;
                    flash.v2 = 0x7F;
                    flash.u3 = (work->flash[i] % 4) * 64 + 0x3F;
                    flash.v3 = 0x7F;
                } else {
                    flash.u0 = (work->flash[i] % 4) * 64;
                    flash.v0 = 0x80;
                    flash.u1 = (work->flash[i] % 4) * 64 + 0x3F;
                    flash.v1 = 0x80;
                    flash.u2 = (work->flash[i] % 4) * 64;
                    flash.v2 = 0xBF;
                    flash.u3 = (work->flash[i] % 4) * 64 + 0x3F;
                    flash.v3 = 0xBF;
                }
                SetSemiTrans(&flash, 1);
                if (depth >= 0) {
                    GsSortPoly(&flash, ot, depth);
                }
                work->flash[i]++;
            }
        }
        work->count--;
        if (work->count < 0) {
            work->count = 0;
        }
        if (work->count > 0 && work->count <= 10) {
            work->timer += step;
            PopMatrix();
            return 4;
        }
        if (work->count == 0 && work->state == 4) {
            work->state = 5;
            work->timer += step;
            PopMatrix();
            return 1;
        }
    }
    work->timer += step;
    PopMatrix();
    if (work->landed[0] == 1) {
        work->landed[0] = 2;
    } else if (work->landed[3] == 1) {
        work->landed[3] = 2;
    } else if (work->landed[6] == 1) {
        work->landed[6] = 2;
    } else if (work->landed[9] == 1) {
        work->landed[9] = 2;
    } else {
        return (work->flash[0] == 7) * 2;
    }
    return 4;
}
