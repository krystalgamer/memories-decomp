#include "../types.h"

/* The Build Deck card list, panes and per-frame update, fourteen contiguous
   functions from BuildDeck_CompareSortKeyDescending (0x80032B60) through
   BuildDeck_EnterWideConfirmMode: the descending and ascending unsigned-key
   comparators, both with ascending name-key ties, the sort and list input;
   the active-card lookup, deck and chest pane input and the pane transition;
   the open-slot test, exit confirmation and BuildDeck_Update with its step
   table; and the two confirm-mode setters. */

#include "duel_card.h"
#include "../psyq/qsort.h"
#include "card_list_text_boxes.h"
#include "rand_get_interval.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"
#include "card_list_sort.h"
#include "display_object.h"
#include "input.h"
#include "sound.h"
#include "card_list_sort_icons.h"
#include "build_deck_test_and_set_initialized.h"
#include "duel_card_viewer.h"
#include "build_deck_card_counts.h"
#include "duel_card_stat_display.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "build_deck_pane_input.h"
#include "graphics_frame.h"
#include "build_deck_update_pane_transition.h"
#include "text_channel_state.h"
#include "fade.h"
#include "fade_overlay_controls.h"
#include "text_box_lifecycle.h"
#include "text_box_update_channels.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "build_deck_update.h"
#include "duel_transition_step_table.h"
#include "../psyq/rand.h"
#include "card_list_setup.h"
#include "dialog_choice.h"
#include "build_deck_draw_card_list_page_data.h"
#include "duel_effect_mode_7.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
BuildDeckTransitionState *gBuildDeck_pState;
u8 gBuildDeck_bConfirmMode;

s32 BuildDeck_CompareSortKeyDescending(
    CardEntry *arg0,
    CardEntry *arg1
)
{
    u32 a = arg0->key;
    u32 b = arg1->key;

    if (a == b) {
        if (
            gCard_asNameSortKey[((s16)arg0->id) - 1] <
            gCard_asNameSortKey[((s16)arg1->id) - 1]
        )
            return -1;
        return 1;
    }
    if (a < b)
        return 1;
    return -1;
}

#ifndef CARD_LIST_VISIBLE_ROWS
#define CARD_LIST_VISIBLE_ROWS 8
#endif

s32 BuildDeck_CompareSortKeyAscending(
    CardEntry *arg0,
    CardEntry *arg1
)
{
    u32 a = arg0->key;
    u32 b = arg1->key;

    if (a == b) {
        if (
            gCard_asNameSortKey[((s16)arg0->id) - 1] <
            gCard_asNameSortKey[((s16)arg1->id) - 1]
        )
            return -1;
        return 1;
    }
    if (b < a)
        return 1;
    return -1;
}

/* Card-list sort. Builds a 32-bit sort key into each sixteen-byte row of the
   list at p and hands the block to qsort with one of two comparators, chosen
   by the list's sort_mode.

   Levers that mattered here:
   - The rows are walked with a pointer, not indexed. Indexing p[i] lets
     combine_givs merge the key access at +0 with the id/flag accesses at +4
     into a single giv and loses eight increments; walking the pointer keeps
     retail's second giv at +4.
   - The stat reads are written inline at every use rather than through a
     local. A local for gDuel_adwCardStats[id - 1] keeps the value in its own
     register and shifts the whole loop's allocation by one.
   - The key expressions are inline for the same reason: with atk and def as
     locals gcc schedules both multiplies before either shift, where retail
     finishes the attack half before starting the defence half.
   - Halved stats are spelled value * (CARD_STAT_SCALE / 2) and the comparison
     multiplies back up. Written the other way round, as
     (value * CARD_STAT_SCALE) >> 1, gcc folds the shift into the constant and
     emits sll 16 where retail has sll 17.
   - The first loop increments the index before the row pointer and every
     other loop the row pointer first, which is the order retail schedules the
     two induction variables in each. */

