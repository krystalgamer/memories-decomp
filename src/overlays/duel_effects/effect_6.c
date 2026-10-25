#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_6.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "texture_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "damage_semantic_views.h"
#include "fallback_semantic_views.h"

#ifdef VERSION_EUROPE
#define EFFECT_6_START_HEIGHT 106
#define EFFECT_6_TARGET_SPREAD 13
#else
#define EFFECT_6_START_HEIGHT 98
#define EFFECT_6_TARGET_SPREAD 12
#endif

void DuelEffect_RunDirectDamage(void *buffer, s32 phase)
{
    MATRIX world;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color;
    MATRIX saved;
    DuelEffect6Work *work;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_DirectDamageInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_DirectDamageConfigs[phase];
            for (i = 0; i < 16; i++) {
                x = (rand() - rand()) % 4096;
                y = (rand() - rand()) % 4096;
                for (j = 0; j < 4; j++) {
                    setVector(&work->trails[i][j],
                              x * 70 / 4096, y * EFFECT_6_START_HEIGHT / 4096, 0);
                }
                work->velocities[i].vx =
                    (((rand() - rand()) % 4096) * 35 / 4096 -
                     work->trails[i][0].vx) / work->config->duration;
                work->velocities[i].vy =
                    (work->config->target_y +
                     ((rand() - rand()) % 4096) * EFFECT_6_TARGET_SPREAD / 4096 -
                     work->trails[i][0].vy) / work->config->duration;
                DuelEffect_SetGrayColor((u8 *)&work->colors[i], 1);
                DuelEffect_SetRgbColor((u8 *)&work->colors[i],
                             work->config->color[0], work->config->color[1],
                             work->config->color[2]);
                work->ages[i] = 0;
                work->sizes[i] = work->config->size;
                work->scales[i] = 0;
                work->states[i] = 0;
                DuelEffect_InitParticleVectors(work->config->particle_speed, work->particles[i],
                             work->particle_velocities[i], 32);
            }
            setVector(&work->column, 0, -60, 0);
            work->column_step = 12;
            work->column_size = 3;
            for (i = 0; i < 3; i++) {
                DuelEffect_InitCircle32Vertices(work->config->radii[i], work->rings[i]);
            }
            DuelEffect_SetGrayColor((u8 *)&work->background_color, 1);
            DuelEffect_SetRgbColor((u8 *)&work->column_color,
                         work->config->column_color[0],
                         work->config->column_color[1],
                         work->config->column_color[2]);
            work->column_mode = 0;
            work->active = 0;
            work->hold = 0;
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
        for (i = 0; i < work->active; i++) {
            if (work->states[i] == 0) {
                setVector(&scale, 4096, 4096, 4096);
                DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
                for (j = 0; j < 4; j++) {
                    color = work->colors[i];
                    color.r /= j + 1;
                    color.g /= j + 1;
                    color.b /= j + 1;
                    DuelEffect_DrawFixedTextureQuad((u8 *)&color, work->sizes[i] / (j + 1),
                                 &work->trails[i][j]);
                    work->trails[i][j].vx += work->velocities[i].vx / (j + 1);
                    work->trails[i][j].vy += work->velocities[i].vy / (j + 1);
                }
                if (work->ages[i] >= work->config->duration) {
                    work->states[i] = 1;
                } else {
                    work->ages[i]++;
                    work->sizes[i] -= work->config->shrink;
                }
            } else if (work->states[i] == 1) {
                gDuelEffect_pActiveRequest->field_1D = 1;
                if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->colors[i]) &&
                    work->variant < 5) {
                    setVector(&scale, 4096, 4096, 4096);
                    DuelEffect_SetTransformMatrix(&world, &saved, &work->trails[i][0],
                                 &rotation, &scale, 1);
                    for (j = 0; j < work->config->particle_count; j++) {
                        DuelEffect_DrawFixedTextureQuad((u8 *)&work->colors[i],
                                     work->config->particle_size,
                                     &work->particles[i][j]);
                        addVector(&work->particles[i][j],
                                  &work->particle_velocities[i][j]);
                    }
                    scale.vx = work->scales[i] >> 1;
                    scale.vy = work->scales[i] >> 1;
                    scale.vz = 0;
                    DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
                    DuelEffect_DrawTexturedRingStrip((u8 *)&work->colors[i], work->rings[0],
                                 work->rings[1], work->rings[2], 0);
                    DuelEffect_DrawLayeredTileQuad((u8 *)&work->colors[i], work->sizes[i] * 2,
                                 0, &position);
                    if (work->scales[i] < 0x6000) {
                        work->scales[i] += 0x1000;
                    }
                    DuelEffect_DarkenColor((u8 *)&work->colors[i], 15);
                } else {
                    if ((u16)DuelEffect_IsColorBlack((u8 *)&work->colors[i])) {
                        work->states[i] = 2;
                    }
                }
            }
        }
        if (!(work->tick & 1)) {
            work->active++;
            if (work->active > work->config->count) {
                work->active = work->config->count;
            }
        }
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
        if (work->states[work->config->count - 1] != 0 &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->column_color) &&
            work->variant < 5) {
            DuelEffect_DrawProjectedNumber(work->config->column_value, (u8 *)&work->column_color,
                         &work->column, work->column_mode, work->column_size, 0);
            if (work->column_size >= 15) {
                if (work->hold >= 32) {
                    work->column_mode = 1;
                    DuelEffect_DarkenColor((u8 *)&work->column_color, 15);
                } else {
                    work->hold++;
                }
            } else {
                work->column.vy += work->column_step;
                work->column_size += 3;
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->background_color) &&
            work->variant < 5) {
            DuelEffect_DrawFullscreenGradient((u8 *)&work->background_color);
            if (work->states[work->config->count - 1] == 0) {
                DuelEffect_StepColorToTarget((u8 *)&work->background_color,
                             work->config->color[0] >> 1,
                             work->config->color[1] >> 1,
                             work->config->color[2] >> 1, 4);
            } else {
                DuelEffect_DarkenColor((u8 *)&work->background_color, 4);
            }
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if (work->variant >= 5) {
            if (work->states[work->config->count - 1] != 0) {
                gDuelEffect_bRequestFinished = 1;
            }
        } else {
            if ((u16)DuelEffect_IsColorBlack((u8 *)&work->colors[work->config->count - 1]) &&
                (u16)DuelEffect_IsColorBlack((u8 *)&work->background_color) &&
                (u16)DuelEffect_IsColorBlack((u8 *)&work->column_color)) {
                gDuelEffect_bRequestFinished = 1;
            }
        }
    }
}
