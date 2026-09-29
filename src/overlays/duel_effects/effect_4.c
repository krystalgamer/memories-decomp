#include "../../types.h"
#include "effect_4.h"

void func_80159AAC(void *buffer, s32 phase)
{
    MATRIX world;
    POLY_FT4 polygon;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    POLY_FT4 *packet;
    DuelEffect4Work *work;
    s32 i;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    packet = &polygon;
    scale = D_80146248;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B704[phase];
            for (i = 0; i < 3; i++) {
                func_8014EA7C(work->config->radii[i], work->rings[i]);
            }
            for (i = 0; i < 2; i++) {
                func_8014EB1C(24 + i * 6, 28 + i * 6, work->card_rings[i], 4);
            }
            func_8014EF2C(16, work->rotations);
            func_8014F030(work->config->particle_speed, work->positions,
                         work->velocities, 24);
            func_8014F010((u8 *)&work->color, 1);
            func_8014F010((u8 *)&work->card_color, 1);
            func_8014F010((u8 *)&work->screen_color, 0);
            work->stage = 0;
            work->scale = 4096;
            work->frame = 0;
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
        func_801531C4(&world);
        if ((u16)func_8014D3AC((u8 *)&work->color) ||
            (u16)func_8014D3AC((u8 *)&work->card_color)) {
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            func_801513F4(&world, &saved, &position, &rotation, &scale, 1);
            func_801558F4((u8 *)&work->color, work->card_rings[0],
                         work->card_rings[1], 0, 1);
            setPolyFT4(packet);
            packet->tpage = D_8015B748.pairs[work->config->texture][0];
            packet->clut = D_8015B748.pairs[work->config->texture][1];
            setRGB0(packet, work->card_color.r, work->card_color.g,
                    work->card_color.b);
            setUV4(packet,
                (work->config->texture - 5) * 64, 128,
                (work->config->texture - 5) * 64 + 63, 128,
                (work->config->texture - 5) * 64, 255,
                (work->config->texture - 5) * 64 + 63, 255);
            setVector(&quad[0], -work->config->card_width,
                      -work->config->card_height, 0);
            setVector(&quad[1], work->config->card_width,
                      -work->config->card_height, 0);
            setVector(&quad[2], -work->config->card_width,
                      work->config->card_height, 0);
            setVector(&quad[3], work->config->card_width,
                      work->config->card_height, 0);
            if ((*(u32 *)&work->card_color & 0xFFFFFF) == 0x808080) {
                func_80151218(packet, quad, 0, 0);
            } else {
                func_80151218(packet, quad, 0, 1);
            }
            if (work->frame >= work->config->duration) {
                if (work->stage == 0) {
                    func_8014F010((u8 *)&work->card_color, 0);
                    func_8014F010((u8 *)&work->screen_color, 255);
                    work->stage = 1;
                }
                D_8009B264->field_1D = 1;
                for (i = 0; i < 24; i++) {
                    func_8014F2D4(quad, &work->positions[i]);
                    func_80156064((u8 *)&work->color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
                scale.vx = work->scale >> 3;
                scale.vy = work->scale >> 3;
                scale.vz = 4096;
                func_801514BC(&saved, &scale);
                func_80156448((u8 *)&work->color, work->rings[0],
                             work->rings[1], work->rings[2], 0);
                scale.vx = work->scale;
                scale.vy = work->scale;
                scale.vz = 4096;
                func_801514BC(&saved, &scale);
                func_80155BC0((u8 *)&work->color, work->config->sprite_size,
                             0, &position);
                scale.vx = 4096;
                scale.vy = work->scale;
                scale.vz = 4096;
                for (i = 0; i < 16; i++) {
                    func_801513F4(&world, &saved, &position, &work->rotations[i],
                                 &scale, 2);
                    func_80155D90((u8 *)&work->color, work->config->widths,
                                 work->config->height, 0);
                    applyVector(&work->rotations[i], 128, 128, 128, +=);
                }
                func_80153F28((u8 *)&work->color, 15);
                if (work->scale < 0x7000) {
                    work->scale += 0x1000;
                }
            } else {
                func_80153F98((u8 *)&work->color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 31);
                func_80153F98((u8 *)&work->card_color, 128, 128, 128, 31);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->screen_color)) {
            func_801556F4((u8 *)&work->screen_color, 31);
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color)) {
            D_8009B261 = 1;
        }
    }
}
