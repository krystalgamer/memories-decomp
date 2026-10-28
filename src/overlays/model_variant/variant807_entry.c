#include "../../types.h"
#include "variant807_entry.h"

/* Header 807's entry: while the ambient light dims, a glow and a halo of
 * triangles gather on a model part and orbiting sprites spiral out of it.
 * The light then collapses into a spinning ring on the opposing slot that
 * stretches upwards, fans out a rim and throws off sparks that shed tumbling
 * fragments; a flash ends it and a fading core glow remains while the light
 * comes back. */

static const VECTOR D_8013DB98 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model807State *work;
    MATRIX light;
    MATRIX m;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR origin;
    SVECTOR quad[4];
    POLY_FT4 ft4;
    POLY_F4 f4;
    POLY_G3 g3;
    POLY_G4 g4;
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 step;
    GsOT *ot;
    void *entry;
    s32 slot;
    s32 direction;
    s32 i;
    /* J doubles as the first random spread, the ring angle and the orbiter
     * frame, and B as the fragment frame; sharing them fixes the register
     * allocation. */
    s32 j;
    s32 b, c;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013DB98;
    memset(&origin, 0, 8);
    work = (Model807State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013DBE0[command];
        for (i = 0; i < work->config->count; i++) {
            j = (rand() - rand()) % 4096;
            b = (rand() - rand()) % 4096;
            c = direction * (rand() % 4096);
            setVector(&work->points[i], work->config->spread * j / 4096,
                      work->config->spread * b / 4096, work->config->spread * c / 4096);
            setVector(&work->offsets[i], 0, 0, 0);
            setVector(&work->velocities[i], work->config->speed * j / 4096,
                      work->config->speed * b / 4096, work->config->speed * c / 4096);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            for (j = 0; j < 16; j++) {
                setVector(&work->fragments[i][j], 0, 0, 0);
                setVector(&work->fragment_velocities[i][j], RANDOM_SPREAD(work->config->fragment_spread),
                          RANDOM_SPREAD(work->config->fragment_spread),
                          RANDOM_SPREAD(work->config->fragment_spread));
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->ring[i][0], 0, 0, 0);
            setVector(&work->ring[i][1], 0, 0, 0);
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DBA8[i]);
        }
        setVector(&work->center, 0, 0, 0);
        work->glow_r = work->config->r;
        work->glow_g = work->config->g;
        work->glow_b = work->config->b;
        work->flash = 0xFF;
        work->halo_r = work->config->halo_r;
        work->halo_g = work->config->halo_g;
        work->halo_b = work->config->halo_b;
        work->height_scale = 0x200;
        work->width_scale = 0x80;
        work->level = 0x800;
        work->phase = 0;
        work->spark_count = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_end) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 16);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 16);
    }
    if (work->phase == 0) {
        if (work->level > -0x200) {
            work->level -= step << 6;
        } else {
            work->phase = 1;
            work->level = -0x200;
        }
    } else if (work->phase == 5) {
        if (work->level < 0x800) {
            work->level += step << 6;
        } else {
            work->phase = 6;
            work->level = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }
    if (work->elapsed >= work->config->collapse && work->phase == 2) {
        work->phase = 3;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->flash != 0) {
        setPolyF4(&f4);
        setRGB0(&f4, work->flash, work->flash, work->flash);
#ifdef VERSION_FRENCH
        setXY4(&f4, 0, 0, 320, 0, 0, 256, 320, 256);
#else
        setXY4(&f4, 0, 0, 320, 0, 0, 240, 320, 240);
#endif
        Graphics_SubmitDrawModePacket((u32 *)&f4, ot, 0, 1);
        if (work->flash >= 16) {
            work->flash -= work->config->core_fade;
        } else {
            work->flash = 0;
        }
    }

    if (((*(u32 *)&work->glow_r & 0xFFFFFF) || (*(u32 *)&work->halo_r & 0xFFFFFF)) && work->phase < 2) {
        setPolyFT4(&ft4);
        setRGB0(&ft4, work->glow_r, work->glow_g, work->glow_b);
        ft4.tpage = work->texture[1] >> 16;
        ft4.clut = work->texture[1];
        entry = Model_GetSlotDataEntry(slot, work->config->part);
        GsGetLwUnit(entry, &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
        setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
        setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
        setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        saved = m;
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag) - 16;
        setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
        }

        setPolyG3(&g3);
        setRGB0(&g3, 0, 0, 0);
        setRGB1(&g3, work->halo_r, work->halo_g, work->halo_b);
        setRGB2(&g3, work->halo_r, work->halo_g, work->halo_b);
        setVector(&quad[0], -work->config->halo_size, -work->config->halo_size, 0);
        setVector(&quad[1], work->config->halo_size, -work->config->halo_size, 0);
        for (i = 0; i < work->config->count; i++) {
            otz = RotAverage3(&work->points[i], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                              (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
        }
        setVector(&scale, 0x800, 0x800, 0x800);
        GsSetLsMatrix(&light);
        m = saved;
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setRGB0(&g3, 0, 0, 0);
        setRGB1(&g3, work->glow_r, work->glow_g, work->glow_b);
        setRGB2(&g3, work->glow_r, work->glow_g, work->glow_b);
        for (i = 0; i < work->config->count; i++) {
            otz = RotAverage3(&work->points[i], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                              (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
        }
        setVector(&scale, 0x1000, 0x1000, 0x1000);

        ft4.tpage = work->texture[0] >> 16;
        ft4.clut = work->texture[0];
        setVector(&quad[0], -work->config->orbiter_size, -work->config->orbiter_size, 0);
        setVector(&quad[1], work->config->orbiter_size, -work->config->orbiter_size, 0);
        setVector(&quad[2], -work->config->orbiter_size, work->config->orbiter_size, 0);
        setVector(&quad[3], work->config->orbiter_size, work->config->orbiter_size, 0);
        for (i = 0; i < work->config->count; i++) {
            GsSetLsMatrix(&light);
            GsGetLwUnit(entry, &m);
            setVector(&pos, m.t[0], m.t[1], m.t[2]);
            addVector(&pos, &work->offsets[i]);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            j = rand() % 3;
            setUV4(&ft4, j * 32 + 128, 0, j * 32 + 159, 0, j * 32 + 128, 0x1F,
                   j * 32 + 159, 0x1F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
            addVector(&work->offsets[i], &work->velocities[i]);
        }
        if (work->phase == 1) {
            if (work->glow_r >= 5) work->glow_r -= work->config->halo_fade;
            else work->glow_r = 0;
            if (work->glow_g >= 5) work->glow_g -= work->config->halo_fade;
            else work->glow_g = 0;
            if (work->glow_b >= 5) work->glow_b -= work->config->halo_fade;
            else work->glow_b = 0;
            if (work->halo_r >= 5) work->halo_r -= work->config->halo_fade;
            else work->halo_r = 0;
            if (work->halo_g >= 5) work->halo_g -= work->config->halo_fade;
            else work->halo_g = 0;
            if (work->halo_b >= 5) work->halo_b -= work->config->halo_fade;
            else work->halo_b = 0;
        }
    } else if (work->phase == 1) {
        work->phase = 2;
        Model_CopySlotPosition(slot ^ 1, (u16 *)&work->center);
        for (i = 0; i < 16; i++) {
            j = i * 256 % 4096;
            setVector(&work->ring[i][0], work->config->radius * ccos(j) / 4096, work->config->height >> 2,
                      work->config->radius * csin(j) / 4096);
            setVector(&work->ring[i][1], work->config->radius * ccos(j) / 4096, 0,
                      work->config->radius * csin(j) / 4096);
            setVector(&work->rim[i], work->config->radius * (ccos(j) << 1) / 4096, 0,
                      work->config->radius * (csin(j) << 1) / 4096);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->sparks[i], work->config->radius * 3 * ((rand() - rand()) % 4096) / 4096,
                      work->config->radius * 3 * ((rand() - rand()) % 4096) / 4096,
                      work->config->radius * 3 * ((rand() - rand()) % 4096) / 4096);
            work->spark_r[i] = work->config->r;
            work->spark_g[i] = work->config->g;
            work->spark_b[i] = work->config->b;
        }
        work->glow_r = 0;
        work->glow_g = 0;
        work->glow_b = 0;
        work->halo_r = 0;
        work->halo_g = 0;
        work->halo_b = 0;
    }

    if (work->phase == 2 || work->phase == 3) {
        setPolyG4(&g4);
        setRGB0(&g4, work->glow_r, work->glow_g, work->glow_b);
        setRGB1(&g4, work->glow_r, work->glow_g, work->glow_b);
        setRGB2(&g4, work->halo_r, work->halo_g, work->halo_b);
        setRGB3(&g4, work->halo_r, work->halo_g, work->halo_b);
        setVector(&scale, work->width_scale, work->height_scale, work->width_scale);
        setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
        pos.vy = -work->config->height;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&work->ring[15][0], &work->ring[0][0], &work->ring[15][1],
                                  &work->ring[0][1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
            } else {
                otz = RotAverage4(&work->ring[i][0], &work->ring[i + 1][0], &work->ring[i][1],
                                  &work->ring[i + 1][1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }
        if (work->height_scale < 0x4000) {
            work->height_scale += 0x200;
        } else {
            work->height_scale = 0x4000;
            setPolyG3(&g3);
            setRGB0(&g3, work->halo_r, work->halo_g, work->halo_b);
            setRGB1(&g3, 0, 0, 0);
            setRGB2(&g3, 0, 0, 0);
            setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
            pos.vy = 0;
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < 16; i++) {
                if (i == 15) {
                    otz = RotAverage3(&origin, &work->rim[15], &work->rim[0], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                } else {
                    otz = RotAverage3(&origin, &work->rim[i], &work->rim[i + 1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                }
            }
            setVector(&scale, 0xC00, 0xC00, 0xC00);
            GsSetLsMatrix(&light);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (i = 0; i < 16; i++) {
                if (i == 15) {
                    otz = RotAverage3(&origin, &work->rim[15], &work->rim[0], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                } else {
                    otz = RotAverage3(&origin, &work->rim[i], &work->rim[i + 1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                }
            }
        }
        if (work->phase == 3 && work->width_scale < 0x6000) {
            for (i = 0; i < work->spark_count; i++) {
                if (work->spark_r[i] != 0 || work->spark_g[i] != 0 || work->spark_b[i] != 0) {
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    setPolyFT4(&ft4);
                    setRGB0(&ft4, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
                    ft4.tpage = work->texture[1] >> 16;
                    ft4.clut = work->texture[1];
                    setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
                    addVector(&pos, &work->sparks[i]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    for (j = 0; j < work->config->fragment_count; j++) {
                        setPolyFT4(&ft4);
                        setRGB0(&ft4, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
                        ft4.tpage = work->texture[0] >> 16;
                        ft4.clut = work->texture[0];
                        setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
                        addVector(&pos, &work->sparks[i]);
                        addVector(&pos, &work->fragments[i][j]);
                        GsSetLsMatrix(&light);
                        RotTrans(&pos, (VECTOR *)m.t, &flag);
                        RotMatrix(&rot, &m);
                        ScaleMatrix(&m, &scale);
                        GsSetLsMatrix(&m);
                        setVector(&quad[0], -work->config->fragment_size, -work->config->fragment_size, 0);
                        setVector(&quad[1], work->config->fragment_size, -work->config->fragment_size, 0);
                        setVector(&quad[2], -work->config->fragment_size, work->config->fragment_size, 0);
                        setVector(&quad[3], work->config->fragment_size, work->config->fragment_size, 0);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                        b = rand() % 3;
                        setUV4(&ft4, b * 32 + 128, 0, b * 32 + 159, 0, b * 32 + 128, 0x1F,
                               b * 32 + 159, 0x1F);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                        }
                        addVector(&work->fragments[i][j], &work->fragment_velocities[i][j]);
                    }
                    if (work->spark_r[i] >= 16) work->spark_r[i] -= work->config->spark_fade;
                    else work->spark_r[i] = 0;
                    if (work->spark_g[i] >= 16) work->spark_g[i] -= work->config->spark_fade;
                    else work->spark_g[i] = 0;
                    if (work->spark_b[i] >= 16) work->spark_b[i] -= work->config->spark_fade;
                    else work->spark_b[i] = 0;
                }
            }
            work->spark_count += step;
            if (work->spark_count > 32) {
                work->spark_count = 32;
            }
            work->width_scale += 0x400;
        } else if (work->width_scale >= 0x6000) {
            for (i = 0; i < work->spark_count; i++) {
                if (work->spark_r[i] != 0 || work->spark_g[i] != 0 || work->spark_b[i] != 0) {
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    setPolyFT4(&ft4);
                    setRGB0(&ft4, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
                    ft4.tpage = work->texture[1] >> 16;
                    ft4.clut = work->texture[1];
                    setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
                    addVector(&pos, &work->sparks[i]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                    setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                    setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    for (j = 0; j < work->config->fragment_count; j++) {
                        setPolyFT4(&ft4);
                        setRGB0(&ft4, work->spark_r[i], work->spark_g[i], work->spark_b[i]);
                        ft4.tpage = work->texture[0] >> 16;
                        ft4.clut = work->texture[0];
                        setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
                        addVector(&pos, &work->sparks[i]);
                        addVector(&pos, &work->fragments[i][j]);
                        GsSetLsMatrix(&light);
                        RotTrans(&pos, (VECTOR *)m.t, &flag);
                        RotMatrix(&rot, &m);
                        ScaleMatrix(&m, &scale);
                        GsSetLsMatrix(&m);
                        setVector(&quad[0], -work->config->fragment_size, -work->config->fragment_size, 0);
                        setVector(&quad[1], work->config->fragment_size, -work->config->fragment_size, 0);
                        setVector(&quad[2], -work->config->fragment_size, work->config->fragment_size, 0);
                        setVector(&quad[3], work->config->fragment_size, work->config->fragment_size, 0);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                        b = rand() % 3;
                        setUV4(&ft4, b * 32 + 128, 0, b * 32 + 159, 0, b * 32 + 128, 0x1F,
                               b * 32 + 159, 0x1F);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                        }
                        addVector(&work->fragments[i][j], &work->fragment_velocities[i][j]);
                    }
                    if (work->spark_r[i] >= 16) work->spark_r[i] -= work->config->spark_fade;
                    else work->spark_r[i] = 0;
                    if (work->spark_g[i] >= 16) work->spark_g[i] -= work->config->spark_fade;
                    else work->spark_g[i] = 0;
                    if (work->spark_b[i] >= 16) work->spark_b[i] -= work->config->spark_fade;
                    else work->spark_b[i] = 0;
                }
            }
            work->spark_count += step;
            if (work->spark_count > 32) {
                work->spark_count = 32;
            }
            work->flash = 0xFF;
            work->phase = 4;
        }
        if (work->glow_r < work->config->halo_r - 15) work->glow_r += work->config->glow_fade;
        else work->glow_r = work->config->halo_r;
        if (work->glow_g < work->config->halo_g - 15) work->glow_g += work->config->glow_fade;
        else work->glow_g = work->config->halo_g;
        if (work->glow_b < work->config->halo_b - 15) work->glow_b += work->config->glow_fade;
        else work->glow_b = work->config->halo_b;
        if (work->halo_r < work->config->halo_r - 15) work->halo_r += work->config->halo_fade;
        else work->halo_r = work->config->halo_r;
        if (work->halo_g < work->config->halo_g - 15) work->halo_g += work->config->halo_fade;
        else work->halo_g = work->config->halo_g;
        if (work->halo_b < work->config->halo_b - 15) work->halo_b += work->config->halo_fade;
        else work->halo_b = work->config->halo_b;
    }

    if (work->phase >= 5) {
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setPolyFT4(&ft4);
        setRGB0(&ft4, work->glow_r, work->glow_g, work->glow_b);
        ft4.tpage = work->texture[1] >> 16;
        ft4.clut = work->texture[1];
        setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setVector(&quad[0], -work->config->core_size, -work->config->core_size, 0);
        setVector(&quad[1], work->config->core_size, -work->config->core_size, 0);
        setVector(&quad[2], -work->config->core_size, work->config->core_size, 0);
        setVector(&quad[3], work->config->core_size, work->config->core_size, 0);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
        setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
        }
        for (j = 0; j < work->config->fragment_count; j++) {
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->glow_r, work->glow_g, work->glow_b);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setVector(&pos, work->center.vx, work->center.vy, work->center.vz);
            addVector(&pos, &work->fragments[15][j]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->fragment_size, -work->config->fragment_size, 0);
            setVector(&quad[1], work->config->fragment_size, -work->config->fragment_size, 0);
            setVector(&quad[2], -work->config->fragment_size, work->config->fragment_size, 0);
            setVector(&quad[3], work->config->fragment_size, work->config->fragment_size, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            b = rand() % 3;
            setUV4(&ft4, b * 32 + 128, 0, b * 32 + 159, 0, b * 32 + 128, 0x1F,
                   b * 32 + 159, 0x1F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
            addVector(&work->fragments[15][j], &work->fragment_velocities[15][j]);
        }
        if (work->glow_r >= 16) work->glow_r -= work->config->core_fade;
        else work->glow_r = 0;
        if (work->glow_g >= 16) work->glow_g -= work->config->core_fade;
        else work->glow_g = 0;
        if (work->glow_b >= 16) work->glow_b -= work->config->core_fade;
        else work->glow_b = 0;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 6 && work->flash == 0) {
        return 2;
    }
    if (work->phase == 4) {
        work->phase = 5;
        work->glow_r = work->config->r;
        work->glow_g = work->config->g;
        work->glow_b = work->config->b;
        return 1;
    }
    if (work->phase == 3) {
        return 4;
    }
    return 0;
}
