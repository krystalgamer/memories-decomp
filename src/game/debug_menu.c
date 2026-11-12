
#include "../types.h"

/* The debug menu, in address order: its setup and primary-page value
   editor (DebugMenu_UpdateCursorLayout through DebugMenu_UpdateCampaignEntry,
   with the European language entry), the eight frontend scene states
   (DebugMenu_UpdateMapEntry through DebugMenu_EnterDeckEditor), and the
   leave entries, two-player duel entry and per-frame update.

   DebugMenu_EnterMappedMode and DebugMenu_Exit
   leave the menu (the European build adds DebugMenu_EnterCredits after
   them); DebugMenu_UpdateTwoPlayerDuelEntry and DebugMenu_ResetEntryState
   run the two-player load step. DebugMenu_Update runs the active step from
   the primary or alternate page table. Otherwise it prints the menu and
   handles the pad: the directions move the cursor, Select flips the page
   and rebuilds the second text box, and confirm selects the step under the
   cursor. DebugMenu_UpdateBustUpEntry is entry 6 of the primary page table;
   the empty DebugMenu_NoOpBustUpFrame before it (absent from the European
   image) is the per-frame call it makes. */

#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "debug_menu.h"
#include "debug_menu.h"
#include "frontend_step_tables.h"
#include "display_object_brightness.h"
#include "file_transfer.h"
#include "input.h"
#include "sound.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "../psyq/stdio.h"
#include "script_scene.h"
#include "duel_effect_text.h"
#include "duel_effect_text.h"
#include "text_helpers.h"
#include "display_object_system.h"
#include "main_mode_state.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_states.h"
#include "fade.h"
#include "mem_card.h"
#include "duel_scene.h"
#include "build_deck.h"
#include "movie_playback_control.h"
#include "duel_session_controls.h"
#include "main_services.h"
#include "../psyq/setjmp.h"
#include "../overlays/free_duel/free_duel.h"
#include "display_effect_update_menu_record.h"
#include "rand_get_interval.h"
#include "menu_record.h"
#include "display_object_layout.h"
#include "../psyq/rand.h"

#define DISPLAY_OBJECT_WORD_BYTES(field) ((u8 *)&(field))
#include "../game/display_object.h"

/* Zero-initialized storage this unit owns at 0x800EAED8-0x800EAF08 (#2602), a
 * common symbol split.yaml's debug_menu_bss_commons subsegment
 * allocates. */
u8 gDebug_szEditorCaretLine[48];

/* Small zero-initialized storage this unit owns. The globals are common
 * symbols; each build's debug_menu_commons subsegment
 * allocates them, and the bytes no global names, at their retail addresses. */
u8 D_8009B2B2;
s8 gDebug_abEditorCaretColumns[3];
u8 gDebug_bEditorLeadingLineCount;
u8 gDebugMenu_bCampaignEditField;
u8 gDebug_abEditorDigitCounts[3];
u16 gDebug_nLastSoundID[2];
u16 gDebug_nSceneOrSoundID;
u16 gDebug_nEditedBgmId;
u16 gDebug_nEditedCdSoundId;
u16 gDebug_nLastBgmId;
s8 gDebug_bEditorRow;
u16 D_8009B2DE;
u8 gDebug_bEditorRowCount;
DisplayObject *gDebugMenu_pCursorQuad;
u8 gDebugMenu_bLastMapId;
s8 gDebug_bEditorDigit;
u8 gDebug_bEditorFlags;
u8 gDebugMenu_bStep;
u8 *gDebug_pszEditorFormat;
u8 gDebugMenu_bPage;
s8 gDebugMenu_bCursor;

/* Initialized data this unit owns at 0x80090CB4 through 0x80090D7C, previously a generated
   blob (#2602).

   Six debug HUD format strings that DebugMenu_OpenValueEditor prints, and the index
   table DebugMenu_EnterMappedMode walks. They are one translation unit because they are
   one contiguous range, not because they are one idea.

   Each string carries its explicit size. In the image every one is padded to
   a four-byte boundary -- the first is thirty-seven characters and occupies
   forty bytes -- and a plain u8 name[] = "..." would emit strlen + 1 with no
   padding, because GCC aligns a char array to one byte. The six would then
   pack tightly and move everything after them.

   DebugMenu_UpdateCampaignEntry uses the shared declarations of gDebugMenu_szMessageEditorFormat and gDebugMenu_szCampaignEditorFormat;
   their names retain the original R_MIPS_HI16/R_MIPS_LO16 relocations. */
