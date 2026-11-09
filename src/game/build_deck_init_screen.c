/*
 * BuildDeck_InitScreen sets up the Build Deck screen: it initialises the two
 * 0x6344-byte deck records (recent-drop ranks, the 40-card deck list with
 * its stats, and the 722-card chest list with its counts), sorts both lists,
 * then creates the screen's eleven display objects. It builds under
 * gcc_2_8_1_g8_split_comm, the profile of its neighbours card_list_setup.c
 * and build_deck_input.c.
 *
 * Shapes measured on this body under strength reduction:
 * - the list headers, totals and rank row are reached as members of `state`
 *   and the chest sort receives &state->lists[0] last, so loop.c bases every
 *   record address on that state + 4 value, as retail does;
 * - the rank table is indexed by the card id directly; both list loops walk
 *   `entry` (and the deck and quantity cursors) beside the shared counter `j`;
 * - the deck stats are read as gDuel_adwCardStats[card - 1], which keeps the
 *   decrement ahead of the scale;
 * - `card` holds the deck card id and then the chest card id, and `n` is
 *   both the drop index and the chest total. Those shared lifetimes, the
 *   `on` local and the literal 0x80 decide the $a0-$t3 allocation.
 *
 * The display objects keep their raw slot offsets on the record.
 */
#include "../types.h"
#include "graphics_frame.h"
#include "card_list_setup.h"
#include "duel_card.h"
#include "build_deck_transition_state.h"
#include "build_deck_card_counts.h"
#include "card_list_sort.h"
#include "build_deck_draw_card_list_page.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "sound.h"
#include "build_deck_init_screen.h"
#include "card_list_sort_icons.h"
#include "build_deck_test_and_set_initialized.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
BuildDeckTransitionState *gBuildDeck_pState;

#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

#ifndef BUILD_DECK_CURSOR_Y
#define BUILD_DECK_CURSOR_Y 0x29
#endif

#ifndef BUILD_DECK_LIST_Y
#define BUILD_DECK_LIST_Y 0x2A
#endif

void BuildDeck_InitScreen(BuildDeckTransitionState *base, void *deck, s32 other, s32 flags)
{
    BuildDeckTransitionState *state;
    u8 *object;
    u8 *label;
    CardList *chest_list;
    CardList *deck_list;
    CardEntry *entry;
    u8 *quantity;
    s16 *drops;
    u16 *cards;
    u8 *icons;
    s32 pane;
    s32 j;
    s32 on;
    s32 total;
    s32 n;
    s32 card;
#ifdef VERSION_EUROPE
    s32 quad_kind;
    s32 shade;
    DisplayObject *quad;
#endif

    CardList_LoadSharedGraphics();
    SD_BGMPlay(0x70E0);
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    state = base;
    Library_PruneRecentCardDrops();
    pane = 0;
    on = 1;
    icons = gCardList_SortIconTable;
    gBuildDeck_pState = state;
    ((u8 *)state)[0x6343] = flags;
    state[0].deck_cards = deck;
    state[1].deck_cards = (u16 *)other;
    state[0].pane_index = 0;
    state[1].pane_index = 0;
    do {
        if (state->deck_cards != 0) {
            drops = gDuel_awRecentCardDrops;
            state->card_sort_rank[0] = 0;
            for (j = 1; j < CARD_ID_END; j++) {
                state->card_sort_rank[j] = 0;
                for (n = 15; n >= 0; n--) {
                    if (drops[n] == j) {
                        state->card_sort_rank[j] = n + 1;
                    }
                }
            }
            deck_list = &state->lists[1];
            total = 0;
            state->lists[1].kind = on;
            state->lists[1].first_target = 0;
            state->lists[1].first = 0;
            state->lists[1].cursor = 0;
            state->lists[1].sort_choice = 0;
            state->lists[1].sort_mode =
                icons[state->lists[1].kind * 16 + 1] & 0xF;
            cards = state->deck_cards;
            entry = deck_list->entries;
            for (j = 0; j < DECK_SIZE; j++, entry++, cards++) {
                entry->flags = 0;
                entry->id = 0;
                card = *cards;
                if (card != 0) {
                    entry->id = card;
                    entry->flags = on;
                    entry->type = (gDuel_adwCardStats[card - 1] >> 26) & 0x1F;
                    entry->attack = (gDuel_adwCardStats[card - 1] & 0x1FF) * 10;
                    total++;
                    entry->defense = ((gDuel_adwCardStats[card - 1] >> 9) & 0x1FF) * 10;
                }
            }
            /* Keep the signed -1 materialization, not unsigned 65535. */
            *(s16 *)&deck_list->entries[DECK_SIZE].id = -1;
            state->deck_total = total;
            state->lists[1].row_count = DECK_SIZE;
            state->lists[1].sort_row_count = DECK_SIZE;
            BuildDeck_SortCardList(&state->lists[1]);
            BuildDeck_RecountDeckCards(state);
            chest_list = &state->lists[0];
            n = 0;
            chest_list->kind = 0;
            chest_list->first_target = 0;
            chest_list->first = 0;
            chest_list->row_count = CARD_COUNT;
            chest_list->cursor = 0;
            chest_list->sort_choice = 0;
            chest_list->sort_mode = icons[chest_list->kind * 16 + 1] & 0xF;
            quantity = (u8 *)state->deck_cards + 0x50;
            entry = chest_list->entries;
            for (j = 0; j < CARD_COUNT; j++, entry++, quantity++) {
                card = j + 1;
                entry->flags = 0;
                entry->id = card;
                entry->type = (gDuel_adwCardStats[j] >> 26) & 0x1F;
                entry->attack = (gDuel_adwCardStats[j] & 0x1FF) * 10;
                entry->defense = ((gDuel_adwCardStats[j] >> 9) & 0x1FF) * 10;
                state->chest_card_quantities[card] = *quantity;
                if (*quantity != 0) {
                    entry->flags = on;
                    n += *quantity;
                } else if (state->deck_card_quantities[card] != 0) {
                    entry->flags = 0x80;
                }
            }
            chest_list->entries[CARD_COUNT].id = 0;
            state->chest_total = n;
            state->lists[0].sort_row_count = CARD_COUNT;
            state->lists[0].row_count = CARD_COUNT;
            BuildDeck_SortCardList(&state->lists[0]);
        }
        pane++;
        state++;
    } while (pane < 2);

    state = gBuildDeck_pState;
#ifdef VERSION_EUROPE
    quad_kind = 4;
#endif
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0, 0xC, 0x208);
#ifdef VERSION_EUROPE
    shade = 0x808080;
