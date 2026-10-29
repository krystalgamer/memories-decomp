#include "../../types.h"
#include "variant782_entry.h"

/* Header 782's entry: a flare drops onto the opposing slot, which then sits
 * under a growing two-layer dome over a Gouraud base disc while debris
 * sprites fly out; the dome shrinks, a ring wave spreads and fades, the
 * slot's height scale recovers and animated sparks rise and fall. */

static const VECTOR D_8013D25C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model782State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 flare;
    POLY_G4 g4;
    POLY_G3 g3;
    POLY_G3 disc;
    POLY_FT4 debris;
    POLY_FT4 spark;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 slot;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 a, b, c;
    s32 depth;
    SVECTOR *outer, *inner, *cone, *ring;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013D25C;
    work = (Model782State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D2C0[command];
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        setVector(&work->origin, 0, -0xAF0, pos.vz);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        /* Retail widens the queried dimensions but never reads them. */
        if (rot.vx < rot.vz) {
            rot.vx = rot.vz;
        }
        outer = work->outer;
        inner = work->inner;
        cone = work->cone;
        ring = work->base;
        for (i = 0; i < 16; outer++, inner++, cone++, ring++, i++) {
            setVector(outer, work->config->ring_radius * ccos(i * 256 % 4096) / 4096, 0,
                      work->config->ring_radius * csin(i * 256 % 4096) / 4096);
            setVector(inner, work->config->ring_depth * ccos(i * 256 % 4096) / 4096,
                      -work->config->ring_depth * csin(0x80) / 4096,
                      work->config->ring_depth * csin(i * 256 % 4096) / 4096);
            setVector(cone, work->config->ring_radius * (ccos(i * 256 % 4096 + 0x80) << 1) / 4096,
                      -work->config->cone_height,
                      work->config->ring_radius * (csin(i * 256 % 4096 + 0x80) << 1) / 4096);
            setVector(ring, work->config->base_radius * ccos(i * 256 % 4096) / 4096, 0,
                      work->config->base_radius * csin(i * 256 % 4096) / 4096);
        }
        /* The debris loops reuse the ring pointers; separate ones change the
         * register allocation. */
        outer = work->debris;
        inner = work->debris_velocity;
        for (i = 0; i < work->config->debris_count; outer++, inner++, i++) {
            a = rand() % 4096 - rand() % 4096;
            b = -(rand() % 4096);
            c = rand() % 4096 - rand() % 4096;
            setVector(outer, work->config->debris_spread * a / 4096,
                      work->config->debris_rise * b / 4096, work->config->debris_spread * c / 4096);
            setVector(inner, work->config->debris_speed * a / 4096,
                      work->config->debris_speed * b / 4096, work->config->debris_speed * c / 4096);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->sparks[i], work->config->size / 2 * ((rand() - rand()) % 4096) / 4096,
                      -work->config->spark_rise * ((rand() - rand()) % 4096) / 4096,
                      work->config->size / 2 * ((rand() - rand()) % 4096) / 4096);
            work->spark_fall[i] = rand() % (work->config->spark_fall * 2) + work->config->spark_fall;
            work->spark_frame[i] = rand() % 3;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D26C[i]);
        }
        work->flare_level = 0x80;
        work->dome_level = 0xC0;
        work->wave_level = 0xFF;
        work->spark_level = 0x80;
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->dome_scale = 0x1000;
        work->base_scale = 0x1000;
        work->debris_frame = 0;
        work->wave_radius = work->config->size / 4;
        work->wave_height = work->config->wave_height;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, work->config->camera);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 9, &rot, work->config->camera);
    }
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->phase < 3) {
        setPolyG3(&disc);
        setRGB0(&disc, 0xF0, 0xF0, 0xF0);
        setRGB1(&disc, 0, 0, 0);
        setRGB2(&disc, 0, 0, 0);
        copyVector(&pos, &work->origin);
        pos.vy = 0;
        setVector(&rot, 0, 0, 0);
        setVector(&scale, work->base_scale, work->base_scale, work->base_scale);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage3(&rot, &work->base[15], &work->base[0], (PSXLONG *)&disc.x0,
                                    (PSXLONG *)&disc.x1, (PSXLONG *)&disc.x2, &p, &flag);
            } else {
                depth = RotAverage3(&rot, &work->base[i], &work->base[i + 1], (PSXLONG *)&disc.x0,
                                    (PSXLONG *)&disc.x1, (PSXLONG *)&disc.x2, &p, &flag);
            }
            depth += 0x1000;
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&disc, ot, (u16)depth, 2);
            }
        }
        if (work->base_scale < 0x32C8) {
            work->base_scale += work->config->dome_grow;
        }
        Model_SetSlotAmbient(slot ^ 1, work->ambient, work->ambient, work->ambient);
        if (work->ambient > 0x400) {
            work->ambient -= step << 6;
        }
    }
    if (work->elapsed < work->config->dome_start) {
        PopMatrix();
        work->elapsed += step;
        return 0;
    }

    if (work->phase == 2 && work->dome_level != 0) {
        if (work->dome_scale < 0x61A8) {
            work->dome_scale += work->config->grow;
        } else {
            work->dome_scale += work->config->grow_late;
        }
        setPolyG4(&g4);
        setPolyG3(&g3);
        setRGB0(&g4, work->dome_level, work->dome_level, 0);
        setRGB1(&g4, work->dome_level, work->dome_level, 0);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        copyVector(&pos, &work->origin);
        pos.vy = 0;
        setVector(&rot, 0, 0, 0);
        setVector(&scale, work->dome_scale, work->dome_scale, work->dome_scale);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        outer = work->outer;
        inner = work->inner;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&outer[15], &outer[0], &inner[15], &inner[0],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            } else {
                depth = RotAverage4(&outer[i], &outer[i + 1], &inner[i], &inner[i + 1],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)depth, 1);
            }
        }

        setRGB0(&g3, work->dome_level, work->dome_level, 0);
        setRGB1(&g3, work->dome_level, work->dome_level, 0);
        setRGB2(&g3, 0, 0, 0);
        setVector(&scale, work->dome_scale / 2, work->dome_scale / 2, work->dome_scale / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        outer = work->outer;
        inner = work->inner;
        cone = work->cone;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&outer[15], &outer[0], &inner[15], &inner[0],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            } else {
                depth = RotAverage4(&outer[i], &outer[i + 1], &inner[i], &inner[i + 1],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)depth, 1);
            }
            if (i == 15) {
                depth = RotAverage3(&outer[15], &outer[0], &cone[15], (PSXLONG *)&g3.x0,
                                    (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            } else {
                depth = RotAverage3(&outer[i], &outer[i + 1], &cone[i], (PSXLONG *)&g3.x0,
                                    (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)depth, 3);
            }
        }

        setRGB0(&g3, work->dome_level, work->dome_level, work->dome_level);
        setRGB1(&g3, work->dome_level, work->dome_level, work->dome_level);
        setRGB2(&g3, 0, 0, 0);
        setVector(&scale, work->dome_scale / 2, work->dome_scale / 4, work->dome_scale / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage3(&outer[15], &outer[0], &cone[15], (PSXLONG *)&g3.x0,
                                    (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            } else {
                depth = RotAverage3(&outer[i], &outer[i + 1], &cone[i], (PSXLONG *)&g3.x0,
                                    (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)depth, 3);
            }
        }

        setRGB0(&g4, work->dome_level, work->dome_level, work->dome_level);
        setRGB1(&g4, work->dome_level, work->dome_level, work->dome_level);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        setVector(&scale, work->dome_scale - 0xFF, work->dome_scale - 0xFF, work->dome_scale - 0xFF);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        outer = work->outer;
        inner = work->inner;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&outer[15], &outer[0], &inner[15], &inner[0],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            } else {
                depth = RotAverage4(&outer[i], &outer[i + 1], &inner[i], &inner[i + 1],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)depth, 1);
            }
        }

        setVector(&scale, (work->dome_scale - 0xFF) / 2, (work->dome_scale - 0xFF) / 2,
                  (work->dome_scale - 0xFF) / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        outer = work->outer;
        inner = work->inner;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&outer[15], &outer[0], &inner[15], &inner[0],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            } else {
                depth = RotAverage4(&outer[i], &outer[i + 1], &inner[i], &inner[i + 1],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)depth, 1);
            }
        }
        if (work->dome_level >= 0x20) {
            work->dome_level -= step * 7;
        } else {
            work->dome_level = 0;
        }
    }

    if (work->phase == 2 && work->debris_frame < 8) {
        SetPolyFT4(&debris);
        debris.tpage = work->texture[2] >> 16;
        debris.clut = work->texture[2];
        setRGB0(&debris, 0x80, 0x80, 0x80);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&quad[0], -work->config->debris_size, -work->config->debris_size, 0);
        setVector(&quad[1], work->config->debris_size, -work->config->debris_size, 0);
        setVector(&quad[2], -work->config->debris_size, work->config->debris_size, 0);
        setVector(&quad[3], work->config->debris_size, work->config->debris_size, 0);
        outer = work->debris;
        inner = work->debris_velocity;
        for (i = 0; i < work->config->debris_count; i++) {
            copyVector(&pos, &work->origin);
            pos.vy = 0;
            addVector(&pos, &outer[i]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            depth = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&debris.x0,
                                (PSXLONG *)&debris.x1, (PSXLONG *)&debris.x2, (PSXLONG *)&debris.x3, &p, &flag);
            setUV4(&debris, work->debris_frame * 32, 0x80, work->debris_frame * 32 + 0x1F, 0x80,
                   work->debris_frame * 32, 0x9F, work->debris_frame * 32 + 0x1F, 0x9F);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&debris, ot, (u16)depth, 1);
            }
            addVector(&outer[i], &inner[i]);
        }
        work->debris_frame++;
    }
    if (work->phase == 2) {
        Model_SetSlotPlaybackDirection(slot ^ 1, 0);
    }

    if (work->elapsed < work->config->end || work->phase < 4) {
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setPolyFT4(&flare);
        setRGB0(&flare, work->flare_level, work->flare_level, work->flare_level);
        flare.tpage = work->texture[1] >> 16;
        flare.clut = work->texture[1];
        GsSetLsMatrix(&light);
        RotTrans(&work->origin, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setVector(&quad[0], -work->config->size, -work->config->size, 0);
        setVector(&quad[1], work->config->size, -work->config->size, 0);
        setVector(&quad[2], -work->config->size, work->config->size, 0);
        setVector(&quad[3], work->config->size, work->config->size, 0);
        depth = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&flare.x0,
                            (PSXLONG *)&flare.x1, (PSXLONG *)&flare.x2, (PSXLONG *)&flare.x3, &p, &flag);
        setUV4(&flare, 0, 0, 0x7F, 0, 0, 0x7F, 0x7F, 0x7F);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&flare, ot, (u16)depth, 1);
        }
        if (work->origin.vy < -work->config->size) {
            work->origin.vy += work->config->rise;
            if (work->origin.vy > -work->config->size && work->phase == 0) {
                work->origin.vy = -work->config->size;
                setVector(&scale, 0x1000, 1, 0x1000);
                Model_UpdateCoordinateUnitMatrix((struct _GsCOORDUNIT *)Model_GetCurrentDataEntry(slot ^ 1), &scale);
                work->phase = 1;
            }
        }
    }

    if (work->phase == 2 && work->dome_level == 0 && work->elapsed >= work->config->wave_start) {
        outer = work->outer;
        inner = work->inner;
        for (i = 0; i < 16; outer++, inner++, i++) {
            setVector(outer, work->wave_radius * ccos(i * 256 % 4096) / 4096, 0,
                      work->wave_radius * csin(i * 256 % 4096) / 4096);
            setVector(inner, work->wave_radius * ccos(i * 256 % 4096) / 4096, -work->wave_height,
                      work->wave_radius * csin(i * 256 % 4096) / 4096);
        }
        Model_SetSlotAmbient(slot ^ 1, 0x800, 0x800, 0x800);
        work->phase = 3;
    }

    if (work->phase >= 3 && work->wave_level != 0) {
        setPolyG4(&g4);
        setRGB0(&g4, work->wave_level, work->wave_level, work->wave_level);
        setRGB1(&g4, work->wave_level, work->wave_level, work->wave_level);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        copyVector(&pos, &work->origin);
        pos.vy = 0;
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        outer = work->outer;
        inner = work->inner;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                depth = RotAverage4(&outer[15], &outer[0], &inner[15], &inner[0],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            } else {
                depth = RotAverage4(&outer[i], &outer[i + 1], &inner[i], &inner[i + 1],
                                    (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                                    (PSXLONG *)&g4.x3, &p, &flag);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)depth, 1);
            }
        }
        if (work->phase == 3) {
            work->dome_scale = 1;
            work->wave_radius += work->config->wave_step;
            work->wave_height -= work->config->wave_step;
            if (work->wave_radius > work->config->size) {
                work->wave_radius = work->config->size + 0x20;
                Model_SetSlotPlaybackDirection(slot ^ 1, 1);
            }
            if (work->wave_height < -0x6D6) {
                work->wave_height = -0x6D6;
                if (work->phase == 3) {
                    work->phase = 4;
                }
            }
        } else if (work->phase == 4 && work->elapsed >= work->config->end) {
            if (work->config->size / 4 < work->wave_radius) {
                work->wave_radius -= work->config->wave_step * 2;
            } else {
                work->wave_radius -= work->config->wave_step / 2;
                work->phase = 5;
            }
            if (work->wave_radius < 0) {
                work->wave_radius = 0;
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&outer[i], work->wave_radius * ccos(i * 256 % 4096) / 4096, 0,
                      work->wave_radius * csin(i * 256 % 4096) / 4096);
            setVector(&inner[i], work->wave_radius * ccos(i * 256 % 4096) / 4096, work->wave_height,
                      work->wave_radius * csin(i * 256 % 4096) / 4096);
        }
    }

    if (work->phase >= 5 && work->wave_level != 0) {
        if (work->wave_level > 10) {
            work->wave_level -= 7;
        } else {
            work->wave_level = 0;
        }
    }
    if (work->phase >= 5) {
        work->dome_scale += 0x40;
        if (work->dome_scale > 0x1000) {
            work->dome_scale = 0x1000;
        }
        setVector(&scale, 0x1000, work->dome_scale, 0x1000);
        Model_UpdateCoordinateUnitMatrix((struct _GsCOORDUNIT *)Model_GetCurrentDataEntry(slot ^ 1), &scale);
    }
    if (work->phase >= 5 && work->spark_level != 0) {
        setPolyFT4(&spark);
        spark.tpage = work->texture[0] >> 16;
        spark.clut = work->texture[0];
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        copyVector(&pos, &work->origin);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setRGB0(&spark, work->spark_level, work->spark_level, work->spark_level);
        for (i = 0; i < 16; i++) {
            setVector(&quad[0], work->sparks[i].vx - work->config->spark_size,
                      work->sparks[i].vy - work->config->spark_size, work->sparks[i].vz);
            setVector(&quad[1], work->sparks[i].vx + work->config->spark_size,
                      work->sparks[i].vy - work->config->spark_size, work->sparks[i].vz);
            setVector(&quad[2], work->sparks[i].vx - work->config->spark_size,
                      work->sparks[i].vy + work->config->spark_size, work->sparks[i].vz);
            setVector(&quad[3], work->sparks[i].vx + work->config->spark_size,
                      work->sparks[i].vy + work->config->spark_size, work->sparks[i].vz);
            depth = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&spark.x0,
                                (PSXLONG *)&spark.x1, (PSXLONG *)&spark.x2, (PSXLONG *)&spark.x3, &p, &flag);
            setUV4(&spark, work->spark_frame[i] * 32 + 0x80, 0x60, work->spark_frame[i] * 32 + 0x9F, 0x60,
                   work->spark_frame[i] * 32 + 0x80, 0x7F, work->spark_frame[i] * 32 + 0x9F, 0x7F);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&spark, ot, (u16)depth, 1);
            }
            work->sparks[i].vy -= step * work->spark_fall[i];
            work->spark_frame[i]++;
            work->spark_frame[i] %= 3;
        }
        if (work->spark_level >= 10) {
            work->spark_level -= 7;
        } else {
            work->spark_level = 0;
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 1) {
        work->phase = 2;
        return 1;
    }
    if (work->phase >= 5 && work->dome_scale == 0x1000) {
        return 2;
    }
    return 0;
}