#ifdef VERSION_EUROPE
u8 gDebugMenu_szLanguageEditorFormat[0x1C] = "          LANGUAGE = %X\n";
#endif
u8 gDebugMenu_szSoundEditorFormat[0x28] = "          SE= %04X BGM=%03X XA= %03X\n";
u8 gDebugMenu_szMessageEditorFormat[0x18] = "          MSG = %03X\n";
u8 gDebugMenu_szCampaignEditorFormat[0x18] = "          CAMP = %02X\n";
u8 gDebugMenu_szMapEditorFormat[0x1C] = "             MAP = %02X\n";
u8 gDebugMenu_szMenuEditorFormat[0x1C] = "             MENU = %02X\n";
u8 gDebugMenu_szMovieEditorFormat[0x24] = "                          = %02X\n";

/* Indexed by gDebugMenu_bCursor in DebugMenu_EnterMappedMode. */
u8 gDebugMenu_abMainModeByEntry[DEBUG_MENU_ENTRY_COUNT] = {
    0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x05, 0x00,
    0x06, 0x08, 0x09, 0x0A, 0x00, 0x00, 0x00, 0x00,
    0x0B, 0x0C, 0x0D, 0x00,
};

void DebugMenu_UpdateCursorLayout(void)
{
    struct LayoutSource *source = (struct LayoutSource *)TEXT_CHANNEL(1);
    s32 first = source->base;
    DisplayObject *target = gDebugMenu_pCursorQuad;
    s32 index = gDebugMenu_bCursor;
    s32 second;
    s32 third;

    if (index >= 10) {
        first += source->spacing / 2 - 16;
    }
    target->field_38.h.field_38 = first;
    target->position.h.field_28 = first;

    second = first + source->spacing / 2 - 16;
    target->field_40.vertex.x = second;
    target->xy.h.x = second;

    {
        s32 quotient = index / 10;
        s8 remainder = (s8)(index - quotient * 10);
        third = source->row + remainder * 16;
    }
    target->xy.h.y = third;
    target->position.h.field_2A = third;
    third += 16;
    target->field_40.vertex.y = third;
    target->field_38.h.field_3A = third;
}

void DebugMenu_Init(void)
{
    DuelEffectChannel *text_box;
    DisplayObject *object;

    D_8009B2B2 = 0x80;
    gDebugMenu_bStep = 0;
    gDebugMenu_bPage = DEBUG_MENU_PAGE_PRIMARY;
    /* The Japanese and European builds make neither the DuelEffect_SetEntryRangeLayout call
       nor the two text-box field stores. */
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    DuelEffect_SetEntryRangeLayout(1, 1);
#endif
    text_box = TextBox_Create(1, 15, 16, 16, 0x120, 0xA0);
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    text_box->glyph_advance_x = 16;
    text_box->line_advance_y = 16;
#endif
    TextBox_BuildBlocking(text_box);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    gDebugMenu_pCursorQuad = object;
    DisplayObject_InitializeGouraudQuad(object, 0);
    DISPLAY_OBJECT_WORD_BYTES(object->field_44)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_3C)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_34)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_2C)[1] = 0xC0;
    DebugMenu_UpdateCursorLayout();
    Fade_WaitIn();
}

void DebugMenu_OpenValueEditor(
    u8 *data,
    s32 caret_column_0,
    s32 caret_column_1,
    s32 caret_column_2,
    s32 leading_line_count,
    s32 digit_count,
    s32 row_count
)
{
    gDebug_bEditorFlags = 0;
    gDebug_pszEditorFormat = data;
    gDebug_abEditorCaretColumns[0] = caret_column_0;
    gDebug_abEditorCaretColumns[1] = caret_column_1;
    gDebug_abEditorCaretColumns[2] = caret_column_2;
    gDebug_bEditorDigit = 0;
    gDebug_bEditorRow = 0;
    gDebug_bEditorLeadingLineCount = leading_line_count;
    gDebug_abEditorDigitCounts[2] = digit_count;
    gDebug_abEditorDigitCounts[1] = digit_count;
    gDebug_abEditorDigitCounts[0] = digit_count;
    gDebug_bEditorRowCount = row_count;
}