void BuildDeck_SortCardList(CardList *list)
{
    CardEntry *row;
    s32 i;
    s32 n;

    n = list->sort_row_count;
    row = list->entries;
    switch (list->sort_mode) {
    case 1:
        if (list->kind != 0) {
            for (i = 0; i < n; i++, row++) {
                row->key = 0xFFFF;
                if (row->flags != 0) {
                    row->key = ((s16)row->id);
                }
            }
        } else {
            for (i = 0; i < n; row++, i++) {
                row->key = ((s16)row->id);
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 2:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = 0;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 3:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flags != 0) {
                if ((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE >=
                    ((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                        CARD_STAT_SCALE) {
                    row->key =
                        (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 17) |
                        ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                } else {
                    row->key =
                        ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 17) |
                        (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                }
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyDescending);
        break;
    case 4:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flags != 0) {
                row->key =
                    (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 17) |
                    ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyDescending);
        break;
    case 5:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flags != 0) {
                row->key =
                    ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 17) |
                    (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyDescending);
        break;
    case 6:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = (gDuel_adwCardStats[((s16)row->id) - 1] >>
                            CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 8:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = 0x100;
                if (gBuildDeck_pState->card_sort_rank[((s16)row->id)] != 0) {
                    row->key = gBuildDeck_pState->card_sort_rank[((s16)row->id)];
                }
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 9:
        n = list->row_count;
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = Rand_GetInterval(0x1000);
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    }
    CardList_CreateSlotTextBoxes(list, CARD_LIST_VISIBLE_ROWS);
}

/* The two explicit volatile .data input views retain absolute pad loads under
 * the uniform -G8 compiler/assembler profile used by the surrounding card
 * list sort code. */

#ifndef CARD_LIST_LAST_CURSOR
#define CARD_LIST_LAST_CURSOR 7
#endif
#ifndef CARD_LIST_SCROLL_SCALE
#define CARD_LIST_SCROLL_SCALE 152
#endif
#ifndef CARD_LIST_SCROLL_Y_BASE
#define CARD_LIST_SCROLL_Y_BASE 0x29
#endif
#ifndef CARD_LIST_CURSOR_Y_BASE
#define CARD_LIST_CURSOR_Y_BASE 0x2A
#endif

/* Card-list cursor and page input handler.

   Levers that mattered here:
   - The retry path is a backward goto, not a for(;;)/continue loop: with a
     real loop construct loop.c hoists the last cursor constant into a
     callee-saved register instead of rematerialising it at each use.
   - The shared sub-row commit tail is written once and reached by
     "goto finish" from the second branch, which reproduces the retail block
     order (the tail sits between the L1/R1 block and the L2/R2 block).
     Duplicating it in both branches leaves the two copies unmerged.
   - The visible-row subtraction is respelled at every use; a single local
     would collapse the reloads the target makes at each join point.
   - The gCardList_SortIconTable lookup keeps CardList_UpdateSortMode's statement-by-statement
     spelling so the base and slot pseudos land in the same registers.
*/

s32 BuildDeck_UpdateCardListInput(CardList *list)
{
    s32 row;
    s32 sel;
    s32 icon;
    s32 slot;
    u8 *entry;

    row = list->first | list->cursor;
    if (row != 0) {
        row = (list->first + list->cursor + 1) * CARD_LIST_SCROLL_SCALE /
              list->sort_row_count;
    }
    list->scroll_box->xy.h.y = row + CARD_LIST_SCROLL_Y_BASE;

top:
    if (list->first != list->first_target) {
        if (list->first_target < list->first) {
            *(u16 *)&list->first = *(u16 *)&list->first - 1;
        } else {
            *(u16 *)&list->first = *(u16 *)&list->first + 1;
        }
        CardList_CreateSlotTextBoxes(list, CARD_LIST_VISIBLE_ROWS);
        return 1;
    }

    if ((gInput_awPadHeld[0] & PAD_BUTTON_L1_R1_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_awPadHeld[0] & PAD_BUTTON_R1) != 0) {
            if (row == list->row_count - CARD_LIST_VISIBLE_ROWS &&
                list->cursor != CARD_LIST_LAST_CURSOR) {
                sel = CARD_LIST_LAST_CURSOR;
            }
            row += CARD_LIST_VISIBLE_ROWS;
            if (list->row_count - CARD_LIST_VISIBLE_ROWS < row) {
                row = list->row_count - CARD_LIST_VISIBLE_ROWS;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= CARD_LIST_VISIBLE_ROWS;
        }
        if (row < 0) {
            row = 0;
        }
        list->first_target = row;
        if (list->first != row) {
            SD_SEPlayFull(6);
            goto top;
        }
    finish:
        if (sel >= 0) {
            list->cursor = sel;
            list->cursor_box->xy.h.y =
                sel * 22 + CARD_LIST_CURSOR_Y_BASE;
            SD_SEPlayFull(6);
        }
        return 1;
    }

    if ((gInput_awPadRepeat[0] & PAD_BUTTON_TRIGGER_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_awPadRepeat[0] & PAD_BUTTON_R2) != 0) {
            if (row == list->row_count - CARD_LIST_VISIBLE_ROWS &&
                list->cursor != CARD_LIST_LAST_CURSOR) {
                sel = CARD_LIST_LAST_CURSOR;
            }
            row += 0x32;
            if (list->row_count - CARD_LIST_VISIBLE_ROWS < row) {
                row = list->row_count - CARD_LIST_VISIBLE_ROWS;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= 0x32;
            if (row < 0) {
                row = 0;
            }
        }
        if (list->first != row) {
            SD_SEPlayFull(6);
            list->first_target = row;
            list->first = row;
            CardList_CreateSlotTextBoxes(list, CARD_LIST_VISIBLE_ROWS);
            return 1;
        }
        goto finish;
    }

    if ((gInput_awPadRepeat[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
        row = list->first;
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) != 0) {
            list->cursor += 1;
            if (list->cursor >= CARD_LIST_VISIBLE_ROWS) {
                row += 1;
                list->cursor = CARD_LIST_LAST_CURSOR;
            }
        } else {
            list->cursor -= 1;
            if (list->cursor < 0) {
                row -= 1;
                list->cursor = 0;
            }
        }
        list->cursor_box->xy.h.y =
            list->cursor * 22 + CARD_LIST_CURSOR_Y_BASE;
        if (list->row_count - CARD_LIST_VISIBLE_ROWS < row) {
            return 1;
        }
        if (row < 0) {
            return 1;
        }
        SD_SEPlayFull(6);
        list->first_target = row;
        if (list->first != row) {
            goto top;
        }
        return 1;
    }

    if ((gInput_awPadRepeat[0] & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) !=
        0) {
        if ((gInput_awPadRepeat[0] & PAD_BUTTON_START) != 0) {
            list->sort_choice += 1;
            if (list->sort_choice >= 7) {
                list->sort_choice = 0;
            }
        } else {
            list->sort_choice -= 1;
            if (list->sort_choice < 0) {
                list->sort_choice = 6;
            }
        }
        SD_SEPlayFull(0x2F);
        icon = list->kind;
        slot = list->sort_choice;
        icon = icon << 4;
        slot = slot << 1;
        entry = gCardList_SortIconTable + slot;
        icon = icon + (s32)entry;
        list->sort_mode = *(u8 *)(icon + 1) & 0xF;
        BuildDeck_SortCardList(list);
        return 1;
    }

    return 0;
}

s32 BuildDeck_GetActiveCardID(CardList *list)
{
    s32 index = list->first + list->cursor;
    CardEntry *entry = &list->entries[index];
    if (entry->flags == 0) {
        return 0;
    }
    return (s16)entry->id;
}

/* Regional values: the two BuildDeck_CreateCardTransferIcon calls' x, y and last argument (deck
 * pane 0x234, 0x16, 0xA -> 0x235, 0x14, 0xC; chest pane y 0x18 -> 0x14 and
 * last argument 0xC -> 0xD). The European build (src/game/european/) defines
 * its own. */
#ifndef BUILD_DECK_DECK_PANE_X
#define BUILD_DECK_DECK_PANE_X 0x234
#endif
#ifndef BUILD_DECK_DECK_PANE_Y
#define BUILD_DECK_DECK_PANE_Y 0x16
#endif
#ifndef BUILD_DECK_DECK_PANE_ARG4
#define BUILD_DECK_DECK_PANE_ARG4 0xA
#endif
#ifndef BUILD_DECK_CHEST_PANE_Y
#define BUILD_DECK_CHEST_PANE_Y 0x18
#endif
#ifndef BUILD_DECK_CHEST_PANE_ARG4
#define BUILD_DECK_CHEST_PANE_ARG4 0xC
#endif

/* The cancel and confirm buttons. The Japanese release swaps Cross and Circle,
 * so a regional build names its own. */
#ifndef BUILD_DECK_PANE_CANCEL_BUTTON
#define BUILD_DECK_PANE_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif
#ifndef BUILD_DECK_PANE_CONFIRM_MASK
#define BUILD_DECK_PANE_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void BuildDeck_UpdateDeckPaneInput(BuildDeckTransitionState *state) {
    CardList *e;
    s32 r;

    e = &state->lists[state->pane_index];

    BuildDeck_TestAndSetInitialized(state);

    if (BuildDeck_UpdateCardListInput(e) != 0) {
        return;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = r;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        }
        return;
    }

    if (gInput_awPadHeld[0] == PAD_DIRECTION_LEFT) {
        state->state = 1;
        state->viewport_target_x = 0;
        state->next_state = 2;
        return;
    }

    if ((gInput_awPadPressed[0] & BUILD_DECK_PANE_CANCEL_BUTTON) != 0) {
        state->state = 4;
        state->next_state = 3;
        return;
    }

    if ((gInput_awPadRepeat[0] & BUILD_DECK_PANE_CONFIRM_MASK) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            SD_SEPlayFull(7);
            e->entries[e->first + e->cursor].flags = 0;
            BuildDeck_SortCardList(&state->lists[1]);
            BuildDeck_RecountDeckCards(state);
            BuildDeck_ReturnCardToChest(state, r);
            BuildDeck_RefreshCountDisplay(state);
            BuildDeck_CreateCardTransferIcon(r, BUILD_DECK_DECK_PANE_X, BUILD_DECK_DECK_PANE_Y, 0x162, BUILD_DECK_DECK_PANE_ARG4);
            return;
        }
        SD_SEPlayFull(9);
    }
}

