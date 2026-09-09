#include "../types.h"
#include "graphics_frame.h"

#include "fade.h"

/* Refills 15 mirrored band pairs using the configured step. Increasing
   transitions visit the middle pair first and subtract step from the signed
   field_08 accumulator; decreasing transitions visit the outer pair first
   and add step. Values are clamped between level and target_level. After
   the pass, the head update uses the opposite sign, scaled by D_8009B0D8. */

void Fade_StepBands(void)
{
    FadeTransitionState *state = &gFade_State;
    s32 step = state->step;
    s32 i;
    s32 level;
    s32 acc;

    if (state->level < state->target_level) {
        acc = (s16) state->field_08;
        for (i = FADE_BAND_COUNT / 2 - 1; i >= 0; i--) {
            acc = acc - step;
            level = acc;
            if (level < state->level) {
                level = state->level;
            }
            if (state->target_level < level) {
                level = state->target_level;
            }
            state->band_levels[i] = level;
            state->band_levels[FADE_BAND_COUNT - 1 - i] = level;
        }
        state->field_08 = state->field_08 + step * D_8009B0D8;
    } else {
        acc = (s16) state->field_08;
        for (i = 0; i < FADE_BAND_COUNT / 2; i++) {
            acc = acc + step;
            level = acc;
            if (level < state->target_level) {
                level = state->target_level;
            }
            if (state->level < level) {
                level = state->level;
            }
            state->band_levels[i] = level;
            state->band_levels[FADE_BAND_COUNT - 1 - i] = level;
        }
        state->field_08 = state->field_08 - step * D_8009B0D8;
    }
    if (level == state->target_level) {
        state->level = level;
    }
}
