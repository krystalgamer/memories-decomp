#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_21.h"
#include "../../game/shared_work_base.h"
#include "geometry_semantic_views.h"
#include "display_semantic_views.h"
#include "swords_field_semantic_views.h"
#include "../../game/duel_effect_object_pool.h"

void DuelEffect_RunSwordsField(void *buffer, s32 phase)
{
    DuelEffect21Work *slots[2];
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    s32 i;
    s32 a;
    s32 b;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_SwordsFieldInitialScale;
    slots[0] = (DuelEffect21Work *)(gRuntime_pSharedWorkBase + 0x2800);
    slots[1] = (DuelEffect21Work *)(gRuntime_pSharedWorkBase + 0x2880);
    if (phase >= 0) {
        if (slots[__builtin_abs(phase) % 2]->enabled != 0) {
            DuelEffect_SetGrayColor((u8 *)&slots[__builtin_abs(phase) % 2]->color, 0);
            slots[__builtin_abs(phase) % 2]->enabled = 0;
            gDuelEffect_pActiveRequest->phase = phase;
        } else {
            for (i = 0; i < 10; i++) {
                copyVector(&slots[__builtin_abs(phase) % 2]->positions[i],
                           &gDuelEffect_SwordsFieldTilePositions[i][phase % 2]);
            }
            for (i = 0; i < 8; i++) {
                a = rand() % 10;
                b = rand() % 10;
                DuelEffect_SwapVectorRecords(&slots[__builtin_abs(phase) % 2]->positions[a],
                             &slots[__builtin_abs(phase) % 2]->positions[b]);
            }
            DuelEffect_SetGrayColor((u8 *)&slots[__builtin_abs(phase) % 2]->color, 128);
            /* The original initializes only tile 8 after the eight swaps. */
            slots[__builtin_abs(phase) % 2]->tiles[i] = rand() % 3;
            slots[__builtin_abs(phase) % 2]->enabled = 1;
            if (phase < 2) {
                slots[__builtin_abs(phase) % 2]->count = 0;
                for (i = 0; i < 10; i++) {
                    slots[__builtin_abs(phase) % 2]->offsets[i] = 160;
                }
            } else {
                slots[__builtin_abs(phase) % 2]->count = 10;
                for (i = 0; i < 10; i++) {
                    slots[__builtin_abs(phase) % 2]->offsets[i] = 0;
                }
            }
            gDuelEffect_pActiveRequest->phase = ~(__builtin_abs(phase) % 2);
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 2);
        for (i = 0; i < slots[__builtin_abs(phase + 1) % 2]->count; i++) {
            copyVector(&position, &slots[__builtin_abs(phase + 1) % 2]->positions[i]);
            position.vy -= slots[__builtin_abs(phase + 1) % 2]->offsets[i];
            DuelEffect_DrawTranslatedTileRect((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color,
                         &position, 30, 80,
                         slots[__builtin_abs(phase + 1) % 2]->tiles[i]);
            slots[__builtin_abs(phase + 1) % 2]->tiles[i] =
                (slots[__builtin_abs(phase + 1) % 2]->tiles[i] + 1) % 3;
            if (slots[__builtin_abs(phase + 1) % 2]->offsets[i] != 0) {
                slots[__builtin_abs(phase + 1) % 2]->offsets[i] -= 16;
            } else {
                gDuelEffect_pActiveRequest->progress = 1;
                slots[__builtin_abs(phase + 1) % 2]->offsets[i] = 0;
                if (i == 9) {
                    gDuelEffect_pActiveRequest->progress = 2;
                }
            }
        }
        if (++slots[__builtin_abs(phase + 1) % 2]->count > 10) {
            slots[__builtin_abs(phase + 1) % 2]->count = 10;
        }
        if (__builtin_abs(phase) > 2) {
            DuelEffect_DarkenColor((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color, 4);
        }
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color) ||
            slots[__builtin_abs(phase + 1) % 2]->enabled == 0) {
            slots[__builtin_abs(phase + 1) % 2]->enabled = 0;
            DuelEffect_SetGrayColor((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color, 0);
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
