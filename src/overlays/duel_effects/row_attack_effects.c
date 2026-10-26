#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_16.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "primitive_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "row_strike_semantic_views.h"
#include "fallback_semantic_views.h"
#include "effect_23.h"
#include "exodia_semantic_views.h"
#include "selected_cards_semantic_views.h"

void DuelEffect_RunRowStrike(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR translation;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    SVECTOR quad[4];
    CVECTOR color;
    POLY_FT4 *packet;
    DuelEffect16Work *work;
    s32 i;
    s32 j;
    s32 k;
    s32 l;

    memset(&translation, 0, sizeof(translation));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_RowStrikeInitialScale;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_RowStrikeConfig;
            for (i = 0; i < 5; i++) {
                for (k = 0; k < 5; k++) {
                    for (l = 0; l < 4; l++) {
                        setVector(&work->paths[k][l][i],
                            work->config->spread * ((rand() - rand()) % 4096) / 4096,
                            k * work->config->vertical_step - 256,
                            work->config->spread * ((rand() - rand()) % 4096) / 4096);
                        if (k == 4) {
                            work->paths[k][l][i].vx = 0;
                            work->paths[k][l][i].vz = 0;
                        }
                    }
                }
                DuelEffect_InitRandomVectorsXYZ(work->clouds[i], work->config->particle_spread,
                    work->config->particle_spread, work->config->particle_spread, 32);
                /* Retail advances overlapping 32-vector windows by five vectors. */
                DuelEffect_InitParticleVectorsNegatedY(8, &work->positions[i * 5], &work->velocities[i * 5], 32);
                work->scales[i] = 4096;
                work->ages[i] = 0;
                DuelEffect_SetGrayColor((u8 *)&work->primary[i], 192);
                DuelEffect_SetRgbColor((u8 *)&work->secondary[i], work->config->color.r,
                    work->config->color.g, work->config->color.b);
                setVector(&work->origins[i],
                    (70 * i - 140) * (gDuelEffect_DrawOffset.vz < 0 ? -1 : 1),
                    -24, gDuelEffect_DrawOffset.vz >= 0 ? 95 : -95);
                work->activated[i] = 0;
            }
            for (k = 0; k < 3; k++) {
                DuelEffect_InitXZRingVertices(work->config->ring_radii[k], work->config->ring_radii[k],
                    work->config->ring_height * k, work->rings[k], 32);
            }
            work->count = 0;
            work->hits = 0;
            work->tick = 0;
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            DuelEffect_DrawErrorCross(1);
        } else {
            DuelEffect_DrawErrorCross(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        for (j = 0; j < work->count; j++) {
            DuelEffect_SetTransformMatrix(&world, &saved, &work->origins[j], &rotation, &scale, 3);
            if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->primary[j]) ||
                (u16)DuelEffect_HasNonzeroColor((u8 *)&work->secondary[j])) {
                setPolyFT4(packet);
                packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[14][0];
                packet->clut = gDuelEffect_TexturePageClutPairs.pairs[14][1];
                setUV4(packet, 80, 0, 127, 0, 80, 127, 127, 127);
                setRGB0(packet, work->primary[j].r, work->primary[j].g, work->primary[j].b);
                for (i = 0; i < 4; i++) {
                    for (k = 0; k < work->ages[j]; k++) {
                        for (l = 0; l < 4; l++) {
                            copyVector(&quad[l],
                                &work->paths[k + l / 2][i][j]);
                            quad[l].vx += (s16)DuelEffect_PowS16(-1, l + 1) *
                                work->config->widths[i];
                        }
                        DuelEffect_ProjectSubmitFt4(packet, quad, 32, 1);
                    }
                    if (work->ages[j] == 4) {
                        DuelEffect_DrawLayeredTileQuad((u8 *)&work->secondary[j], work->config->glow_sizes[i] * 2,
                            32, &work->paths[work->ages[j]][i][j]);
                    } else {
                        DuelEffect_DrawLayeredTileQuad((u8 *)&work->secondary[j], work->config->glow_sizes[i],
                            32, &work->paths[work->ages[j]][i][j]);
                    }
                }
                color = work->secondary[j];
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
                copyVector(&translation, &work->paths[work->ages[j]][0][j]);
                addVector(&translation, &work->origins[j]);
                DuelEffect_SetTransformMatrix(&world, &saved, &translation, &rotation, &scale, 2);
                DuelEffect_DrawSharedBaseTriangles((u8 *)&color, work->config->line_width, work->clouds[j], 32, 32);
                work->ages[j]++;
                if (work->ages[j] >= 5) {
                    if (work->activated[j] == 0) {
                        work->hits++;
                        work->activated[j] = 1;
                        gDuelEffect_pActiveRequest->progress = work->hits;
                    }
                    work->ages[j] = 4;
                    for (k = 0; k < 32; k++) {
                        DuelEffect_InitOffsetQuadVertices(quad, &(&work->positions[j * 5])[k]);
                        DuelEffect_DrawRandomTileQuad((u8 *)&work->secondary[j], quad, 1);
                        addVector(&(&work->positions[j * 5])[k],
                            &(&work->velocities[j * 5])[k]);
                    }
                    setVector(&scale, work->scales[j], 4096, work->scales[j]);
                    DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
                    DuelEffect_DrawTexturedRingStrip((u8 *)&color, work->rings[0], work->rings[1], work->rings[2], 32);
                    setVector(&scale, work->scales[j] / 2, 6144, work->scales[j] / 2);
                    DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
                    DuelEffect_DrawTexturedRingStrip((u8 *)&work->secondary[j],
                        work->rings[0], work->rings[1], work->rings[2], 32);
                    if (work->scales[j] < 16384) {
                        work->scales[j] += 4096;
                    }
                    DuelEffect_DarkenColor((u8 *)&work->primary[j], 31);
                    DuelEffect_DarkenColor((u8 *)&work->secondary[j], 15);
                }
            }
        }
        work->tick++;
        if (!(work->tick & 7)) {
            work->count++;
            if (work->count > 5) {
                work->count = 5;
            }
        }
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->primary[4]) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->secondary[4])) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}

