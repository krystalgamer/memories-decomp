#include "../../types.h"
#include "effect_21.h"

void func_8014E3EC(void *buffer, s32 phase)
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
    scale = D_80146158;
    slots[0] = (DuelEffect21Work *)(D_80010000 + 0x2800);
    slots[1] = (DuelEffect21Work *)(D_80010000 + 0x2880);
    if (phase >= 0) {
        if (slots[__builtin_abs(phase) % 2]->enabled != 0) {
            func_8014F010((u8 *)&slots[__builtin_abs(phase) % 2]->color, 0);
            slots[__builtin_abs(phase) % 2]->enabled = 0;
            D_8009B264->field_1A = phase;
        } else {
            for (i = 0; i < 10; i++) {
                copyVector(&slots[__builtin_abs(phase) % 2]->positions[i],
                           &D_8015AB68[i][phase % 2]);
            }
            for (i = 0; i < 8; i++) {
                a = rand() % 10;
                b = rand() % 10;
                func_8014FED4(&slots[__builtin_abs(phase) % 2]->positions[a],
                             &slots[__builtin_abs(phase) % 2]->positions[b]);
            }
            func_8014F010((u8 *)&slots[__builtin_abs(phase) % 2]->color, 128);
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
            D_8009B264->field_1A = ~(__builtin_abs(phase) % 2);
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        func_801513F4(&world, &saved, &position, &rotation, &scale, 2);
        for (i = 0; i < slots[__builtin_abs(phase + 1) % 2]->count; i++) {
            copyVector(&position, &slots[__builtin_abs(phase + 1) % 2]->positions[i]);
            position.vy -= slots[__builtin_abs(phase + 1) % 2]->offsets[i];
            func_80157494((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color,
                         &position, 30, 80,
                         slots[__builtin_abs(phase + 1) % 2]->tiles[i]);
            slots[__builtin_abs(phase + 1) % 2]->tiles[i] =
                (slots[__builtin_abs(phase + 1) % 2]->tiles[i] + 1) % 3;
            if (slots[__builtin_abs(phase + 1) % 2]->offsets[i] != 0) {
                slots[__builtin_abs(phase + 1) % 2]->offsets[i] -= 16;
            } else {
                D_8009B264->field_1D = 1;
                slots[__builtin_abs(phase + 1) % 2]->offsets[i] = 0;
                if (i == 9) {
                    D_8009B264->field_1D = 2;
                }
            }
        }
        if (++slots[__builtin_abs(phase + 1) % 2]->count > 10) {
            slots[__builtin_abs(phase + 1) % 2]->count = 10;
        }
        if (__builtin_abs(phase) > 2) {
            func_80153F28((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color, 4);
        }
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color) ||
            slots[__builtin_abs(phase + 1) % 2]->enabled == 0) {
            slots[__builtin_abs(phase + 1) % 2]->enabled = 0;
            func_8014F010((u8 *)&slots[__builtin_abs(phase + 1) % 2]->color, 0);
            D_8009B261 = 1;
        }
    }
}
