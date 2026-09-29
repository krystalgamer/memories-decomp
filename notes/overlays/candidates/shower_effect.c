/* CANDIDATE, NOT MATCHING: French func_8014C8FC (0xA7C bytes), North
 * American func_8014D5D4, effect id 10. Compiled with gcc_2_8_1_g0_split
 * against the North American image it is at EXACT LENGTH (671 of 671
 * instructions) with an empty opcode census and SEVEN register-only rows
 * (2026-09-29, from 126 register rows and 15 structural rows). The residue
 * is one thing seen twice: the i-loop preheader hoists `quad = frame.quad`
 * (sp+160) before the vertex base (sp+24) where retail hoists them the
 * other way round, and retail's spill slots are 192 frame_step, 196 i*8,
 * 200 &frame.quad, 204 &frame against ours 192 frame_step, 196 &frame.quad,
 * 200 i*8, 204 &frame. Reload assigns those slots in ascending pseudo
 * number, so retail's fp+160 pseudo was created AFTER the k body's `i * 8`
 * temporary and is still hoisted out of the i loop across the colour test
 * (loop.c moves a user variable out of a maybe_never region only when it
 * is set and used in one block, and reg_in_basic_block_p tests that with
 * REGNO_FIRST_UID, which a hoisted insn's new uid never equals, so a
 * variable declared in the j or k body is hoisted once and then stuck in
 * the i body by construction; compiler temporaries move freely). Indexing
 * the adjustments through `packet` (`((SVECTOR *)((u8 *)packet + 0x48))[k]`)
 * to give the giv an opaque base was +10 and loses the packet register, so
 * which temporary retail's fp+160 pseudo was is not established.
 * Every spelling measured either keeps the user variable (this file: the
 * slot and hoist order are the only differences) or lets cse fold the
 * address into the vertex cursor and merge the two k-loop givs (-2 to +9):
 * frame.quad[k] inline, (&frame.quad[0])[k], (s32)-cast bases, array
 * pointer casts, a block-local `SVECTOR *q = &frame.quad[k]` before or
 * after the vertex pointer, a `u8 *base = (u8 *)&frame` variable with or
 * without `quad`, an `off = i * 8` variable (+9, spills), `quad` declared
 * in the i, j or k body, and `do { } while (0)` round the assignment.
 * `quad` assigned before the i loop (the previous state) is the same seven
 * rows plus the assignment sitting in the guard's delay slot instead of the
 * preheader.
 *
 * Levers that landed on 2026-09-29, each measured against the alternatives:
 * - the init-path quads loop and the update's quads[1] loop count with
 *   `i`, and only the drawing loop's inner counter is `j` (126 register
 *   rows to 13): one `j` for all three was a pseudo hot enough to take s1
 *   before `state`, which swapped s1/s2 through the whole function;
 * - `packet->clut` written before setUV4, right after `packet->tpage`
 *   (15 structural rows to 7);
 * - func_8014F490 takes three arguments, `(s16)(12 - j)` and `24 - j`; the
 *   fourth argument the earlier draft passed was the k loop's `move a3,zero`
 *   read as an argument, and the shared utility_helpers.h prototype is right
 *   as it stands;
 * - `quad = frame.quad` at the top of the i body, before the colour test,
 *   so loop.c hoists it into the preheader (4 structural rows to 0);
 * - `drop = (SVECTOR *)(i * 8 + (s32)state + 0x264)` for retail's
 *   `addu t1,t4,s1` operand order;
 * - func_8014EC8C's fourth argument is `(u16)(j * 8)` at the call, which
 *   gives retail's `andi 0xfff8` with the shared s32 prototype.
 * Earlier levers that still hold: `image = D_8015A8C8` as a base local
 * assigned inside the `if` that reads it, `words` as a base local for the
 * two texture-word reads, the odd-parity colour arm dividing 0xC0 by
 * j + 1, the count loop with one entry test, func_8014F010 before
 * `timers[i] = 0`, the done flags compared as one word, one frame struct
 * for the update's locals, and byte-offset vertex pointers at the top of
 * the innermost body.
 * The includes below are as they would be from src/overlays/duel_effects/;
 * only the shared headers are needed. */
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
#include "layered_drawing.h"
#include "textured_quads.h"
#include "dispatch.h"
#include "drawing_tail.h"
#include "shower_effect.h"

void func_8014C8FC(void *buffer, s32 phase)
{
    ShowerEffectState *state;
    ShowerEffectFrame frame;
    POLY_FT4 *packet;
    s32 frame_step;
    GsIMAGE *image;
    s32 i;
    s32 j;
    s32 k;
    SVECTOR *quad;

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
            for (i = 0; i < 2; i++) {
                func_8014EC8C(0xAF, 0xC2, 0x10, (u16)(i * 8), state->quads[i], 4);
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
                    for (i = 0; i < 4; i++) {
                        state->quads[1][i].vy = -state->rise;
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
            for (i = 0; i < state->count; i++) {
                quad = frame.quad;
                if ((u16)func_8014D3AC(state->colors[i]) != 0) {
                    ShowerEffectTextureWords *words = (ShowerEffectTextureWords *)&D_8015B748;

                    setPolyFT4(packet);
                    packet->tpage = words[12].page;
                    packet->clut = words[12].clut;
                    setUV4(packet, 0xE0, 0xC0, 0xFF, 0xC0, 0xE0, 0xDF, 0xFF, 0xDF);
                    for (j = 0; j < 4; j++) {
                        if (((state->updates + i + j) & 1) == 0) {
                            setRGB0(packet, state->colors[i][0] / (j + 1),
                                    state->colors[i][1] / (j + 1),
                                    state->colors[i][2] / (j + 1));
                        } else {
                            setRGB0(packet, 0xC0 / (j + 1), 0xC0 / (j + 1), 0xC0 / (j + 1));
                        }
                        func_8014F490(frame.quad, (s16)(12 - j), 24 - j);
                        for (k = 0; k < 4; k++) {
                            SVECTOR *vertex = (SVECTOR *)((u8 *)&frame + k * 8 + 0x88);
                            SVECTOR *drop = (SVECTOR *)(i * 8 + (s32)state + 0x264);

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
