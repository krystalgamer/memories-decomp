#define gFade_State gJapanese_FadeState
#define D_8009B0D8 gJapanese_FrameStep
#define D_8009B0D8_IN_DATA
#include "../../types.h"
#include "../graphics_frame.h"
#include "../fade.h"

void Fade_StepBands(void)
{
    FadeTransitionState *state = &gJapanese_FadeState;
    s32 step = state->step;
    s32 i;
    s32 level;
    s32 acc;

    if (state->level < state->target_level) {
        acc = (s16)state->field_08;
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
        state->field_08 =
            state->field_08 + step * *(volatile s32 *)&gJapanese_FrameStep;
    } else {
        acc = (s16)state->field_08;
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
        state->field_08 =
            state->field_08 - step * *(volatile s32 *)&gJapanese_FrameStep;
    }
    if (level == state->target_level) {
        state->level = level;
    }
}
