/* CANDIDATE, NOT MATCHING: French func_8014C8FC (0xA7C bytes), North
 * American func_8014D5D4, effect id 10. Compiled with gcc_2_8_1_g0_split
 * against the North American image it is at EXACT LENGTH (671 of 671
 * instructions) and every difference is register allocation: the state
 * pointer and the position vector swap s1/s2, the packet pointer and the
 * rotation vector swap s4/s5, the temporaries of the scale copy and the
 * phase reload shift by one, the spill slots at sp+0xC0..0xD0 are
 * permuted, and the i-loop preheader hoists &frame.quad before &frame
 * where retail hoists them the other way round. Twelve spellings of where
 * the quad pointer is assigned (before the loop, at the top of the i, j or
 * k body, walked as a biv, per-iteration from &frame.quad, index form,
 * an explicit frame base pointer) were measured; the index form and the
 * per-iteration pointer merge the two cursors into one, the assignments
 * outside the k loop hoist in the wrong order, and none moves the
 * allocation.
 *
 * Levers that did land, each measured against the alternatives:
 * - `image = D_8015A8C8` as a base local assigned inside the `if` that
 *   reads it keeps the three GsIMAGE reads as displacements off one
 *   register instead of folded symbol offsets; the same base-local
 *   spelling (`words`) does it for the two texture-word reads;
 * - the odd-parity colour arm divides the constant 0xC0 by j + 1;
 * - the count loop has no explicit guard (one entry test, not two);
 * - `func_8014F010` is called before `timers[i] = 0`;
 * - the done flags are compared as one word (`*(u32 *)&done_a == 0x10001`);
 * - the update's locals are one frame struct and the vertex updates use
 *   byte-offset pointers assigned at the top of the innermost body.
 * The includes below are as they would be from src/overlays/duel_effects/. */
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../game/func_80058E1C.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../game/duel_effect_request.h"
#define D_8009B264_VISIBLE
#include "../../unmatched.h"
#include "utility_helpers.h"
#include "drawing_helpers.h"
#include "color_helpers.h"
#include "effect_routines.h"
#include "shower_effect.h"