void DuelEffect_RunBackRowSweep(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    SVECTOR vertices[4];
    POLY_FT4 *packet;
    DuelEffect23Work *work;
    s32 i;
    s32 j;

    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_BackRowSweepInitialScale;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        Duel_CollectFieldRowCardObjects(gDuelEffect_CardObjectScratchWords, 1);
        setVector(&work->position,
                  (gDuelEffect_DrawOffset.vz >= 0 ? -140 : 140) + (gDuelEffect_DrawOffset.vz >= 0 ? -64 : 64),
                  -184, gDuelEffect_DrawOffset.vz >= 0 ? -190 : 190);
        work->speed = gDuelEffect_DrawOffset.vz >= 0 ? 4 : -4;
        setVector(&work->rotation, 0, 0, gDuelEffect_DrawOffset.vz >= 0 ? -384 : 384);
        work->count = 0;
        while (gDuelEffect_CardObjectScratchWords[work->count] != 0) {
            work->count++;
        }
        for (i = 4; i >= 0; i--) {
            work->states[i] = 0;
        }
        for (i = 0; i < work->count; i++) {
            setVector(&work->card_velocities[i], work->speed / 2,
                      -__builtin_abs(work->speed) * (rand() % 4096) / 4096,
                      __builtin_abs(work->speed) * ((rand() - rand()) % 4096) / 4096);
            setVector(&work->card_rotations[i],
                      (rand() % 5 + 2) * (gDuelEffect_DrawOffset.vz >= 0 ? -1 : 1),
                      (rand() % 5 + 2) * (gDuelEffect_DrawOffset.vz >= 0 ? -1 : 1),
                      (rand() % 5 + 2) * (gDuelEffect_DrawOffset.vz >= 0 ? -1 : 1));
            if ((s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 ==
                (gDuelEffect_DrawOffset.vz >= 0 ? -140 : 140)) {
                work->states[0] = 1;
            } else if ((s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 ==
                (gDuelEffect_DrawOffset.vz >= 0 ? -70 : 70)) {
                work->states[1] = 1;
            } else if ((s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 == 0) {
                work->states[2] = 1;
            } else if ((s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 ==
                (gDuelEffect_DrawOffset.vz >= 0 ? 70 : -70)) {
                work->states[3] = 1;
            } else if ((s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 ==
                (gDuelEffect_DrawOffset.vz >= 0 ? 140 : -140)) {
                work->states[4] = 1;
            }
        }
        for (i = 0; i < 5; i++) {
            setVector(&work->slots[i],
                      (gDuelEffect_DrawOffset.vz >= 0 ? -140 : 140) +
                      i * 70 * (gDuelEffect_DrawOffset.vz >= 0 ? 1 : -1),
                      -24, gDuelEffect_DrawOffset.vz >= 0 ? -95 : 95);
            for (j = 0; j < 3; j++) {
                copyVector(&work->particles[i][j], &work->slots[i]);
                setVector(&work->velocities[i][j],
                          work->speed * (rand() % 4096) / 4096,
                          -__builtin_abs(work->speed) * (rand() % 4096) / 4096,
                          __builtin_abs(work->speed) * ((rand() - rand()) % 4096) / 4096);
            }
            work->ages[i] = 0;
        }
        work->active = 0;
        work->hidden = 0;
        DuelEffect_SetGrayColor((u8 *)&work->color, 1);
        work->stage = 0;
        work->swing = 0;
    } else {
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        DuelEffect_SetTransformMatrix(&world, &saved, &work->position, &work->rotation, &scale, 1);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
            setPolyFT4(packet);
            packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[19][0];
            packet->clut = gDuelEffect_TexturePageClutPairs.pairs[19][1];
            setRGB0(packet, work->color.r, work->color.g, work->color.b);
            setUV4(packet, 144, 128, 191, 128, 144, 255, 191, 255);
            setVector(&vertices[0], 0, 0, 0);
            setVector(&vertices[1], work->speed >= 0 ? 48 : -48, 0, 0);
            setVector(&vertices[2], 0, 128, 0);
            setVector(&vertices[3], work->speed >= 0 ? 48 : -48, 128, 0);
            if (*(u16 *)&work->color == 0x8080 && work->color.b == 128) {
                DuelEffect_ProjectSubmitFt4(packet, vertices, 32, 0);
            } else {
                DuelEffect_ProjectSubmitFt4(packet, vertices, 32, 1);
            }
            if (work->position.vy < -104) {
                work->position.vy += 3;
            } else {
                work->stage = 2;
                work->position.vx += work->speed;
                if (work->swing == 0) {
                    if (work->rotation.vx < 320) {
                        work->rotation.vx += 32;
                    } else {
                        SD_SEPlayFull(36);
                        work->swing = 1;
                    }
                } else {
                    if (work->rotation.vx > -320) {
                        work->rotation.vx -= 32;
                    } else {
                        SD_SEPlayFull(36);
                        work->swing = 0;
                    }
                }
            }
            if (work->speed > 0) {
                if (work->position.vx > 210) {
                    DuelEffect_DarkenColor((u8 *)&work->color, 8);
                }
                for (i = 0; i < 5; i++) {
                    if (work->position.vx >= i * 70 - 44 && work->states[i] == 1) {
                        work->active++;
                        work->states[i] = 2;
                    }
                }
            } else if (work->speed < 0) {
                if (work->position.vx < -210) {
                    DuelEffect_DarkenColor((u8 *)&work->color, 8);
                }
                for (i = 0; i < 5; i++) {
                    if (work->position.vx <= 236 - i * 70 && work->states[i] == 1) {
                        work->active++;
                        work->states[i] = 2;
                    }
                }
            }
            if (work->stage == 0) {
                work->stage = DuelEffect_StepColorToTarget((u8 *)&work->color, 128, 128, 128, 15);
            }
        }
        if (work->stage >= 2) {
            for (i = 0; i < work->active; i++) {
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->attribute |= 0x50000000;
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 += work->card_velocities[i].vx;
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32 += work->card_velocities[i].vy;
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34 += work->card_velocities[i].vz;
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_20 += work->card_rotations[i].vx;
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_21 += work->card_rotations[i].vx;
                ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_22 += work->card_rotations[i].vx;
                if (((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[0] > 15) {
                    ((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[0] -= 15;
                } else {
                    ((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[0] = 0;
                }
                if (((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[1] > 15) {
                    ((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[1] -= 15;
                } else {
                    ((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[1] = 0;
                }
                if (((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[2] > 15) {
                    ((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[2] -= 15;
                } else {
                    ((u8 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb)[2] = 0;
                }
                if ((((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->rgb & 0xFFFFFF) == 0) {
                    if (work->hidden < work->active) {
                        ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->flags &= ~0x40;
                        work->hidden++;
                    }
                }
            }
            for (i = 0; i < 5; i++) {
                if (work->ages[i] < 24 && work->states[i] >= 2) {
                    setPolyFT4(packet);
                    packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[20][0];
                    packet->clut = gDuelEffect_TexturePageClutPairs.pairs[20][1];
                    setRGB0(packet, 32, 32, 32);
                    DuelEffect_InitSquareVertices(vertices, 32);
                    setVector(&rotation, 0, 0, 0);
                    setUV4(packet,
                        ((work->ages[i] / 3) % 4) * 32, (work->ages[i] / 12) * 32 + 192,
                        ((work->ages[i] / 3) % 4) * 32 + 31, (work->ages[i] / 12) * 32 + 192,
                        ((work->ages[i] / 3) % 4) * 32, (work->ages[i] / 12) * 32 + 223,
                        ((work->ages[i] / 3) % 4) * 32 + 31, (work->ages[i] / 12) * 32 + 223);
                    work->ages[i]++;
                    for (j = 0; j < 3; j++) {
                        DuelEffect_SetTransformMatrix(&world, &saved, &work->particles[i][j], &rotation, &scale, 1);
                        DuelEffect_ProjectSubmitFt4(packet, vertices, 1, 1);
                        addVector(&work->particles[i][j], &work->velocities[i][j]);
                    }
                }
            }
        }
        PopMatrix();
        if (work->count != 0) {
            if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color) &&
                (((DisplayObject *)gDuelEffect_CardObjectScratchWords[work->count - 1])->rgb & 0xFFFFFF) == 0) {
                gDuelEffect_bRequestFinished = 1;
            }
        } else {
            if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color)) {
                gDuelEffect_bRequestFinished = 1;
            }
        }
    }
}
