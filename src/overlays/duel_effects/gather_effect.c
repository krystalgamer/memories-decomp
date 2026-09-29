#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/memory.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../unmatched.h"
#include "utility_helpers.h"
#include "drawing_helpers.h"
#include "color_helpers.h"
#include "layered_drawing.h"
#include "textured_quads.h"
#include "dispatch.h"
#include "drawing_tail.h"
#include "gather_effect.h"

#ifdef VERSION_EUROPE
#define GATHER_CURVE_HEIGHT 106
#else
#define GATHER_CURVE_HEIGHT 98
#endif

void func_801481A8(void *buffer, s32 phase)
{
    GatherEffectState *state;
    GatherEffectFrame frame;
    MATRIX saved;
    POLY_FT4 *packet;
    s32 frame_step;
    SVECTOR *particle;
    SVECTOR *velocity;
    SVECTOR *particle_b;
    SVECTOR *velocity_b;
    s32 i;
    s32 j;

    memset(&frame.rotation, 0, sizeof(frame.rotation));
    memset(&frame.position, 0, sizeof(frame.position));
    packet = &frame.polygon;
    frame.scale = D_80146024;
    state = buffer;
    if (phase >= 0) {
        if (phase >= 10) {
            state->step = 1;
        } else {
            state->step = 0;
            state->descriptor = &D_8015A5F8;
            func_8014F89C(state->particles, state->descriptor->spread,
                          state->descriptor->spread, 32);
            func_8014F89C(state->particles_b, state->descriptor->spread,
                          state->descriptor->spread, 48);
            state->tpage = 0x2B;
            state->clut = 0x3EA8;
            state->sprite_u = (phase % 8) * 16 + 0x80;
            state->sprite_v = (phase / 8) * 16 + 0x30;
            for (i = 0; i < 3; i++) {
                func_8014EA7C(state->descriptor->ring_radius[i], state->rings[i]);
            }
            for (i = 0; i < 2; i++) {
                func_8014EB1C(0x46 + i * 6, GATHER_CURVE_HEIGHT + i * 6, state->curves[i], 4);
            }
            for (i = 0; i < 32; i++) {
                SVECTOR *velocity = (SVECTOR *)((u8 *)state + i * 8 + 0x104);

                velocity->vx = -state->particles[i].vx / state->descriptor->frames;
                velocity->vy = -state->particles[i].vy / state->descriptor->frames;
                velocity->vz = 0;
                state->sizes[i] = state->descriptor->size;
                func_8014F010(state->colors[i], 1);
                state->flags[i] = 0;
            }
            for (i = 0; i < 48; i++) {
                SVECTOR *velocity = (SVECTOR *)((u8 *)state + i * 8 + 0x744);

                velocity->vx = -state->particles_b[i].vx / state->descriptor->frames;
                velocity->vy = -state->particles_b[i].vy / state->descriptor->frames;
                velocity->vz = 0;
                func_8014F010(state->colors_b[i], 1);
            }
            func_8014EF2C(16, state->rotations);
            func_8014F010(state->color, 1);
            state->done = 0;
            state->frame = 0;
            state->particle_count = 0;
            state->particle_b_count = 0;
            state->scale = 0x1000;
        }
    } else {
        if (state->step != 0) {
            if (state->step == 1) {
                func_8014E35C(1);
            } else {
                func_8014E35C(0);
            }
            state->step++;
            if (state->step >= 0xB5) {
                D_8009B261 = 1;
            }
            return;
        }
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        func_801531C4(&frame.world);
        func_801513F4(&frame.world, &saved, &frame.position, &frame.rotation, &frame.scale, 1);
        setPolyFT4(packet);
        packet->tpage = state->tpage;
        packet->clut = state->clut;
        setUV4(packet, state->sprite_u, state->sprite_v,
               state->sprite_u + 15, state->sprite_v,
               state->sprite_u, state->sprite_v + 15,
               state->sprite_u + 15, state->sprite_v + 15);
        state->particle_count += 2;
        if (state->particle_count >= 0x21) {
            state->particle_count = 0x20;
        }
        state->particle_b_count += 3;
        if (state->particle_b_count >= 0x31) {
            state->particle_b_count = 0x30;
        }
        for (i = 0; i < state->particle_count; i++) {
            if ((u16)func_8014D3AC(state->colors[i]) != 0) {
                setRGB0(packet, state->colors[i][0], state->colors[i][1], state->colors[i][2]);
                func_8014F358(frame.quad, state->sizes[i]);
                for (j = 0; j < 4; j++) {
                    SVECTOR *vertex = (SVECTOR *)((u8 *)&frame + j * 8 + 0x68);
                    SVECTOR *particle = (SVECTOR *)((u8 *)state + i * 8 + 0x4);

                    vertex->vx += particle->vx;
                    vertex->vy += particle->vy;
                    vertex->vz += particle->vz;
                }
                func_80151218(packet, frame.quad, 0, 1);
                if (__builtin_abs(state->particles[i].vx) >= 17 ||
                    __builtin_abs(state->particles[i].vy) >= 17) {
                    particle = (SVECTOR *)((u8 *)state + i * 8 + 0x4);
                    velocity = (SVECTOR *)((u8 *)state + i * 8 + 0x104);
                    particle->vx += velocity->vx;
                    particle->vy += velocity->vy;
                    particle->vz += velocity->vz;
                } else if (i == 31) {
                    state->done = 2;
                }
                state->sizes[i] -= state->descriptor->size * 3 / 4 / state->descriptor->frames;
                if (state->flags[i] == 0) {
                    state->flags[i] = func_80153F98(state->colors[i], 0xFF, 0xFF, 0xFF, 0x2F);
                } else {
                    func_80153F28(state->colors[i], 0xF);
                }
            }
        }
        for (i = 0; i < state->particle_b_count; i++) {
            if ((u16)func_8014D3AC(state->colors_b[i]) != 0) {
                func_8014F2D4(frame.quad, &state->particles_b[i]);
                func_80156064(state->colors_b[i], frame.quad, 0);
                if (__builtin_abs(state->particles_b[i].vx) >= 17 ||
                    __builtin_abs(state->particles_b[i].vy) >= 17) {
                    particle_b = (SVECTOR *)((u8 *)state + i * 8 + 0x5C4);
                    velocity_b = (SVECTOR *)((u8 *)state + i * 8 + 0x744);
                    particle_b->vx += velocity_b->vx;
                    particle_b->vy += velocity_b->vy;
                    particle_b->vz += velocity_b->vz;
                    func_80153F98(state->colors_b[i], 0xB0, 0xB0, 0xFF, 0xF);
                } else {
                    func_80153F28(state->colors_b[i], 0xF);
                }
            }
        }
        if ((u16)func_8014D3AC(state->color) != 0) {
            func_801558F4(state->color, state->curves[0], state->curves[1], 0, 1);
            frame.scale.vx = state->scale >> 3;
            frame.scale.vy = state->scale >> 3;
            frame.scale.vz = 0x1000;
            func_801514BC(&saved, &frame.scale);
            func_80156448(state->color, state->rings[0], state->rings[1], state->rings[2], 0);
            frame.scale.vx = state->scale;
            frame.scale.vy = state->scale;
            frame.scale.vz = 0x1000;
            func_801514BC(&saved, &frame.scale);
            func_80155BC0(state->color, state->descriptor->curve_size, 0, &frame.position);
            frame.scale.vx = 0x1000;
            frame.scale.vy = state->scale;
            frame.scale.vz = 0x1000;
            for (i = 0; i < 16; i++) {
                SVECTOR *rotate;

                func_801513F4(&frame.world, &saved, &frame.position, &state->rotations[i], &frame.scale, 2);
                func_80155D90(state->color, state->descriptor->widths,
                              state->descriptor->height, 0);
                rotate = (SVECTOR *)((u8 *)state + i * 8 + 0x544);
                rotate->vx += 0x80;
                rotate->vy += 0x80;
                rotate->vz += 0x80;
            }
            if (state->scale < 0x7000) {
                state->scale += 0x800;
            }
            if (state->done == 0) {
                state->done = func_80153F98(state->color, 0xB0, 0xB0, 0xFF, 0xF);
            } else if (state->done == 2) {
                func_80153F28(state->color, 0xF);
            }
        }
        state->frame += frame_step;
        PopMatrix();
        if ((u16)func_8014D378(state->colors[31]) != 0 &&
            (u16)func_8014D378(state->color) != 0) {
            D_8009B261 = 1;
        }
    }
}