/*
 * The union row view prevents GCC from coalescing the first computed row
 * address with its symbol base. The address stays in v0 and is copied to the
 * long-lived pointer, while the second arm's direct index keeps its one-use
 * address in v0. Sized volatile .data views keep all six pad names absolute
 * under the uniform G8 compiler and assembler profile.
 */
/* The confirm buttons (Cross|Square). The Japanese release swaps Cross and
 * Circle, so a regional build names its own. */
#ifndef DEBUG_SCREEN_CONFIRM_MASK
#define DEBUG_SCREEN_CONFIRM_MASK 0xC0
#endif

/* The cancel button (Circle), swapped the same way. */
#ifndef DEBUG_SCREEN_CANCEL_BUTTON
#define DEBUG_SCREEN_CANCEL_BUTTON 0x20
#endif

/* gDebug_nSceneOrSoundID heads the three FrontendDebugValues rows. */
#define FRONTEND_DEBUG_VALUES (*(FrontendDebugValues *)&gDebug_nSceneOrSoundID)

/* This unit's .sdata. It is defined ahead of DebugMenu_UpdateValueEditor,
 * whose "\n" and "%s\n" literals follow it in .sdata. GCC 2.8.1 aligns
 * arrays to four bytes, so the halfwords before the masks stay scalar; the
 * masks start on a word boundary in every region. */
u16 gDebugMenu_wMessageId = 0x0500;
u16 gDebugMenu_wCampaignScene = 1;
/* Two unreferenced halfwords the European build does not have. */
#ifndef VERSION_EUROPE
static u16 sFrontendDebugValue48 = 0;
static u16 sFrontendDebugValue4A = 1;
#endif
u16 gDebugMenu_awNibbleMasks[4] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };

