/* The debug menu's per-frame update and its bust-up entry step, in address
   order. DebugMenu_Update runs the active step from the primary or alternate
   page table. Otherwise it prints the menu and handles the pad: the
   directions move the cursor, Select flips the page and rebuilds the second
   text box, and confirm selects the step under the cursor. DebugMenu_UpdateBustUpEntry is entry 6 of the primary page table;
   the empty DebugMenu_NoOpBustUpFrame before it (absent from the European
   image) is the per-frame call it makes. */
#include "../types.h"
#include "text_box_blocking_build.h"
#include "display_effect_update_menu_record.h"
#include "rand_get_interval.h"
#include "debug_menu_no_op_bust_up_frame.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "display_object.h"
#include "display_object_brightness.h"
#include "display_object_layout.h"
#include "campaign_scene_package.h"
#include "frontend_debug_state.h"
#include "debug_menu_update.h"
#include "debug_font_format_data.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_set_entry_range_layout.h"
#include "../psyq/rand.h"
#include "input.h"
#include "text_box_lifecycle.h"
#include "frontend_step_tables.h"
#include "frontend_debug_tables.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "debug_menu_update_cursor_layout.h"
#include "text_box_runtime.h"
#include "../unmatched.h"
#include "debug_menu_mapped_mode.h"
#include "debug_menu_exit.h"
#include "debug_menu_two_player_entry.h"
#include "frontend_scene_states.h"
#include "debug_menu_editor_entries.h"
#include "frontend_debug_constants.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
DisplayObject *gDebugMenu_pCursorQuad;
u16 D_8009B2DE;
s8 gDebugMenu_bCursor;
u8 gDebugMenu_bPage;
u8 gDebugMenu_bStep;

/* Initialized data this unit owns at 0x80090D7C: the two step tables DebugMenu_Update
 * dispatches through.
 *
 * It picks between them on gDebugMenu_bPage and indexes the winner with
 * `gDebugMenu_bStep & FRONTEND_STEP_INDEX_MASK`. Both are written here rather than
 * resolved out of the
 * blob at 0x80090BA8 because every entry is a function this tree already
 * names. All twenty-three are matching C; DebugMenu_UpdateSoundEntry and
 * DebugMenu_UpdateCampaignEntry share debug_menu_editor_entries.c and the next
 * eight states share frontend_scene_states.c.
 *
 * The two are adjacent and that is load bearing. gDebugMenu_apfnAlternatePageSteps holds only two
 * entries while the index mask permits thirty-two, so an index above 1 runs
 * straight into gDebugMenu_apfnPrimaryPageSteps -- whose first entry is the same DebugMenu_ResetEntryState
 * that gDebugMenu_apfnAlternatePageSteps starts with. Whether that is deliberate reuse or simply an
 * index the game never produces is not established here, so the two arrays
 * are defined separately, in this order, exactly as the image has them.
 *
 * What this does NOT claim: the tables are attributed to the frontend
 * because their entries live in frontend_scene_states.c,
 * debug_menu_leave_entries.c and
 * their neighbours in the 0x80030000 range, not because anything in the tree
 * names them. The state byte they index, gDebugMenu_bStep, is also written by the
 * memory card paths, so this is not evidence that the tables are
 * frontend-only. */

/* The European build's alternate page has two more entries: the credits and
 * the language editor. */
void (*gDebugMenu_apfnAlternatePageSteps[])(void) = {
    DebugMenu_ResetEntryState,
    DebugMenu_UpdateTwoPlayerDuelEntry,
#ifdef VERSION_EUROPE
    DebugMenu_EnterCredits,
    DebugMenu_UpdateLanguageEntry,
#endif
};

void (*gDebugMenu_apfnPrimaryPageSteps[])(void) = {
    DebugMenu_ResetEntryState,
    DebugMenu_EnterMappedMode,       /* 3D */
    DebugMenu_UpdateCampaignEntry,   /* Campaign */
    DebugMenu_EnterDuel,             /* DUEL */
    DebugMenu_EnterMappedMode,       /* Detail */
    DebugMenu_UpdateSoundEntry,      /* Sound */
    DebugMenu_UpdateBustUpEntry,     /* BustUp */
    DebugMenu_EnterMappedMode,       /* 3D MAP */
    DebugMenu_EnterDeckEditor,       /* DeckEdit */
    DebugMenu_EnterMappedMode,       /* FreeDUEL */
    DebugMenu_UpdateTitleEntry,      /* TITLE */
    DebugMenu_EnterMappedMode,       /* NAME */
    DebugMenu_EnterMappedMode,       /* Password */
    DebugMenu_UpdateMovieEntry,      /* MOVIE */
    DebugMenu_UpdateLoadEntry,       /* Load */
    DebugMenu_UpdateSaveEntry,       /* Save */
    DebugMenu_UpdateTradeEntry,      /* Trade */
    DebugMenu_EnterMappedMode,       /* Option */
    DebugMenu_EnterMappedMode,       /* LOSE */
    DebugMenu_EnterMappedMode,       /* HIRATA's */
    DebugMenu_Exit,                  /* EXIT */
};

/* The second text box, and the cancel and confirm buttons. The Japanese
   channel record is 0x60 bytes and that release swaps Cross for Circle, so a
   regional build supplies its own. */
