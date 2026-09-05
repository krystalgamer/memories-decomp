#include "../types.h"

#include "fade.h"

/* Refills the fade band ramp symmetrically from both ends toward the middle.
   Each pass walks the running level one step further from field_08, clamps it
   between level and target_level, and writes it to band i and band 29 - i.
   The direction of both the walk and the clamp order follows whether the
   current level is below or above the target. */

extern s32 D_8009B0D8;

void func_800151D8(void)
{
    FadeTransitionState *state = &D_800E9EC8;
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
