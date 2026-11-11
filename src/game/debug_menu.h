#ifndef MEMORIES_DECOMP_DEBUG_MENU_H
#define MEMORIES_DECOMP_DEBUG_MENU_H
#include "../types.h"
#include "display_object.h"

/* The primary debug page's value editor (DebugMenu_UpdateValueEditor, declared in
 * frontend_debug_state.h) and its two entries, adjacent in the image at
 * 0x800307B8 and 0x80030998. Both entries drive the digit editor and
 * return through DebugMenu_RestorePrimaryDisplayObject when it cancels. */

/* Entry 5 of gDebugMenu_apfnPrimaryPageSteps (debug_menu.c), the
 * "Sound" label: edits and plays sound IDs. */
void DebugMenu_UpdateSoundEntry(void);

#ifdef VERSION_EUROPE
extern void DebugMenu_UpdateLanguageEntry(void);
#endif

/* Entry 2 of gDebugMenu_apfnPrimaryPageSteps (debug_menu.c), the
 * "Campaign" label.
 * Switches between message-preview and campaign-index editors, preserving
 * the dialog-choice cleanup before returning to the editor. */
void DebugMenu_UpdateCampaignEntry(void);

/* One step handler of the two tables at 0x80090D7C and 0x80090D84.
 *
 * It tears the screen down -- Fade_WaitInitOut, DisplayObject_Reset,
 * DuelEffect_ResetState -- and then longjmps to gMain_RecoveryJmpBuf with 2, so it does not
 * return to its caller. The definition's own comment records that the
 * imported longjmp prototype lacks noreturn and that retail emits no epilogue
 * after the call, which is why this is still spelled as an ordinary void
 * function. */
void DebugMenu_Exit(void);

/* The European alternate debug page's credits step, defined beside
 * DebugMenu_Exit in debug_menu.c. */
void DebugMenu_EnterCredits(void);

/* One step handler of the two tables at 0x80090D7C and 0x80090D84.
 *
 * It reads gDebugMenu_bCursor as an index into gDebugMenu_abMainModeByEntry and publishes that byte as
 * the next mode in gMain_bActiveModeState, clearing gFreeDuel_bReturnFlags and D_8009B2B2
 * on the way. frontend_debug_tables.h documents both the index and the table
 * it selects from. */
void DebugMenu_EnterMappedMode(void);

void DebugMenu_NoOpBustUpFrame(void);

/* Entry 1 of the frontend step table gDebugMenu_apfnAlternatePageSteps (debug_menu.c).
 * On entry it latches bit 0x80 of gDebugMenu_bStep and clears gSaveData_PairSetupState and
 * gSaveData_LoadPairState. It then polls SaveData_UpdateDuelLoad until that returns non-zero, and a
 * result of 1 loads the main-menu package and selects mode 0x10 before the
 * step byte is cleared. */
void DebugMenu_UpdateTwoPlayerDuelEntry(void);

/* Entry 0 of both frontend step tables: clears the step byte gDebugMenu_bStep and
 * does nothing else. */
void DebugMenu_ResetEntryState(void);

void DebugMenu_Update(void);

/* Entry 6 of the frontend step table gDebugMenu_apfnPrimaryPageSteps (debug_menu.c).
 * On entry it loads campaign scene package 0. Each frame it then runs
 * DebugMenu_NoOpBustUpFrame, and while D_8009B2DE is non-zero it passes
 * gDisplayEffect_aMenuRecords and
 * Rand_GetInterval(4) to DisplayEffect_UpdateMenuRecord. Once D_8009B2DE is zero it makes two display objects
 * renderable and clears the step byte. */
void DebugMenu_UpdateBustUpEntry(void);

/* The two records DebugMenu_UpdateCursorLayout reads and writes. Both are
 * narrow views: they name only the fields this function touches and carry the
 * rest as padding, so neither is a claim about the whole record. */

/* Read side: a view of TEXT_CHANNEL(1), the DuelEffectChannel at
 * gText_aChannels[1]. Only the three values at 0x3C are
 * named, and the view stays because the two do not agree on +0x40: this
 * function reads it unsigned (`row`, u16) and DuelEffectChannel declares
 * the same halfword `s16 field_40`. Same offset, different signedness, so
 * it is not converted to the member; the pointer is cast at the use. */
struct LayoutSource {
    u8 pad0[0x3C];
    s16 base;
    s16 spacing;
    u16 row;
};

