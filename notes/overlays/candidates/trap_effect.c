/* CANDIDATE, NOT MATCHING: French func_80147B18 (0x690 bytes), North
 * American func_801499DC, effect id 8 (the request duel_trap_resolution.c
 * creates). Compiled with gcc_2_8_1_g0_split against the North American image
 * it is ONE instruction short: retail has a dead `lhu $v0, 0x306($s0)` (a
 * velocities[i].vy read whose value the next `lw $v0, 0($s6)` overwrites)
 * right after the particle loop's vz store, and every other instruction of
 * the 420 aligns. That dead load is unique in the whole bank, so it is a
 * quirk of this function's source; identical-arm `if`s, `continue`/`break`
 * tests, volatile reads and dead reads of velocity->vy were all measured and
 * none reproduces it (gcc deletes the branch and its load together).
 *
 * Levers that reached this state, each measured against the alternatives:
 * - the particle update wants byte-offset pointers assigned AFTER the two
 *   calls, `(SVECTOR *)((u8 *)state + i * 8 + 0x104)`: that gives retail's
 *   cursor giv (state + 8i) with the pointers derived by `addiu` and the
 *   first component folded into the cursor's displacement; the index form
 *   folds every component (-4), pointer locals from `&state->particles[i]`
 *   give offset givs with the base added at use, and an explicit cursor
 *   local is the same as the byte-offset spelling;
 * - `packet->tpage` and `packet->clut` written immediately after
 *   setPolyGT4, before the UV and RGB stores (58 -> 32 differences);
 * - `packet = &polygon` assigned after the scale copy, not as an
 *   initializer (the pointer's callee-saved register is materialised after
 *   the scale address);
 * - the whole middle of the update is inside `if (func_8014D3AC(color))`;
 * - the line-width argument is loaded `lhu`, which needs func_80156E58's
 *   second parameter declared u16 rather than s16; that prototype change
 *   keeps primitive_draw.c byte-identical against the French image but is
 *   not applied to the shared header while this unit is not matching.
 *
 * Callee func_8014E35C is 0x90 bytes in the French image and 0xCC in the
 * North American one (US func_80147BE0): a regional edit in that callee.
 * The includes below are as they would be from src/overlays/duel_effects/. */
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/memory.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../unmatched.h"
#include "utility_helpers.h"
#include "drawing_helpers.h"
#include "effect_routines.h"
#include "trap_effect.h"

void func_80147B18(void *buffer, s32 phase)
{
    TrapEffectState *state;
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    VECTOR scale;
    POLY_GT4 polygon;
    SVECTOR quad[4];
    CVECTOR half;
    POLY_GT4 *packet;
    s32 frame_step;
    s32 i;
    s32 shift;

    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146014;
    packet = &polygon;
    state = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            state->step = 1;
        } else {
            state->step = 0;
            state->descriptor = &D_8015A514[phase];
            state->width = state->descriptor->width;
            state->height = state->descriptor->height;
            state->position.vx = D_8015B7F8.vx;
            state->position.vy = D_8015B7F8.vy;
            state->position.vz = D_8015B7F8.vz;
            for (i = 0; i < 3; i++) {
                func_8014EE0C(state->descriptor->ring_radius[i],
                              state->descriptor->ring_radius[i],
                              state->descriptor->ring_angle_step * i,
                              state->rings[i], 32);
            }
            func_8014F754(state->lines, state->descriptor->line_spread,
                          state->descriptor->line_spread,
                          state->descriptor->line_spread,
                          state->descriptor->line_count);
            func_8014F180(state->descriptor->particle_speed, state->particles,
                          state->velocities, state->descriptor->particle_count);
            state->scale = 0x1000;
            state->frame = 0;
            func_8014F020((u8 *)&state->color, state->descriptor->red,
                          state->descriptor->green, state->descriptor->blue);
        }
    } else {
        if (state->step != 0) {
            /*STEP*/
            if (state->step == 1) {
                func_8014E35C(1);
            } else {
                func_8014E35C(0);
            }
            /*ENDSTEP*/
            state->step++;
            if (state->step >= 0xB5) {
                D_8009B261 = 1;
            }
            return;
        }
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        func_801513F4(&world, &saved, &state->position, &rotation, &scale, 1);
        if ((u16)func_8014D3AC((u8 *)&state->color) != 0) {
            func_80156E58((u8 *)&state->color, state->descriptor->line_width,
                          state->lines, state->descriptor->line_count, 0x40);
            for (i = 0; i < state->descriptor->particle_count; i++) {
                SVECTOR *particle;
                SVECTOR *velocity;

                func_8014F2D4(quad, &state->particles[i]);
                func_80156064((u8 *)&state->color, quad, 1);
                particle = (SVECTOR *)((u8 *)state + i * 8 + 0x104);
                velocity = (SVECTOR *)((u8 *)state + i * 8 + 0x304);
                particle->vx += velocity->vx;
                particle->vy += velocity->vy;
                particle->vz += velocity->vz;
            }
            func_80155BC0((u8 *)&state->color, state->descriptor->curve_step, 0x40, &rotation);
            setPolyGT4(packet);
            packet->tpage = D_8015B748[11][0];
            packet->clut = D_8015B748[11][1];
            setUV4(packet, 0, 0, 0x1F, 0, 0, 0x7F, 0x1F, 0x7F);
            setRGB0(packet, 0, 0, 0);
            setRGB1(packet, 0, 0, 0);
            setRGB2(packet, state->color.r, state->color.g, state->color.b);
            setRGB3(packet, state->color.r, state->color.g, state->color.b);
            quad[0].vx = -state->width;
            quad[0].vy = -state->height;
            quad[0].vz = 0;
            quad[1].vx = state->width;
            quad[1].vy = -state->height;
            quad[1].vz = 0;
            quad[2].vx = -state->width;
            quad[2].vy = 0;
            quad[2].vz = 0;
            quad[3].vx = state->width;
            quad[3].vy = 0;
            quad[3].vz = 0;
            func_8015131C(packet, quad, 0x20, 1);
            state->width -= state->descriptor->width_step;
            if (state->width < state->descriptor->width_min) {
                state->width = state->descriptor->width_min;
            }
            state->height += state->descriptor->height_step;
            if (state->height > state->descriptor->height_max) {
                state->height = state->descriptor->height_max;
            }
            if (state->height == state->descriptor->height_max &&
                state->width == state->descriptor->width_min) {
                func_80153F28((u8 *)&state->color, 8);
            }
            if (state->descriptor->has_rings != 0) {
                half = state->color;
                half.r >>= 1;
                half.g >>= 1;
                half.b >>= 1;
                scale.vx = state->scale;
                scale.vy = 0x1000;
                scale.vz = state->scale;
                func_801513F4(&world, &saved, &state->position, &rotation, &scale, 3);
                shift = 1;
                func_8015616C((u8 *)&half, state->rings[0], state->rings[1],
                              state->rings[2], shift);
                scale.vx = state->scale >> shift;
                scale.vy = 0x1800;
                scale.vz = state->scale >> shift;
                func_801514BC(&saved, &scale);
                func_8015616C((u8 *)&state->color, state->rings[0], state->rings[1],
                              state->rings[2], shift);
                if (state->scale < 0x4000) {
                    state->scale += 0x1000;
                }
            }
        }
        state->frame += frame_step;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&state->color) != 0) {
            D_8009B261 = 1;
        }
    }
}