#ifndef DEBUG_MENU_SECOND_BOX
#define DEBUG_MENU_SECOND_BOX(boxes) (&(boxes)[1])
#endif
#ifndef DEBUG_MENU_CANCEL_BUTTON
#define DEBUG_MENU_CANCEL_BUTTON PAD_BUTTON_CANCEL
#define DEBUG_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void DebugMenu_Update(void) {
    DuelEffectChannel *boxes;
    s32 i;
    s8 d;
    s32 one;
    s32 k;
    s32 e;
    s32 t;

    rand();
    e = gDebugMenu_bStep;
    i = 0;
    if (e != 0) {
        if (gDebugMenu_bPage != DEBUG_MENU_PAGE_PRIMARY) {
            gDebugMenu_apfnAlternatePageSteps[e & FRONTEND_STEP_INDEX_MASK]();
        } else {
            gDebugMenu_apfnPrimaryPageSteps[e & FRONTEND_STEP_INDEX_MASK]();
        }
        return;
    }
    for (; i < 0x16; i++) {
        FntPrint(gDebugMenu_szLineBreak);
    }
    if ((gInput_awPadRepeat[0] & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_RIGHT) != 0) {
                if (gDebugMenu_bCursor + DEBUG_MENU_COLUMN_ENTRY_COUNT <
                    DEBUG_MENU_ENTRY_COUNT) {
                    gDebugMenu_bCursor =
                        gDebugMenu_bCursor + DEBUG_MENU_COLUMN_ENTRY_COUNT;
                }
            } else {
                if (gDebugMenu_bCursor - DEBUG_MENU_COLUMN_ENTRY_COUNT >= 0) {
                    gDebugMenu_bCursor =
                        gDebugMenu_bCursor - DEBUG_MENU_COLUMN_ENTRY_COUNT;
                }
            }
        }
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_UP) != 0) {
            t = (u8)gDebugMenu_bCursor;
            if (gDebugMenu_bCursor >= DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                d = t - 1;
                gDebugMenu_bCursor = d;
                if (d < DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_EXIT;
                }
            } else {
                d = t - 1;
                gDebugMenu_bCursor = d;
                if (d < 0) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_TITLE;
                }
            }
        }
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) != 0) {
            t = (u8)gDebugMenu_bCursor;
            if (gDebugMenu_bCursor >= DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                d = t + 1;
                gDebugMenu_bCursor = d;
                if (d >= DEBUG_MENU_ENTRY_COUNT) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_NAME;
                }
            } else {
                d = t + 1;
                gDebugMenu_bCursor = d;
                if (d >= DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_3D;
                }
            }
        }
        DebugMenu_UpdateCursorLayout();
    }
    if ((gInput_awPadPressed[0] & DEBUG_MENU_CANCEL_BUTTON) != 0) {
        k = DEBUG_MENU_ENTRY_EXIT;
        if (gDebugMenu_bCursor != k) {
            gDebugMenu_bCursor = k;
            DebugMenu_UpdateCursorLayout();
            return;
        }
        gDebugMenu_bStep = 0x14;
        return;
    }
    if ((gInput_awPadPressed[0] & PAD_BUTTON_SELECT) != 0) {
        one = 1;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        /* The Japanese and European builds make neither this call nor the
           two field stores below. */
        DuelEffect_SetEntryRangeLayout(one, one);
#endif
        gDebugMenu_bPage = gDebugMenu_bPage ^ one;
        TextBox_Create(1, gDebugMenu_bPage + 0xF, 0x10, 0x10, 0x120, 0xA0);
        /* The array base has to stay in a local: writing `&gText_aChannels[1]`
         * directly folds the record offset into the address computation and
         * drops the `addiu` retail keeps for the call argument. */
        boxes = gText_aChannels;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        boxes[1].glyph_advance_x = 0x10;
        boxes[1].line_advance_y = 0x10;
#endif
        TextBox_BuildBlocking(DEBUG_MENU_SECOND_BOX(boxes));
        return;
    }
    if ((gInput_awPadPressed[0] & DEBUG_MENU_CONFIRM_MASK) != 0) {
        gDebugMenu_bStep = gDebugMenu_bCursor + 1;
    }
}

/* GCC 2.8.1 aligns arrays to four bytes, so the halfwords before the masks
 * stay scalar; the masks start on a word boundary in every region. */
u16 gDebugMenu_wMessageId = 0x0500;
u16 gDebugMenu_wCampaignScene = 1;
/* Two unreferenced halfwords the European build does not have. */
#ifndef VERSION_EUROPE
static u16 sFrontendDebugValue48 = 0;
static u16 sFrontendDebugValue4A = 1;
#endif
u16 gDebugMenu_awNibbleMasks[4] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };

char gDebugMenu_szLineBreak[4] = "\n";
char gDebugMenu_szStringLineFormat[4] = "%s\n";

#ifndef VERSION_EUROPE
void DebugMenu_NoOpBustUpFrame(void)
{
}
#endif

void DebugMenu_UpdateBustUpEntry(void)
{
#ifdef VERSION_EUROPE
    /* The European entry (the same slot of the debug menu's handler table)
       only clears the step flags; the European image has no
       DebugMenu_NoOpBustUpFrame. */
    gDebugMenu_bStep = 0;
#else
    u8 flags = gDebugMenu_bStep;

    if (!(flags & FRONTEND_STEP_FLAG_ENTERED)) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        D_8009B2DE = 0;
        Campaign_LoadScenePackage(0);
    }

    DebugMenu_NoOpBustUpFrame();

    if (D_8009B2DE == 0) {
        DisplayObject *first = TEXT_CHANNEL(1)->render_object;
        DisplayObject *second = gDebugMenu_pCursorQuad;
        u16 second_flags;

        first->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;

        second_flags = second->flags;
        gDebugMenu_bStep = 0;
        second_flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        second->flags = second_flags;
    } else {
        s32 value = Rand_GetInterval(4);
        DisplayEffect_UpdateMenuRecord(gDisplayEffect_aMenuRecords, value);
    }
#endif
}
