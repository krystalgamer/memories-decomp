#include "../types.h"
#include "func_80032B38.h"
#include "graphics_frame.h"
#include "sound.h"
#include "build_deck_update_pane_transition.h"

extern u16 gGraphics_uViewportX asm("gGraphics_sViewportX");

void BuildDeck_UpdatePaneTransition(BuildDeckTransitionState *state)
{
    s32 ticks;

    if (func_80032B38(state) == 0) {
        s32 diff = state->viewport_target_x - (s16)gGraphics_uViewportX;

        state->viewport_step_x = diff / 16;
        state->transition_ticks = 16;
        SD_SEPlayFull(30);
    }

    gGraphics_uViewportX += (u16)state->viewport_step_x;
    ticks = state->transition_ticks - 1;
    state->transition_ticks = ticks;
    if (ticks == 0) {
        u16 position = (u16)state->viewport_target_x;

        state->pane_index = 0;
        gGraphics_uViewportX = position;
        if ((s32)position << 16) {
            state->pane_index = 1;
        }
        state->state = state->next_state;
    }
}
