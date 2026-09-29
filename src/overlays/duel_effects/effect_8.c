#include "../../types.h"
#include "effect_8.h"

void func_80147B18(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    VECTOR scale;
    POLY_GT4 polygon;
    SVECTOR quad[4];
    CVECTOR color_copy;
    POLY_GT4 *packet;
    DuelEffect8Work *work;
    s32 i;
    s32 frame_step;

    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146014;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015A514[phase];
            work->width = work->config->initial_width;
            work->height = work->config->initial_height;
            copyVector(&work->origin, &D_8015B7F8);
            for (i = 0; i < 3; i++) {
                func_8014EE0C(work->config->radii[i], work->config->radii[i],
                             work->config->ring_height * i, work->rings[i], 32);
            }
            func_8014F754(work->rays, work->config->ray_range,
                         work->config->ray_range, work->config->ray_range,
                         work->config->ray_count);
            func_8014F180(work->config->particle_speed, work->positions,
                         work->velocities, work->config->particle_count);
            work->scale = 4096;
            work->frame = 0;
            func_8014F020((u8 *)&work->color, work->config->color.r,
                         work->config->color.g, work->config->color.b);
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
        func_801513F4(&world, &saved, &work->origin, &rotation, &scale, 1);
        if ((u16)func_8014D3AC((u8 *)&work->color)) {
            func_80156E58((u8 *)&work->color, work->config->ray_width, work->rays,
                         work->config->ray_count, 64);
            for (i = 0; i < work->config->particle_count; i++) {
                func_8014F2D4(quad, &work->positions[i]);
                func_80156064((u8 *)&work->color, quad, 1);
                addVector(&work->positions[i], &work->velocities[i]);
                /* The zero acceleration retains the original halfword read. */
                work->velocities[i].vy -= 0;
            }
            func_80155BC0((u8 *)&work->color, work->config->sprite_size,
                         64, &rotation);
            setPolyGT4(packet);
            packet->tpage = D_8015B748.named.page1;
            packet->clut = D_8015B748.named.clut1;
            setUV4(packet, 0, 0, 31, 0, 0, 127, 31, 127);
            setRGB0(packet, 0, 0, 0);
            setRGB1(packet, 0, 0, 0);
            setRGB2(packet, work->color.r, work->color.g, work->color.b);
            setRGB3(packet, work->color.r, work->color.g, work->color.b);
            setVector(&quad[0], -work->width, -work->height, 0);
            setVector(&quad[1], work->width, -work->height, 0);
            setVector(&quad[2], -work->width, 0, 0);
            setVector(&quad[3], work->width, 0, 0);
            func_8015131C(packet, quad, 32, 1);
            work->width -= work->config->width_step;
            if (work->width < work->config->minimum_width) {
                work->width = work->config->minimum_width;
            }
            work->height += work->config->height_step;
            if (work->height > work->config->maximum_height) {
                work->height = work->config->maximum_height;
            }
            if (work->height == work->config->maximum_height &&
                work->width == work->config->minimum_width) {
                func_80153F28((u8 *)&work->color, 8);
            }
            if (work->config->draw_rings != 0) {
                color_copy = work->color;
                color_copy.r >>= 1;
                color_copy.g >>= 1;
                color_copy.b >>= 1;
                scale.vx = work->scale;
                scale.vy = 4096;
                scale.vz = work->scale;
                func_801513F4(&world, &saved, &work->origin, &rotation, &scale, 3);
                func_8015616C((u8 *)&color_copy, work->rings[0],
                             work->rings[1], work->rings[2], 1);
                scale.vx = work->scale >> 1;
                scale.vy = 6144;
                scale.vz = work->scale >> 1;
                func_801514BC(&saved, &scale);
                func_8015616C((u8 *)&work->color, work->rings[0],
                             work->rings[1], work->rings[2], 1);
                if (work->scale < 0x4000) {
                    work->scale += 0x1000;
                }
            }
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color)) {
            D_8009B261 = 1;
        }
    }
}
