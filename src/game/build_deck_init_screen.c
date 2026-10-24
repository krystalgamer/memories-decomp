/*
 * BuildDeck_InitScreen sets up the Build Deck screen: it initialises the two
 * 0x6344-byte deck records (recent-drop ranks, the 40-card deck list with
 * its stats, and the 722-card chest list with its counts), sorts both lists,
 * then creates the screen's eleven display objects. It stands in its own
 * file because it is measured under gcc_2_8_1_g8_split_no_strength_reduce;
 * its neighbours use gcc_2_8_1_g0_split and gcc_2_8_1_g8.
 *
 * Levers measured on this body:
 * - -fno-strength-reduce: the target's loops keep explicit counters and
 *   unbiased cursors, so the drop table is walked backwards by hand and each
 *   list has its own record cursor;
 * - one counter `j` serves the rank loop and both list loops, and the rank
 *   loop's inner index reuses `n`, the chest total. Those shared lifetimes
 *   are what order the global allocation of $a2-$t1; the two list heads are
 *   separate names (`deck_list`, `chest_list`) for the same reason;
 * - the card-stat address is built as v--, v *= 4, v += all_stats after the
 *   store of v, and `v` is reused for the chest count cursor, which keeps
 *   both in $a0;
 * - `icons` and `all_stats` are locals set in that order ahead of `lists`,
 *   which is the order retail materialises $fp, $s7 and $s0 in;
 * - the final row count goes through the s32 local `rows`. loop.c merges
 *   two loads of one constant only when the first is at least as wide as the
 *   second, so an SImode 0x2D2 after the HImode one stays unmerged, neither
 *   load is worth hoisting alone, and $s7 stays with the card stats.
 *
 * The list and pane fields are written through CardList and
 * BuildDeckTransitionState, whose layouts are asserted in their headers. Each
 * conversion was measured on its own and in groups: the object is unchanged.
 * Two rules came out of that, and both cost an object to learn:
 * - the cast goes inline at the access. Naming a `CardList *` for the pane and
 *   writing the fields through it is one pseudo more than this body has room
 *   for, and it changes the allocation;
 * - the base pointer has to stay the one the access already uses. `chest_total`
 *   and `deck_total` are therefore still raw: they sit at +0x5A9C and +0x5AA0
 *   of the record, `lists` is the record plus 4, and no struct has `lists` as
 *   its base, so reaching them as members means addressing through `state`
 *   instead -- which moves the store to another register. Spelling either of
 *   them as a member changes the object on its own.
 */
#define GRAPHICS_VIEWPORT_IN_DATA
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
#include "card_type_icon_table.h"
#include "build_deck_init_screen.h"
#include "card_list_sort_icons.h"
#include "build_deck_test_and_set_initialized.h"

#define CARD_LIST_VIEW(list) ((CardList *)(list))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define BUILD_DECK_TRANSITION_STATE_VIEW(state) \
    ((BuildDeckTransitionState *)(state))
#define CARD_STAT_WORD(record) (*(s32 *)(record))

#ifndef BUILD_DECK_CURSOR_Y
#define BUILD_DECK_CURSOR_Y 0x29
#endif

#ifndef BUILD_DECK_LIST_Y
#define BUILD_DECK_LIST_Y 0x2A
#endif

