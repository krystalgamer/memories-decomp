/* CANDIDATE, NOT MATCHING: French func_80148BA4 (0x13EC bytes), North
 * American func_80158E84, effect id 0x11. Compiled with gcc_2_8_1_g0_split
 * against the North American image it is 20 instructions short (1255 of
 * 1275) after one draft and two rounds. The init path (descriptor copy,
 * card collection and the per-card move/spin vectors through ratan2, the
 * spark and curve seeding) matches apart from the loop counter and offset
 * givs swapping s3/s4. The update's residue is in the spark loop (retail
 * addresses the spark through a base + offset giv for the call and through
 * two other cursors for the range test and the update, and reads the
 * descriptor size once per respawn component before the modulo), in the
 * two texture-word reads (they want the same base-local spelling that the
 * shower candidate documents), and in the inner rotation loops of the
 * spinning quad, whose k counter retail keeps in s2. Levers that landed:
 * the frame locals are separate (POLY_GT4 is 0x34 bytes, so the quad after
 * it is 8-aligned), `state->step = 0` before the descriptor store, the card
 * pointer re-read from D_8015B7A0[i] for every field, the spin pointer
 * assigned after the move stores, u16 `spread` for the doubled size, the
 * curve position pointer assigned after the two calls, and `fade >= 9` as
 * the then-arm. func_8015405C is called with an unmasked u16 three times,
 * so this unit needs the u16 declaration effect_routines.h carries rather
 * than color_helpers.h's u8 one; the callee itself only matches with u8.
 *
 * BLOCKED BY THE DECLARATION RULE (measured 2026-09-29): retail passes
 * state->fade to func_8015405C three times unmasked, which needs the u16
 * prototype effect_routines.h carries, while color_helpers.h declares the
 * u8 one that color_transition.c is matched with. Changing the shared
 * declaration and the definition to u16 turns the callee's own `andi 0xff`
 * masks into `andi 0xffff` (three instructions of color_transition.c's -S
 * output), so no single declaration serves both units; with the u8
 * prototype from color_helpers.h this candidate is still 0x139C against
 * 0x13EC and gains three `andi 0xff` at the fade calls. func_8014FABC has no
 * declaration under src/ and would belong to vortex_effect.h. Nothing else
 * of the 20-instruction residue was worked on this pass.
 * The includes below are as they would be from src/overlays/duel_effects/. */
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../game/duel_card.h"
#include "../../game/model_state_setters.h"
#include "../../game/screen_projection.h"
#include "../../unmatched.h"
#include "utility_helpers.h"
#include "drawing_helpers.h"
#include "packet_helpers.h"
#include "effect_routines.h"
#include "vortex_effect.h"

