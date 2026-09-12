#include "../types.h"
#include "build_deck_transition_state.h"
#include "build_deck_update_pane_transition.h"
#include "duel_transition_step_table.h"
#include "build_deck_pane_input.h"
#include "func_800339D0.h"

/* Initialized data at 0x80090DF8 and 0x80090E0C, previously a generated blob
   (#2602).

   D_80090DF8 is the build-deck pane transition step table.
   func_800339D0.c dispatches D_80090DF8[state & 0x3F] with the
   shared BuildDeckTransitionState. The mask permits sixty-four entries and
   there are five; that is recorded rather than corrected, because nothing
   shows the game produces a larger index and widening the array would change
   the bytes.

   D_80090E0C's first seven words are the divisor ladder
   Text_EncodeDecimalDigits indexes by digit count and then walks with
   d /= TEXT_DECIMAL_RADIX: 1, 1, 10, 100, 1000, 10000, 100000.

   The twelve words after it have no reader in tracked C and no symbol of
   their own. They are transcribed as they stand rather than named, because
   moving them out of the blob is what #2602 asks for and a name for them is
   not yet supported by anything. */
void (*D_80090DF8[])(BuildDeckTransitionState *) = {
    BuildDeck_UpdatePaneTransition,
    BuildDeck_UpdatePaneTransition,
    func_800336F0,
    func_8003353C,
    func_800339D0,
};

s32 D_80090E0C[] = {
    /* The divisor ladder, indexed by digit count. */
    0x00000001,
    0x00000001,
    0x0000000A,
    0x00000064,
    0x000003E8,
    0x00002710,
    0x000186A0,
    /* No reader in tracked C, and no symbol of its own. */
    0xF0000000,
    0xFF00EF00,
    0xFEF00EF0,
    0xFFF0EFF0,
    0xF0EF00EF,
    0xFFEFEFEF,
    0xFEFF0EFF,
    0xFFFFEFFF,
    0xBBBBFFFF,
    0x99997777,
    0x5555DDDD,
    0x00003333,
};