#endif
    label = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DISPLAY_OBJECT_VIEW(label)->field_67 = 0;
    DISPLAY_OBJECT_VIEW(label)->xy.word =
        DISPLAY_OBJECT_VIEW(object)->xy.word;
    DISPLAY_OBJECT_VIEW(label)->field_4C = (s32)BuildDeck_DrawCardListPage;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 1, 0xC, 0x208);
    label = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DISPLAY_OBJECT_VIEW(label)->field_67 = 1;
    DISPLAY_OBJECT_VIEW(label)->xy.word =
        DISPLAY_OBJECT_VIEW(object)->xy.word;
    DISPLAY_OBJECT_VIEW(label)->field_4C = (s32)BuildDeck_DrawCardListPage;
    state->state = 2;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x136, BUILD_DECK_CURSOR_Y, 0, 4, 0xC, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 8);
    *(u8 **)((u8 *)state + 0x2D3C) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x26A, BUILD_DECK_CURSOR_Y, 0, 4, 0xC, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 8);
    *(u8 **)((u8 *)state + 0x5A88) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0, BUILD_DECK_LIST_Y, 0, 4, 2, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    *(u8 **)((u8 *)state + 0x2D38) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x148, BUILD_DECK_LIST_Y, 0, 4, 3, 0xC, 0x218);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    *(u8 **)((u8 *)state + 0x5A84) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 9, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 0xA, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 0xB, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
#ifdef VERSION_EUROPE
    quad = DISPLAY_OBJECT_VIEW(DisplayObject_AcquireSlot(
        DisplayObject_FindFreeGeneralSlot(), quad_kind));
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -3);
    quad->position.word = 0xF80000;
    quad->xy.word = 0xF80140;
    quad->field_38.word = 0x1000000;
    quad->field_40.word = 0x1000140;
    quad->field_3C.word = shade;
    quad->field_44.word = shade;
    quad = DISPLAY_OBJECT_VIEW(DisplayObject_AcquireSlot(
        DisplayObject_FindFreeGeneralSlot(), quad_kind));
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -3);
    quad->position.word = 0;
    quad->xy.word = 0x140;
    quad->field_38.word = 0x80000;
    quad->field_40.word = 0x80140;
    quad->field_2C.word = shade;
    quad->field_34.word = shade;
#endif
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 3, 0, 3, 0xB, 0x2F8);
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
    BuildDeck_RefreshCountDisplay(state);
}

s32 BuildDeck_TestAndSetInitialized(BuildDeckTransitionState *state)
{
    u16 flags = state->state;
    if ((flags & 0x8000) == 0) {
        state->state = flags | 0x8000;
        return 0;
    }
    return 1;
}
