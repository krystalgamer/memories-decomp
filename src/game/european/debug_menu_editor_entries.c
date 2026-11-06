#include "../../types.h"
#define D_8009AF4C_IS_AGGREGATE
#define FRONTEND_DEBUG_SOUND_ROW_VIEW
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#define DEBUG_MENU_EDITOR_ENTRIES_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C230-0x8009C260 (#2602); its
 * layout differs from the North American block the shared source defines. The
 * globals are common symbols; split.yaml's debug_menu_editor_entries_commons
 * subsegment allocates them, and the bytes no global names, at their retail
 * addresses. */
u8 gDebug_abEditorDigitCounts[3];
s8 gDebug_abEditorCaretColumns[3];
u8 gDebug_bEditorLeadingLineCount;
struct DisplayObject *gDebugMenu_pCursorQuad;
u16 gDebug_nSceneOrSoundID;
u16 gDebug_nEditedBgmId;
u16 gDebug_nEditedCdSoundId;
u16 gDebug_nLastSoundID[2];
s8 gDebug_bEditorRow;
u8 gDebug_bEditorRowCount;
u16 gDebug_nLastBgmId;
s8 gDebug_bEditorDigit;
u8 gDebugMenu_bLastMapId;
u8 gDebug_bEditorFlags;
u8 gDebugMenu_bStep;
u8 *gDebug_pszEditorFormat;
u8 gDebugMenu_bPage;
s8 gDebugMenu_bCursor;
u8 D_8009B2B2;
u8 gDebugMenu_bCampaignEditField;

#define VERSION_EUROPE
#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x40
#include "../debug_menu_editor_entries.c"
