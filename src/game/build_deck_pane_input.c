#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../types.h"
#include "build_deck_active_card.h"
#include "func_80032B38.h"
#include "card_list_sort.h"
#include "card_list_text_boxes.h"
#include "card_constants.h"
#include "duel_card_viewer.h"
#include "input.h"
#include "sound.h"
#include "build_deck_card_counts.h"
#include "build_deck_transition_state.h"
#include "duel_card_stat_display.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "build_deck_pane_input.h"

void BuildDeck_UpdateDeckPaneInput(BuildDeckTransitionState *state) {
#define p ((u8 *)state)
    u8 *e;
    s32 r;

    e = p + (state->pane_index * 0x2D4C + 4);

    func_80032B38(state);

    if (BuildDeck_UpdateCardListInput((CardList *)e) != 0) {
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_TRIANGLE) != 0) {
        r = BuildDeck_GetActiveCardID((CardList *)e);
        if (r != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = r;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        }
        return;
    }

    if (gInput_wPad1Held == PAD_DIRECTION_LEFT) {
        state->state = 1;
        state->viewport_target_x = 0;
        state->next_state = 2;
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        state->state = 4;
        state->next_state = 3;
        return;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_CONFIRM_MASK) != 0) {
        r = BuildDeck_GetActiveCardID((CardList *)e);
        if (r != 0) {
            SD_SEPlayFull(7);
            *(e + 0xD - -((*(s16 *)(e + 0x2D3C) +
                             *(s8 *)(e + 0x2D48)) * 0x10)) = 0;
            func_80032C48((CardList *)(p + 0x2D50));
            func_8003201C(p);
            func_80031EE4(p, r);
            func_80031E5C(p);
            func_80031574(r, 0x234, 0x16, 0x162, 0xA);
            return;
        }
        SD_SEPlayFull(9);
    }
#undef p
}

void BuildDeck_UpdateChestPaneInput(BuildDeckTransitionState *state)
{
    CardList *e;
    s32 r;
    u32 c;

    e = &state->lists[state->pane_index];
    func_80032B38(state);
    if (BuildDeck_UpdateCardListInput(e) != 0) {
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_TRIANGLE) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = r;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        }
        return;
    }

    if (gInput_wPad1Held == PAD_DIRECTION_RIGHT) {
        state->viewport_target_x = 0x140;
        state->state = 1;
        state->next_state = 3;
        return;
    }

    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        state->state = 4;
        state->next_state = 2;
        return;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_CONFIRM_MASK) == 0) {
        return;
    }

    r = BuildDeck_GetActiveCardID(e);
    c = 1;
    if ((u32)(r - EXODIA_FIRST_CARD_ID) < EXODIA_PIECE_COUNT) {
        c = state->deck_card_quantities[r] < c;
    }

    if (r != 0 && c != 0) {
        if (state->deck_total < DECK_SIZE &&
            state->chest_card_quantities[r] != 0 &&
            state->deck_card_quantities[r] < DECK_CARD_COPY_LIMIT) {
            SD_SEPlayFull(7);
            BuildDeck_AddCard((s32)state, r);
            func_80031F7C((u8 *)state, r);
            func_80031E5C((u8 *)state);
            func_80031574(r, 3, 0x18, 0x11C, 0xC);
            return;
        }
    }

    SD_SEPlayFull(9);
}
