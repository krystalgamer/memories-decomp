#ifndef GAME_DEBUG_VALUE_EDITOR_CONFIG_H
#define GAME_DEBUG_VALUE_EDITOR_CONFIG_H

#include "../types.h"
#include "frontend_debug_state.h"

#ifdef FRONTEND_DEBUG_ROW_VIEWS
extern FrontendDebugColumns gDebug_abEditorCaretColumns;
extern FrontendDebugDigitCounts gDebug_abEditorDigitCounts;
#else
extern u8 gDebug_abEditorCaretColumns;
extern u8 gDebug_abEditorDigitCounts;
#endif

extern u8 gDebug_bEditorCaretColumn1;
extern u8 gDebug_bEditorCaretColumn2;
extern u8 gDebug_bEditorLeadingLineCount;
extern u8 gDebug_bEditorDigitCount1;
extern u8 gDebug_bEditorDigitCount2;
extern u8 gDebug_bEditorRowCount;
extern u8 gDebug_bEditorFlags;
extern u8 *G32 gDebug_pszEditorFormat;

#endif
