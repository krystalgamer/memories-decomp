#include "../../types.h"
#include "effect_13.h"

void func_801503F8(void *buffer, s32 phase, s16 number)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color;
    DuelEffect13Work *work;
    s32 i;
    s32 frame_step;
    VECTOR *scale_pointer;
    SVECTOR *position_pointer;

    scale_pointer = &scale;
    position_pointer = &position;
    memset(position_pointer, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    *scale_pointer = D_80146178;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 4) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015AE94[phase];
            setVector(&work->offset, D_8015B7F8.vx, D_8015B7F8.vy, D_8015B7F8.vz);
            setVector(&work->origin, 0, -work->config->height, 0);
            addVector(&work->origin, &D_8015B7F8);
            for (i = 0; i < 48; i++) {
                func_8014FABC(work->config->radius, work->config->size >> 1,
                             work->config->height, 8, work->paths[i]);
                setVector(&work->endpoints[i], 0, 0, 0);
                func_8014F020((u8 *)&work->colors[i], work->config->color[0],
                             work->config->color[1], work->config->color[2]);
                work->ages[i] = 0;
                work->states[i] = 0;
                work->angles[i] = rand() % 512;
            }
            setVector(&work->number_position, 0, 0, 0);
            setVector(&work->number_velocity, 0, -12, 0);
            work->active = 0;
            work->bounces = 0;
            work->frame = 0;
            work->tick = 1;
            work->ready = 0;
            work->number_mode = 0;
            work->number = number;
            func_8014F020((u8 *)&work->glow_color, work->config->color[0] >> 3,
                         work->config->color[1] >> 3, work->config->color[2] >> 3);
            if (work->config->background == 0) {
                func_8014F010((u8 *)&work->background_color, 0);
            } else {
                func_8014F010((u8 *)&work->background_color, 1);
            }
            func_8014F020((u8 *)&work->number_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            D_8009B261 = 1;
        }
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        if ((u16)func_8014D3AC((u8 *)&work->glow_color)) {
            setVector(&rotation, 0, 0, work->tick * 128);
            func_801513F4(&world, &saved, &work->origin, &rotation, scale_pointer, 0);
            if (work->config->blend != 1) {
                D_8015B748.named.page_at_00 = getTPage(D_8015AC48[0].pmode,
                    work->config->blend, D_8015AC48[0].px, D_8015AC48[0].py);
            }
            func_80155BC0((u8 *)&work->glow_color, work->config->size, 1, position_pointer);
            if (work->config->blend != 1) {
                color = work->glow_color;
                color.r /= 4;
                color.r /= 4;
                color.r /= 4;
                D_8015B748.named.page_at_00 = getTPage(D_8015AC48[0].pmode,
                    1, D_8015AC48[0].px, D_8015AC48[0].py);
                func_80155BC0((u8 *)&color, work->config->size * 2, 16, position_pointer);
            }
            if (work->active == 48) {
                func_80153F28((u8 *)&work->glow_color, 15);
            } else {
                func_80153F98((u8 *)&work->glow_color, work->config->color[0] >> 1,
                             work->config->color[1] >> 1, work->config->color[2] >> 1, 15);
            }
        }
        if (work->config->background == 1 &&
            (u16)func_8014D3AC((u8 *)&work->background_color)) {
            setVector(&rotation, 0, 0, 0);
            func_801513F4(&world, &saved, &work->origin, &rotation, &scale, 0);
            func_801573A8((u8 *)&work->background_color, work->config->size * 2, 32);
            if (work->ready == 0) {
                work->ready = func_80153F98((u8 *)&work->background_color, 128, 128, 128, 8);
            }
            if (work->active == 48) {
                func_80153F28((u8 *)&work->background_color, 8);
            }
        }
        for (i = 0; i < work->active; i++) {
            setVector(&rotation, 0, work->angles[i], 0);
            func_801513F4(&world, &saved, &work->offset, &rotation, &scale, 2);
            if ((u16)func_8014D3AC((u8 *)&work->colors[i])) {
                if (work->states[i] != 2) {
                    work->states[i] = func_801570B0((u8 *)&work->colors[i], work->ages[i],
                        work->paths[i], 16, 1, &work->endpoints[i], work->config->blend,
                        &work->offset);
                } else {
                    func_801570B0((u8 *)&work->colors[i], work->ages[i],
                        work->paths[i], 32, 0, &work->endpoints[i], work->config->blend,
                        &work->offset);
                }
                work->ages[i]++;
                if (work->ages[i] >= 8) {
                    work->ages[i] = 7;
                    func_80153F28((u8 *)&work->colors[i], 15);
                }
            }
        }
        if (work->config->blend != 1) {
            D_8015B748.named.page_at_00 = getTPage(D_8015AC48[0].pmode,
                work->config->blend, D_8015AC48[0].px, D_8015AC48[0].py);
        }
        for (i = 0; i < work->active; i++) {
            if (work->states[i] == 2 &&
                (u16)func_8014D3AC((u8 *)&work->colors[i])) {
                D_8009B264->field_1D = 1;
                func_801513F4(&world, &saved, &work->endpoints[i], &rotation, &scale, 0);
                func_80155BC0((u8 *)&work->colors[i], work->config->particle_size, 1, &position);
            }
        }
        if (work->active >= 12 && (u16)func_8014D3AC((u8 *)&work->number_color)) {
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, 0, 0, 0);
            func_801513F4(&world, &saved, &work->offset, &rotation, &scale, 0);
            func_801566D4(-__builtin_abs(work->number), (u8 *)&work->number_color,
                         &work->number_position, work->number_mode, 10, 1);
            addVector(&work->number_position, &work->number_velocity);
            work->number_velocity.vy += 3;
            if (work->number_position.vy >= 0) {
                work->number_position.vy = 0;
                if (work->bounces < 5) {
                    work->bounces++;
                    work->number_velocity.vy = -12 / work->bounces;
                } else {
                    work->number_mode = 1;
                    work->number_velocity.vy = 0;
                    func_80153F28((u8 *)&work->number_color, 15);
                }
            }
        }
        work->active += 2;
        if (work->active > 48) {
            work->active = 48;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->colors[47]) &&
            (u16)func_8014D378((u8 *)&work->background_color) &&
            (u16)func_8014D378((u8 *)&work->number_color)) {
            D_8009B261 = 1;
        }
    }
}
