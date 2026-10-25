#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_7.h"
#include "fallback_semantic_views.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "texture_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "damage_semantic_views.h"

#ifdef VERSION_EUROPE
#define EFFECT_7_TRAIL_SPREAD 13
#define EFFECT_7_TARGET_HEIGHT 106
#else
#define EFFECT_7_TRAIL_SPREAD 12
#define EFFECT_7_TARGET_HEIGHT 98
#endif

void DuelEffect_RunReflectedDirectDamage(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX matrix;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color;
    MATRIX saved;
    DuelEffect7Work *work;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_ReflectedDirectDamageInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 5) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_ReflectedDirectDamageConfigs[phase];
            for (i = 0; i < 16; i++) {
                x = (rand() - rand()) % 4096;
                y = (rand() - rand()) % 4096;
                for (j = 0; j < 4; j++) {
                    setVector(&work->trails[i][j], x * 35 / 4096,
                              work->config->start_y + y * EFFECT_7_TRAIL_SPREAD / 4096, 0);
                }
                work->velocities[i].vx =
                    (((rand() - rand()) % 4096) * 70 / 4096 -
                     work->trails[i][0].vx) / work->config->duration;
                work->velocities[i].vy =
                    (((rand() - rand()) % 4096) * EFFECT_7_TARGET_HEIGHT / 4096 -
                     work->trails[i][0].vy) / work->config->duration;
                DuelEffect_SetRgbColor((u8 *)&work->colors[i], work->config->color[0],
                             work->config->color[1], work->config->color[2]);
                work->ages[i] = 0;
                work->sizes[i] = work->config->size;
                work->scales[i] = 0;
                work->states[i] = 0;
                DuelEffect_InitParticleVectors(work->config->particle_speed, work->particles[i],
                             work->particle_velocities[i], 32);
            }
            setVector(&work->number_position, 0, 0, 0);
            for (i = 0; i < 3; i++) {
                DuelEffect_InitCircle32Vertices(work->config->radii[i], work->rings[i]);
            }
            DuelEffect_SetGrayColor((u8 *)&work->background_color, 1);
            DuelEffect_SetRgbColor((u8 *)&work->number_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
            work->number_mode = 0;
            work->active = 0;
            work->bounces = 0;
            work->frame = 0;
            work->tick = 0;
            setVector(&work->number_position, 0, 0, 0);
            setVector(&work->number_velocity, 0, -16, 0);
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
                    work->sizes[i] += work->config->growth;
                }
            } else if (work->states[i] == 1) {
                gDuelEffect_pActiveRequest->field_1D = 1;
                if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->colors[i])) {
                    setVector(&scale, 4096, 4096, 4096);
                    DuelEffect_SetTransformMatrix(&world, &saved, &work->trails[i][0],
                                 &rotation, &scale, 1);
                    for (j = 0; j < work->config->particle_count; j++) {
                        DuelEffect_DrawFixedTextureQuad((u8 *)&work->colors[i], work->config->particle_size,
                                     &work->particles[i][j]);
                        addVector(&work->particles[i][j],
                                  &work->particle_velocities[i][j]);
                    }
                    scale.vx = work->scales[i] >> 1;
                    scale.vy = work->scales[i] >> 1;
                    scale.vz = 0;
                    matrix = saved;
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    DuelEffect_DrawTexturedRingStrip((u8 *)&work->colors[i], work->rings[0],
                                 work->rings[1], work->rings[2], 0);
                    DuelEffect_DrawLayeredTileQuad((u8 *)&work->colors[i], work->sizes[i], 0, &position);
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
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->number_color)) {
            DuelEffect_DrawProjectedNumber(work->config->number, (u8 *)&work->number_color,
                         &work->number_position, work->number_mode, 16, 0);
            addVector(&work->number_position, &work->number_velocity);
            work->number_velocity.vy += 4;
            if (work->number_position.vy >= 0) {
                work->number_position.vy = 0;
                if (work->bounces < 5) {
                    work->bounces++;
                    work->number_velocity.vy = -16 / work->bounces;
                } else {
                    work->number_mode = 1;
                    work->number_velocity.vy = 0;
                    DuelEffect_DarkenColor((u8 *)&work->number_color, 15);
                }
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->background_color)) {
            DuelEffect_DrawFullscreenGradient((u8 *)&work->background_color);
            if (work->states[work->config->count - 1] == 0) {
                DuelEffect_StepColorToTarget((u8 *)&work->background_color,
                             work->config->color[0] >> 1, work->config->color[1] >> 1,
                             work->config->color[2] >> 1, 4);
            } else {
                DuelEffect_DarkenColor((u8 *)&work->background_color, 4);
            }
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->colors[work->config->count - 1]) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->background_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->number_color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
