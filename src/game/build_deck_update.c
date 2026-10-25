#define D_8009B140_IN_DATA
#define D_8009AF74_IN_DATA
#define D_8009B09C_IN_DATA
#define GDIALOG_CHOICE_IN_DATA
#include "../types.h"
#include "build_deck_test_and_set_initialized.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "fade.h"
#include "fade_overlay_controls.h"
#include "card_constants.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "text_box_update_channels.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "build_deck_transition_state.h"
#include "graphics_frame.h"
#include "build_deck_update.h"
#include "duel_transition_step_table.h"
#include "../psyq/rand.h"
#include "card_list_setup.h"
#include "dialog_choice.h"
#include "../unmatched.h"

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

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->field_0C)

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
 * otherwise, which is then waited on until its field_30 pointer is filled),
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
                ))->field_59 = 0xA;
            } else {
                box = TextBox_CreateFlagged(0, 9, 0x30, BUILD_DECK_NARROW_BOX_Y, 0xE0, 0x30, BUILD_DECK_NARROW_BOX_FLAGS);
                box->field_59 = 0xA;
                do {
                    TextBox_UpdateChannels();
                } while (box->field_30 == 0);
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