s32 DebugMenu_UpdateValueEditor(void)
{
    s32 a[5] = {1, 10, 100, 1000, 10000};
    s32 b[4] = {0xA, 0x9A, 0x99A, 0x999A};
    s32 c[5] = {1, 0x10, 0x100, 0x1000, 0x10000};
    u8 d[8];
    s32 ret;
    s32 i;
    s32 n;
    s32 e;
    s32 step;
    u8 *db;
    s32 k;
    u8 *z;
    u16 *p;
    FrontendDebugRowPointer row;
    s32 val;
    s32 mask;
    u8 digit_count;
    u8 f;
    u8 *r;
    s32 sc;
    s32 jj;
    s32 cc;
    s32 qq;

    ret = 0;
    *(Bytes8 *)d = *(Bytes8 *)gDebugMenu_awNibbleMasks;
    db = d;
    digit_count = gDebug_abEditorDigitCounts[gDebug_bEditorRow];
    /* Confirm, cancel and an input without a cursor move break out of the
       switch past the caret-line rebuild. */
    switch (gDebug_bEditorFlags & 0x80) {
    case 0:
        gDebug_bEditorFlags = gDebug_bEditorFlags | 0x80;
        if ((gDebug_bEditorFlags & 0x40) != 0) {
            jj = (s8)digit_count - 1;
            row.words = &FRONTEND_DEBUG_VALUES.row[gDebug_bEditorRow];
            val = *row.words;
            *row.words = 0;
            p = row.words;
            do {
                k = a[jj];
                qq = val / k;
                *p = *p | (qq << (jj * 4));
                jj = jj - 1;
                val = val - k * qq;
            } while (jj >= 0);
        }
        if (0) {
    default:
            if ((gDebug_bEditorFlags & 1) == 0
                && ((gInput_awPadHeld[0] | gInput_awPadHeld[1]) & 0x800) == 0) {
                if (((gInput_awPadPressed[0] | gInput_awPadPressed[1]) & DEBUG_SCREEN_CONFIRM_MASK) != 0) {
                    ret = 1;
                    break;
                }
                if (((gInput_awPadPressed[0] | gInput_awPadPressed[1]) & DEBUG_SCREEN_CANCEL_BUTTON) != 0) {
                    ret = -1;
                    break;
                }
                if (((gInput_awPadRepeat[0] | gInput_awPadRepeat[1]) & 0x5000) != 0) {
                    e = gDebug_bEditorDigit;
                    val = FRONTEND_DEBUG_VALUES.row[gDebug_bEditorRow];
                    step = c[e];
                    if (((gInput_awPadRepeat[0] | gInput_awPadRepeat[1]) & 0x4000) != 0) {
                        step = -step;
                    }
                    if ((gDebug_bEditorFlags & 0x40) != 0) {
                        /* A u16 cursor folds the negation and swaps the addu operands. */
                        mask = *(u16 *)(db - -(e * 2));
                        i = e;
                        if (i < (s8)digit_count) {
                            n = (s8)digit_count;
                            do {
                                /* The empty nested loop stops expand_end_loop
                                   from rotating the early breaks to the
                                   bottom of the carry loop. */
                                do {
                                } while (0);
                                cc = val & mask;
                                val = val & ~mask;
                                cc = cc + step;
                                if (step >= 0) {
                                    if (cc < b[i]) {
                                        break;
                                    }
                                } else {
                                    if (cc >= 0) {
                                        break;
                                    }
                                    val = val | ((b[i] - 1) & mask);
                                }
                                cc = 0;
                                mask = mask * 0x10;
                                step = step * 0x10;
                                i = i + 1;
                            } while (i < n);
                        }
                        val = val | cc;
                    } else {
                        val = val + step;
                        val = val & (c[(s8)digit_count] - 1);
                    }
                    FRONTEND_DEBUG_VALUES.row[gDebug_bEditorRow] = val;
                }
                if (((gInput_awPadRepeat[0] | gInput_awPadRepeat[1]) & 0xA000) == 0) {
                    break;
                }
                if (((gInput_awPadRepeat[0] | gInput_awPadRepeat[1]) & 0x2000) != 0) {
                    gDebug_bEditorDigit = gDebug_bEditorDigit - 1;
                    if (gDebug_bEditorDigit < 0) {
                        gDebug_bEditorRow = gDebug_bEditorRow + 1;
                        if (gDebug_bEditorRow >= (s8)gDebug_bEditorRowCount) {
                            gDebug_bEditorRow = gDebug_bEditorRowCount - 1;
                            gDebug_bEditorDigit = 0;
                        } else {
                            f = gDebug_abEditorDigitCounts[gDebug_bEditorRow] - 1;
                            gDebug_bEditorDigit = f;
                        }
                    }
                } else {
                    gDebug_bEditorDigit = gDebug_bEditorDigit + 1;
                    if (gDebug_bEditorDigit >= (s8)digit_count) {
                        gDebug_bEditorDigit = 0;
                        gDebug_bEditorRow = gDebug_bEditorRow - 1;
                        if (gDebug_bEditorRow < 0) {
                            f = gDebug_abEditorDigitCounts[0] - 1;
                            gDebug_bEditorRow = 0;
                            gDebug_bEditorDigit = f;
                        }
                    }
                }
            }
        }
        r = gDebug_szEditorCaretLine;
        sc = 0x20;
        i = 0x27;
        z = r + i;
        do {
            *z = sc;
            i = i - 1;
            z = z - 1;
        } while (i >= 0);
        r = &gDebug_szEditorCaretLine[
            gDebug_abEditorCaretColumns[gDebug_bEditorRow] - gDebug_bEditorDigit];
        r[0] = 0x2A;
        r[1] = 0;
    }
    i = (s8)gDebug_bEditorLeadingLineCount;
    if (i != 0) {
        do {
            FntPrint("\n");
            i = i - 1;
        } while (i != 0);
    }
    FntPrint((char *)gDebug_pszEditorFormat, gDebug_nSceneOrSoundID,
             gDebug_nEditedBgmId, gDebug_nEditedCdSoundId);
    FntPrint("%s\n", gDebug_szEditorCaretLine);
    return ret;
}

