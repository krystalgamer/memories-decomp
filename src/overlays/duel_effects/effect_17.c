#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_17.h"
#include "handler_semantic_views.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "primitive_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "stat_penalty_semantic_views.h"
#include "exodia_semantic_views.h"
#include "fallback_semantic_views.h"
#include "live_handler_data_semantic_views.h"

void DuelEffect_RunBoardDestruction(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    VECTOR scale;
    POLY_GT4 polygon;
    SVECTOR vertices[4];
    CVECTOR color;
    POLY_GT4 *packet;
    DuelEffect17Work *work;
    s32 i;
    s32 j;
    s32 k;

    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_BoardDestructionInitialScale;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross = 1;
        } else {
            work->cross = 0;
            work->config = &gDuelEffect_BoardDestructionConfigs[phase];
            copyVector(&work->origin, &gDuelEffect_DrawOffset);
            if (work->config->mode == 0) {
                Duel_CollectMatchingFieldCardObjects(gDuelEffect_CardObjectScratchWords, -1);
                setVector(&work->center, 0, -work->config->height, 0);
            } else {
                Duel_CollectMatchingFieldCardObjects(gDuelEffect_CardObjectScratchWords, 0);
                setVector(&work->center, gDuelEffect_DrawOffset.vx,
                          gDuelEffect_DrawOffset.vy - work->config->height, gDuelEffect_DrawOffset.vz);
            }
            work->card_count = 0;
            for (i = 0; gDuelEffect_CardObjectScratchWords[i] != 0; i++) {
                setVector(&work->card_velocities[i],
                          (work->center.vx - (s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30) / 16,
                          (work->center.vy - (s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32) / 16,
                          (work->center.vz - ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34) / 16);
                setVector(&work->card_rotations[i],
                          (ratan2(work->card_velocities[i].vz, work->card_velocities[i].vy) % 256) / 16,
                          (ratan2(work->card_velocities[i].vx, work->card_velocities[i].vz) % 256) / 16,
                          (ratan2(work->card_velocities[i].vy, work->card_velocities[i].vx) % 256) / 16);
                DuelEffect_InitRandomVectorsXYZ(work->card_particles[i], 128, 128, 128, 16);
                work->card_count++;
                work->card_states[i] = 0;
                work->card_ages[i] = 0;
            }
            DuelEffect_InitRandomVectorsXYZ(work->particles, work->config->radius * 2,
                         work->config->radius * 2, work->config->radius * 2, 64);
            for (i = 0; i < 64; i++) {
                setVector(&work->velocities[i], -work->particles[i].vx / 32,
                          -work->particles[i].vy / 32, -work->particles[i].vz / 32);
                DuelEffect_SetRgbColor((u8 *)&work->particle_colors[i], work->config->color[0] / 8,
                             work->config->color[1] / 8, work->config->color[2] / 8);
            }
            for (i = 0; i < 48; i++) {
                DuelEffect_InitJaggedColumnVertices(24, 8, work->config->height, 8, work->paths[i]);
                DuelEffect_SetRgbColor((u8 *)&work->path_colors[i], work->config->burst_color[0],
                             work->config->burst_color[1], work->config->burst_color[2]);
                setVector(&work->endpoints[i], 0, 0, 0);
                work->path_ages[i] = 0;
                work->path_states[i] = 0;
                work->angles[i] = rand() % 512;
            }
            work->tick = 0;
            DuelEffect_SetRgbColor((u8 *)&work->color, work->config->color[0] / 8,
                         work->config->color[1] / 8, work->config->color[2] / 8);
            DuelEffect_SetGrayColor((u8 *)&work->burst_color, 0);
            work->field_1D94 = 0;
            work->completed = 0;
            work->stage = 0;
            work->brightness = 128;
            work->active_cards = 0;
            work->active_particles = 0;
            work->active_paths = 0;
        }
    } else if (work->cross) {
        if (work->cross == 1) {
            DuelEffect_DrawErrorCross(1);
        } else {
            DuelEffect_DrawErrorCross(0);
        }
        work->cross++;
        if (work->cross > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        if (work->stage < 2) {
            if (work->brightness > 8) {
                work->brightness -= 8;
            } else {
                work->brightness = 0;
            }
        } else {
            if (work->brightness < 120) {
                work->brightness += 8;
            } else {
                work->brightness = 128;
                work->stage = 3;
            }
        }
        DuelEffect_SetFogBaseColor(work->brightness, work->brightness, work->brightness);
        setVector(&rotation, 0, 0, work->config->spin * work->tick % 4096);
        DuelEffect_SetTransformMatrix(&world, &saved, &work->center, &rotation, &scale, 0);
        for (i = 0; i < work->active_particles; i++) {
            if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->particle_colors[i])) {
                DuelEffect_InitOffsetQuadVertices(vertices, &work->particles[i]);
                if (!(i & 3)) {
                    DuelEffect_DrawLayeredTileQuad((u8 *)&work->particle_colors[i], 8, 1, &work->particles[i]);
                }
                DuelEffect_DrawRandomTileQuad((u8 *)&work->particle_colors[i], vertices, 1);
                if (__builtin_abs(work->particles[i].vx) > 32 ||
                    __builtin_abs(work->particles[i].vy) > 32 ||
                    __builtin_abs(work->particles[i].vz) > 32) {
                    addVector(&work->particles[i], &work->velocities[i]);
                    DuelEffect_StepColorToTarget((u8 *)&work->particle_colors[i], work->config->color[0],
                                 work->config->color[1], work->config->color[2], 31);
                } else {
                    DuelEffect_DarkenColor((u8 *)&work->particle_colors[i], 31);
                    if ((u16)DuelEffect_IsColorBlack((u8 *)&work->particle_colors[i]) &&
                        work->card_states[work->card_count - 1] == 0) {
                        setVector(&work->particles[i],
                                  work->config->radius * (s32)((u32)((rand() - rand()) % 4096) << 1) / 4096,
                                  work->config->radius * (s32)((u32)((rand() - rand()) % 4096) << 1) / 4096,
                                  work->config->radius * (s32)((u32)((rand() - rand()) % 4096) << 1) / 4096);
                        setVector(&work->velocities[i], -work->particles[i].vx / 32,
                                  -work->particles[i].vy / 32, -work->particles[i].vz / 32);
                        DuelEffect_SetRgbColor((u8 *)&work->particle_colors[i], work->config->color[0] / 8,
                                     work->config->color[1] / 8, work->config->color[2] / 8);
                    }
                    if (!(u16)DuelEffect_IsColorBlack((u8 *)&work->particle_colors[i]) && work->stage >= 2) {
                        work->stage = 2;
                    }
                }
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
            setPolyGT4(packet);
            packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[15][0];
            packet->clut = gDuelEffect_TexturePageClutPairs.pairs[15][1];
            setUV4(packet, 96, 0, 159, 0, 96, 63, 159, 63);
            setVector(&vertices[0], -work->config->radius, -work->config->radius, 0);
            setVector(&vertices[1], -work->config->radius, 0, 0);
            setVector(&vertices[2], 0, -work->config->radius, 0);
            setVector(&vertices[3], 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            setRGB3(packet, 0, 0, 0);
            for (j = 0; j < 3; j++) {
                setVector(&scale, 4096 - j * 1280, 4096 - j * 1280, 0);
                setRGB0(packet, work->color.r / (j + 1), work->color.g / (j + 1), work->color.b / (j + 1));
                setRGB1(packet, work->color.r / (j + 1), work->color.g / (j + 1), work->color.b / (j + 1));
                setRGB2(packet, work->color.r / (j + 1), work->color.g / (j + 1), work->color.b / (j + 1));
                for (i = 0; i < 4; i++) {
                    for (k = 0; k < 3; k++) {
                        setVector(&rotation, 0, 0,
                                  -(work->tick * ((j * 2 + 1) * work->config->fan_spin) + k * 32 + i * 1024) % 4096);
                        DuelEffect_SetTransformMatrix(&world, &saved, &work->center, &rotation, &scale, 0);
                        DuelEffect_ProjectSubmitGt4(packet, vertices, 1, 1);
                    }
                }
                if (work->field_1D94 < 4096 && work->tick < 999) {
                    work->field_1D94 += 64;
                } else if (work->tick >= 999) {
                    work->field_1D94 -= 64;
                }
                if (work->stage < 2) {
                    work->stage = DuelEffect_StepColorToTarget((u8 *)&work->color, work->config->color[0],
                                              work->config->color[1], work->config->color[0], 2);
                } else {
                    DuelEffect_DarkenColor((u8 *)&work->color, 1);
                }
            }
        }
        if (work->card_count && work->stage < 2) {
            work->active_particles++;
            if (work->active_particles > 64) {
                work->active_particles = 64;
            }
        }
        if (work->stage != 0 && work->tick > 60) {
            if (work->card_count == 0 && work->stage == 1) {
                work->stage = 2;
            }
            for (i = 0; i < work->active_cards; i++) {
                if (work->card_states[i] == 0) {
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 += work->card_velocities[i].vx;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32 += work->card_velocities[i].vy;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34 += work->card_velocities[i].vz;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_44.h.field_44 -= 256;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_44.h.field_46 -= 256;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.rotation_x += work->card_rotations[i].vx;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.rotation_y += work->card_rotations[i].vy;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.rotation_z += work->card_rotations[i].vz;
                    work->card_ages[i]++;
                    if (work->card_ages[i] > 16) {
                        work->card_states[i] = 1;
                    }
                }
                if (work->card_states[i] == 1) {
                    work->completed++;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->flags &= ~0x40;
                    work->card_states[i] = 2;
                    DuelEffect_SetRgbColor((u8 *)&work->burst_color, work->config->burst_color[0],
                                 work->config->burst_color[1], work->config->burst_color[2]);
                }
            }
            if (!(work->tick & 7)) {
                work->active_cards++;
                if (work->active_cards > work->card_count) {
                    work->active_cards = work->card_count;
                }
            }
        }
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        DuelEffect_SetTransformMatrix(&world, &saved, &work->center, &rotation, &scale, 0);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->burst_color)) {
            DuelEffect_DrawLayeredTileQuad((u8 *)&work->burst_color, 32, 1, 0);
            color = work->burst_color;
            color.r >>= 1;
            color.g >>= 1;
            color.b >>= 1;
            DuelEffect_DrawSharedBaseTriangles((u8 *)&color, 3, work->card_particles[work->completed - 1], 16, 36);
            DuelEffect_DarkenColor((u8 *)&work->burst_color, 15);
            if (work->card_states[work->card_count - 1] >= 2 &&
                (u16)DuelEffect_IsColorBlack((u8 *)&work->burst_color)) {
                work->stage = 2;
            }
        }
        if (work->config->mode && work->card_count) {
            for (i = 0; i < work->active_paths; i++) {
                setVector(&rotation, 0, work->angles[i], 0);
                DuelEffect_SetTransformMatrix(&world, &saved, &work->origin, &rotation, &scale, 2);
                if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->path_colors[i])) {
                    if (work->path_states[i] != 2) {
                        work->path_states[i] = DuelEffect_DrawGradientPath((u8 *)&work->path_colors[i], work->path_ages[i],
                            work->paths[i], 16, 1, &work->endpoints[i], 1, &work->origin);
                    } else {
                        DuelEffect_DrawGradientPath((u8 *)&work->path_colors[i], work->path_ages[i],
                            work->paths[i], 32, 0, &work->endpoints[i], 1, &work->origin);
                    }
                    if (work->path_states[i] == 2 &&
                        (u16)DuelEffect_HasNonzeroColor((u8 *)&work->path_colors[i])) {
                        DuelEffect_SetTransformMatrix(&world, &saved, &work->endpoints[i], &rotation, &scale, 0);
                        DuelEffect_DrawLayeredTileQuad((u8 *)&work->path_colors[i], 4, 1, 0);
                    }
                    work->path_ages[i]++;
                    if (work->path_ages[i] >= 8) {
                        work->path_ages[i] = 7;
                        DuelEffect_DarkenColor((u8 *)&work->path_colors[i], 15);
                        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->path_colors[i]) && work->stage < 2) {
                            DuelEffect_SetRgbColor((u8 *)&work->path_colors[i], work->config->burst_color[0],
                                         work->config->burst_color[1], work->config->burst_color[2]);
                            work->path_ages[i] = 0;
                        }
                    }
                }
            }
        }
        if (work->stage == 1) {
            work->active_paths++;
            if (work->active_paths > 48) {
                work->active_paths = 48;
            }
        }
        work->tick++;
        PopMatrix();
        if (work->stage == 3 && work->tick > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
