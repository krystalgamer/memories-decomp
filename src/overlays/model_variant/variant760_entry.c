#include "../../types.h"
#include "variant760_entry.h"

/* Header 760's entry: after the delay a pulsing ring of quads with a
 * flickering flash grows over the slot while its colours cycle, a spray of
 * spinning lines bursts outward, then sparks rise and scatter before every
 * spark fades out. The spark phase returns 1 and the end returns 2. */

static const VECTOR D_8013CF68 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model760State *work;
    MATRIX light;
    MATRIX m;
    POLY_G4 g4;
    POLY_FT4 flash;
    POLY_FT4 spark;
    POLY_FT4 burst;
    GsGLINE line;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR center;
    SVECTOR inner[16];
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 side;
    s32 step;
    GsOT *ot;
    s32 i, j;
    s32 otz;
    SVECTOR *v;
    SVECTOR *w;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CF68;
    memset(&center, 0, 8);
    work = (Model760State *)context;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CFCC[command];
        v = work->ring;
        for (i = 0; i < 16; v++, i++) {
            v->vx = work->config->radius * ccos((i << 8) % 4096) / 4096;
            v->vy = work->config->radius * csin((i << 8) % 4096) / 4096;
            v->vz = 0;
        }
        v = work->shards;
        w = work->spins;
        for (i = 0; i < work->config->shard_count; v++, w++, i++) {
            v->vx = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            v->vy = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            v->vz = work->config->shard_spread * (rand() % 4096 - rand() % 4096) / 4096;
            w->vx = rand() % 64;
            w->vy = rand() % 64;
            w->vz = rand() % 64;
        }
        v = &work->flash[0];
        setVector(v, -work->config->flash_half, -work->config->flash_half, 0);
        v = &work->flash[1];
        setVector(v, work->config->flash_half, -work->config->flash_half, 0);
        v = &work->flash[2];
        setVector(v, -work->config->flash_half, work->config->flash_half, 0);
        v = &work->flash[3];
        setVector(v, work->config->flash_half, work->config->flash_half, 0);
        v = &work->spark[0];
        setVector(v, -work->config->spark_half_width, 0, -work->config->spark_half_depth);
        v = &work->spark[1];
        setVector(v, work->config->spark_half_width, 0, -work->config->spark_half_depth);
        v = &work->spark[2];
        setVector(v, -work->config->spark_half_width, 0, work->config->spark_half_depth);
        v = &work->spark[3];
        setVector(v, work->config->spark_half_width, 0, work->config->spark_half_depth);
        work->red = work->config->r;
        work->green = work->config->g;
        work->blue = work->config->b;
        work->white = 0x80;
        work->flash_level = 0;
        work->ambient = 0x800;
        work->radius = work->config->radius;
        work->line_scale = 0x1000;
        work->line_count = 0;
        work->spark_shown = 0;
        work->fade_phase = 0;
        work->phase = 0;
        work->red_rising = 0;
        work->green_rising = 0;
        work->unknown_2EC = 0;
        work->white_rising = 0;
        work->elapsed = 0;
        work->timer = 0;
        work->origin.vx = 0;
        work->origin.vy = 0;
        work->origin.vz = 0;
        for (i = 0; i < work->config->spark_count; i++) {
            work->alpha[i] = 0x80;
            work->frames[i] = 0;
        }
        work->spark_done = 0;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(slot, 1, &D_8013CF78[i]);
        }
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->fade_phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->fade_phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->ambient = 0x800;
            work->fade_phase = 2;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    if (work->phase == 0) {
        Model_SetSlotPlaybackDirection(0, 0);
        Model_SetSlotPlaybackDirection(1, 0);
        work->phase = 1;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    work->timer += step;

    if (work->phase >= 2) {
        if (work->red >= 0x20) {
            work->red -= step * 4;
        } else {
            work->red = 0;
        }
    } else if (work->red_rising == 0) {
        if (work->red >= 0x20) {
            work->red -= step * 8;
        } else {
            work->red = 0x1F;
            work->red_rising = 1;
        }
    } else if (work->red < 0xA0) {
        work->red += step * 8;
    } else {
        work->red = 0xA0;
        work->red_rising = 0;
    }
    if (work->phase >= 2) {
        if (work->green >= 0x20) {
            work->green -= step * 4;
        } else {
            work->green = 0;
        }
    } else if (work->green_rising == 0) {
        if (work->green >= 0x20) {
            work->green -= step * 8;
        } else {
            work->green = 0x1F;
            work->green_rising = 1;
        }
    } else if (work->green < 0xA0) {
        work->green += step * 8;
    } else {
        work->green = 0xA0;
        work->green_rising = 0;
    }
    if (work->phase >= 2) {
        if (work->white >= 0x20) {
            work->white -= step * 4;
        } else {
            work->white = 0;
        }
    } else if (work->white_rising == 0) {
        if (work->white >= 0x20) {
            work->white -= step * 8;
        } else {
            work->white = 0x1F;
            work->white_rising = 1;
        }
    } else if (work->white < 0xA0) {
        work->white += step * 8;
    } else {
        work->white = 0xA0;
        work->white_rising = 0;
    }

    SetPolyFT4(&flash);
    SetPolyFT4(&burst);
    flash.tpage = work->texture[1] >> 16;
    flash.clut = work->texture[1];
    burst.tpage = work->texture[0] >> 16;
    burst.clut = work->texture[0];
    setRGB0(&flash, work->flash_level, work->flash_level, work->flash_level);
    GsSetLsMatrix(&light);
    v = work->flash;
    GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->model), &m);
    pos.vx = m.t[0];
    pos.vy = m.t[1];
    pos.vz = m.t[2];
    setVector(&rot, side * work->config->tilt, 0, 0);
    pos.vz += side * work->config->height;
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    otz = RotAverage4(v, &work->flash[1], &work->flash[2], &work->flash[3],
                      (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                      (PSXLONG *)&flash.x3, &p, &flag);
    setUV4(&flash, (s16)(work->timer % 3) * 32, 0, (s16)(work->timer % 3) * 32 + 31, 0,
           (s16)(work->timer % 3) * 32, 31, (s16)(work->timer % 3) * 32 + 31, 31);
    if (otz >= 0) {
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 1);
    }
    if (work->phase >= 2) {
        if (work->flash_level >= 0x20) {
            work->flash_level -= step * 8;
        } else {
            work->flash_level = 0;
        }
    } else {
        work->flash_level += step;
        if (work->flash_level > 0x80) {
            work->flash_level = 0x80;
        }
    }

    SetPolyG4(&g4);
    v = work->ring;
    for (i = 0; i < 16; v++, i++) {
        copyVector(&inner[i], v);
        applyVector(&inner[i], 2, 2, 2, /=);
    }
    GsSetLsMatrix(&light);
    GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->model), &m);
    pos.vx = m.t[0];
    pos.vy = m.t[1];
    pos.vz = m.t[2];
    setVector(&rot, side * work->config->tilt, 0, 0);
    if (work->timer * 64 > 0x2000) {
        scale.vx = 0x3000;
        scale.vy = 0x3000;
        scale.vz = 0x3000;
    } else {
        scale.vx += work->timer * 64;
        scale.vy += work->timer * 64;
    }
    pos.vz += side * work->config->height;
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    v = work->ring;
    for (i = 0; i < 16; i++) {
        setRGB0(&g4, work->red, work->green, work->blue);
        setRGB1(&g4, work->red, work->green, work->blue);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        if (i == 15) {
            otz = RotAverage4(&v[15], v, &inner[15], &inner[0],
                              (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        } else {
            otz = RotAverage4(&v[i], &v[i + 1], &inner[i], &inner[i + 1],
                              (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
        setRGB0(&g4, work->white, work->white, work->white);
        setRGB1(&g4, work->white, work->white, work->white);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        if (i == 15) {
            otz = RotAverage4(&v[15], v, &center, &center,
                              (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        } else {
            otz = RotAverage4(&v[i], &v[i + 1], &center, &center,
                              (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
    }

    if (work->phase == 1) {
        if (work->line_scale > 0) {
            line.attribute = 0x50000000;
            line.r0 = 0;
            line.g0 = 0;
            line.b0 = 0;
            line.r1 = 0x80;
            line.g1 = 0x80;
            line.b1 = 0x80;
            scale.vx = work->line_scale;
            scale.vy = work->line_scale;
            scale.vz = work->line_scale;
            v = work->shards;
            w = work->spins;
            for (i = 0; i < work->config->shard_count; v++, i++) {
                GsSetLsMatrix(&light);
                rot.vx = w->vx;
                rot.vy = w->vy;
                rot.vz = w->vz;
                rot.vx *= work->timer;
                rot.vy *= work->timer;
                rot.vx += v->vx;
                rot.vy += v->vy;
                rot.vz += v->vz;
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = (RotTransPers(v, (PSXLONG *)&line.x0, &p, &flag) +
                       RotTransPers(&center, (PSXLONG *)&line.x1, &p, &flag)) / 2;
                if (otz >= 0) {
                    GsSortGLine(&line, ot, otz);
                }
            }
            work->line_count += step;
            if (work->line_count > work->config->shard_count) {
                work->line_count = work->config->shard_count;
            }
            work->line_scale -= work->timer * 2;
            if (work->line_scale < 0) {
                work->line_scale = 0;
            }
        }
        if (work->timer < work->config->burst_time) {
            goto end;
        }
    }
    if (work->phase == 1) {
        Model_SetSlotPlaybackDirection(0, 1);
        Model_SetSlotPlaybackDirection(1, 1);
    }
    if (work->elapsed < work->config->spark_start) {
        work->elapsed += step;
        goto end;
    }
    v = work->shards;
    if (work->phase == 1) {
        for (i = 0; i < work->config->spark_count; i++) {
            v[i].vx = work->config->radius * ((s32)((u32)(rand() % 4096 - rand() % 4096) << 1)) / 4096;
            v[i].vy = work->config->radius * ((s32)((u32)(rand() % 4096 - rand() % 4096) << 1)) / 4096;
            v[i].vz = 0;
        }
        v = work->spins;
        for (i = 0; i < work->config->spark_count; i++) {
            v[i].vx = 0;
            v[i].vy = 0;
            v[i].vz = 0;
        }
    }
    if (work->phase < 2) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->model), &m);
        work->origin.vx = m.t[0];
        work->origin.vy = m.t[1];
        work->origin.vz = m.t[2];
        work->phase = 2;
    }
    SetPolyFT4(&spark);
    v = work->spark;
    spark.tpage = work->texture[2] >> 16;
    spark.clut = work->texture[2];
    scale.vx = 0x1000;
    scale.vy = 0x1000;
    scale.vz = 0x1000;
    w = work->shards;
    if (work->phase == 2) {
        work->spark_shown++;
        if (work->spark_shown > work->config->spark_count) {
            work->spark_shown = work->config->spark_count;
        }
        for (i = 0; i < work->spark_shown; i++) {
            if (work->alpha[i] >= 0x1A) {
                setRGB0(&spark, work->alpha[i], work->alpha[i], work->alpha[i]);
                pos.vx = work->origin.vx;
                pos.vy = work->origin.vy;
                pos.vz = work->origin.vz;
                pos.vy += work->config->height / 2;
                pos.vz += side * work->config->height * 2;
                addVector(&pos, &w[i]);
                addVector(&pos, &work->spins[i]);
                applyVector(&work->spins[i], 0,
                            work->config->spark_rise * csin(-side * work->config->tilt) / 4096,
                            side * work->config->spark_rise * ccos(-side * work->config->tilt) / 4096, +=);
                for (j = 0; j < 4; j++) {
                    setVector(&rot, side * work->config->tilt, 0, j << 9);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(v, &v[1], &v[2], &v[3],
                                      (PSXLONG *)&spark.x0, (PSXLONG *)&spark.x1, (PSXLONG *)&spark.x2,
                                      (PSXLONG *)&spark.x3, &p, &flag);
                    setUV4(&spark, 0, 0x60, 0x3F, 0x60, 0, 0x6F, 0x3F, 0x6F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&spark, ot, (u16)otz, 3);
                    }
                }
                /* Retail's precedence slip: the alpha drops by the comparison
                 * result and any non-zero remainder is cleared. */
                if (work->alpha[i] -= step * 8 < 0x19) {
                    work->alpha[i] = 0;
                }
            }
        }
        if (work->alpha[work->config->spark_count - 1] == 0) {
            work->phase = 3;
            work->spark_shown = 0;
            v = work->shards;
            for (i = 0; i < work->config->spark_count; v++, i++) {
                v->vx = work->config->spark_scatter * (rand() % 4096 - rand() % 4096) / 4096;
                v->vy = work->config->spark_scatter * (rand() % 4096 - rand() % 4096) / 4096;
                v->vz = 0;
                work->alpha[i] = 0x80;
            }
        }
    } else if (work->phase == 3) {
        work->spark_shown++;
        if (work->spark_shown > work->config->spark_count) {
            work->spark_shown = work->config->spark_count;
        }
        for (i = 0; i < work->spark_shown; i++) {
            if (work->alpha[i] != 0) {
                setRGB0(&spark, work->alpha[i], work->alpha[i], work->alpha[i]);
                setVector(&pos, 0, -350, side * 450);
                addVector(&pos, &w[i]);
                setVector(&rot, 0, 0, 0);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&work->flash[0], &work->flash[1], &work->flash[2], &work->flash[3],
                                  (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                                  (PSXLONG *)&flash.x3, &p, &flag);
                setRGB0(&flash, 0x80, 0x80, 0x80);
                setUV4(&flash, (s16)(work->timer % 3) * 32, 0, (s16)(work->timer % 3) * 32 + 31, 0,
                       (s16)(work->timer % 3) * 32, 31, (s16)(work->timer % 3) * 32 + 31, 31);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 1);
                }
                if (work->frames[i] < 4) {
                    otz = RotAverage4(&work->flash[0], &work->flash[1], &work->flash[2], &work->flash[3],
                                      (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2,
                                      (PSXLONG *)&burst.x3, &p, &flag);
                    setRGB0(&burst, 0x80, 0x80, 0x80);
                    setUV4(&burst, work->frames[i] * 64, 0x20, work->frames[i] * 64 + 63, 0x20,
                           work->frames[i] * 64, 0x5F, work->frames[i] * 64 + 63, 0x5F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)otz, 1);
                    }
                    work->frames[i]++;
                }
                for (j = 0; j < 4; j++) {
                    setVector(&rot, side * work->config->tilt, 0, j << 9);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(v, &v[1], &v[2], &v[3],
                                      (PSXLONG *)&spark.x0, (PSXLONG *)&spark.x1, (PSXLONG *)&spark.x2,
                                      (PSXLONG *)&spark.x3, &p, &flag);
                    setUV4(&spark, 0, 0x60, 0x3F, 0x60, 0, 0x6F, 0x3F, 0x6F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&spark, ot, (u16)otz, 3);
                    }
                }
                if (work->alpha[i] < 0x19) {
                    work->alpha[i] = 0;
                    work->spark_done++;
                } else {
                    work->alpha[i] -= step * 8;
                }
            }
            if (work->spark_done == work->config->spark_count) {
                work->phase = 4;
            }
        }
        PopMatrix();
        return 1;
    } else {
        work->fade_phase = 1;
        if (work->elapsed >= work->config->end) {
            PopMatrix();
            return 2;
        }
        work->elapsed += step;
    }
end:
    PopMatrix();
    return 0;
}