void BuildDeck_UpdateChestPaneInput(BuildDeckTransitionState *state)
{
    CardList *e;
    s32 r;
    u32 c;

    e = &state->lists[state->pane_index];
    BuildDeck_TestAndSetInitialized(state);
    if (BuildDeck_UpdateCardListInput(e) != 0) {
        return;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = r;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        }
        return;
    }

    if (gInput_awPadHeld[0] == PAD_DIRECTION_RIGHT) {
        state->viewport_target_x = 0x140;
        state->state = 1;
        state->next_state = 3;
        return;
    }

    if ((gInput_awPadPressed[0] & BUILD_DECK_PANE_CANCEL_BUTTON) != 0) {
        state->state = 4;
        state->next_state = 2;
        return;
    }

    if ((gInput_awPadRepeat[0] & BUILD_DECK_PANE_CONFIRM_MASK) == 0) {
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
            BuildDeck_TakeCardFromChest(state, r);
            BuildDeck_RefreshCountDisplay(state);
            BuildDeck_CreateCardTransferIcon(r, 3, BUILD_DECK_CHEST_PANE_Y, 0x11C, BUILD_DECK_CHEST_PANE_ARG4);
            return;
        }
    }

    SD_SEPlayFull(9);
}

#define BUILD_DECK_PANE_TRANSITION_TICKS 16