#ifdef VERSION_EUROPE
#include "card_preview.h"
#endif

#ifdef VERSION_EUROPE
/* A European-only debug-menu entry, with its own slot in the menu's handler
   table: it edits gText_bLanguageIndex (the index card_preview.c
   reads) through the same editor as the sound entry, and sets it back to
   BUILD_LANGUAGE_INDEX from 5 up. */
void DebugMenu_UpdateLanguageEntry(void)
{
    s32 flags;
    s32 result;

    flags = gDebugMenu_bStep;
    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        DebugMenu_DimPrimaryDisplayObject();
        gDebug_nSceneOrSoundID = gText_bLanguageIndex;
        DebugMenu_OpenValueEditor(gDebugMenu_szLanguageEditorFormat, 0x15, 0x15, 0x15, 5, 1, 1);
        gDebug_abEditorDigitCounts[2] = 3;
        gDebug_abEditorDigitCounts[1] = 3;
        return;
    }

    result = DebugMenu_UpdateValueEditor();
    if (result == 0) {
        return;
    }
    if (result < 0) {
        gDebugMenu_bStep = 0;
        DebugMenu_RestorePrimaryDisplayObject();
        return;
    }
    if (gText_bLanguageIndex != gDebug_nSceneOrSoundID) {
        gText_bLanguageIndex = gDebug_nSceneOrSoundID;
        if (gText_bLanguageIndex >= 5) {
            gText_bLanguageIndex = BUILD_LANGUAGE_INDEX;
        }
    }
}
#endif

void DebugMenu_UpdateSoundEntry(void)
{
    s32 flags;
    s32 count;
    s32 result;

    flags = gDebugMenu_bStep;
    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        count = 3;
        DebugMenu_DimPrimaryDisplayObject();
        gDebug_nSceneOrSoundID = gDebug_nLastSoundID[0];
        gDebug_nEditedBgmId = gDebug_nLastBgmId;
        gDebug_nEditedCdSoundId = gDebug_nLastSoundID[1];
        DebugMenu_OpenValueEditor(gDebugMenu_szSoundEditorFormat, 0x11, 0x19, 0x21, 9, 4, count);
        gDebug_abEditorDigitCounts[2] = count;
        gDebug_abEditorDigitCounts[1] = count;
        return;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_START) != 0) {
        SD_StopAll();
        return;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_SELECT) != 0) {
        File_CancelTransfers();
        return;
    }

    result = DebugMenu_UpdateValueEditor();
    if (result == 0) {
        return;
    }
    if (result < 0) {
        gDebugMenu_bStep = 0;
        DebugMenu_RestorePrimaryDisplayObject();
        return;
    }

    switch (gDebug_bEditorRow) {
    case 0:
        gDebug_nLastSoundID[0] = gDebug_nSceneOrSoundID;
        if (gDebug_bEditorDigit == 3) {
            SD_ResetPendingEntries();
            SD_ReplaceValueLink((u16)((s16)gDebug_nLastSoundID[0] >> 12));
            return;
        }
        SD_SEPlayFull(gDebug_nSceneOrSoundID & 0xFFF);
        return;
    case 1:
        gDebug_nLastBgmId = gDebug_nEditedBgmId;
        SD_BGMPlay(gDebug_nEditedBgmId);
        return;
    case 2:
        gDebug_nLastSoundID[1] = gDebug_nEditedCdSoundId;
        if ((gInput_awPadPressed[0] & PAD_BUTTON_SQUARE) != 0) {
            SD_CdSoundSeek((u16)gDebug_nEditedCdSoundId);
            return;
        }
        SD_CdSoundPlay(gDebug_nEditedCdSoundId);
        return;
    }
}