void func_80148BA4(void *buffer, s32 phase)
{
    VortexEffectState *state;
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    VECTOR scale;
    POLY_GT4 polygon;
    SVECTOR quad[4];
    u8 half[4];
    POLY_GT4 *packet = &polygon;
    s32 i;
    s32 j;
    s32 k;
    u16 spread;

    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146034;
    state = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            state->step = 1;
        } else {
            state->step = 0;
            state->descriptor = &D_8015A60C[phase];
            state->center.vx = D_8015B7F8.vx;
            state->center.vy = D_8015B7F8.vy;
            state->center.vz = D_8015B7F8.vz;
            if (state->descriptor->mode == 0) {
                Duel_CollectMatchingFieldCardObjects((u32 *)D_8015B7A0, -1);
                state->position.vx = 0;
                state->position.vz = 0;
                state->position.vy = -state->descriptor->radius;
            } else {
                Duel_CollectMatchingFieldCardObjects((u32 *)D_8015B7A0, 0);
                state->position.vx = D_8015B7F8.vx;
                state->position.vy = D_8015B7F8.vy - state->descriptor->radius;
                state->position.vz = D_8015B7F8.vz;
            }
            state->card_count = 0;
            if (D_8015B7A0[0] != (DisplayObject *)0) {
                i = 0;
                do {
                    SVECTOR *move = (SVECTOR *)((u8 *)state + i * 8 + 0xC);
                    SVECTOR *spin;

                    move->vx = (state->position.vx - (s16)D_8015B7A0[i]->field_30.h.field_30) / 16;
                    move->vy = (state->position.vy - (s16)D_8015B7A0[i]->field_30.h.field_32) / 16;
                    move->vz = (state->position.vz - D_8015B7A0[i]->field_34.h.field_34) / 16;
                    spin = (SVECTOR *)((u8 *)state + i * 8 + 0xAC);
                    spin->vx = ratan2(state->moves[i].vz, state->moves[i].vy) % 256 / 16;
                    spin->vy = ratan2(state->moves[i].vx, state->moves[i].vz) % 256 / 16;
                    spin->vz = ratan2(state->moves[i].vy, state->moves[i].vx) % 256 / 16;
                    func_8014F608(state->trails[i], 0x80, 0x80, 0x80, 16);
                    state->card_count++;
                    state->card_flags[i] = 0;
                    state->card_steps[i] = 0;
                    i++;
                } while (D_8015B7A0[i] != (DisplayObject *)0);
            }
            spread = state->descriptor->size * 2;
            func_8014F608(state->sparks, spread, spread, spread, 64);
            for (i = 0; i < 64; i++) {
                SVECTOR *velocity = (SVECTOR *)((u8 *)state + i * 8 + 0xD4C);

                velocity->vx = -state->sparks[i].vx / 32;
                velocity->vy = -state->sparks[i].vy / 32;
                velocity->vz = -state->sparks[i].vz / 32;
                func_8014F020(state->spark_colors[i], state->descriptor->red >> 3,
                              state->descriptor->green >> 3, state->descriptor->blue >> 3);
            }
            for (i = 0; i < 48; i++) {
                SVECTOR *curve;

                func_8014FABC(0x18, 8, state->descriptor->radius, 8, state->curves[i]);
                func_8014F020(state->curve_colors[i], state->descriptor->red_b,
                              state->descriptor->green_b, state->descriptor->blue_b);
                curve = (SVECTOR *)((u8 *)state + i * 8 + 0x1B4C);
                curve->vx = 0;
                curve->vy = 0;
                curve->vz = 0;
                state->curve_steps[i] = 0;
                state->curve_states[i] = 0;
                state->curve_angles[i] = rand() % 512;
            }
            state->frames = 0;
            func_8014F020(state->color, state->descriptor->red >> 3,
                          state->descriptor->green >> 3, state->descriptor->blue >> 3);
            func_8014F010(state->color_b, 0);
            state->spin = 0;
            state->arrived = 0;
            state->stage = 0;
            state->fade = 0x80;
            state->cards_active = 0;
            state->spark_count = 0;
            state->curve_count = 0;
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
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        if (state->stage < 2) {
            if (state->fade >= 9) {
                state->fade -= 8;
            } else {
                state->fade = 0;
            }
        } else if (state->fade < 0x78) {
            state->fade += 8;
        } else {
            state->fade = 0x80;
            state->stage = 3;
        }
        func_8015405C(state->fade, state->fade, state->fade);
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = state->descriptor->spin * state->frames % 4096;
        func_801513F4(&world, &saved, &state->position, &rotation, &scale, 0);
        for (i = 0; i < state->spark_count; i++) {
            if ((u16)func_8014D3AC(state->spark_colors[i]) != 0) {
                SVECTOR *spark = &state->sparks[i];

                func_8014F2D4(quad, spark);
                if ((i & 3) == 0) {
                    func_80155BC0(state->spark_colors[i], 8, 1, spark);
                }
                func_80156064(state->spark_colors[i], quad, 1);
                if (__builtin_abs(state->sparks[i].vx) >= 0x21 ||
                    __builtin_abs(state->sparks[i].vy) >= 0x21 ||
                    __builtin_abs(state->sparks[i].vz) >= 0x21) {
                    SVECTOR *position = (SVECTOR *)((u8 *)state + i * 8 + 0xB4C);
                    SVECTOR *velocity = (SVECTOR *)((u8 *)state + i * 8 + 0xD4C);

                    position->vx += velocity->vx;
                    position->vy += velocity->vy;
                    position->vz += velocity->vz;
                    func_80153F98(state->spark_colors[i], state->descriptor->red,
                                  state->descriptor->green, state->descriptor->blue, 0x1F);
                } else {
                    func_80153F28(state->spark_colors[i], 0x1F);
                    if ((u16)func_8014D378(state->spark_colors[i]) != 0) {
                        if (state->card_flags[state->card_count - 1] == 0) {
                            SVECTOR *position;
                            SVECTOR *velocity;
                            s32 offset;

                            position = (SVECTOR *)((u8 *)state + i * 8 + 0xB4C);
                            offset = (rand() - rand()) % 4096 * 2;
                            position->vx = state->descriptor->size * offset / 4096;
                            offset = (rand() - rand()) % 4096 * 2;
                            position->vy = state->descriptor->size * offset / 4096;
                            offset = (rand() - rand()) % 4096 * 2;
                            position->vz = state->descriptor->size * offset / 4096;
                            velocity = (SVECTOR *)((u8 *)state + i * 8 + 0xD4C);
                            velocity->vx = -state->sparks[i].vx / 32;
                            velocity->vy = -state->sparks[i].vy / 32;
                            velocity->vz = -state->sparks[i].vz / 32;
                            func_8014F020(state->spark_colors[i], state->descriptor->red >> 3,
                                          state->descriptor->green >> 3,
                                          state->descriptor->blue >> 3);
                        }
                    }
                    if ((u16)func_8014D378(state->spark_colors[i]) == 0) {
                        if (state->stage < 2) {
                            state->stage = 2;
                        }
                    }
                }
            }
        }
        if ((u16)func_8014D3AC(state->color) != 0) {
            setPolyGT4(packet);
            packet->tpage = ((VortexEffectTextureWords *)&D_8015B748)[15].page;
            setUV4(packet, 0x60, 0, 0x9F, 0, 0x60, 0x3F, 0x9F, 0x3F);
            packet->clut = ((VortexEffectTextureWords *)&D_8015B748)[15].clut;
            quad[0].vx = -state->descriptor->size;
            quad[0].vy = -state->descriptor->size;
            quad[0].vz = 0;
            quad[1].vx = -state->descriptor->size;
            quad[1].vy = 0;
            quad[1].vz = 0;
            quad[2].vx = 0;
            quad[2].vy = -state->descriptor->size;
            quad[2].vz = 0;
            quad[3].vx = 0;
            quad[3].vy = 0;
            quad[3].vz = 0;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            setRGB3(packet, 0, 0, 0);
            for (k = 0; k < 3; k++) {
                scale.vx = 0x1000 - k * 0x500;
                scale.vy = 0x1000 - k * 0x500;
                scale.vz = 0;
                setRGB0(packet, state->color[0] / (k + 1), state->color[1] / (k + 1),
                        state->color[2] / (k + 1));
                setRGB1(packet, state->color[0] / (k + 1), state->color[1] / (k + 1),
                        state->color[2] / (k + 1));
                setRGB2(packet, state->color[0] / (k + 1), state->color[1] / (k + 1),
                        state->color[2] / (k + 1));
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 3; j++) {
                        rotation.vx = 0;
                        rotation.vy = 0;
                        rotation.vz = -(state->frames * ((k * 2 + 1) * state->descriptor->rate) +
                                              j * 32 + i * 1024) % 4096;
                        func_801513F4(&world, &saved, &state->position,
                                      &rotation, &scale, 0);
                        func_8015131C(packet, quad, 1, 1);
                    }
                }
            }
            if (state->spin < 0x1000) {
                if (state->frames < 0x3E7) {
                    state->spin += 0x40;
                } else {
                    state->spin -= 0x40;
                }
            } else if (state->frames >= 0x3E7) {
                state->spin -= 0x40;
            }
            if (state->stage < 2) {
                state->stage = func_80153F98(state->color, state->descriptor->red,
                                             state->descriptor->green,
                                             state->descriptor->red, 2);
            } else {
                func_80153F28(state->color, 1);
            }
        }
        if (state->card_count != 0 && state->stage < 2) {
            state->spark_count++;
            if (state->spark_count >= 0x41) {
                state->spark_count = 0x40;
            }
        }
        if (state->stage != 0 && state->frames >= 0x3D) {
            if (state->card_count == 0 && state->stage == 1) {
                state->stage = 2;
            }
            for (i = 0; i < state->cards_active; i++) {
                DisplayObject *object = D_8015B7A0[i];

                if (state->card_flags[i] == 0) {
                    object->field_30.h.field_30 += state->moves[i].vx;
                    object->field_30.h.field_32 += state->moves[i].vy;
                    object->field_34.h.field_34 += state->moves[i].vz;
                    object->field_44.h.field_44 -= 0x100;
                    object->field_44.h.field_46 -= 0x100;
                    object->field_20.b.field_20 += state->spins[i].vx;
                    object->field_20.b.field_21 += state->spins[i].vy;
                    object->field_20.b.field_22 += state->spins[i].vz;
                    state->card_steps[i]++;
                    if (state->card_steps[i] >= 0x11) {
                        state->card_flags[i] = 1;
                    }
                }
                if (state->card_flags[i] == 1) {
                    state->arrived++;
                    object->flags &= 0xFFBF;
                    state->card_flags[i] = 2;
                    func_8014F020(state->color_b, state->descriptor->red_b,
                                  state->descriptor->green_b, state->descriptor->blue_b);
                }
            }
            if ((state->frames & 7) == 0) {
                state->cards_active++;
                if (state->cards_active > state->card_count) {
                    state->cards_active = state->card_count;
                }
            }
        }
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        func_801513F4(&world, &saved, &state->position, &rotation, &scale, 0);
        if ((u16)func_8014D3AC(state->color_b) != 0) {
            func_80155BC0(state->color_b, 0x20, 1, (SVECTOR *)0);
            *(u32 *)half = *(u32 *)state->color_b;
            half[0] >>= 1;
            half[1] >>= 1;
            half[2] >>= 1;
            func_80156E58(half, 3, (SVECTOR *)((u8 *)state + state->arrived * 128 + 0xCC), 16, 0x24);
            func_80153F28(state->color_b, 0xF);
            if (state->card_flags[state->card_count - 1] >= 2) {
                if ((u16)func_8014D378(state->color_b) != 0) {
                    state->stage = 2;
                }
            }
        }
        if (state->descriptor->mode != 0 && state->card_count != 0 && state->curve_count != 0) {
            for (i = 0; i < state->curve_count; i++) {
                u8 *color = state->curve_colors[i];

                rotation.vx = 0;
                rotation.vy = state->curve_angles[i];
                rotation.vz = 0;
                func_801513F4(&world, &saved, &state->center, &rotation,
                              &scale, 2);
                if ((u16)func_8014D3AC(color) != 0) {
                    if (state->curve_states[i] != 2) {
                        state->curve_states[i] = func_801570B0(color, state->curve_steps[i],
                            state->curves[i], 0x10, 1, &state->curve_positions[i], 1, &state->center);
                    } else {
                        func_801570B0(color, state->curve_steps[i], state->curves[i], 0x20, 0,
                                      &state->curve_positions[i], 1, &state->center);
                    }
                    if (state->curve_states[i] == 2 && (u16)func_8014D3AC(color) != 0) {
                        func_801513F4(&world, &saved, &state->curve_positions[i],
                                      &rotation, &scale, 0);
                        func_80155BC0(color, 4, 1, (SVECTOR *)0);
                    }
                    state->curve_steps[i]++;
                    if (state->curve_steps[i] >= 8) {
                        state->curve_steps[i] = 7;
                        func_80153F28(color, 0xF);
                        if ((u16)func_8014D378(color) != 0 && state->stage < 2) {
                            func_8014F020(color, state->descriptor->red_b,
                                          state->descriptor->green_b, state->descriptor->blue_b);
                            state->curve_steps[i] = 0;
                        }
                    }
                }
            }
        }
        if (state->stage == 1) {
            state->curve_count++;
            if (state->curve_count >= 0x31) {
                state->curve_count = 0x30;
            }
        }
        state->frames++;
        PopMatrix();
        if (state->stage == 3 && state->frames >= 0xB5) {
            D_8009B261 = 1;
        }
    }
}
