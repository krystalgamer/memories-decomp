#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/rand.h"
#include "../../game/func_80058E1C.h"
#include "../../unmatched.h"
#include "utility_helpers.h"
#include "drawing_helpers.h"
#include "color_helpers.h"
#include "layered_drawing.h"
#include "textured_quads.h"
#include "dispatch.h"
#include "tile_effect.h"

#ifdef VERSION_EUROPE
#define TILE_ROW_STEP 30
#define TILE_Y_ORIGIN 106
#define TILE_V_BOTTOM -120
#else
#define TILE_ROW_STEP 28
#define TILE_Y_ORIGIN 98
#define TILE_V_BOTTOM 126
#endif

void func_80149F90(void *buffer, s32 phase)
{
    TileEffectState *state;
    POLY_GT4 polygon;
    POLY_FT4 sparkle;
    SVECTOR quad[4];
    POLY_GT4 *tile = &polygon;
    POLY_FT4 *spark = &sparkle;
    s32 frame_step;
    s32 k;
    s32 i;

    state = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            state->step = 1;
        } else {
            state->step = 0;
            state->descriptor = &D_8015A648[phase];
            func_8014F564(state->texture_words, &D_8015A62C, 1);
            for (k = 0; k < 5; k++) {
                for (i = 0; i < 7; i++) {
                    func_8014F010(state->colors_a[k][i], 0x80);
                    func_8014F010(state->colors_c[k][i], 0x80);
                    func_8014F010(state->colors_b[k][i], 0x80);
                    state->active[k][i] = 0;
                    state->sparkle[k][i] = rand() % 4;
                    state->cursor[k] = 6;
                    state->countdown[k] = state->descriptor->delay;
                    state->radius[k][i] = state->descriptor->size +
                                          rand() % (state->descriptor->size / 2);
                    state->jitter[k][i] = (rand() - rand()) % 12;
                }
            }
            state->frame = 0;
            state->updates = 0;
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
        setPolyGT4(tile);
        tile->tpage = state->texture_words[0];
        tile->clut = state->texture_words[1];
        setPolyFT4(spark);
        spark->tpage = ((TileEffectTextureWords *)&D_8015B748)[state->descriptor->texture].page;
        spark->clut = ((TileEffectTextureWords *)&D_8015B748)[state->descriptor->texture].clut;
        for (k = 0; k < 5; k++) {
            for (i = 0; i < 7; i++) {
                if ((u16)func_8014D3AC(state->colors_a[k][i]) != 0 ||
                    (u16)func_8014D3AC(state->colors_c[k][i]) != 0 ||
                    (u16)func_8014D3AC(state->colors_b[k][i]) != 0) {
                    setRGB0(tile, state->colors_a[k][i][0], state->colors_a[k][i][1], state->colors_a[k][i][2]);
                    setRGB1(tile, state->colors_a[k][i][0], state->colors_a[k][i][1], state->colors_a[k][i][2]);
                    setRGB2(tile, state->colors_c[k][i][0], state->colors_c[k][i][1], state->colors_c[k][i][2]);
                    setRGB3(tile, state->colors_c[k][i][0], state->colors_c[k][i][1], state->colors_c[k][i][2]);
                    tile->x0 = k * 28 - 70;
                    tile->y0 = i * TILE_ROW_STEP - TILE_Y_ORIGIN;
                    tile->x1 = k * 28 - 42;
                    tile->y1 = i * TILE_ROW_STEP - TILE_Y_ORIGIN;
                    tile->x2 = k * 28 - 70;
                    tile->y2 = i * TILE_ROW_STEP - (TILE_Y_ORIGIN - TILE_ROW_STEP);
                    tile->x3 = k * 28 - 42;
                    tile->y3 = i * TILE_ROW_STEP - (TILE_Y_ORIGIN - TILE_ROW_STEP);
                    setUV4(tile, tile->x0 + 70, tile->y0 + TILE_Y_ORIGIN,
                           tile->x0 + 98, tile->y0 + TILE_Y_ORIGIN,
                           tile->x0 + 70, tile->y0 + TILE_V_BOTTOM,
                           tile->x0 + 98, tile->y0 + TILE_V_BOTTOM);
                    quad[0].vx = tile->x0 + state->jitter[k][i];
                    quad[0].vy = tile->y0 + state->jitter[k][i];
                    quad[0].vz = 0;
                    quad[1].vx = tile->x1 + state->jitter[k][i];
                    quad[1].vy = tile->y1 + state->jitter[k][i];
                    quad[1].vz = 0;
                    quad[2].vx = tile->x2 + state->jitter[k][i];
                    quad[2].vy = tile->y2 + state->jitter[k][i];
                    quad[2].vz = 0;
                    quad[3].vx = tile->x3 + state->jitter[k][i];
                    quad[3].vy = tile->y3 + state->jitter[k][i];
                    quad[3].vz = 0;
                    if (state->active[k][i] == 1) {
                        func_80153F28(state->colors_a[k][i], 0xF);
                        func_80153F28(state->colors_c[k][i], 0x1F);
                        setRGB0(spark, state->colors_b[k][i][0], state->colors_b[k][i][1], state->colors_b[k][i][2]);
                        if (state->descriptor->texture == 4) {
                            setUV4(spark, state->sparkle[k][i] * 32 - 0x80, 0x40,
                                   state->sparkle[k][i] * 32 - 0x61, 0x40,
                                   state->sparkle[k][i] * 32 - 0x80, 0x7F,
                                   state->sparkle[k][i] * 32 - 0x61, 0x7F);
                        } else {
                            setUV4(spark, state->sparkle[k][i] * 32, 0x80,
                                   state->sparkle[k][i] * 32 + 0x1F, 0x80,
                                   state->sparkle[k][i] * 32, 0xBF,
                                   state->sparkle[k][i] * 32 + 0x1F, 0xBF);
                        }
                        spark->x0 = quad[0].vx - state->radius[k][i];
                        spark->y0 = quad[0].vy - state->radius[k][i] * 3;
                        spark->x1 = quad[1].vx + state->radius[k][i];
                        spark->y1 = quad[1].vy - state->radius[k][i] * 3;
                        spark->x2 = quad[2].vx - state->radius[k][i];
                        spark->y2 = quad[2].vy + state->radius[k][i];
                        spark->x3 = quad[3].vx + state->radius[k][i];
                        spark->y3 = quad[3].vy + state->radius[k][i];
                        func_80152F9C(spark, 1);
                        func_80153F28(state->colors_b[k][i], 8);
                        state->sparkle[k][i] = (state->sparkle[k][i] + 1) % 4;
                        func_801530B0(tile, 1);
                    } else {
                        func_801530B0(tile, 0);
                    }
                }
            }
            if (state->cursor[k] < 7) {
                i = rand() % state->countdown[k];
                if (i == 0) {
                    state->active[k][state->cursor[k]] = 1;
                    state->countdown[k] = state->descriptor->delay;
                    if (state->cursor[k] == 0) {
                        state->cursor[k] = 7;
                    } else {
                        state->cursor[k]--;
                    }
                } else {
                    state->countdown[k]--;
                }
            }
        }
        state->frame += frame_step;
        state->updates++;
        if (state->descriptor->frames < state->frame) {
            D_8009B261 = 1;
        }
    }
}