/*
 * `DebugMenu_UpdateCampaignEntry` matches all 632 bytes on uniform `gcc_2_8_1_g8_split` after
 * selecting true-width DATA scalars for the active mode and campaign scene
 * index. The array spellings leave ten differing words; changing only the mode
 * leaves three and changing only the scene index leaves nine. Selecting both
 * restores the `$at` store expansions and the shared editor-value register
 * allocation without mixed thresholds or a pinned local. The primary campaign
 * display pointer independently requires its DATA view for the absolute load
 * and store.
 *
 * The scene index's final store is `sb`, not `sh`: the reference's missing
 * `D_8009B27A` is the canonical byte `gCampaignSceneIndex`. `Main_RunCampaign`
 * now selects the same existing DATA spelling through unmatched.h
 * instead of redeclaring it locally. The debug selector, format strings,
 * dialog object and effect-entry byte at `+0x18` use shared declarations.
 * The diagnostic's original format at `0x80010288` is retained, as are the
 * message-preview cleanup, editor toggle and campaign transition.
 *
 * The canonical rows and six terminal refinement records are unchanged;
 * one post-terminal resolution records the declaration and layout evidence.
 */
/* Regional value: the preview text box's height (0x30 -> 0x40). The
 * European build (src/game/european/) defines its own. */
#ifndef DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT
#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x30
#endif

void DebugMenu_UpdateCampaignEntry(void)
{
    DuelEffectChannel *textbox;
    s32 result;
    s32 flags;
    u16 selection;

    /* Entry and a Select press both reopen the editor for the chosen
       field. */
    switch (gDebugMenu_bStep & 0x80) {
    case 0:
        gDebugMenu_bStep = gDebugMenu_bStep | 0x80;
        DebugMenu_DimPrimaryDisplayObject();
        Campaign_LoadScenePackage(0);
        gDebugMenu_bCampaignEditField = 0;
        if (0) {
    default:
            if ((gInput_awPadPressed[0] & PAD_BUTTON_SELECT) == 0) {
                break;
            }
            gDebugMenu_bCampaignEditField = gDebugMenu_bCampaignEditField ^ 1;
        }
        if (gDebugMenu_bCampaignEditField == 0) {
            selection = gDebugMenu_wMessageId;
            gDebug_nSceneOrSoundID = selection;
            DebugMenu_OpenValueEditor(gDebugMenu_szMessageEditorFormat, 0x12, 0, 0, 2, 3, 1);
        } else {
            selection = gDebugMenu_wCampaignScene;
            gDebug_nSceneOrSoundID = selection;
            DebugMenu_OpenValueEditor(gDebugMenu_szCampaignEditorFormat, 0x12, 0, 0, 2, 2, 1);
        }
    }

    result = DebugMenu_UpdateValueEditor();
    flags = gDebugMenu_bStep;
    if ((flags & 0x40) != 0) {
        TextBox_UpdateChannels();
        textbox = gText_aChannels;
        if ((gDebugMenu_bStep & 0x20) == 0) {
            if ((textbox->flags_34 & TEXT_BOX_FLAG_DONE) != 0) {
                textbox->prompt_object = Dialog_OpenChoice(textbox);
                gDebugMenu_bStep = gDebugMenu_bStep | 0x20;
            }
        } else if (Input_IsPad1ConfirmPressed() != 0) {
            TextBox_Destroy(textbox);
            DisplayObject_ReleaseIfPresent(gCampaign_PrimaryDisplayObject);
            gDebugMenu_bStep = gDebugMenu_bStep & 0x9F;
            gDebug_bEditorFlags = gDebug_bEditorFlags & 0xFE;
        }
    } else if (result != 0) {
        if (result < 0) {
            gDebugMenu_bStep = 0;
            DebugMenu_RestorePrimaryDisplayObject();
            return;
        }
        if (gDebugMenu_bCampaignEditField == 0) {
            gDebugMenu_bStep = flags | 0x40;
            gDebugMenu_wMessageId = gDebug_nSceneOrSoundID;
            gDebug_bEditorFlags = gDebug_bEditorFlags | 1;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
            /* The Japanese and European builds make neither this call nor
               the diagnostic print. */
            DuelEffect_SetEntryRangeLayout(0, 2);
            printf("0001 = %d\n",
                   gDuelEffect_aEntries[gDuelEffect_awEntryRangeBoundaries[0]].layout_selector);
#endif
            TextBox_Create(0, gDebugMenu_wMessageId, 0x10, 0xB0, 0x120,
                           DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT);
            gCampaign_PrimaryDisplayObject = Campaign_CreatePrimaryDisplayObject();
            return;
        }
        selection = gDebug_nSceneOrSoundID;
        gMain_bActiveModeState = 2;
        gDebugMenu_wCampaignScene = selection;
        gCampaignSceneIndex = selection;
    }
}