void func_8014C8FC(void *buffer, s32 phase)
{
    ShowerEffectState *state;
    ShowerEffectFrame frame;
    POLY_FT4 *packet;
    SVECTOR *quad;
    s32 frame_step;
    GsIMAGE *image;
    s32 i;
    s32 j;
    s32 k;

    memset(&frame.rotation, 0, sizeof(frame.rotation));
    memset(&frame.position, 0, sizeof(frame.position));
    packet = &frame.polygon;
    frame.scale = D_80146138;
    state = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            state->step = 1;
        } else {
            state->step = 0;
            state->descriptor = &D_8015AB14[phase];
            for (j = 0; j < 2; j++) {
                func_8014EC8C(0xAF, 0xC2, 0x10, j * 8, state->quads[j], 4);
            }
            func_8014F3E8(state->vertices, 0xAF, 0x10, 0xC2);
            func_8014F608(state->vectors, 0xAF, 0x10, 0xC2, 64);
            for (i = 0; i < 64; i++) {
                state->speeds[i] = state->descriptor->speed / 2 +
                                   state->descriptor->speed * (rand() % 0x1000) / 0x1000;
            }
            func_8014F608(state->drops, 0x8B, 0, 0xB6, 64);
            for (i = 0; i < 64; i++) {
                func_8014F010(state->colors[i], 1);
                state->timers[i] = 0;
            }
            func_8014F020(state->color, state->descriptor->red >> 3,
                          state->descriptor->green >> 3, state->descriptor->blue >> 3);
            func_8014F010(state->color_b, 0xFF);
            func_8014F010(state->color_c, 1);
            func_8014F010(state->color_d, 1);
            state->rise = 8;
            state->stage = 0;
            state->done_b = 0;
            state->done_a = 0;
            state->frame = 0;
            state->updates = 0;
            state->count = 0;
            if (state->descriptor->mode != 1) {
                image = D_8015A8C8;
                D_8015B778 = ((*(u16 *)&image[12].pmode & 3) << 7) |
                             ((state->descriptor->mode & 3) << 5) |
                             (((s32)(*(u16 *)&image[12].py & 0x100) << 16) >> 20) |
                             ((*(u16 *)&image[12].px & 0x3FF) >> 6) |
                             ((*(u16 *)&image[12].py & 0x200) << 2);
            }
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
        frame.world = *(MATRIX *)Model_GetLightSourceMatrix();
        func_801513F4(&frame.world, &frame.saved, &frame.position, &frame.rotation, &frame.scale, 2);
        if ((u16)func_8014D3AC(state->color) != 0) {
            if (state->stage == 0) {
                func_80156FA4(state->color, state->vertices, state->descriptor->mode, 1);
                func_80156FA4(state->color_d, state->vertices, state->descriptor->mode, 1);
                if (state->descriptor->mode == 1) {
                    func_80153F98(state->color, state->descriptor->red,
                                  state->descriptor->green, state->descriptor->blue, 4);
                    state->done_a = func_80153F98(state->color_d, 0xC0, 0xC0, 0xC0, 2);
                } else {
                    state->done_a = func_80153F98(state->color, state->descriptor->red,
                                                  state->descriptor->green,
                                                  state->descriptor->blue, 4);
                }
            }
        }
        if ((u16)func_8014D3AC(state->color_c) != 0) {
            if (state->stage == 0) {
                func_801558F4(state->color_c, state->quads[0], state->quads[1], 1, 1);
                if (state->rise < state->descriptor->rise_max) {
                    state->rise += state->descriptor->rise_step;
                    for (j = 0; j < 4; j++) {
                        state->quads[1][j].vy = -state->rise;
                    }
                }
                if (state->done_b == 0) {
                    state->done_b = func_80153F98(state->color_c, 0xFF, 0xFF, 0xFF, 8);
                }
            }
        }
        func_801513F4(&frame.world, &frame.saved, &frame.position, &frame.rotation, &frame.scale, 0);
        if ((u16)func_8014D3AC(state->color) != 0) {
            if (state->stage == 1) {
                for (i = 0; i < 64; i++) {
                    func_8014F2D4(frame.quad, &state->vectors[i]);
                    func_80156064(state->color, frame.quad, 0x50);
                    state->vectors[i].vy -= state->speeds[i];
                }
                func_80153F28(state->color, 8);
            }
        }
        if (state->stage == 0) {
            quad = frame.quad;
            for (i = 0; i < state->count; i++) {
                if ((u16)func_8014D3AC(state->colors[i]) != 0) {
                    ShowerEffectTextureWords *words = (ShowerEffectTextureWords *)&D_8015B748;

                    setPolyFT4(packet);
                    packet->tpage = words[12].page;
                    setUV4(packet, 0xE0, 0xC0, 0xFF, 0xC0, 0xE0, 0xDF, 0xFF, 0xDF);
                    packet->clut = words[12].clut;
                    for (j = 0; j < 4; j++) {
                        if (((state->updates + i + j) & 1) == 0) {
                            setRGB0(packet, state->colors[i][0] / (j + 1),
                                    state->colors[i][1] / (j + 1),
                                    state->colors[i][2] / (j + 1));
                        } else {
                            setRGB0(packet, 0xC0 / (j + 1), 0xC0 / (j + 1), 0xC0 / (j + 1));
                        }
                        func_8014F490(frame.quad, 12 - j, 24 - j, 0);
                        for (k = 0; k < 4; k++) {
                            SVECTOR *vertex = (SVECTOR *)((u8 *)&frame + k * 8 + 0x88);
                            SVECTOR *drop = (SVECTOR *)((u8 *)state + i * 8 + 0x264);

                            vertex->vx += drop->vx;
                            vertex->vy += drop->vy;
                            vertex->vz += drop->vz;
                            quad[k].vy += (j + 1) * 10;
                            quad[k].vy -= state->timers[i];
                        }
                        func_80151218(packet, frame.quad, 1, state->descriptor->mode);
                    }
                    state->timers[i] += 12;
                    if (state->timers[i] >= 0xA1) {
                        func_80153F28(state->colors[i], 0xF);
                        if ((u16)func_8014D378(state->colors[i]) != 0) {
                            state->timers[i] = 0;
                            func_8014F010(state->colors[i], 1);
                        }
                    } else {
                        func_80153F98(state->colors[i], state->descriptor->red >> 1,
                                      state->descriptor->green >> 1,
                                      state->descriptor->blue >> 1, 0xF);
                    }
                }
            }
            if ((state->updates & 3) == 0) {
                state->count += 1;
                if (state->count >= 0x41) {
                    state->count = 0x40;
                }
            }
        }
        if ((u16)func_8014D3AC(state->color_b) != 0) {
            if (state->stage == 1) {
                func_801556F4(state->color_b, 0xF);
            }
        }
        if (*(u32 *)&state->done_a == 0x10001) {
            D_8009B264->field_1D = 1;
        }
        if (state->stage == 0) {
            if (phase < -1) {
                if (state->descriptor->mode == 2) {
                    func_8014F010(state->color, 0x40);
                }
                state->stage = 1;
            }
        }
        state->frame += frame_step;
        state->updates++;
        PopMatrix();
        if ((u16)func_8014D378(state->color) != 0 &&
            (u16)func_8014D378(state->color_b) != 0 &&
            state->stage == 1) {
            D_8009B261 = 1;
        }
    }
}
