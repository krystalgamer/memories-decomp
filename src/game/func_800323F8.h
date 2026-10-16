#ifndef MEMORIES_DECOMP_FUNC_800323F8_H
#define MEMORIES_DECOMP_FUNC_800323F8_H

#include "../types.h"
#include "build_deck_transition_state.h"

/* Sets up the Build Deck screen over two contiguous workspaces at `base`.
 * The implementation retains a byte cursor to preserve retail allocation;
 * that cursor does not change the caller-visible workspace type.
 * `deck` is the first pane's deck (forty card ids, then the 722 chest counts at +0x50),
 * `other` the second pane's deck or 0, and `flags` the byte kept at +0x6343.
 * Two callers: Main_RunBuildDeckMenu and Main_RunDuel. */
void BuildDeck_InitScreen(BuildDeckTransitionState *base, void *deck, s32 other, s32 flags);

#endif