/* The frontend's eight contiguous scene states, in address order.
   gDebugMenu_bStep carries the entered and asynchronous-pending state shared by the
   setup/polling steps. */

void DebugMenu_UpdateMapEntry(void)
{
    u8 flags = gDebugMenu_bStep;
    int result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDebug_nSceneOrSoundID = gDebugMenu_bLastMapId;
        DebugMenu_OpenValueEditor(gDebugMenu_szMapEditorFormat, 0x14, 0, 0, 0xC, 2, 1);
    }
    result = DebugMenu_UpdateValueEditor();
    if (result != 0) {
        if (result < 0) {
            gDebugMenu_bStep = 0;
        } else {
            gCampaignMap_bEntryLocation = 0;
            gMain_bActiveModeState = 5;
            gDebugMenu_bLastMapId = (u8)gDebug_nSceneOrSoundID;
        }
    }
}

void DebugMenu_UpdateTitleEntry(void)
{
    u8 flags = gDebugMenu_bStep;
    int result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDebug_nSceneOrSoundID = gMain_bRequestedMenuID;
        DebugMenu_OpenValueEditor(gDebugMenu_szMenuEditorFormat, 0x15, 0, 0, 0x12, 2, 1);
    }
    result = DebugMenu_UpdateValueEditor();
    if (result != 0) {
        if (result < 0) {
            gDebugMenu_bStep = 0;
        } else {
            D_8009B268 = 1;
            gMain_bActiveModeState = 8;
            gMain_bRequestedMenuID = (u8)gDebug_nSceneOrSoundID;
        }
    }
}

void DebugMenu_UpdateMovieEntry(void)
{
    {
        u8 flags = gDebugMenu_bStep;

        if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
            gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
            gDebug_nSceneOrSoundID = 0;
            DebugMenu_OpenValueEditor(gDebugMenu_szMovieEditorFormat, 0x1D, 0, 0, 5, 2, 1);
        }
    }
    {
        u8 flags = gDebugMenu_bStep;

        if (flags & FRONTEND_STEP_FLAG_ASYNC_PENDING) {
            u32 word = gFile_dwTransferState;
            if ((word & 0x02000000) == 0)
                gDebugMenu_bStep = flags & ~FRONTEND_STEP_FLAG_ASYNC_PENDING;
        } else {
            int result = DebugMenu_UpdateValueEditor();

            if (result) {
                if (result < 0)
                    gDebugMenu_bStep = 0;
                else {
                    Movie_Play(gDebug_nSceneOrSoundID);
                    gDebugMenu_bStep |= FRONTEND_STEP_FLAG_ASYNC_PENDING;
                }
            }
        }
    }
}

void DebugMenu_UpdateLoadEntry(void)
{
    u8 flags = gDebugMenu_bStep;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDuel_bEffectState = DUEL_EFFECT_STATE_NOOP_3;
    }
    if (DuelEffect_UpdateState() == 0) {
        gDebugMenu_bStep = 0;
    }
}

void DebugMenu_UpdateSaveEntry(void)
{
    u8 flags = gDebugMenu_bStep;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gDuel_bEffectState = DUEL_EFFECT_STATE_NOOP_4;
    }
    if (DuelEffect_UpdateState() == 0) {
        gDebugMenu_bStep = 0;
    }
}

void DebugMenu_UpdateTradeEntry(void)
{
    u8 flags = gDebugMenu_bStep;
    int result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gSaveData_PairSetupState = 0;
        gSaveData_LoadPairState = 0;
    }
    result = SaveData_UpdateTradeLoad();
    if (result != 0) {
        if (result == 1) {
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
            gMain_bActiveModeState = 14;
        }
        gDebugMenu_bStep = 0;
    }
}

void DebugMenu_EnterDuel(void)
{
    gDuel_wBgmId = 0x71D0;
    gDuel_bPostDuelMainMode = 0;
    Main_StartDuel(-1, 1, 0x8000, 0x8000);
}

