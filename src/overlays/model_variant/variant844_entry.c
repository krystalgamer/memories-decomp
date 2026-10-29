#include "../../types.h"
#include "variant844_entry.h"

/* Header 844's entry: while the ambient light dims, two ribbon trails
 * follow pairs of model parts, then two rounds of sparks, flares and
 * four-petal bursts open around the opposing slot before a spinning beam
 * of light rises from it; once the bursts are done the light comes back. */

static const VECTOR D_8013D5C8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model844State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_G4 g4;
    POLY_FT4 sprite;
    POLY_FT4 petal;
    POLY_GT4 ray;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag;
    PSXLONG interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    POLY_FT4 *ft;
    POLY_GT4 *gt;
    s32 i, j, k;
    s16 sign;
    s32 otz;
    SVECTOR *edge;
    SVECTOR *edge_b;
    SVECTOR *edge_c;
    SVECTOR *edge_d;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D5C8;
    work = (Model844State *)context;
    ft = &petal;
    gt = &ray;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D62C[command];
        edge = work->trail_a;
        edge_b = work->trail_b;
        edge_c = work->trail_c;
        edge_d = work->trail_d;
        for (i = 0; i < 64; i++) {
            setVector(edge, 0, 0, 0);
            setVector(edge_b, 0, 0, 0);
            setVector(edge_c, 0, 0, 0);
            setVector(edge_d, 0, 0, 0);
            edge++;
            edge_b++;
            edge_c++;
            edge_d++;
            for (j = 0; j < 2; j++) {
                work->trail_colors[j][i][0] = work->config->trail_r;
                work->trail_colors[j][i][1] = work->config->trail_g;
                work->trail_colors[j][i][2] = work->config->trail_b;
            }
        }
        edge = work->sparks;
        for (i = 0; i < work->config->spark_count; i++) {
            setVector(edge, work->config->radius * ccos(rand() % 4096) / 4096,
                      work->config->radius * csin(rand() % 4096) / 4096, 0);
            work->spark_frames[0][i] = 0;
            work->spark_frames[1][i] = 0;
            edge++;
        }
        for (i = 0; i < work->config->flare_count; i++) {
            setVector(&work->flares[i], work->config->radius * ccos(rand() % 4096) / 4096,
                      work->config->radius * csin(rand() % 4096) / 4096, 0);
            work->flare_frames[0][i] = 0;
            work->flare_frames[1][i] = 0;
            work->burst_scale[0][i] = 0x1000;
            work->burst_scale[1][i] = 0x1000;
            work->burst_colors[0][i][0] = work->config->burst_r;
            work->burst_colors[0][i][1] = work->config->burst_g;
            work->burst_colors[0][i][2] = work->config->burst_b;
            work->burst_colors[1][i][0] = work->config->burst_r;
            work->burst_colors[1][i][1] = work->config->burst_g;
            work->burst_colors[1][i][2] = work->config->burst_b;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->spins[i], rand() % 4096, rand() % 4096, rand() % 4096);
        }
        work->beam_color.rgb[0] = work->config->burst_r;
        work->beam_color.rgb[1] = work->config->burst_g;
        work->beam_color.rgb[2] = work->config->burst_b;
        work->tpage = 0xAE;
        /* Palette at x 512, y 244. */
        work->clut = 0x3D20;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D5D8[i]);
        }
        work->spark_count[0] = 0;
        work->spark_count[1] = 0;
        work->flare_count[0] = 0;
        work->flare_count[1] = 0;
        work->trail_index[0] = 0;
        work->trail_index[1] = 0;
        work->level = 0x800;
        work->phase = 0;
        work->trail_on[0] = 0;
        work->trail_on[1] = 0;
        work->elapsed = 0;
        work->beam_height = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    setPolyFT4(ft);
    setPolyGT4(gt);
    if (work->phase == 0) {
        if (work->level > -0x200) {
            work->level -= step << 6;
        } else {
            work->level = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->level < 0x800) {
            work->level += step << 6;
        } else {
            work->phase = 3;
            work->level = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->trail_start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->trail_start && work->elapsed <= work->config->trail_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[0]), &matrix);
        edge = work->trail_a;
        setVector(&edge[work->trail_index[0]], matrix.t[0], matrix.t[1], matrix.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[1]), &matrix);
        edge_b = work->trail_b;
        setVector(&edge_b[work->trail_index[0]], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->trail_index[0] = (work->trail_index[0] + 1) % 64;
        if (work->trail_index[0] >= 2 && work->trail_on[0] == 0) {
            work->trail_on[0] = 1;
        }
    }
    if (work->elapsed >= work->config->trail2_start && work->elapsed <= work->config->trail2_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[2]), &matrix);
        edge = work->trail_c;
        setVector(&edge[work->trail_index[1]], matrix.t[0], matrix.t[1], matrix.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[3]), &matrix);
        edge_b = work->trail_d;
        setVector(&edge_b[work->trail_index[1]], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->trail_index[1] = (work->trail_index[1] + 1) % 64;
        if (work->trail_index[1] >= 2 && work->trail_on[1] == 0) {
            work->trail_on[1] = 1;
        }
    }

    if (work->trail_on[0] > 0) {
        edge = work->trail_a;
        edge_b = work->trail_b;
        setPolyG4(&g4);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        k = 1;
        for (i = work->trail_index[0] - 1; i > 0; i--) {
            setRGB0(&g4, work->trail_colors[0][i][0] / k, work->trail_colors[0][i][1] / k,
                    work->trail_colors[0][i][2] / k);
            setRGB1(&g4, work->trail_colors[0][i][0] / (k + 1), work->trail_colors[0][i][1] / (k + 1),
                    work->trail_colors[0][i][2] / (k + 1));
            k++;
            for (j = 0; j < work->config->segments; j++) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -j * work->config->segment_step, 0);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&edge[i], &edge[i - 1], &edge_b[i], &edge_b[i - 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &interpolation, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
            }
            /* The red channel's floor clears green, as the original does. */
            if (work->trail_colors[0][i][0] >= 16) {
                work->trail_colors[0][i][0] -= 15;
            } else {
                work->trail_colors[0][i][1] = 0;
            }
            if (work->trail_colors[0][i][1] >= 16) {
                work->trail_colors[0][i][1] -= 15;
            } else {
                work->trail_colors[0][i][1] = 0;
            }
            if (work->trail_colors[0][i][2] >= 16) {
                work->trail_colors[0][i][2] -= 15;
            } else {
                work->trail_colors[0][i][2] = 0;
            }
        }
    }
    if (work->trail_on[1] > 0) {
        edge = work->trail_c;
        edge_b = work->trail_d;
        setPolyG4(&g4);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        k = 1;
        for (i = work->trail_index[1] - 1; i > 0; i--) {
            setRGB0(&g4, work->trail_colors[1][i][0] / k, work->trail_colors[1][i][1] / k,
                    work->trail_colors[1][i][2] / k);
            setRGB1(&g4, work->trail_colors[1][i][0] / (k + 1), work->trail_colors[1][i][1] / (k + 1),
                    work->trail_colors[1][i][2] / (k + 1));
            k++;
            for (j = 0; j < work->config->segments; j++) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -j * work->config->segment_step, 0);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&edge[i], &edge[i - 1], &edge_b[i], &edge_b[i - 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &interpolation, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
            }
            if (work->trail_colors[1][i][0] >= 16) {
                work->trail_colors[1][i][0] -= 8;
            } else {
                work->trail_colors[1][i][0] = 0;
            }
            if (work->trail_colors[1][i][1] >= 16) {
                work->trail_colors[1][i][1] -= 8;
            } else {
                work->trail_colors[1][i][1] = 0;
            }
            if (work->trail_colors[1][i][2] >= 16) {
                work->trail_colors[1][i][2] -= 8;
            } else {
                work->trail_colors[1][i][2] = 0;
            }
        }
    }

    if (work->elapsed >= work->config->burst_start) {
        setPolyFT4(&sprite);
        setRGB0(&sprite, 0xFF, 0, 0);
        sprite.tpage = work->texture[0] >> 16;
        sprite.clut = work->texture[0];
        setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
        setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
        edge = work->sparks;
        for (i = 0; i < work->spark_count[0]; i++) {
            if (work->spark_frames[0][i] < 8) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                position.vx += edge->vx;
                position.vy += edge->vy;
                position.vz += edge->vz;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&sprite.x0,
                                  (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &interpolation,
                                  &flag);
                setUV4(&sprite, work->spark_frames[0][i] * 32, 0x40, work->spark_frames[0][i] * 32 + 31, 0x40,
                       work->spark_frames[0][i] * 32, 0x5F, work->spark_frames[0][i] * 32 + 31, 0x5F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                }
                work->spark_frames[0][i]++;
            }
            edge++;
        }
        work->spark_count[0] += step;
        if (work->spark_count[0] > work->config->spark_count) {
            work->spark_count[0] = work->config->spark_count;
        }
        setRGB0(&sprite, 0xFF, 0x80, 0x80);
        sprite.tpage = work->texture[2] >> 16;
        sprite.clut = work->texture[2];
        setVector(&vertices[0], -work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[1], work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[2], -work->config->flare_width, work->config->flare_height, 0);
        setVector(&vertices[3], work->config->flare_width, work->config->flare_height, 0);
        edge = work->flares;
        for (i = 0; i < work->flare_count[0]; i++) {
            if (work->flare_frames[0][i] < 4) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                position.vx += edge->vx;
                position.vy += edge->vy;
                position.vz += edge->vz;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&sprite.x0,
                                  (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &interpolation,
                                  &flag);
                setUV4(&sprite, work->flare_frames[0][i] * 32 - 128, 0, work->flare_frames[0][i] * 32 - 97, 0,
                       work->flare_frames[0][i] * 32 - 128, 0x3F, work->flare_frames[0][i] * 32 - 97, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                }
                work->flare_frames[0][i]++;
            }
            edge++;
        }
        ft->tpage = work->tpage;
        ft->clut = work->clut;
        setUV4(ft, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
        edge = work->sparks;
        for (i = 0; i < work->flare_count[0]; i++) {
            if (work->burst_colors[0][i][0] != 0 || work->burst_colors[0][i][1] != 0 ||
                work->burst_colors[0][i][2] != 0) {
                setRGB0(ft, work->burst_colors[0][i][0], work->burst_colors[0][i][1], work->burst_colors[0][i][2]);
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                position.vx += edge->vx;
                position.vy += edge->vy;
                position.vz += edge->vz;
                setVector(&scale, work->burst_scale[0][i], work->burst_scale[0][i], work->burst_scale[0][i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    setVector(&vertices[0], -sign * work->config->burst_size,
                              (j - 2 < 0 ? 1 : -1) * work->config->burst_size, 0);
                    setVector(&vertices[1], 0, (j - 2 < 0 ? 1 : -1) * work->config->burst_size, 0);
                    setVector(&vertices[2], -sign * work->config->burst_size, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&ft->x0,
                                      (PSXLONG *)&ft->x1, (PSXLONG *)&ft->x2, (PSXLONG *)&ft->x3, &interpolation,
                                      &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft, ot, (u16)otz, 1);
                    }
                }
                if (work->burst_scale[0][i] <= 30000) {
                    work->burst_scale[0][i] += 0x2000;
                } else {
                    work->burst_scale[0][i] += 0x40;
                }
                if (work->burst_colors[0][i][0] >= 16) {
                    work->burst_colors[0][i][0] -= 15;
                } else {
                    work->burst_colors[0][i][0] = 0;
                }
                if (work->burst_colors[0][i][1] >= 16) {
                    work->burst_colors[0][i][1] -= 15;
                } else {
                    work->burst_colors[0][i][1] = 0;
                }
                if (work->burst_colors[0][i][2] >= 16) {
                    work->burst_colors[0][i][2] -= 15;
                } else {
                    work->burst_colors[0][i][2] = 0;
                }
            }
            edge++;
        }
        work->flare_count[0] += step;
        if (work->flare_count[0] > work->config->flare_count) {
            work->flare_count[0] = work->config->flare_count;
        }
    }

    if (work->elapsed >= work->config->burst2_start) {
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setPolyFT4(&sprite);
        setRGB0(&sprite, 0xFF, 0, 0);
        sprite.tpage = work->texture[0] >> 16;
        sprite.clut = work->texture[0];
        setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
        setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
        edge = work->sparks;
        for (i = 0; i < work->spark_count[1]; i++) {
            if (work->spark_frames[1][i] < 8) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                position.vx += edge->vx;
                position.vy += edge->vy;
                position.vz += edge->vz;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&sprite.x0,
                                  (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &interpolation,
                                  &flag);
                setUV4(&sprite, work->spark_frames[1][i] * 32, 0x40, work->spark_frames[1][i] * 32 + 31, 0x40,
                       work->spark_frames[1][i] * 32, 0x5F, work->spark_frames[1][i] * 32 + 31, 0x5F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                }
                work->spark_frames[1][i]++;
            }
            edge++;
        }
        work->spark_count[1] += step;
        if (work->spark_count[1] > work->config->spark_count) {
            work->spark_count[1] = work->config->spark_count;
        }
        setRGB0(&sprite, 0xFF, 0x80, 0x80);
        sprite.tpage = work->texture[2] >> 16;
        sprite.clut = work->texture[2];
        setVector(&vertices[0], -work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[1], work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[2], -work->config->flare_width, work->config->flare_height, 0);
        setVector(&vertices[3], work->config->flare_width, work->config->flare_height, 0);
        edge = work->flares;
        for (i = 0; i < work->flare_count[1]; i++) {
            if (work->flare_frames[1][i] < 4) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                position.vx += edge->vx;
                position.vy += edge->vy;
                position.vz += edge->vz;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&sprite.x0,
                                  (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3, &interpolation,
                                  &flag);
                setUV4(&sprite, work->flare_frames[1][i] * 32 - 128, 0, work->flare_frames[1][i] * 32 - 97, 0,
                       work->flare_frames[1][i] * 32 - 128, 0x3F, work->flare_frames[1][i] * 32 - 97, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)otz, 1);
                }
                work->flare_frames[1][i]++;
            }
            edge++;
        }
        ft->tpage = work->tpage;
        ft->clut = work->clut;
        setUV4(ft, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
        edge = work->flares;
        for (i = 0; i < work->flare_count[1]; i++) {
            if (work->burst_colors[1][i][0] != 0 || work->burst_colors[1][i][1] != 0 ||
                work->burst_colors[1][i][2] != 0) {
                setRGB0(ft, work->burst_colors[1][i][0], work->burst_colors[1][i][1], work->burst_colors[1][i][2]);
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                position.vx += edge->vx;
                position.vy += edge->vy;
                position.vz += edge->vz;
                setVector(&scale, work->burst_scale[1][i], work->burst_scale[1][i], work->burst_scale[1][i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    setVector(&vertices[0], -sign * work->config->burst_size,
                              (j - 2 < 0 ? 1 : -1) * work->config->burst_size, 0);
                    setVector(&vertices[1], 0, (j - 2 < 0 ? 1 : -1) * work->config->burst_size, 0);
                    setVector(&vertices[2], -sign * work->config->burst_size, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&ft->x0,
                                      (PSXLONG *)&ft->x1, (PSXLONG *)&ft->x2, (PSXLONG *)&ft->x3, &interpolation,
                                      &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft, ot, (u16)otz, 1);
                    }
                }
                if (work->burst_scale[1][i] <= 30000) {
                    work->burst_scale[1][i] += 0x2000;
                } else {
                    work->burst_scale[1][i] += 0x40;
                }
                if (work->burst_colors[1][i][0] >= 16) {
                    work->burst_colors[1][i][0] -= 15;
                } else {
                    work->burst_colors[1][i][0] = 0;
                }
                if (work->burst_colors[1][i][1] >= 16) {
                    work->burst_colors[1][i][1] -= 15;
                } else {
                    work->burst_colors[1][i][1] = 0;
                }
                if (work->burst_colors[1][i][2] >= 16) {
                    work->burst_colors[1][i][2] -= 15;
                } else {
                    work->burst_colors[1][i][2] = 0;
                }
            }
            edge++;
        }
        work->flare_count[1] += step;
        if (work->flare_count[1] > work->config->flare_count) {
            work->flare_count[1] = work->config->flare_count;
        }
    }

    if (work->elapsed >= work->config->beam_start && (work->beam_color.word & 0xFFFFFF)) {
        gt->tpage = work->tpage;
        gt->clut = work->clut;
        setUV4(gt, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
        setRGB0(gt, 0, 0, 0);
        setRGB1(gt, 0, 0, 0);
        setRGB2(gt, work->beam_color.rgb[0], work->beam_color.rgb[1], work->beam_color.rgb[2]);
        setRGB3(gt, work->beam_color.rgb[0], work->beam_color.rgb[1], work->beam_color.rgb[2]);
        setVector(&vertices[0], -4, -160, 0);
        setVector(&vertices[1], 4, -160, 0);
        setVector(&vertices[2], -12, 0, 0);
        setVector(&vertices[3], 12, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&scale, 0x1000, work->beam_height, 0x1000);
        setSemiTrans(gt, 1);
        for (i = 0; i < 32; i++) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&work->spins[i], &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&gt->x0,
                              (PSXLONG *)&gt->x1, (PSXLONG *)&gt->x2, (PSXLONG *)&gt->x3, &interpolation, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(gt, ot, otz);
            }
        }
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        sign = 1;
        setUV4(ft, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
        setRGB0(ft, work->beam_color.rgb[0], work->beam_color.rgb[1], work->beam_color.rgb[2]);
        for (j = 0; j < 4; j++) {
            sign *= -1;
            setVector(&vertices[0], -sign * 128, (j - 2 >= 0 ? -1 : 1) * 128, 0);
            setVector(&vertices[1], 0, (j - 2 >= 0 ? -1 : 1) * 128, 0);
            setVector(&vertices[2], -sign * 128, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&ft->x0,
                              (PSXLONG *)&ft->x1, (PSXLONG *)&ft->x2, (PSXLONG *)&ft->x3, &interpolation, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)ft, ot, (u16)otz, 1);
            }
        }
        if (work->beam_height < 0x2000) {
            work->beam_height += 0x800;
        } else {
            work->beam_height += 0x100;
        }
        if (work->beam_color.rgb[0] >= 16) {
            work->beam_color.rgb[0] -= 15;
        } else {
            work->beam_color.rgb[0] = 0;
        }
        if (work->beam_color.rgb[1] >= 16) {
            work->beam_color.rgb[1] -= 15;
        } else {
            work->beam_color.rgb[1] = 0;
        }
        if (work->beam_color.rgb[2] >= 16) {
            work->beam_color.rgb[2] -= 15;
        } else {
            work->beam_color.rgb[2] = 0;
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 0 && work->elapsed >= work->config->burst_start) {
        work->phase = 1;
        return 4;
    }
    if (work->phase == 1 && work->elapsed >= work->config->burst2_start) {
        work->phase = 2;
        return 1;
    }
    if (work->phase == 3 && work->elapsed > work->config->end) {
        return 2;
    }
    return 0;
}
