#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_22.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "primitive_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "ritual_semantic_views.h"
#include "exodia_semantic_views.h"
#include "fallback_semantic_views.h"

void DuelEffect_RunRitual(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    POLY_GT4 gradient;
    GsGLINE line;
    SVECTOR vertices[4];
    CVECTOR color;
    MATRIX backup;
    s32 flag;
    s32 p;
    s32 depth;
    POLY_FT4 *packet;
    POLY_GT4 *gradient_packet;
    GsGLINE *line_packet;
    DuelEffect22Work *work;
    s32 i;
    s32 j;
    s32 k;
    s32 z;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_RitualInitialScale;
    packet = &polygon;
    gradient_packet = &gradient;
    line_packet = &line;
    work = buffer;
    if (phase >= 0) {
        work->cross_frame = 0;
        switch (phase) {
        case 671: work->config = &gDuelEffect_RitualConfigs[1]; break;
        case 673: work->config = &gDuelEffect_RitualConfigs[2]; break;
        case 674: work->config = &gDuelEffect_RitualConfigs[3]; break;
        case 675: work->config = &gDuelEffect_RitualConfigs[4]; break;
        case 676: work->config = &gDuelEffect_RitualConfigs[5]; break;
        case 677: work->config = &gDuelEffect_RitualConfigs[6]; break;
        case 678: work->config = &gDuelEffect_RitualConfigs[7]; break;
        case 679: work->config = &gDuelEffect_RitualConfigs[8]; break;
        case 680: work->config = &gDuelEffect_RitualConfigs[9]; break;
        case 691: work->config = &gDuelEffect_RitualConfigs[10]; break;
        case 692: work->config = &gDuelEffect_RitualConfigs[11]; break;
        case 693: work->config = &gDuelEffect_RitualConfigs[12]; break;
        case 694: work->config = &gDuelEffect_RitualConfigs[13]; break;
        case 695: work->config = &gDuelEffect_RitualConfigs[14]; break;
        case 696: work->config = &gDuelEffect_RitualConfigs[15]; break;
        case 697: work->config = &gDuelEffect_RitualConfigs[16]; break;
        case 698: work->config = &gDuelEffect_RitualConfigs[17]; break;
        case 699: work->config = &gDuelEffect_RitualConfigs[18]; break;
        case 700: work->config = &gDuelEffect_RitualConfigs[19]; break;
        case 721: work->config = &gDuelEffect_RitualConfigs[20]; break;
        case 665: work->config = &gDuelEffect_RitualConfigs[21]; break;
        case 666: work->config = &gDuelEffect_RitualConfigs[22]; break;
        case 667: work->config = &gDuelEffect_RitualConfigs[23]; break;
        default: work->cross_frame = 1;
        case 670: work->config = &gDuelEffect_RitualConfigs[0]; break;
        }
        if (work->cross_frame == 1) {
            return;
        }
        Duel_CheckRitual((DuelRitualResult *)gDuelEffect_CardObjectScratchWords, phase);
        setVector(&work->positions[0], 70, -48,
                  (s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[0])->field_34.h.field_34 >= 0 ? 190 : -190);
        setVector(&work->positions[1], -70, -48,
                  (s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[0])->field_34.h.field_34 >= 0 ? 190 : -190);
        work->texture_frames[0] = rand() % 4;
        work->texture_frames[1] = rand() % 4;
        for (i = 0; i < 3; i++) {
            setVector(&work->card_velocities[i],
                      -(s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 / 32,
                      (-96 - (s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32) / 32,
                      -(s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34 / 32);
            setVector(&work->card_rotations[i], 2,
                      -((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_21 / 32,
                      -((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_22 / 32);
            work->card_states[i] = 0;
        }
        for (i = 0; i < 2; i++) {
            DuelEffect_InitHalfStepXYRingVertices(24 + 6 * i, 28 + 6 * i, work->rings[i], 4);
        }
        DuelEffect_InitRandomVectorsXY(work->points, work->config->spread, work->config->spread, 32);
        DuelEffect_InitRandomVectorsUnscaled(32, work->rotations);
        DuelEffect_InitRandomVectorsXYZ(work->particles, 392, 392, 392, 32);
        for (i = 0; i < 64; i++) {
            setVector(&work->particle_velocities[i],
                      -work->particles[i].vx / 8,
                      -work->particles[i].vy / 8,
                      -work->particles[i].vz / 8);
            DuelEffect_SetRgbColor((u8 *)&work->colors[i],
                         work->config->color[0], work->config->color[1], work->config->color[2]);
            j = (rand() - rand()) % 4096;
            k = (rand() - rand()) % 4096;
            z = (rand() - rand()) % 4096;
            setVector(&work->line_inner[i], j * 328 / 4096, k * 328 / 4096, z * 328 / 4096);
            setVector(&work->line_outer[i], j * 392 / 4096, k * 392 / 4096, z * 392 / 4096);
            setVector(&work->line_velocities[i],
                      -work->line_outer[i].vx / 8,
                      -work->line_outer[i].vy / 8,
                      -work->line_outer[i].vz / 8);
        }
        work->ring_scale = 4096;
        work->brightness = 128;
        work->active_cards = 1;
        work->stage = 0;
        work->sound_played = 0;
        work->frame = 0;
        work->hold = 0;
        work->active_particles = 0;
        work->fan_speed = 8;
        DuelEffect_SetGrayColor((u8 *)&work->portal_color, 1);
        DuelEffect_SetGrayColor((u8 *)&work->flame_color, 1);
        DuelEffect_SetRgbColor((u8 *)&work->flash_color,
                     work->config->color[0], work->config->color[1], work->config->color[2]);
        DuelEffect_SetRgbColor((u8 *)&work->particle_color,
                     work->config->color[0], work->config->color[1], work->config->color[2]);
        DuelEffect_SetRgbColor((u8 *)&work->fan_color,
                     work->config->fan_color[0] / 8,
                     work->config->fan_color[1] / 8,
                     work->config->fan_color[2] / 8);
    } else {
        if (work->cross_frame) {
            if (work->cross_frame == 1) {
                DuelEffect_DrawErrorCross(1);
            } else {
                DuelEffect_DrawErrorCross(0);
            }
            work->cross_frame++;
            if (work->cross_frame > 180) {
                gDuelEffect_bRequestFinished = 1;
            }
            return;
        }
        Model_SetFrameStepOverride(1);
        if (work->stage == 0) {
            if (work->brightness > 4) {
                work->brightness -= 4;
            } else {
                work->brightness = 0;
            }
        } else if (work->stage == 4) {
            if (work->brightness < 126) {
                work->brightness += 2;
            } else {
                work->brightness = 128;
                work->stage = 5;
            }
        }
        DuelEffect_SetFogBaseColor(work->brightness, work->brightness, work->brightness);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 2);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->portal_color)) {
            for (i = 0; i < 2; i++) {
                setPolyFT4(packet);
                packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[17][0];
                packet->clut = gDuelEffect_TexturePageClutPairs.pairs[17][1];
                setRGB0(packet, work->portal_color.r, work->portal_color.g, work->portal_color.b);
                setUV4(packet, 96, 64, 159, 64, 96, 127, 159, 127);
                DuelEffect_InitRectVertices(vertices, (s16)((s16)DuelEffect_PowS16(-1, i + 1) * 24), 24);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->positions[i]);
                }
                if (work->portal_color.r == 128 && work->portal_color.g == 128 && work->portal_color.b == 128) {
                    DuelEffect_ProjectSubmitFt4(packet, vertices, 1, 0);
                } else {
                    DuelEffect_ProjectSubmitFt4(packet, vertices, 1, 1);
                }
            }
            if (work->stage < 4) {
                DuelEffect_StepColorToTarget((u8 *)&work->portal_color, 128, 128, 128, 4);
            } else {
                DuelEffect_DarkenColor((u8 *)&work->portal_color, 8);
            }
        }
        if (work->portal_color.r == 128 && work->portal_color.g == 128 && work->portal_color.b == 128 &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->flame_color)) {
            setPolyFT4(packet);
            setRGB0(packet, work->flame_color.r, work->flame_color.g, work->flame_color.b);
            for (i = 0; i < 2; i++) {
                packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[4][0];
                packet->clut = gDuelEffect_TexturePageClutPairs.pairs[4][1];
                setUV4(packet, 128 + work->texture_frames[i] * 32, 64,
                       159 + work->texture_frames[i] * 32, 64,
                       128 + work->texture_frames[i] * 32, 127,
                       159 + work->texture_frames[i] * 32, 127);
                DuelEffect_InitRectVertices(vertices, 24, 36);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->positions[i]);
                    vertices[j].vy -= 48;
                }
                DuelEffect_ProjectSubmitFt4(packet, vertices, 1, 1);
                /* Separate gates preserve the original nested-loop pointer allocation. */
                if (work->stage >= 2) {
                    work->texture_frames[i] = (work->texture_frames[i] + 1) % 4;
                } else if ((u16)(work->frame % 3) == 0) {
                    work->texture_frames[i] = (work->texture_frames[i] + 1) % 4;
                }
            }
            if (work->stage < 4) {
                if (work->sound_played == 0) {
                    SD_SEPlayFull(31);
                    work->sound_played = 1;
                }
                DuelEffect_StepColorToTarget((u8 *)&work->flame_color, 128, 128, 128, 4);
            } else {
                DuelEffect_DarkenColor((u8 *)&work->flame_color, 8);
            }
        }
        if (work->flame_color.r == 128 && work->flame_color.g == 128 && work->flame_color.b == 128 &&
            work->stage == 0) {
            for (i = 0; i < work->active_cards; i++) {
                if ((s16)((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32 <= -96) {
                    if (work->card_states[i] == 0) {
                        ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 = 0;
                        *(s16 *)&((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32 = -96;
                        ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34 = 0;
                        work->card_states[i] = 1;
                        work->active_cards++;
                        if (work->active_cards > 3) {
                            work->active_cards = 3;
                        }
                    }
                } else {
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30 += work->card_velocities[i].vx;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32 += work->card_velocities[i].vy;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34 += work->card_velocities[i].vz;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_20 += work->card_rotations[i].vx;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_21 += work->card_rotations[i].vy;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_20.b.field_22 += work->card_rotations[i].vz;
                }
            }
            if (work->card_states[2] == 1) {
                for (i = 0; i < 3; i++) {
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_44.h.field_44 = 0;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_44.h.field_46 = 0;
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->flags &= ~0x40;
                    work->stage = 1;
                }
            }
        }
        if ((work->stage == 1 || work->stage == 2) &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->flash_color)) {
            if (work->stage == 1) {
                work->stage = 2;
            }
            setVector(&position, 0, -96, 0);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
            DuelEffect_DrawSharedBaseTriangles((u8 *)&work->flash_color, work->config->point_size, work->points, 32, 1);
            DuelEffect_DrawQuadContour((u8 *)&work->flash_color, work->rings[0], work->rings[1], 1, 1);
            setVector(&scale, work->ring_scale, work->ring_scale, 4096);
            DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
            setVector(&position, 0, 0, 0);
            DuelEffect_DrawLayeredTileQuad((u8 *)&work->flash_color, 12, 1, &position);
            DuelEffect_DrawFadingFullscreenColor((u8 *)&work->flash_color, 8);
            if (work->ring_scale < 28672) {
                work->ring_scale += 4096;
            }
        }
        if ((work->stage == 2 || work->stage == 3) &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->particle_color)) {
            if (work->frame % 2 == 0) {
                DuelEffect_SetGrayColor((u8 *)&color, 255);
            } else {
                color = work->particle_color;
            }
            setVector(&scale, 4096, 4096, 4096);
            setVector(&position, 0, -96, 0);
            GsSetLsMatrix(&world);
            RotTrans(&position, (VECTOR *)&saved.t[0], (PSXLONG *)&flag);
            ScaleMatrix(&saved, &scale);
            backup = saved;
            for (i = 0; i < 32; i++) {
                saved = backup;
                RotMatrix(&work->rotations[i], &saved);
                GsSetLsMatrix(&saved);
                DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&color, work->config->inner_widths, work->config->inner_height, 1);
                applyVector(&work->rotations[i], 16, 16, 16, +=);
            }
            setVector(&rotation, 0, 0, -(work->frame * 12) % 4096);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
            setVector(&position, 0, 0, 0);
            DuelEffect_DrawLayeredTileQuad((u8 *)&color, 32, 64, &position);
            setVector(&rotation, 0, 0, 0);
            setVector(&position, 0, -96, 0);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 2);
            for (i = 0; i < work->active_particles; i++) {
                line_packet->attribute = 0x50000000;
                setRGB0(line_packet, work->particle_color.r, work->particle_color.g, work->particle_color.b);
                setRGB1(line_packet, 0, 0, 0);
                RotTransPers(&work->line_inner[i], (PSXLONG *)&line_packet->x0, (PSXLONG *)&p, (PSXLONG *)&flag);
                depth = RotTransPers(&work->line_outer[i], (PSXLONG *)&line_packet->x1, (PSXLONG *)&p, (PSXLONG *)&flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortGLine(line_packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2));
                }
                if (__builtin_abs(work->line_inner[i].vx) > 64 ||
                    __builtin_abs(work->line_inner[i].vy) > 64 ||
                    __builtin_abs(work->line_inner[i].vz) > 64) {
                    addVector(&work->line_inner[i], &work->line_velocities[i]);
                    addVector(&work->line_outer[i], &work->line_velocities[i]);
                } else {
                    j = (rand() - rand()) % 4096;
                    k = (rand() - rand()) % 4096;
                    z = (rand() - rand()) % 4096;
                    setVector(&work->line_inner[i], j * 328 / 4096, k * 328 / 4096, z * 328 / 4096);
                    setVector(&work->line_outer[i], j * 392 / 4096, k * 392 / 4096, z * 392 / 4096);
                    setVector(&work->line_velocities[i],
                              -work->line_outer[i].vx / 8,
                              -work->line_outer[i].vy / 8,
                              -work->line_outer[i].vz / 8);
                }
            }
            if (work->frame % 2 == 0) {
                work->active_particles++;
                if (work->active_particles > 64) {
                    work->active_particles = 64;
                }
            }
            if (work->active_particles == 64) {
                work->hold++;
            }
            if (work->hold > work->config->duration) {
                work->stage = 3;
            }
        }
        if (work->stage == 2 || work->stage == 3) {
            for (i = 0; i < work->active_particles; i++) {
                if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->colors[i])) {
                    DuelEffect_InitOffsetQuadVertices(vertices, &work->particles[i]);
                    if (i % 4 == 0) {
                        DuelEffect_DrawLayeredTileQuad((u8 *)&work->colors[i], 4, 32, &work->particles[i]);
                    }
                    DuelEffect_DrawRandomTileQuad((u8 *)&work->colors[i], vertices, 32);
                    if (__builtin_abs(work->particles[i].vx) > 32 ||
                        __builtin_abs(work->particles[i].vy) > 32 ||
                        __builtin_abs(work->particles[i].vz) > 32) {
                        addVector(&work->particles[i], &work->particle_velocities[i]);
                    } else {
                        DuelEffect_DarkenColor((u8 *)&work->colors[i], 31);
                        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->colors[i])) {
                            setVector(&work->particles[i],
                                      ((rand() - rand()) % 4096) * 392 / 4096,
                                      ((rand() - rand()) % 4096) * 392 / 4096,
                                      ((rand() - rand()) % 4096) * 392 / 4096);
                            setVector(&work->particle_velocities[i],
                                      -work->particles[i].vx / 8,
                                      -work->particles[i].vy / 8,
                                      -work->particles[i].vz / 8);
                            DuelEffect_SetRgbColor((u8 *)&work->colors[i],
                                         work->config->color[0], work->config->color[1], work->config->color[2]);
                        }
                    }
                }
            }
        }
        if ((work->stage == 2 || work->stage == 3) &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->fan_color)) {
            setVector(&position, 0, -96, 0);
            setPolyGT4(gradient_packet);
            gradient_packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[15][0];
            gradient_packet->clut = gDuelEffect_TexturePageClutPairs.pairs[15][1];
            setUV4(gradient_packet, 96, 0, 159, 0, 96, 63, 159, 63);
            setVector(&vertices[0], -196, -196, 0);
            setVector(&vertices[1], -196, 0, 0);
            setVector(&vertices[2], 0, -196, 0);
            setVector(&vertices[3], 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            setRGB3(gradient_packet, 0, 0, 0);
            for (i = 0; i < 3; i++) {
                setVector(&scale, 4096 - 1280 * i, 4096 - 1280 * i, 0);
                setRGB0(gradient_packet, work->fan_color.r / (i + 1),
                        work->fan_color.g / (i + 1), work->fan_color.b / (i + 1));
                setRGB1(gradient_packet, work->fan_color.r / (i + 1),
                        work->fan_color.g / (i + 1), work->fan_color.b / (i + 1));
                setRGB2(gradient_packet, work->fan_color.r / (i + 1),
                        work->fan_color.g / (i + 1), work->fan_color.b / (i + 1));
                for (j = 0; j < 4; j++) {
                    for (k = 0; k < 3; k++) {
                        setVector(&rotation, 0, 0,
                                  -(work->frame * ((i * 2 + 1) * work->fan_speed) +
                                    k * 32 + j * 1024) % 4096);
                        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
                        DuelEffect_ProjectSubmitGt4(gradient_packet, vertices, 1, 1);
                    }
                }
            }
            DuelEffect_StepColorToTarget((u8 *)&work->fan_color, work->config->fan_color[0],
                         work->config->fan_color[1], work->config->fan_color[2], 2);
            work->fan_speed++;
            if (work->fan_speed > 24) {
                work->fan_speed = 24;
            }
        }
        if (work->stage == 3) {
            DuelEffect_DrawFullscreenColor((u8 *)&work->flash_color);
            if ((u16)DuelEffect_StepColorToTarget((u8 *)&work->flash_color, 192, 192, 192, 4)) {
                work->stage = 4;
                DuelEffect_SetGrayColor((u8 *)&work->flash_color, 255);
                DuelEffect_SetRgbColor((u8 *)&work->particle_color,
                             work->config->color[0], work->config->color[1], work->config->color[2]);
                DuelEffect_InitParticleVectors(4, work->particles, work->particle_velocities, 64);
            }
        }
        if (work->stage >= 4) {
            gDuelEffect_pActiveRequest->field_1D = 1;
            DuelEffect_DrawFadingFullscreenColor((u8 *)&work->flash_color, 4);
            setVector(&position, 0, -96, 0);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
            for (i = 0; i < 64; i++) {
                DuelEffect_InitOffsetQuadVertices(vertices, &work->particles[i]);
                DuelEffect_DrawRandomTileQuad((u8 *)&work->particle_color, vertices, 8);
                addVector(&work->particles[i], &work->particle_velocities[i]);
            }
            setVector(&position, 0, 0, 0);
            DuelEffect_DrawLayeredTileQuad((u8 *)&work->particle_color, 128, 1, &position);
            setVector(&position, 0, -96, 0);
            GsSetLsMatrix(&world);
            RotTrans(&position, (VECTOR *)&saved.t[0], (PSXLONG *)&flag);
            ScaleMatrix(&saved, &scale);
            backup = saved;
            color = work->particle_color;
            color.r /= 2;
            color.g /= 2;
            color.b /= 2;
            for (i = 0; i < 32; i++) {
                saved = backup;
                RotMatrix(&work->rotations[i], &saved);
                GsSetLsMatrix(&saved);
                DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&work->particle_color,
                             work->config->outer_widths, work->config->outer_height, 1);
            }
            DuelEffect_DarkenColor((u8 *)&work->particle_color, 4);
        }
        work->frame++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->flash_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->flash_color) &&
            work->stage == 5) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
