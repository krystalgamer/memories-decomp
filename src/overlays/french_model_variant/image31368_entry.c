#include "../../types.h"
#include "image31368_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Image31368State *work;
    MATRIX view, matrix;
    POLY_FT4 quad;
    GsBOXF box;
    POLY_G3 wing;
    POLY_F4 tile;
    POLY_FT4 ring;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    SVECTOR origin;
    DVECTOR screen[128];
    u16 interpolations[128], flags[128], depths[128];
    u16 *projected_depths;
    PSXLONG flag, interpolation;
    s32 direction, step;
    GsOT *ot;
    SVECTOR *point, *velocity, *drift;
    DVECTOR *xy;
    s32 i, a, b;
    s32 distance;
    s16 half;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D388;
    memset(&origin, 0, 8);
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    work = (Image31368State *)context;
    if (command >= 0) {
        point = work->particles;
        work->command = &D_8013D3D0[command];
        work->position.vx = 0;
        velocity = work->particle_velocities;
        work->position.vy = 0;
        drift = work->drift;
        work->position.vz = direction * work->command->start_depth;
        work->velocity.vx = 0;
        work->velocity.vy = 0;
        work->velocity.vz = 0;
        for (i = 0; i < work->command->particle_count; i++) {
            point->vx = work->command->particle_radius * ccos(rand() % 4096) / 4096;
            point->vy = work->command->particle_radius * csin(rand() % 4096) / 4096;
            point->vz = work->command->particle_radius * ccos(rand() % 4096) / 4096;
            velocity->vx = point->vx;
            velocity->vy = point->vy;
            velocity->vz = point->vz;
            velocity->vx /= -work->command->particle_speed;
            velocity->vy /= -work->command->particle_speed;
            velocity->vz /= -work->command->particle_speed;
            drift->vx = rand() % 16 + 16;
            drift->vy = rand() % 16 + 16;
            drift->vz = rand() % 16 + 16;
            point++;
            velocity++;
            drift++;
        }
        point = work->ring;
        velocity = work->outer;
        for (i = 0; i < 16; i++) {
            point->vx = work->command->ring_radius * ccos((i << 8) % 4096) / 4096;
            point->vy = work->command->ring_radius * csin((i << 8) % 4096) / 4096;
            point->vz = direction * work->command->ring_radius * ccos(128) / 4096;
            velocity->vx = work->command->outer_radius * ccos((i << 8) % 4096) / 4096;
            velocity->vy = work->command->outer_radius * csin((i << 8) % 4096) / 4096;
            velocity->vz = direction * work->command->outer_radius * ccos(128) / 4096;
            point++;
            velocity++;
        }
        point = work->spray;
        velocity = work->spray_velocities;
        xy = work->spray_screen;
        for (i = 0; i < work->command->spray_count; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            a = rand() % 4096;
            b = rand() % 4096;
            velocity->vx = work->command->spray_radius * ccos(a) / 4096 * ccos(b) / 4096;
            velocity->vy = work->command->spray_radius * csin(a) / 4096 * ccos(b) / 4096;
            velocity->vz = work->command->spray_radius * csin(a) / 4096 * ccos(b) / 4096;
            point++;
            velocity++;
            xy->vy = 0;
            xy->vx = 0;
            work->spray_depth[i] = 0;
            xy++;
        }
        work->ring_red = work->command->ring_red;
        work->ring_green = work->command->ring_green;
        work->ring_blue = work->command->ring_blue;
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D398[i]);
        }
        work->ambient = 0x800;
        work->elapsed = 0;
        work->state = 0;
        work->fade = 0;
        work->flash = 0xFF;
        work->burst_level = 0xFF;
        work->shown = 0;
        work->growth = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->state == 0) {
        work->ambient = (work->ambient < -511) ? -512 : work->ambient - (step << 6);
    } else if (work->state >= 4) {
        work->ambient = 0x800;
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    PushMatrix();
    view = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->command->start) {
        PopMatrix();
        work->elapsed += step;
        return 0;
    }
    if (work->elapsed < work->command->release) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
    }
    if (work->elapsed >= work->command->release) {
        if (work->state == 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
            work->velocity.vx = matrix.t[0];
            work->velocity.vy = matrix.t[1];
            work->velocity.vz = matrix.t[2];
            for (i = 0; i < work->command->wing_count; i++) {
                work->particles[0].vx = rand() % 4096;
                work->particles[0].vy = rand() % 4096;
                work->particles[0].vz = rand() % 4096;
            }
            work->shown = 0;
            work->state = 1;
            position.vx = work->velocity.vx;
            position.vy = work->velocity.vy;
            position.vz = work->velocity.vz;
        }
    }
    if (work->elapsed >= work->command->release && work->state < 3) {
        if (work->state == 1) {
            work->position.vx = work->velocity.vx;
            work->position.vy = work->velocity.vy;
            work->position.vz = work->velocity.vz;
            work->velocity.vx = -work->velocity.vx;
            work->velocity.vy = -350 - work->velocity.vy;
            work->velocity.vz = direction * 450 - work->velocity.vz;
            work->velocity.vx /= work->command->travel_steps;
            work->velocity.vy /= work->command->travel_steps;
            work->velocity.vz /= work->command->travel_steps;
            work->state = 2;
        }
        position.vx = 0;
        position.vy = 0;
        position.vz = 0;
        work->position.vx += work->velocity.vx;
        work->position.vy += work->velocity.vy;
        work->position.vz += work->velocity.vz;
        if (direction * work->position.vz >= 450) {
            work->state = 3;
            work->position.vz = direction * 450;
        }
    }
    if (work->state == 3 && work->elapsed < work->command->burst_time) {
        position.vx = 0;
        position.vy = 0;
        position.vz = 0;
    }
    if (work->elapsed >= work->command->burst_time) {
        if (work->state == 3) {
            work->state = 4;
            for (i = 0; i < work->command->burst_count; i++) {
                setVector(&work->particles[i], work->command->burst_radius * (rand() % 4096 - rand() % 4096) / 4096, work->command->burst_radius * (rand() % 4096 - rand() % 4096) / 4096, -direction * work->command->burst_radius * (rand() % 4096) / 4096);
                setVector(&work->particle_velocities[i], work->particles[i].vx / work->command->burst_speed, work->particles[i].vy / work->command->burst_speed, work->particles[i].vz / work->command->burst_speed);
            }
        }
        if (work->flash != 0) {
            setlen(&tile, 5);
            setcode(&tile, 0x28);
            tile.r0 = work->flash;
            tile.g0 = work->flash;
            tile.b0 = work->flash;
            tile.x0 = 0;
            tile.y0 = 0;
            tile.x1 = 320;
            tile.y1 = 0;
            tile.x2 = 0;
            tile.y2 = 256;
            tile.x3 = 320;
            tile.y3 = 256;
            Graphics_SubmitDrawModePacket((u32 *)&tile, ot, 0, 1);
            if (work->flash >= 10) {
                work->flash -= step * 3;
            } else {
                work->flash = 0;
            }
        }
        if (work->burst_level != 0) {
            box.h = 1;
            box.w = 1;
            box.r = work->burst_level;
            box.g = work->burst_level;
            box.b = work->burst_level;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            position.vx = work->position.vx;
            position.vy = work->position.vy;
            position.vz = work->position.vz;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->particles;
            RotTransPersN(point, screen, depths, interpolations, flags, work->command->burst_count);
            box.attribute = 0x40000000;
            velocity = work->particle_velocities;
            flag = 0;
            for (i = 0; i < work->command->burst_count; i++) {
                box.x = screen[i].vx;
                box.y = screen[i].vy;
                if (depths[i] != 0) {
                    GsSortBoxFill(&box, ot, depths[i] / 4);
                }
                point->vx += velocity->vx;
                point->vy += velocity->vy;
                point->vz += velocity->vz;
                point++;
                velocity++;
            }
            if (work->burst_level >= 10) {
                work->burst_level -= step;
            } else {
                work->burst_level = 0;
            }
        }
        if (work->fade != 0) {
            setlen(&quad, 9);
            setcode(&quad, 0x2C);
            quad.tpage = work->texture[1] >> 16;
            quad.clut = work->texture[1];
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = work->command->spin_rate / 2;
            rotation.vz *= work->elapsed;
            quad.r0 = work->fade;
            quad.g0 = work->fade;
            quad.b0 = work->fade;
            corners[0].vx = -work->command->half_size;            corners[0].vy = -work->command->half_size;
            corners[0].vz = 0;
            corners[1].vx = work->command->half_size;            corners[1].vy = -work->command->half_size;
            corners[1].vz = 0;
            corners[2].vx = -work->command->half_size;            corners[2].vy = work->command->half_size;
            corners[2].vz = 0;
            corners[3].vx = work->command->half_size;
            corners[3].vy = work->command->half_size;
            corners[3].vz = 0;
            scale.vx = 0x3000;
            scale.vy = 0x3000;
            scale.vz = 0x3000;
            position.vx = work->position.vx;
            position.vy = work->position.vy;
            position.vz = work->position.vz;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                &interpolation, &flag);
            SetSemiTrans(&quad, 1);
            if (depth >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            corners[0].vx = -work->command->half_size * 2;            corners[0].vy = -work->command->half_size * 2;
            corners[0].vz = 0;
            corners[1].vx = work->command->half_size * 2;            corners[1].vy = -work->command->half_size * 2;
            corners[1].vz = 0;
            corners[2].vx = -work->command->half_size * 2;            corners[2].vy = work->command->half_size * 2;
            corners[2].vz = 0;
            corners[3].vx = work->command->half_size * 2;            corners[3].vy = work->command->half_size * 2;
            corners[3].vz = 0;
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                &interpolation, &flag);
            SetSemiTrans(&quad, 1);
            if (depth >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            if (work->fade >= 10) {
                work->fade -= step;
            } else {
                work->fade = 0;
            }
        }
    }
    if (work->elapsed < work->command->burst_time) {
        setlen(&quad, 9);
        setcode(&quad, 0x2C);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        quad.r0 = work->fade;
        quad.g0 = work->fade;
        quad.b0 = work->fade;
        quad.u0 = 127;
        quad.v0 = 0;
        quad.u1 = 254;
        quad.v1 = 0;
        quad.u2 = 127;
        quad.v2 = 127;
        quad.u3 = 254;
        quad.v3 = 127;
        if (work->state < 2) {
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = work->command->spin_rate / 2;
            rotation.vz *= work->elapsed;
        }
        if (work->state >= 2) {
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = work->command->spin_rate * 4;
            rotation.vz *= work->elapsed;
        }
        if (work->state >= 3) {
            scale.vx = 0x3000;
            scale.vy = 0x3000;
            scale.vz = 0x3000;
        }
        corners[0].vx = -work->command->half_size;        corners[0].vy = -work->command->half_size;
        corners[0].vz = 0;
        corners[1].vx = work->command->half_size;        corners[1].vy = -work->command->half_size;
        corners[1].vz = 0;
        corners[2].vx = -work->command->half_size;        corners[2].vy = work->command->half_size;
        corners[2].vz = 0;
        corners[3].vx = work->command->half_size;        corners[3].vy = work->command->half_size;
        corners[3].vz = 0;
        GsSetLsMatrix(&view);
        position.vx += work->position.vx;
        position.vy += work->position.vy;
        position.vz += work->position.vz;
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
        SetSemiTrans(&quad, 1);
        if (depth >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
        corners[0].vx = -work->command->half_size * 2;        corners[0].vy = -work->command->half_size * 2;
        corners[0].vz = 0;
        corners[1].vx = work->command->half_size * 2;        corners[1].vy = -work->command->half_size * 2;
        corners[1].vz = 0;
        corners[2].vx = -work->command->half_size * 2;        corners[2].vy = work->command->half_size * 2;
        corners[2].vz = 0;
        corners[3].vx = work->command->half_size * 2;        corners[3].vy = work->command->half_size * 2;
        corners[3].vz = 0;
        depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
        SetSemiTrans(&quad, 1);
        if (depth >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
        if (work->state == 0) {
            box.h = 1;
            box.w = 1;
            box.r = 0x80;
            box.g = 0x80;
            box.b = 0x80;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->particles;
            velocity = work->particle_velocities;
            projected_depths = depths;
            RotTransPersN(point, screen, projected_depths, interpolations, flags, work->shown);
            box.attribute = 0;
            flag = 0;
            for (i = 0; i < work->shown; i++) {
                distance = point->vx;
                half = work->command->half_size;
                if (distance < 0) {
                    distance = -distance;
                }
                if (half / 2 < distance) {
                    box.x = screen[i].vx;
                    box.y = screen[i].vy;
                    GsSortBoxFill(&box, ot, projected_depths[i] / 4);
                    point->vx += velocity->vx;
                    point->vy += velocity->vy;
                    point->vz += velocity->vz;
                }
                point++;
                velocity++;
            }
            work->shown += step;
            if (work->command->particle_count < work->shown) {
                work->shown = work->command->particle_count;
            }
            if (work->fade < 122) {
                work->fade += step;
            } else {
                work->fade = 128;
            }
        }
        if (work->state == 3) {
            for (i = 0; i < work->command->wing_count; i++) {
                setlen(&wing, 6);
                setcode(&wing, 0x30);
                corners[0].vx = work->command->wing_width;                corners[0].vy = work->command->wing_height;
                corners[0].vz = 0;
                corners[1].vx = -work->command->wing_width;                corners[1].vy = work->command->wing_height;
                corners[1].vz = 0;
                wing.r0 = work->command->wing_red;
                wing.g0 = work->command->wing_green;
                wing.b0 = work->command->wing_blue;
                wing.r1 = 0;
                wing.g1 = 0;
                wing.b1 = 0;
                wing.r2 = 0;
                wing.g2 = 0;
                wing.b2 = 0;
                scale.vx = 0x1000;
                scale.vy = 0x1000;
                scale.vz = 0x1000;
                GsSetLsMatrix(&view);
                copyVector(&rotation, &work->drift[i]);
                rotation.vx *= work->elapsed;
                rotation.vy *= work->elapsed;
                rotation.vz *= work->elapsed;
                addVector(&rotation, &work->particles[i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&view, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotTransPers3(&origin, &corners[0], &corners[1],
                                      (PSXLONG *)&wing.x0, (PSXLONG *)&wing.x1, (PSXLONG *)&wing.x2,
                                      &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&wing, ot, (u16)depth, 1);
                }
                corners[0].vx = work->command->wing_width / 2;
                corners[0].vy = work->command->wing_height / 2;
                corners[0].vz = 0;
                corners[1].vx = -work->command->wing_width / 2;
                corners[1].vy = work->command->wing_height / 2;
                corners[1].vz = 0;
                wing.r0 = 0x80;
                wing.g0 = 0x80;
                wing.b0 = 0x80;
                wing.r1 = 0;
                wing.g1 = 0;
                wing.b1 = 0;
                wing.r2 = 0;
                wing.g2 = 0;
                wing.b2 = 0;
                depth = RotTransPers3(&origin, &corners[0], &corners[1],
                                      (PSXLONG *)&wing.x0, (PSXLONG *)&wing.x1, (PSXLONG *)&wing.x2,
                                      &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&wing, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (work->elapsed >= work->command->release) {
        SetPolyFT4(&ring);
        ring.r0 = work->ring_red;
        ring.g0 = work->ring_green;
        ring.b0 = work->ring_blue;
        ring.tpage = work->texture[0] >> 16;
        ring.clut = work->texture[0];
        ring.u0 = 0;
        ring.v0 = 0x40;
        ring.u1 = 0;
        ring.v1 = 0xBF;
        ring.u2 = 0x7F;
        ring.v2 = 0x40;
        ring.u3 = 0x7F;
        ring.v3 = 0xBF;
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->command->slot), &matrix);
        point = work->ring;
        velocity = work->outer;
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        scale.vx = work->growth + 0x1000;
        scale.vy = work->growth + 0x1000;
        scale.vz = 0x1000;
        work->growth += step << 9;
        if (work->growth > 0x7FCD) {
            work->growth = 0x7FCD;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        GsSetLsMatrix(&view);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&view, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&point[15], &point[0], &velocity[15], &velocity[0],
                                    (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                                    &interpolation, &flag);
            } else {
                depth = RotAverage4(&point[i], &point[i + 1], &velocity[i], &velocity[i + 1],
                                    (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                                    &interpolation, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 3);
            }
        }
        box.h = 1;
        box.w = 1;
        box.r = work->ring_red;
        box.g = work->ring_green;
        box.b = work->ring_blue;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        GsSetLsMatrix(&view);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        box.attribute = 0x40000000;
        point = work->spray;
        for (i = 0; i < work->command->spray_count; i++) {
            if (point->vy < 350) {
                box.x = screen[0].vx;
                box.y = screen[0].vy;
                if (work->spray_depth[i] != 0) {
                    GsSortBoxFill(&box, ot, work->spray_depth[i] / 4);
                }
            }
            point++;
        }
        point = work->spray;
        xy = work->spray_screen;
        RotTransPersN(point, xy, work->spray_depth, interpolations, flags, work->command->spray_count);
        box.attribute = 0;
        flag = 0;
        for (i = 0; i < work->command->spray_count; i++) {
            if (point->vy < 350) {
                box.x = xy->vx;
                box.y = xy->vy;
                if (work->spray_depth[i] != 0) {
                    GsSortBoxFill(&box, ot, work->spray_depth[i] / 4);
                }
                flag = 1;
            }
            point++;
            xy++;
        }
        gteMIMefunc(work->spray, work->spray_velocities, work->command->spray_count, 0x800);
        velocity = work->spray_velocities;
        for (i = 0; i < work->command->spray_count; i++) {
            velocity->vy += (350 - velocity->vy) / work->command->spray_radius / 2;
            velocity++;
        }
        if (work->ring_red >= 16) {
            work->ring_red -= step * 2;
        } else {
            work->ring_red = 0;
        }
        if (work->ring_green >= 16) {
            work->ring_green -= step * 2;
        } else {
            work->ring_green = 0;
        }
        if (work->ring_blue >= 16) {
            work->ring_blue -= step * 2;
        } else {
            work->ring_blue = 0;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->state == 3) {
        return 4;
    }
    if (work->state == 4) {
        work->state = 5;
        return 1;
    }
    if (work->fade == 0) {
        return 2;
    }
    return 0;
}
