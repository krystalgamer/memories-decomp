#include "../../types.h"
#include "variant780_entry.h"

/* Header 780's entry: the opposing slot is held still while a glow grows
 * at a model part and later spins a fan of rays. Bands of Gouraud rings
 * climb around the held slot, then a tiled disc and a rising cylinder
 * flare under it and sparks drift upwards; the slot is released near the
 * end while the ambient light dims and recovers. */

static const VECTOR D_8013CDE0 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)
#define SIGNED_BY(s, v) ((s) < 0 ? -(v) : (v))

s32 func_8013B004(u8 *context, s32 command)
{
    Model780State *work;
    MATRIX light;
    MATRIX m;
    POLY_G4 beam;
    POLY_FT4 grid;
    POLY_G4 core;
    POLY_FT4 spark;
    POLY_FT4 glow;
    POLY_G3 fan;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[16];
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 step;
    GsOT *ot;
    s32 i, j, up, n;
    s16 size;
    SVECTOR *ring;
    SVECTOR *outer;
    SVECTOR *inner;
    s16 sign;
    s32 down;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CDE0;
    work = (Model780State *)context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        ring = work->ring;
        outer = work->outer;
        inner = work->inner;
        work->config = &D_8013CE44[command];
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&pos);
        Model_SetSlotPlaybackDirection(slot ^ 1, 0);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&work->centre);
        if (pos.vz < pos.vx) {
            size = pos.vx;
        } else {
            size = pos.vz;
        }
        work->radius = size / 2 + 200;
        if (work->radius > 0x200) {
            work->radius = 0x200;
        }
        for (i = 0; i < 16; i++) {
            /* The angle is spelled out per call and the band pointers step
             * before the cylinder point, which fixes the register use. */
            setVector(ring, work->radius * (ccos(i * 256 % 4096) << 1) / 4096, 0,
                      work->radius * (csin(i * 256 % 4096) << 1) / 4096);
            setVector(outer, (work->radius + 32) * (ccos(i * 256 % 4096) << 1) / 4096, 0,
                      (work->radius + 32) * (csin(i * 256 % 4096) << 1) / 4096);
            setVector(inner, (work->radius - 32) * (ccos(i * 256 % 4096) << 1) / 4096, 0,
                      (work->radius - 32) * (csin(i * 256 % 4096) << 1) / 4096);
            ring++;
            outer++;
            inner++;
            setVector(&work->core[i], work->radius * ccos(i * 256 % 4096) / 4096, 64,
                      work->radius * csin(i * 256 % 4096) / 4096);
        }
        for (i = 0; i < work->config->count; i++) {
            work->column[i] = 1;
            work->scale[i] = 0x1000;
        }
        for (i = 0; i < work->config->spark_count; i++) {
            setVector(&work->sparks[i], RANDOM_SPREAD(work->radius / 2), 0, RANDOM_SPREAD(work->radius / 2));
            work->spark[i] = 120;
        }
        for (i = 0; i < work->config->ray_count; i++) {
            setVector(&work->rays[i], RANDOM_SPREAD(work->config->ray_spread),
                      RANDOM_SPREAD(work->config->ray_spread), RANDOM_SPREAD(work->config->ray_spread));
        }
        work->burst_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->burst_clut = 0x3D28;
#else
        work->burst_clut = 0x3D20;
