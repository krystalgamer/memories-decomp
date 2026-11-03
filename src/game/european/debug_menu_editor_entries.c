#include "../../types.h"
#define D_8009AF4C_IS_AGGREGATE
#define GINPUT_PAD1_HELD_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD2_HELD_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD2_PRESSED_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD2_REPEAT_SIZED_IN_DATA_VOLATILE
#define FRONTEND_DEBUG_SOUND_ROW_VIEW
#define CAMPAIGN_PRIMARY_OBJECT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#define DEBUG_MENU_EDITOR_ENTRIES_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C230-0x8009C260 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 gDebug_abEditorDigitCounts[3];
u8 D_8009C233_sles_03947;
s8 gDebug_abEditorCaretColumns[3];
u8 D_8009C237_sles_03947;
u8 gDebug_bEditorLeadingLineCount;
u8 D_8009C239_sles_03947;
u16 D_8009C23A_sles_03947;
struct DisplayObject *gDebugMenu_pCursorQuad;
u16 gDebug_nSceneOrSoundID;
u16 gDebug_nEditedBgmId;
u16 gDebug_nEditedCdSoundId;
u16 D_8009C246_sles_03947;
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
u8 D_8009C25B_sles_03947;
u16 D_8009C25C_sles_03947;
u8 gDebugMenu_bCampaignEditField;
u8 D_8009C25F_sles_03947;

#define VERSION_EUROPE
#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x40
#define TEXT_LANGUAGE_INDEX_IN_DATA
#include "../debug_menu_editor_entries.c"
