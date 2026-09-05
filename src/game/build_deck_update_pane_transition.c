#include "../types.h"

typedef struct {
    u8 pad_0000[0x5AA4];
    s32 target;
    s32 step;
    s32 ticks;
    u8 pad_5AB0[0x633E - 0x5AB0];
    s16 previous;
    u16 next;
    u8 arrived;
} ScrollState;

extern u16 gGraphics_sViewportX;
extern s32 func_80032B38(void);
extern void SD_SEPlayFull(s32);

void BuildDeck_UpdatePaneTransition(ScrollState *state)
{
    s32 ticks;

    if (func_80032B38() == 0) {
        /* Keep the sound ID live for the signed-divide branch delay slot. */
        register s32 sound asm("$4") = 30;
        s32 diff = state->target - (s16)gGraphics_sViewportX;

        state->step = diff / 16;
        state->ticks = 16;
        SD_SEPlayFull(sound);
    }

    gGraphics_sViewportX += (u16)state->step;
    ticks = state->ticks - 1;
    state->ticks = ticks;
    if (ticks == 0) {
        u16 position = (u16)state->target;

        state->arrived = 0;
        gGraphics_sViewportX = position;
        if ((s32)position << 16) {
            state->arrived = 1;
        }
        state->previous = state->next;
    }
}