void BuildDeck_UpdatePaneTransition(BuildDeckTransitionState *state)
{
    s32 ticks;

    if (BuildDeck_TestAndSetInitialized(state) == 0) {
        s32 diff = state->viewport_target_x - (s16)gGraphics_sViewportX;

        state->viewport_step_x = diff / BUILD_DECK_PANE_TRANSITION_TICKS;
        state->transition_ticks = BUILD_DECK_PANE_TRANSITION_TICKS;
        SD_SEPlayFull(30);
    }

    gGraphics_sViewportX += (u16)state->viewport_step_x;
    ticks = state->transition_ticks - 1;
    state->transition_ticks = ticks;
    if (ticks == 0) {
        u16 position = (u16)state->viewport_target_x;

        state->pane_index = 0;
        gGraphics_sViewportX = position;
        if ((s32)position << 16) {
            state->pane_index = 1;
        }
        state->state = state->next_state;
    }
}

/* Initialized data this unit owns at 0x80090DF8 and 0x80090E0C, previously a generated blob
   (#2602).

   gBuildDeck_apfnStepHandlers is the build-deck pane transition step table.
   BuildDeck_Update dispatches gBuildDeck_apfnStepHandlers[state & 0x3F] with the
   shared BuildDeckTransitionState. The mask permits sixty-four entries and
   there are five; that is recorded rather than corrected, because nothing
   shows the game produces a larger index and widening the array would change
   the bytes.

   gText_DecimalDivisorView's first seven words are the divisor ladder
   Text_EncodeDecimalDigits indexes by digit count and then walks with
   d /= TEXT_DECIMAL_RADIX: 1, 1, 10, 100, 1000, 10000, 100000.

   The glyph nibble patterns and colour masks follow it. Only Japan's text
   renderer reads them (DuelEffect entries), but every image carries them. */
