#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_5.h"
#include "fallback_semantic_views.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "recovery_semantic_views.h"

#ifdef VERSION_EUROPE
#define EFFECT_5_RISING_HEIGHT 106
#else
#define EFFECT_5_RISING_HEIGHT 98
#endif

void DuelEffect_RunLifePointRecovery(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX matrix;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    DuelEffect5Work *work;
    s32 i;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_LifePointRecoveryInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 10) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_LifePointRecoveryConfigs[phase];
            for (i = 0; i < 3; i++) {
                DuelEffect_InitCircle32Vertices(work->config->radii[i], work->rings[i]);
            }
            for (i = 0; i < 64; i++) {
                setVector(&work->rising_positions[i],
                          (rand() - rand()) % 4096 * 70 / 4096, -EFFECT_5_RISING_HEIGHT,
                          (rand() - rand()) % 4096 * 70 / 4096);
                DuelEffect_SetGrayColor((u8 *)&work->rising_colors[i], 1);
            }
            setVector(&work->number_position, 0, 0, 0);
            setVector(&work->number_velocity, 0, -work->config->bounce_speed, 0);
            DuelEffect_InitParticleVectors(work->config->particle_speed, work->positions,
                         work->velocities, work->config->particle_count);
            DuelEffect_InitRandomVectorsUnscaled(work->config->strip_count, work->rotations);
            DuelEffect_SetGrayColor((u8 *)&work->background_color, 1);
            DuelEffect_SetRgbColor((u8 *)&work->color, work->config->color[0],
                         work->config->color[1], work->config->color[2]);
            DuelEffect_SetRgbColor((u8 *)&work->number_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
            work->spawned = 0;
            work->bounces = 1;
            work->hold_frames = 0;
            work->number_state = 0;
            work->scale = 4096;
            work->frame = 0;
            work->tick = 0;
            work->variant = phase;
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
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        DuelEffect_InitBaseMatrix(&world);
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
        for (i = 0; i < work->spawned; i++) {
            if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->rising_colors[i])) {
                DuelEffect_InitOffsetQuadVertices(quad, &work->rising_positions[i]);
                DuelEffect_DrawRandomTileQuad((u8 *)&work->rising_colors[i], quad, 0);
                work->rising_positions[i].vy += work->config->rise_speed;
            }
            if (work->rising_positions[i].vy < 0) {
                DuelEffect_StepColorToTarget((u8 *)&work->rising_colors[i], work->config->color[0],
                             work->config->color[1], work->config->color[2], 31);
            } else {
                DuelEffect_DarkenColor((u8 *)&work->rising_colors[i], 15);
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->background_color)) {
            DuelEffect_DrawFullscreenGradient((u8 *)&work->background_color);
            if (work->rising_positions[work->config->strip_count - 1].vy < 0) {
                DuelEffect_StepColorToTarget((u8 *)&work->background_color,
                             work->config->color[0] >> 1,
                             work->config->color[1] >> 1,
                             work->config->color[2] >> 1, 4);
            } else if (work->variant < 5) {
                DuelEffect_DarkenColor((u8 *)&work->background_color, 4);
            }
        }
        if (work->rising_positions[work->config->strip_count - 1].vy > 0) {
            if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->number_color)) {
                DuelEffect_DrawProjectedNumber(work->config->number, (u8 *)&work->number_color,
                             &work->number_position, work->number_state, 16, 0);
                addVector(&work->number_position, &work->number_velocity);
                work->number_velocity.vy += 4;
                if (work->number_position.vy >= 0) {
                    work->number_position.vy = 0;
                    if (work->bounces < 5) {
                        work->bounces++;
                        work->number_velocity.vy = -work->config->bounce_speed / work->bounces;
                    } else {
                        if (work->variant < 5) {
                            work->number_state = 1;
                            work->number_velocity.vy = 0;
                            DuelEffect_DarkenColor((u8 *)&work->number_color, 15);
                        } else {
                            work->hold_frames++;
                        }
                    }
                }
            }
            if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
                for (i = 0; i < work->config->particle_count; i++) {
                    DuelEffect_InitOffsetQuadVertices(quad, &work->positions[i]);
                    DuelEffect_DrawRandomTileQuad((u8 *)&work->color, quad, 0);
                    if (work->variant < 5 || work->hold_frames == 0) {
                        addVector(&work->positions[i], &work->velocities[i]);
                    }
                }
                scale.vx = work->scale >> 3;
                scale.vy = work->scale >> 3;
                scale.vz = 4096;
                matrix = saved;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                DuelEffect_DrawGradientRingStrip((u8 *)&work->color, work->rings[0],
                             work->rings[1], work->rings[2], 0);
                scale.vx = work->scale;
                scale.vy = work->scale;
                scale.vz = 4096;
                matrix = saved;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                DuelEffect_DrawLayeredTileQuad((u8 *)&work->color, work->config->sprite_size,
                             0, &position);
                scale.vx = 4096;
                scale.vy = work->scale;
                scale.vz = 4096;
                for (i = 0; i < work->config->strip_count; i++) {
                    DuelEffect_SetTransformMatrix(&world, &saved, &position, &work->rotations[i],
                                 &scale, 0);
                    DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&work->color, work->config->widths,
                                 work->config->height, 0);
                    if (work->variant < 5 || work->hold_frames == 0) {
                        applyVector(&work->rotations[i], 128, 128, 128, +=);
                    }
                }
                if (work->variant < 5) {
                    DuelEffect_DarkenColor((u8 *)&work->color, 15);
                }
                if (work->scale < 0x7000) {
                    work->scale += 0x1000;
                }
            }
        }
        work->spawned += frame_step;
        if (work->spawned > work->config->strip_count) {
            work->spawned = work->config->strip_count;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->background_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->number_color) && work->variant < 5) {
            gDuelEffect_bRequestFinished = 1;
        } else if ((u16)DuelEffect_IsColorBlack((u8 *)&work->rising_colors[work->config->strip_count - 1]) &&
                   work->variant >= 5 && work->hold_frames >= 24) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
