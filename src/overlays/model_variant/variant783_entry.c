#include "../../types.h"
#include "variant783_entry.h"
#include "../../game/screen_projection.h"

/* Header 783's entry: particles gather into an orb at a part of the slot,
 * the orb flies across to the opposing side and bursts into dots and rays,
 * while a white flash fades out; once the ring starts, a spinning textured
 * ring opens around the slot and throws out falling sparks. The slot's
 * ambient level dips until the burst and comes back afterwards. */

static const VECTOR D_8013D4F0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model783State *work;
    MATRIX base, matrix;
    POLY_FT4 orb;
    GsBOXF dot;
    POLY_G3 ray;
    POLY_F4 flash;
    POLY_FT4 ring;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    DVECTOR screen[128];
    u16 projection[128];
    u16 flags[128];
    u16 depths[128];
    PSXLONG flag, interpolation;
    s32 direction, step;
    GsOT *ot;
    s32 i;
    s32 a, b;
    SVECTOR *point, *other, *spin;
    DVECTOR *projected;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D4F0;
    memset(&origin, 0, 8);
    work = (Model783State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        /* The walking pointers (and the inline ring angle below) give the
         * retail induction registers and register allocation. */
        point = work->particles;
        work->config = &D_8013D538[command];
        other = work->particle_steps;
        spin = work->spins;
        setVector(&work->position, 0, -work->config->lift, 0);
        setVector(&work->step, 0, 0, 0);
        for (i = 0; i < work->config->gather_count; i++) {
            point->vx = work->config->gather_speed * ccos(rand() % 4096) / 4096;
            point->vy = work->config->gather_speed * csin(rand() % 4096) / 4096;
            point->vz = work->config->gather_speed * ccos(rand() % 4096) / 4096;
            other->vx = point->vx;
            other->vy = point->vy;
            other->vz = point->vz;
            other->vx = other->vx / -work->config->gather_divisor;
            other->vy = other->vy / -work->config->gather_divisor;
            other->vz = other->vz / -work->config->gather_divisor;
            setVector(spin, rand() % 16 + 16, rand() % 16 + 16, rand() % 16 + 16);
            point++;
            other++;
            spin++;
        }
        point = work->ring_outer;
        other = work->ring_inner;
        for (i = 0; i < 16; i++) {
            point->vx = work->config->ring_outer * ccos(i * 256 % 4096) / 4096;
            point->vy = work->config->ring_outer * csin(i * 256 % 4096) / 4096;
            point->vz = direction * work->config->ring_outer * ccos(128) / 4096;
            other->vx = work->config->ring_inner * ccos(i * 256 % 4096) / 4096;
            other->vy = work->config->ring_inner * csin(i * 256 % 4096) / 4096;
            other->vz = direction * work->config->ring_inner * ccos(128) / 4096;
            point++;
            other++;
        }
        point = work->sparks;
        other = work->spark_steps;
        projected = work->spark_screen;
        for (i = 0; i < work->config->spark_count; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            a = rand() % 4096;
            b = rand() % 4096;
            other->vx = work->config->spark_speed * ccos(a) / 4096 * ccos(b) / 4096;
            other->vy = work->config->spark_speed * csin(a) / 4096 * ccos(b) / 4096;
            other->vz = work->config->spark_speed * csin(a) / 4096 * ccos(b) / 4096;
            point++;
            other++;
            projected->vy = 0;
            projected->vx = 0;
            work->spark_depth[i] = 0;
            projected++;
        }
        work->ring_r = work->config->ring_r;
        work->ring_g = work->config->ring_g;
        work->ring_b = work->config->ring_b;
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D500[i]);
        }
        work->level = 2048;
        work->elapsed = 0;
        work->phase = 0;
        work->orb_r = 0;
        work->orb_g = 0;
        work->orb_b = 0;
        work->flash = 255;
        work->dots = 255;
        work->gathered = 0;
        work->growth = 0;
        setVector(&work->center, 0, 0, 0);
        work->center_set = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase >= 4) {
        work->level = 2048;
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->ring_start) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
    }
    if (work->elapsed >= work->config->ring_start) {
        if (work->phase == 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
            setVector(&work->step, matrix.t[0], matrix.t[1], matrix.t[2]);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            /* Retail reseeds only the first particle, once per ray. */
            for (i = 0; i < work->config->ray_count; i++) {
                setVector(work->particles, rand() % 4096, rand() % 4096, rand() % 4096);
            }
            work->gathered = 0;
            work->phase = 1;
        }
        if (work->elapsed >= work->config->ring_start && work->phase < 3) {
            if (work->phase == 1) {
                setVector(&work->position, work->step.vx, work->step.vy, work->step.vz);
                setVector(&work->step, -work->step.vx, -350 - work->step.vy, direction * 450 - work->step.vz);
                work->step.vx /= work->config->travel_divisor;
                work->step.vy /= work->config->travel_divisor;
                work->step.vz /= work->config->travel_divisor;
                work->phase = 2;
            }
            setVector(&position, 0, 0, 0);
            addVector(&work->position, &work->step);
            if (direction < 0) {
                if (work->position.vz <= direction * 450) {
                    work->phase = 3;
                    work->position.vz = direction * 450;
                }
            } else if (work->position.vz >= 450) {
                work->phase = 3;
                work->position.vz = direction * 450;
            }
        }
    }
    if (work->phase == 3 && work->elapsed < work->config->burst_start) {
        setVector(&position, 0, 0, 0);
    }
    if (work->elapsed >= work->config->burst_start) {
        if (work->phase == 3) {
            work->phase = 4;
            for (i = 0; i < work->config->burst_count; i++) {
                setVector(&work->particles[i], work->config->burst_speed * (rand() % 4096 - rand() % 4096) / 4096,
                    work->config->burst_speed * (rand() % 4096 - rand() % 4096) / 4096,
                    -direction * work->config->burst_speed * (rand() % 4096) / 4096);
                setVector(&work->particle_steps[i], work->particles[i].vx / work->config->burst_divisor,
                    work->particles[i].vy / work->config->burst_divisor,
                    work->particles[i].vz / work->config->burst_divisor);
            }
        }
        if (work->flash != 0) {
            setPolyF4(&flash);
            setRGB0(&flash, work->flash, work->flash, work->flash);
#ifdef VERSION_FRENCH
            setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
#else
            setXY4(&flash, 0, 0, 320, 0, 0, 240, 320, 240);
#endif
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 0, 1);
            if (work->flash >= 10) {
                work->flash -= step * work->config->flash_rate;
            } else {
                work->flash = 0;
            }
        }
        if (work->dots != 0) {
            dot.w = dot.h = 1;
            dot.r = work->dots;
            dot.g = work->dots;
            dot.b = work->dots;
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, 0, 0, 0);
            setVector(&position, work->position.vx, work->position.vy, work->position.vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->particles;
            RotTransPersN(point, screen, depths, projection, flags, work->config->burst_count);
            dot.attribute = 0x40000000;
            other = work->particle_steps;
            flag = 0;
            for (i = 0; i < work->config->burst_count; i++) {
                /* "/ 4" rather than ">> 2" keeps retail's copy before the shift. */
                dot.x = screen[i].vx;
                dot.y = screen[i].vy;
                if (depths[i] != 0) {
                    GsSortBoxFill(&dot, ot, depths[i] / 4);
                }
                addVector(point, other);
                point++;
                other++;
            }
            if (work->dots >= 10) {
                work->dots -= step * work->config->fade_rate;
            } else {
                work->dots = 0;
            }
        }
        if (work->orb_r != 0 || work->orb_g != 0 || work->orb_b != 0) {
            setPolyFT4(&orb);
            orb.tpage = work->texture[1] >> 16;
            orb.clut = work->texture[1];
            setRGB0(&orb, work->orb_r, work->orb_g, work->orb_b);
            setVector(&vertices[0], -work->config->orb_size, -work->config->orb_size, 0);
            setVector(&vertices[1], work->config->orb_size, -work->config->orb_size, 0);
            setVector(&vertices[2], -work->config->orb_size, work->config->orb_size, 0);
            setVector(&vertices[3], work->config->orb_size, work->config->orb_size, 0);
            setVector(&scale, 0x3000, 0x3000, 0x3000);
            setVector(&position, work->position.vx, work->position.vy, work->position.vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
                &interpolation, &flag);
            SetSemiTrans(&orb, 1);
            if (depth >= 0) {
                GsSortPoly(&orb, ot, depth);
            }
            setVector(&vertices[0], -work->config->orb_size * 2, -work->config->orb_size * 2, 0);
            setVector(&vertices[1], work->config->orb_size * 2, -work->config->orb_size * 2, 0);
            setVector(&vertices[2], -work->config->orb_size * 2, work->config->orb_size * 2, 0);
            setVector(&vertices[3], work->config->orb_size * 2, work->config->orb_size * 2, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
                &interpolation, &flag);
            SetSemiTrans(&orb, 1);
            if (depth >= 0) {
                GsSortPoly(&orb, ot, depth);
            }
            if (work->orb_r >= 10) {
                work->orb_r -= step * work->config->fade_rate;
            } else {
                work->orb_r = 0;
            }
            if (work->orb_g >= 10) {
                work->orb_g -= step * work->config->fade_rate;
            } else {
                work->orb_g = 0;
            }
            if (work->orb_b >= 10) {
                work->orb_b -= step * work->config->fade_rate;
            } else {
                work->orb_b = 0;
            }
        }
    }
    if (work->elapsed < work->config->burst_start) {
        setPolyFT4(&orb);
        orb.tpage = work->texture[1] >> 16;
        orb.clut = work->texture[1];
        setRGB0(&orb, work->orb_r, work->orb_g, work->orb_b);
        setUV4(&orb, 0, 0, 63, 0, 0, 63, 63, 63);
        if (work->phase >= 3) {
            setVector(&scale, 0x3000, 0x3000, 0x3000);
        }
        setVector(&vertices[0], -work->config->orb_size, -work->config->orb_size, 0);
        setVector(&vertices[1], work->config->orb_size, -work->config->orb_size, 0);
        setVector(&vertices[2], -work->config->orb_size, work->config->orb_size, 0);
        setVector(&vertices[3], work->config->orb_size, work->config->orb_size, 0);
        GsSetLsMatrix(&base);
        addVector(&position, &work->position);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
            &interpolation, &flag);
        SetSemiTrans(&orb, 1);
        if (depth >= 0) {
            GsSortPoly(&orb, ot, depth);
        }
        setVector(&vertices[0], -work->config->orb_size * 2, -work->config->orb_size * 2, 0);
        setVector(&vertices[1], work->config->orb_size * 2, -work->config->orb_size * 2, 0);
        setVector(&vertices[2], -work->config->orb_size * 2, work->config->orb_size * 2, 0);
        setVector(&vertices[3], work->config->orb_size * 2, work->config->orb_size * 2, 0);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&orb.x0, (PSXLONG *)&orb.x1, (PSXLONG *)&orb.x2, (PSXLONG *)&orb.x3,
            &interpolation, &flag);
        SetSemiTrans(&orb, 1);
        if (depth >= 0) {
            GsSortPoly(&orb, ot, depth);
        }
        if (work->phase == 0) {
            dot.w = dot.h = 1;
            dot.r = 128;
            dot.g = 128;
            dot.b = 128;
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, 0, 0, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->particles;
            other = work->particle_steps;
            RotTransPersN(point, screen, depths, projection, flags, work->gathered);
            dot.attribute = 0;
            flag = 0;
            for (i = 0; i < work->gathered; i++) {
                /* The abs on the left schedules the size load before it. */
                if (__builtin_abs(point->vx) > work->config->orb_size / 2) {
                    dot.x = screen[i].vx;
                    dot.y = screen[i].vy;
                    if (depths[i] != 0) {
                        GsSortBoxFill(&dot, ot, depths[i] / 4);
                    }
                    addVector(point, other);
                }
                point++;
                other++;
            }
            work->gathered += step;
            if (work->gathered > work->config->gather_count) {
                work->gathered = work->config->gather_count;
            }
            if (work->config->orb_r < work->orb_r) {
                work->orb_r = work->config->orb_r;
            } else {
                work->orb_r += step * work->config->fade_rate;
            }
            if (work->config->orb_g < work->orb_g) {
                work->orb_g = work->config->orb_g;
            } else {
                work->orb_g += step * work->config->fade_rate;
            }
            if (work->config->orb_b < work->orb_b) {
                work->orb_b = work->config->orb_b;
            } else {
                work->orb_b += step * work->config->fade_rate;
            }
        }
        if (work->phase == 3) {
            for (i = 0; i < work->config->ray_count; i++) {
                setPolyG3(&ray);
                setVector(&vertices[0], work->config->ray_width, work->config->ray_length, 0);
                setVector(&vertices[1], -work->config->ray_width, work->config->ray_length, 0);
                setRGB0(&ray, work->config->ray_r, work->config->ray_g, work->config->ray_b);
                setRGB1(&ray, 0, 0, 0);
                setRGB2(&ray, 0, 0, 0);
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                copyVector(&rotation, &work->spins[i]);
                rotation.vx *= work->elapsed;
                rotation.vy *= work->elapsed;
                rotation.vz *= work->elapsed;
                addVector(&rotation, &work->particles[i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotTransPers3(&origin, &vertices[0], &vertices[1], (PSXLONG *)&ray.x0,
                    (PSXLONG *)&ray.x1, (PSXLONG *)&ray.x2, &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ray, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->ray_width / 2, work->config->ray_length / 2, 0);
                setVector(&vertices[1], -work->config->ray_width / 2, work->config->ray_length / 2, 0);
                setRGB0(&ray, 128, 128, 128);
                setRGB1(&ray, 0, 0, 0);
                setRGB2(&ray, 0, 0, 0);
                depth = RotTransPers3(&origin, &vertices[0], &vertices[1], (PSXLONG *)&ray.x0,
                    (PSXLONG *)&ray.x1, (PSXLONG *)&ray.x2, &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ray, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (work->elapsed >= work->config->ring_start) {
        SetPolyFT4(&ring);
        point = work->ring_outer;
        setRGB0(&ring, work->ring_r, work->ring_g, work->ring_b);
        ring.tpage = work->texture[0] >> 16;
        ring.clut = work->texture[0];
        setUV4(&ring, 0, 64, 31, 64, 0, 255, 31, 255);
        other = work->ring_inner;
        if (work->center_set == 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
            setVector(&work->center, matrix.t[0], matrix.t[1], matrix.t[2]);
            work->center_set = 1;
        }
        setVector(&scale, work->growth + 4096, work->growth + 4096, 4096);
        work->growth += step << 9;
        if (work->growth > 0x7FCD) {
            work->growth = 0x7FCD;
        }
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&work->center, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&point[15], point, &other[15], other, (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2,
                    (PSXLONG *)&ring.x3, &interpolation, &flag);
            } else {
                depth = RotAverage4(&point[i], &point[i + 1], &other[i], &other[i + 1], (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2,
                    (PSXLONG *)&ring.x3, &interpolation, &flag);
            }
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 3);
            }
        }
        if (work->ring_r != 0 || work->ring_g != 0 || work->ring_b != 0) {
            work->center.vz += direction * 8;
        }
        dot.w = dot.h = 1;
        dot.r = work->ring_r;
        dot.g = work->ring_g;
        dot.b = work->ring_b;
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        dot.attribute = 0x40000000;
        point = work->sparks;
        for (i = 0; i < work->config->spark_count; i++) {
            /* Retail draws these stale dots at the first projected point. */
            if (point->vy < 350) {
                dot.x = screen->vx;
                dot.y = screen->vy;
                if (work->spark_depth[i] != 0) {
                    GsSortBoxFill(&dot, ot, work->spark_depth[i] / 4);
                }
            }
            point++;
        }
        point = work->sparks;
        projected = work->spark_screen;
        RotTransPersN(point, projected, work->spark_depth, projection, flags, work->config->spark_count);
        dot.attribute = 0;
        flag = 0;
        for (i = 0; i < work->config->spark_count; i++) {
            if (point->vy < 350) {
                dot.x = projected->vx;
                dot.y = projected->vy;
                if (work->spark_depth[i] != 0) {
                    GsSortBoxFill(&dot, ot, work->spark_depth[i] / 4);
                }
                flag = 1;
            }
            point++;
            projected++;
        }
        gteMIMefunc(work->sparks, work->spark_steps, work->config->spark_count, 0x800);
        /* Walking the steps by pointer gives retail's .vy induction register. */
        other = work->spark_steps;
        for (i = 0; i < work->config->spark_count; i++) {
            other->vy += (350 - other->vy) / work->config->spark_speed / 2;
            other++;
        }
        if (work->ring_r >= 16) {
            work->ring_r -= step * work->config->ring_fade_rate;
        } else {
            work->ring_r = 0;
        }
        if (work->ring_g >= 16) {
            work->ring_g -= step * work->config->ring_fade_rate;
        } else {
            work->ring_g = 0;
        }
        if (work->ring_b >= 16) {
            work->ring_b -= step * work->config->ring_fade_rate;
        } else {
            work->ring_b = 0;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3) {
        return 4;
    }
    if (work->phase == 4) {
        work->phase = 5;
        return 1;
    }
    if (work->orb_r == 0 && work->orb_g == 0 && work->orb_b == 0 && work->flash == 0) {
        return 2;
    }
    return 0;
}