void (*gBuildDeck_apfnStepHandlers[])(BuildDeckTransitionState *) = {
    BuildDeck_UpdatePaneTransition,
    BuildDeck_UpdatePaneTransition,
    BuildDeck_UpdateChestPaneInput,
    BuildDeck_UpdateDeckPaneInput,
    BuildDeck_UpdateExitConfirmation,
};

s32 gText_DecimalDivisorView[] = {
    /* The divisor ladder, indexed by digit count. */
    0x00000001,
    0x00000001,
    0x0000000A,
    0x00000064,
    0x000003E8,
    0x00002710,
    0x000186A0,
};

/* The European build's divisor table ends with the ladder; these two do not
 * follow it there. */
#ifndef BUILD_DECK_DIVISOR_LADDER_ONLY
/* Japan's text renderer expands each glyph nibble through these sixteen
 * patterns and masks the result with the channel's colour. */
u16 gText_awGlyphNibblePatterns[16] = {
    0x0000,
    0xF000,
    0xEF00,
    0xFF00,
    0x0EF0,
    0xFEF0,
    0xEFF0,
    0xFFF0,
    0x00EF,
    0xF0EF,
    0xEFEF,
    0xFFEF,
    0x0EFF,
    0xFEFF,
    0xEFFF,
    0xFFFF,
};

u16 gText_awGlyphColorMasks[8] = {
    0xFFFF,
    0xBBBB,
    0x7777,
    0x9999,
    0xDDDD,
    0x5555,
    0x3333,
    0x0000,
};
#endif

/* Regional values: the confirmation boxes' y and flags (wide 0x78, 0x1028 ->
 * 0x84, 0x1050; narrow 0x60, 0x20 -> 0x6C, 0x40), the fade level (0xA0 ->
 * 0xC0) and the completion mask (0x2008 -> 0x2010). The European build
 * (src/game/european/) defines its own. */
