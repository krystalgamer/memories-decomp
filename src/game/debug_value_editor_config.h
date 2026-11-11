#ifndef GAME_DEBUG_VALUE_EDITOR_CONFIG_H
#define GAME_DEBUG_VALUE_EDITOR_CONFIG_H

#include "../types.h"
#include "debug_menu.h"

/* Three editor rows: caret anchor column and digit width per row. */
extern FrontendDebugColumns gDebug_abEditorCaretColumns;
extern FrontendDebugDigitCounts gDebug_abEditorDigitCounts;
extern u8 gDebug_bEditorLeadingLineCount;
extern u8 gDebug_bEditorRowCount;
extern u8 gDebug_bEditorFlags;
extern u8 *gDebug_pszEditorFormat;

#endif