void DebugMenu_EnterDeckEditor(void)
{
    BuildDeck_EnterNarrowConfirmMode();
}

void DebugMenu_EnterMappedMode(void)
{
    u8 *p = gDebugMenu_abMainModeByEntry;
    s32 i = gDebugMenu_bCursor;
    u8 v;

    gFreeDuel_bReturnFlags = 0;
    v = p[i];
    D_8009B2B2 = 0;
    gMain_bActiveModeState = v;
}

/* The imported prototype lacks noreturn; retail has no epilogue after this call. */

void DebugMenu_Exit(void)
{
    Fade_WaitInitOut();
    DisplayObject_Reset();
    DuelEffect_ResetState();
    longjmp(gMain_RecoveryJmpBuf, 2);
}

#ifdef VERSION_EUROPE
/* A European-only debug-menu handler, which follows DebugMenu_Exit in the
   image. The European alternate-page step table holds it at element 2,
   between DebugMenu_UpdateTwoPlayerDuelEntry and
   DebugMenu_UpdateLanguageEntry. It sets both gMain_bActiveModeState and
   gMain_bReturnMode to 15, MAIN_MODE_CREDITS. */
void DebugMenu_EnterCredits(void)
{
    gMain_bActiveModeState = 15;
    gMain_bReturnMode = 15;
}
#endif

void DebugMenu_UpdateTwoPlayerDuelEntry(void)
{
    u8 flags = gDebugMenu_bStep;
    s32 result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gSaveData_PairSetupState = 0;
        gSaveData_LoadPairState = 0;
    }
    result = SaveData_UpdateDuelLoad();
    if (result != 0) {
        if (result == 1) {
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
            gMain_bActiveModeState = 0x10;
        }
        gDebugMenu_bStep = 0;
    }
}

void DebugMenu_ResetEntryState(void)
{
    gDebugMenu_bStep = 0;
}

/* Initialized data this unit owns at 0x80090D7C: the two step tables DebugMenu_Update
 * dispatches through.
 *
 * It picks between them on gDebugMenu_bPage and indexes the winner with
 * `gDebugMenu_bStep & FRONTEND_STEP_INDEX_MASK`. Both are written here rather than
 * resolved out of the
 * blob at 0x80090BA8 because every entry is a function this tree already
 * names. All twenty-three are matching C, and all of them are defined in
 * this file.
 *
 * The two are adjacent and that is load bearing. gDebugMenu_apfnAlternatePageSteps holds only two
 * entries while the index mask permits thirty-two, so an index above 1 runs
 * straight into gDebugMenu_apfnPrimaryPageSteps -- whose first entry is the same DebugMenu_ResetEntryState
 * that gDebugMenu_apfnAlternatePageSteps starts with. Whether that is deliberate reuse or simply an
 * index the game never produces is not established here, so the two arrays
 * are defined separately, in this order, exactly as the image has them.
 *
 * What this does NOT claim: the tables are attributed to the frontend
 * because their entries live in this file and
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

/* The cancel and confirm buttons. The Japanese release swaps Cross for
   Circle, so a regional build supplies its own. */
#ifndef DEBUG_MENU_CANCEL_BUTTON
#define DEBUG_MENU_CANCEL_BUTTON PAD_BUTTON_CANCEL
#define DEBUG_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void DebugMenu_Update(void) {
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    DuelEffectChannel *boxes;
#endif
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
        FntPrint("\n");
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
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        /* The array base has to stay in a local: writing `&gText_aChannels[1]`
         * directly folds the record offset into the address computation and
         * drops the `addiu` retail keeps for the call argument. */
        boxes = gText_aChannels;
        boxes[1].glyph_advance_x = 0x10;
        boxes[1].line_advance_y = 0x10;
        TextBox_BuildBlocking(&boxes[1]);
#else
        /* Without the field stores, retail folds the record offset. */
        TextBox_BuildBlocking(&gText_aChannels[1]);
#endif
        return;
    }
    if ((gInput_awPadPressed[0] & DEBUG_MENU_CONFIRM_MASK) != 0) {
        gDebugMenu_bStep = gDebugMenu_bCursor + 1;
    }
}

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