/* The write side, gDebugMenu_pCursorQuad, used to have a narrow view here called
 * LayoutTarget. It is gone: the record is a DisplayObject.
 *
 * That view was declined a merge on the grounds that no file read both its
 * region (from 0x28) and the u16 at 0x08 that
 * the bust-up entry (now in debug_menu.c) named with a local
 * LocalRecord of its own.
 * #3388 retired that LocalRecord for
 * DisplayObject, which names both -- flags at 0x08, and the four position
 * unions whose halfword pairs are exactly the eight fields LayoutTarget
 * listed: position.h at 0x28/0x2A, xy.h at 0x30/0x32, field_38.h at
 * 0x38/0x3A and field_40.h at 0x40/0x42. So the overlapping witness the old
 * comment said did not exist is now in the tree, and the reason to keep a
 * separate view expired with it.
 *
 * LayoutSource above is NOT retired, for a reason that has not expired: it
 * and DuelEffectChannel disagree about the signedness of 0x40. */

void DebugMenu_UpdateCursorLayout(void);

enum { FRONTEND_DEBUG_ROW_COUNT = 3 };

#define FRONTEND_STEP_INDEX_MASK 0x1F
#define FRONTEND_STEP_FLAG_ASYNC_PENDING 0x40
#define FRONTEND_STEP_FLAG_ENTERED 0x80

typedef s8 FrontendDebugColumns[FRONTEND_DEBUG_ROW_COUNT];
typedef u8 FrontendDebugDigitCounts[FRONTEND_DEBUG_ROW_COUNT];
typedef struct {
    u16 row[FRONTEND_DEBUG_ROW_COUNT];
} FrontendDebugValues;
typedef u16 FrontendDebugSoundValues[2];
typedef union {
    u8 *bytes;
    u16 *words;
} FrontendDebugRowPointer;

/* Edits the active frontend debug value and redraws its digit caret. */
s32 DebugMenu_UpdateValueEditor(void);

/* A six-byte extern changes assembler -G4 relocations in the editor's split
 * profile. The bounded value view is taken from this scalar label instead. */
extern u16 gDebug_nSceneOrSoundID;

extern FrontendDebugSoundValues gDebug_nLastSoundID;

extern u8 D_8009B2B2;
extern u8 gDebugMenu_bCampaignEditField;
extern u16 gDebug_nEditedBgmId;
extern u16 gDebug_nEditedCdSoundId;
extern u16 gDebug_nLastBgmId;
extern s8 gDebug_bEditorRow;
extern u16 D_8009B2DE;
extern DisplayObject *gDebugMenu_pCursorQuad;
extern u8 gDebugMenu_bLastMapId;
extern s8 gDebug_bEditorDigit;
extern u8 gDebugMenu_bStep;
extern u8 gDebugMenu_bPage;
extern u8 gDebug_szEditorCaretLine[];

typedef char FrontendDebugColumnsSize[
    sizeof(FrontendDebugColumns) == 3 ? 1 : -1];
typedef char FrontendDebugDigitCountsSize[
    sizeof(FrontendDebugDigitCounts) == 3 ? 1 : -1];
typedef char FrontendDebugValuesSize[
    sizeof(FrontendDebugValues) == 6 ? 1 : -1];
typedef char FrontendDebugValueRow1Offset[
    (u32)&((FrontendDebugValues *)0)->row[1] == 2 ? 1 : -1];
typedef char FrontendDebugValueRow2Offset[
    (u32)&((FrontendDebugValues *)0)->row[2] == 4 ? 1 : -1];
typedef char FrontendDebugSoundValuesSize[
    sizeof(FrontendDebugSoundValues) == 4 ? 1 : -1];

void DebugMenu_Init(void);
void DebugMenu_OpenValueEditor(
    u8 *data,
    s32 caret_column_0,
    s32 caret_column_1,
    s32 caret_column_2,
    s32 leading_line_count,
    s32 digit_count,
    s32 row_count
);

/* The frontend's scene states, the steps of the gDebugMenu_apfnPrimaryPageSteps table in
 * debug_menu.c. Most latch bit 0x80 of gDebugMenu_bStep on their first
 * call to do their setup, poll on later calls, and clear the byte when done.
 * DebugMenu_EnterDuel and DebugMenu_EnterDeckEditor act once without the latch. The number after
 * each name is its gDebugMenu_apfnPrimaryPageSteps index.
 *
 * DebugMenu_UpdateMapEntry is in no table and has no caller in C. */
void DebugMenu_UpdateMapEntry(void);
void DebugMenu_UpdateTitleEntry(void); /* 10 */
void DebugMenu_UpdateMovieEntry(void); /* 13 */
void DebugMenu_UpdateLoadEntry(void); /* 14: runs the duel effect state with gDuel_bEffectState = 3 */
void DebugMenu_UpdateSaveEntry(void); /* 15: the same with gDuel_bEffectState = 4 */
void DebugMenu_UpdateTradeEntry(void); /* 16: polls SaveData_UpdateTradeLoad, then selects mode 14 */
void DebugMenu_EnterDuel(void); /* 3: arms a duel through Main_StartDuel */
void DebugMenu_EnterDeckEditor(void); /* 8: enters narrow-confirm Build Deck */

#endif
