#include "../types.h"
#include "build_deck_test_and_set_initialized.h"

s32 BuildDeck_TestAndSetInitialized(BuildDeckTransitionState *state)
{
    u16 flags = state->state;
    if ((flags & 0x8000) == 0) {
        state->state = flags | 0x8000;
        return 0;
    }
    return 1;
}