#ifndef BUILD_DECK_WIDE_BOX_Y
#define BUILD_DECK_WIDE_BOX_Y 0x78
#endif
#ifndef BUILD_DECK_WIDE_BOX_FLAGS
#define BUILD_DECK_WIDE_BOX_FLAGS 0x1028
#endif
#ifndef BUILD_DECK_NARROW_BOX_Y
#define BUILD_DECK_NARROW_BOX_Y 0x60
#endif
#ifndef BUILD_DECK_NARROW_BOX_FLAGS
#define BUILD_DECK_NARROW_BOX_FLAGS 0x20
#endif
#ifndef BUILD_DECK_CONFIRM_FADE_LEVEL
#define BUILD_DECK_CONFIRM_FADE_LEVEL 0xA0
#endif
#ifndef BUILD_DECK_CONFIRM_COMPLETION_MASK
#define BUILD_DECK_CONFIRM_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#endif

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)

/* Reports whether the deck has a free slot: the first of DECK_SIZE entries in
 * gBuildDeck_pState->lists[1] whose flags byte is zero. The exit step below
 * is its only caller. It was its own gcc_2_8_1_g8 unit and is byte-identical
 * at gcc_2_8_1_g8_split. */
int BuildDeck_HasOpenDeckSlot(void)
{
    int index;
    CardEntry *entry = gBuildDeck_pState->lists[1].entries;
    for (index = 0; index < DECK_SIZE; index++, entry++) {
        if (entry->flags == 0) {
            return 1;
        }
    }
    return 0;
}

/* Handles leaving the deck editor. When the editor's own check passes, the
 * confirm sound plays and, if the deck has an open slot
 * (BuildDeck_HasOpenDeckSlot), bit 14 of the state word at +0x633E is set, a
 * confirmation box is created (the wide
 * one in the 640-wide mode selected by bit 7 of gBuildDeck_bConfirmMode, the narrow one
 * otherwise, which is then waited on until its prompt_object pointer is filled),
 * Fade_SetTargetLevel(0xA0, 2) runs and gFade_bKeepOverlayDepth is set from gDisplayObject_awOrderingTableDepthBases[1] - 8.
 * With bit 14 set the effect channel at gText_aChannels is polled: once its flags
 * match TEXT_BOX_FLAG_DONE under TEXT_BOX_COMPLETION_MASK, the box is
 * destroyed and either bit 14
 * is cleared (narrow mode with a choice made) or the state word is reloaded
 * from +0x6340 and Fade_SetTargetLevel(0xFF, 2) runs. Without bit 14 the CARD_COUNT
 * trunk bytes at +0x5D98 are copied after the DECK_SIZE halfwords of the
 * object at +0. Those halfwords are rebuilt from the 0x10-byte entries at
 * +0x2D54 whose byte 9 is set, Library_PruneRecentCardDrops runs and the state word is
 * cleared. */