#endif
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CDF0[i]);
        }
        work->beam = 1;
        work->rise = 0;
        work->burst = 0;
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->active = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 3;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();

    if (work->beam != 0) {
        setPolyFT4(&glow);
        if (work->elapsed >= work->config->burst_time) {
            setRGB0(&glow, work->beam * 2, work->beam * 2, work->beam * 2);
        } else {
            setRGB0(&glow, work->beam * 2, work->beam * 2, work->beam);
        }
        glow.tpage = work->texture[1] >> 16;
        glow.clut = work->texture[1];
        setUV4(&glow, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
        setVector(&quad[0], -work->config->beam_size, -work->config->beam_size, 0);
        setVector(&quad[1], work->config->beam_size, -work->config->beam_size, 0);
        setVector(&quad[2], -work->config->beam_size, work->config->beam_size, 0);
        setVector(&quad[3], work->config->beam_size, work->config->beam_size, 0);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1,
                          (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
        }
        if (work->elapsed >= work->config->burst_time) {
            setPolyG3(&fan);
            setVector(&rot, work->elapsed * work->config->spin, work->elapsed * work->config->spin,
                      work->elapsed * work->config->spin);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setRGB0(&fan, 0, 0, 0);
            setRGB1(&fan, work->beam, work->beam, work->beam);
            setRGB2(&fan, work->beam, work->beam, work->beam);
            setVector(&quad[0], -work->config->fan_size, -work->config->fan_size, 0);
            setVector(&quad[1], work->config->fan_size, -work->config->fan_size, 0);
            for (i = 0; i < work->config->ray_count; i++) {
                otz = RotAverage3(&work->rays[i], &quad[0], &quad[1], (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1,
                                  (PSXLONG *)&fan.x2, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)otz, 1);
                }
            }
        }
        if (work->phase < 2) {
            work->beam += 8;
            if (work->beam > 120) {
                work->beam = 120;
            }
        } else {
            work->beam -= 8;
            if (work->beam < 12) {
                work->beam = 0;
            }
        }
    }

    if (work->elapsed < work->config->burst_time) {
        for (i = 0; i < work->active; i++) {
            ring = work->ring;
            outer = work->outer;
            inner = work->inner;
            setPolyG4(&beam);
            setVector(&pos, work->centre.vx, work->centre.vy, work->centre.vz);
            GsSetLsMatrix(&light);
            setRGB0(&beam, work->column[i] * 2, work->column[i] * 2, work->column[i]);
            setRGB1(&beam, work->column[i] * 2, work->column[i] * 2, work->column[i]);
            setRGB2(&beam, 0, 0, 0);
            setRGB3(&beam, 0, 0, 0);
            if (i % 2 == 0) {
                pos.vy += -work->config->rise_step * i;
                if (pos.vy > 0) {
                    pos.vy = 0;
                }
            } else if (i / 2 * 2 == i - 1) {
                pos.vy += work->config->rise_step * i;
                if (pos.vy > 0) {
                    pos.vy = 0;
                }
            }
            setVector(&scale, work->scale[i], work->scale[i], work->scale[i]);
            setVector(&rot, 0, 0, 0);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            if (work->scale[i] != 0 && work->column[i] != 0) {
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&ring[15], &ring[0], &outer[15], &outer[0],
                                          (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2,
                                          (PSXLONG *)&beam.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)otz, 1);
                        }
                        otz = RotAverage4(&ring[15], &ring[0], &inner[15], &inner[0],
                                          (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2,
                                          (PSXLONG *)&beam.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)otz, 1);
                        }
                    } else {
                        otz = RotAverage4(&ring[j], &ring[j + 1], &outer[j], &outer[j + 1],
                                          (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2,
                                          (PSXLONG *)&beam.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)otz, 1);
                        }
                        otz = RotAverage4(&ring[j], &ring[j + 1], &inner[j], &inner[j + 1],
                                          (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2,
                                          (PSXLONG *)&beam.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)otz, 1);
                        }
                    }
                }
                if (work->elapsed < work->config->fade_time) {
                    work->column[i] += 15;
                    if (work->column[i] > 0x70) {
                        work->column[i] = 0x70;
                    }
                    work->scale[i] -= 0x200;
                    if (work->scale[i] < 0x400) {
                        work->scale[i] = 0x400;
                    }
                } else {
                    work->scale[i] -= 0x80;
                    if (work->scale[i] < 0x200) {
                        work->scale[i] = 0x80;
                    }
                    work->column[i] -= 15;
                    if (work->column[i] < 16) {
                        work->column[i] = 0;
                    }
                }
            }
        }
        if (++work->active > work->config->count) {
            work->active = work->config->count;
        }
    }

    if (work->elapsed >= work->config->fade_time) {
        setPolyFT4(&grid);
        setPolyG4(&core);
        setRGB0(&core, 0, 0, 0);
        setRGB1(&core, 0, 0, 0);
        if (work->burst < 0x7A) {
            setRGB0(&grid, work->burst * 2, work->burst, work->burst);
            setRGB2(&core, work->burst * 2, work->burst, work->burst);
            setRGB3(&core, work->burst * 2, work->burst, work->burst);
        } else {
            setRGB0(&grid, 0xFF, work->burst, work->burst);
            setRGB2(&core, 0xFF, work->burst, work->burst);
            setRGB3(&core, 0xFF, work->burst, work->burst);
        }
        grid.tpage = work->burst_tpage;
        grid.clut = work->burst_clut;
        setVector(&rot, 0, 0, 0);
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&pos, work->centre.vx, work->centre.vy, work->centre.vz);
        pos.vy = 0;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        /* The disc is laid out as four quadrants of 4x4 tiles. */
        sign = 1;
        for (n = 0; n < 4; n++) {
            sign *= -1;
            for (i = 0; i < 4; i++) {
                /* The near edge's signed step is kept in two counters. */
                for (j = 0, down = -1, up = 1; j < 4; down--, j++, up++) {
                    setVector(&quad[0], sign * -(work->radius / 4) * (i + 1), 0,
                              -(work->radius / 4) * (n - 2 < 0 ? down : up));
                    setVector(&quad[1], sign * -(work->radius / 4) * i, 0,
                              -(work->radius / 4) * (n - 2 < 0 ? down : up));
                    setVector(&quad[2], sign * -(work->radius / 4) * (i + 1), 0,
                              -(work->radius / 4) * SIGNED_BY(n - 2, j));
                    setVector(&quad[3], sign * -(work->radius / 4) * i, 0,
                              -(work->radius / 4) * SIGNED_BY(n - 2, j));
                    setUV4(&grid, (3 - i) * 16, (3 - j) * 16, (3 - i) * 16 + 15, (3 - j) * 16, (3 - i) * 16,
                           (3 - j) * 16 + 15, (3 - i) * 16 + 15, (3 - j) * 16 + 15);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&grid.x0,
                                      (PSXLONG *)&grid.x1, (PSXLONG *)&grid.x2, (PSXLONG *)&grid.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&grid, ot, (u16)otz, 1);
                    }
                }
            }
        }
        for (i = 0; i < 16; i++) {
            copyVector(&quad[i], &work->core[i]);
            quad[i].vy = -work->rise;
        }
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&quad[15], &quad[0], &work->core[15], &work->core[0], (PSXLONG *)&core.x0,
                                  (PSXLONG *)&core.x1, (PSXLONG *)&core.x2, (PSXLONG *)&core.x3, &p, &flag);
            } else {
                otz = RotAverage4(&quad[i], &quad[i + 1], &work->core[i], &work->core[i + 1], (PSXLONG *)&core.x0,
                                  (PSXLONG *)&core.x1, (PSXLONG *)&core.x2, (PSXLONG *)&core.x3, &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&core, ot, (u16)otz, 1);
            }
        }
        work->rise += work->config->rise_speed;
        if (work->rise > 350) {
            work->rise = 350;
        }
        if (work->phase < 2) {
            if (work->burst < 0xF0) {
                work->burst += 15;
            } else {
                work->burst = 0xFF;
            }
        } else if (work->burst >= 16) {
            work->burst -= 15;
        } else {
            work->burst = 0;
            if (work->phase == 3) {
                work->phase = 4;
            }
        }
    }

    if (work->elapsed >= work->config->burst_time) {
        if (work->phase == 0) {
            work->active = 0;
            work->phase = 1;
        } else if (++work->counter % 4 == 0) {
            work->active += step;
            if (work->active > work->config->spark_count) {
                work->active = work->config->spark_count;
            }
        }
        setPolyFT4(&spark);
        for (i = 0; i < work->active; i++) {
            if (work->spark[i] != 0) {
                for (j = 0; j < 4; j++) {
                    spark.tpage = work->texture[0] >> 16;
                    spark.clut = work->texture[0];
                    setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                    setUV4(&spark, (work->elapsed + i) % 3 * 32, 0x40, (work->elapsed + i) % 3 * 32 + 0x1F, 0x40,
                           (work->elapsed + i) % 3 * 32, 0x5F, (work->elapsed + i) % 3 * 32 + 0x1F, 0x5F);
                    setRGB0(&spark, work->spark[i] * 2 / (j + 1), work->spark[i] * 2 / (j + 1),
                            work->spark[i] / (j + 1));
                    setVector(&rot, 0, 0, 0);
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    setVector(&pos, work->centre.vx, work->centre.vy, work->centre.vz);
                    pos.vy = 0;
                    addVector(&pos, &work->sparks[i]);
                    pos.vy += work->config->spark_spacing * j;
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&spark.x0,
                                      (PSXLONG *)&spark.x1, (PSXLONG *)&spark.x2, (PSXLONG *)&spark.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&spark, ot, (u16)otz, 1);
                    }
                    spark.tpage = work->texture[1] >> 16;
                    spark.clut = work->texture[1];
                    setVector(&quad[0], -work->config->spark_size * 2, -work->config->spark_size * 2, 0);
                    setVector(&quad[1], work->config->spark_size * 2, -work->config->spark_size * 2, 0);
                    setVector(&quad[2], -work->config->spark_size * 2, work->config->spark_size * 2, 0);
                    setVector(&quad[3], work->config->spark_size * 2, work->config->spark_size * 2, 0);
                    setUV4(&spark, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&spark.x0,
                                      (PSXLONG *)&spark.x1, (PSXLONG *)&spark.x2, (PSXLONG *)&spark.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&spark, ot, (u16)otz, 1);
                    }
                }
                work->sparks[i].vy -= work->config->spark_rise;
                if (work->sparks[i].vy < -350) {
                    if (work->spark[i] >= 13) {
                        work->spark[i] -= 10;
                    } else {
                        work->spark[i] = 0;
                    }
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->elapsed >= work->config->release_time && work->elapsed < work->config->end_time) {
        Model_SetSlotPlaybackDirection(slot ^ 1, 1);
        return 4;
    }
    if (work->elapsed >= work->config->end_time && work->phase == 1) {
        work->phase = 2;
        return 1;
    }
    return work->phase == 4 ? 2 : 0;
}