void BuildDeck_InitScreen(BuildDeckTransitionState *base, void *deck, s32 other, s32 flags)
{
    u8 *state;
    u8 *lists;
    u8 *object;
    u8 *label;
    u8 *deck_list;
    u8 *chest_list;
    u8 *entry;
    u8 *quantity;
    u8 *counts;
    u8 *icon;
    u16 *drops;
    u16 *cards;
    u16 *drop;
    u8 *rank;
    u8 *rank_row;
    s32 *all_stats;
    s32 icons;
    s32 rows;
    s32 *stats;
    s32 pane;
    s32 j;
    s32 icon_addr;
    s32 id;
    s32 on;
    s32 held;
    s32 deck_total;
    s32 n;
    s32 v;
#ifdef VERSION_EUROPE
    s32 quad_kind;
    s32 shade;
    DisplayObject *quad;
#endif

    CardList_LoadSharedGraphics();
    SD_BGMPlay(0x70E0);
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    state = (u8 *)base;
    Library_PruneRecentCardDrops();
    pane = 0;
    on = 1;
    icons = (s32)gCardList_SortIconTable;
    all_stats = gDuel_adwCardStats;
    lists = state + 4;
    gBuildDeck_pState = BUILD_DECK_TRANSITION_STATE_VIEW(state);
    state[0x6343] = flags;
    *(void **)state = deck;
    *(s32 *)(state + 0x6344) = other;
    BUILD_DECK_TRANSITION_STATE_VIEW(state)->pane_index = 0;
    state[0xC686] = 0;
    do {
        if (*(s32 *)state != 0) {
            drops = (u16 *)gDuel_awRecentCardDrops;
            lists[0x6066] = 0;
            j = 1;
            rank = state + j;
            do {
                rank[0x606A] = 0;
                n = 15;
                rank_row = rank;
                drop = drops + 15;
                do {
                    if ((s16)*drop == j) {
                        rank_row[0x606A] = n + 1;
                    }
                    n--;
                    drop--;
                } while (n >= 0);
                j++;
                rank++;
            } while (j < 0x2D3);
            deck_list = state + 0x2D50;
            deck_total = 0;
            CARD_LIST_VIEW(lists)[1].kind = on;
            j = deck_total;
            CARD_LIST_VIEW(lists)[1].first_target = 0;
            CARD_LIST_VIEW(lists)[1].first = 0;
            CARD_LIST_VIEW(lists)[1].cursor = 0;
            CARD_LIST_VIEW(lists)[1].sort_choice = 0;
            icon = (u8 *)(CARD_LIST_VIEW(lists)[1].kind * 16 + icons);
            CARD_LIST_VIEW(lists)[1].sort_mode = icon[1] & 0xF;
            entry = state + 0x2D58;
            cards = *(u16 **)state;
            for (; j < 0x28; j++, entry += sizeof(CardEntry), cards++) {
                CARD_ENTRY_FROM_MEMBER(entry, defense)->flags = 0;
                CARD_ENTRY_FROM_MEMBER(entry, defense)->id = 0;
                v = *cards;
                if (v != 0) {
                    CARD_ENTRY_FROM_MEMBER(entry, defense)->id = v;
                    v--;
                    v *= 4;
                    v += (s32)all_stats;
                    CARD_ENTRY_FROM_MEMBER(entry, defense)->flags = on;
                    CARD_ENTRY_FROM_MEMBER(entry, defense)->type =
                        (CARD_STAT_WORD(v) >> 26) & 0x1F;
                    CARD_ENTRY_FROM_MEMBER(entry, defense)->attack =
                        (CARD_STAT_WORD(v) & 0x1FF) * 10;
                    deck_total++;
                    CARD_ENTRY_FROM_MEMBER(entry, defense)->defense =
                        ((CARD_STAT_WORD(v) >> 9) & 0x1FF) * 10;
                }
            }
            /* Keep the signed -1 materialization, not unsigned 65535. */
            *(s16 *)&CARD_LIST_VIEW(deck_list)->entries[DECK_SIZE].id = -1;
            *(s32 *)(lists + 0x5A9C) = deck_total;
            CARD_LIST_VIEW(lists)[1].row_count = 0x28;
            CARD_LIST_VIEW(lists)[1].sort_row_count = 0x28;
            BuildDeck_SortCardList(CARD_LIST_VIEW(state + 0x2D50));
            BuildDeck_RecountDeckCards(BUILD_DECK_TRANSITION_STATE_VIEW(state));
            chest_list = lists;
            n = 0;
            j = n;
            held = 0x80;
            CARD_LIST_VIEW(lists)->kind = 0;
            CARD_LIST_VIEW(lists)->first_target = 0;
            CARD_LIST_VIEW(lists)->first = 0;
            CARD_LIST_VIEW(lists)->row_count = 0x2D2;
            CARD_LIST_VIEW(lists)->cursor = 0;
            CARD_LIST_VIEW(lists)->sort_choice = 0;
            icon_addr = CARD_LIST_VIEW(lists)->kind * 16;
            icon_addr += icons;
            CARD_LIST_VIEW(lists)->sort_mode = ((u8 *)icon_addr)[1] & 0xF;
            entry = lists + 0xD;
            stats = all_stats;
            quantity = *(u8 **)state + 0x50;
            for (; j < 0x2D2; stats++, j++, entry += sizeof(CardEntry), quantity++) {
                id = j + 1;
                CARD_ENTRY_FROM_MEMBER(entry, flags)->flags = 0;
                CARD_ENTRY_FROM_MEMBER(entry, flags)->id = id;
                CARD_ENTRY_FROM_MEMBER(entry, flags)->type =
                    (*stats >> 26) & 0x1F;
                CARD_ENTRY_FROM_MEMBER(entry, flags)->attack =
                    (*stats & 0x1FF) * 10;
                CARD_ENTRY_FROM_MEMBER(entry, flags)->defense =
                    ((*stats >> 9) & 0x1FF) * 10;
                v = (s32)state + id;
                counts = (u8 *)v;
                counts[0x5D97] = *quantity;
                if (*quantity != 0) {
                    CARD_ENTRY_FROM_MEMBER(entry, flags)->flags = on;
                    n += *quantity;
                } else if (counts[0x5AC4] != 0) {
                    CARD_ENTRY_FROM_MEMBER(entry, flags)->flags = held;
                }
            }
            CARD_LIST_VIEW(chest_list)->entries[CARD_COUNT].id = 0;
            *(s32 *)(lists + 0x5A98) = n;
            rows = 0x2D2;
            CARD_LIST_VIEW(lists)->sort_row_count = rows;
            CARD_LIST_VIEW(lists)->row_count = rows;
            BuildDeck_SortCardList(CARD_LIST_VIEW(lists));
        }
        pane++;
        lists += 0x6344;
        state += 0x6344;
    } while (pane < 2);

    state = (u8 *)gBuildDeck_pState;
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
    DISPLAY_OBJECT_VIEW(label)->field_30.word =
        DISPLAY_OBJECT_VIEW(object)->field_30.word;
    DISPLAY_OBJECT_VIEW(label)->field_4C = (s32)BuildDeck_DrawCardListPage;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 1, 0xC, 0x208);
    label = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DISPLAY_OBJECT_VIEW(label)->field_67 = 1;
    DISPLAY_OBJECT_VIEW(label)->field_30.word =
        DISPLAY_OBJECT_VIEW(object)->field_30.word;
    DISPLAY_OBJECT_VIEW(label)->field_4C = (s32)BuildDeck_DrawCardListPage;
    BUILD_DECK_TRANSITION_STATE_VIEW(state)->state = 2;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x136, BUILD_DECK_CURSOR_Y, 0, 4, 0xC, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 8);
    *(u8 **)(state + 0x2D3C) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x26A, BUILD_DECK_CURSOR_Y, 0, 4, 0xC, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 8);
    *(u8 **)(state + 0x5A88) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0, BUILD_DECK_LIST_Y, 0, 4, 2, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    *(u8 **)(state + 0x2D38) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x148, BUILD_DECK_LIST_Y, 0, 4, 3, 0xC, 0x218);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    *(u8 **)(state + 0x5A84) = object;
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
    quad->field_30.word = 0xF80140;
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
    quad->field_30.word = 0x140;
    quad->field_38.word = 0x80000;
    quad->field_40.word = 0x80140;
    quad->field_2C.word = shade;
    quad->field_34.word = shade;
#endif
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 3, 0, 3, 0xB, 0x2F8);
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
    BuildDeck_RefreshCountDisplay(BUILD_DECK_TRANSITION_STATE_VIEW(state));
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