void BuildDeck_UpdateExitConfirmation(BuildDeckTransitionState *record)
{
    BuildDeckTransitionState *workspace = record;
    DuelEffectChannel *box;
    u8 *src;
    u8 *dst;
    u16 *slot;
    CardEntry *entry;
    s32 mode;
    s32 i;

    if (BuildDeck_TestAndSetInitialized(workspace) == 0) {
        SD_SEPlayFull(8);
        if (BuildDeck_HasOpenDeckSlot() != 0) {
            /* The mode byte is read before the flag store, as retail
               schedules it. */
            mode = gBuildDeck_bConfirmMode & BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
            workspace->state |= 0x4000;
            if (mode) {
                ((DuelEffectChannel *)TextBox_CreateFlagged(
                    0, 8, 0x28, BUILD_DECK_WIDE_BOX_Y, 0xF0, 0x10, BUILD_DECK_WIDE_BOX_FLAGS
                ))->depth_offset = 0xA;
            } else {
                box = TextBox_CreateFlagged(0, 9, 0x30, BUILD_DECK_NARROW_BOX_Y, 0xE0, 0x30, BUILD_DECK_NARROW_BOX_FLAGS);
                box->depth_offset = 0xA;
                do {
                    TextBox_UpdateChannels();
                } while (box->prompt_object == 0);
            }
            Fade_SetTargetLevel(BUILD_DECK_CONFIRM_FADE_LEVEL, 2);
            gFade_bKeepOverlayDepth = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 8;
        }
    }
    /* Computed before the branch: it is only used on the copy path, so it
       crosses no call, and reorg lifts it into the branch delay slot. */
    src = &workspace->chest_card_quantities[CARD_ID_FIRST];
    if (workspace->state & 0x4000) {
        TextBox_UpdateChannels();
        /* The same variable as the confirmation box, which keeps the
           channel in $s0 across the destroy call. */
        box = gText_aChannels;
        if ((*(u32 *)&box->flags_34 & BUILD_DECK_CONFIRM_COMPLETION_MASK) ==
            TEXT_BOX_FLAG_DONE) {
            TextBox_Destroy(box);
            if (!(gBuildDeck_bConfirmMode & BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG) &&
                gDialog_bChoice != 0) {
                workspace->state &= 0xBFFF;
            } else {
                workspace->state = workspace->next_state;
                Fade_SetTargetLevel(0xFF, 2);
            }
        }
    } else {
        /* One counter for both loops, initialised before the pointers each
           time; that is what ranks the counter and the pointers into
           retail's argument registers. */
        i = 0;
        dst = (u8 *)(workspace->deck_cards + DECK_SIZE);
        for (; i < CARD_COUNT; i++) {
            *dst++ = *src++;
        }
        slot = workspace->deck_cards;
        i = 0;
        entry = workspace->lists[1].entries;
        for (; i < DECK_SIZE; i++) {
            *slot = 0;
            if (entry->flags != 0) {
                *slot = entry->id;
            }
            slot++;
            entry++;
        }
        Library_PruneRecentCardDrops();
        workspace->state = 0;
    }
}

/* Per-frame Build Deck transition driver. It pulses both pane colours, then
 * dispatches the current step through gBuildDeck_apfnStepHandlers; BuildDeck_UpdateExitConfirmation above is the
 * table's exit step. */
s32 BuildDeck_Update(void)
{
    s32 intensity;
    s32 color;
    BuildDeckTransitionState *base;
    DisplayObject *first;
    DisplayObject *second;

    rand();

    intensity = gMain_dwFrameCounter & 0x3F;
    if (intensity >= 0x20) {
        intensity = 0x3F - intensity;
    }

    base = gBuildDeck_pState;
    color = intensity * 2 + 0x40;
    first = base->lists[0].cursor_box;
    second = base->lists[1].cursor_box;

    DISPLAY_OBJECT_COLOR_BYTES(second)[2] = color;
    DISPLAY_OBJECT_COLOR_BYTES(second)[1] = color;
    DISPLAY_OBJECT_COLOR_BYTES(second)[0] = color;
    DISPLAY_OBJECT_COLOR_BYTES(first)[2] = color;
    DISPLAY_OBJECT_COLOR_BYTES(first)[1] = color;
    DISPLAY_OBJECT_COLOR_BYTES(first)[0] = color;

    if (DuelEffect_UpdateState() == 0) {
        gBuildDeck_apfnStepHandlers[gBuildDeck_pState->state & 0x3F](gBuildDeck_pState);
    }

    return gBuildDeck_pState->state;
}

void BuildDeck_EnterNarrowConfirmMode(void)
{
    gBuildDeck_bConfirmMode = 0;
    gMain_bActiveModeState = 7;
}

void BuildDeck_EnterWideConfirmMode(void)
{
    gBuildDeck_bConfirmMode = BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
    gMain_bActiveModeState = 7;
}
