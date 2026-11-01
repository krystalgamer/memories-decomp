#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_TABLES_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_TABLES_H

#include "../types.h"

#define DEBUG_MENU_PAGE_PRIMARY 0
#define DEBUG_MENU_PAGE_ALTERNATE 1
#define DEBUG_MENU_COLUMN_ENTRY_COUNT 10
#define DEBUG_MENU_ENTRY_COUNT 20

#define DEBUG_MENU_ENTRY_3D 0
#define DEBUG_MENU_ENTRY_CAMPAIGN 1
#define DEBUG_MENU_ENTRY_DUEL 2
#define DEBUG_MENU_ENTRY_DETAIL 3
#define DEBUG_MENU_ENTRY_SOUND 4
#define DEBUG_MENU_ENTRY_BUST_UP 5
#define DEBUG_MENU_ENTRY_3D_MAP 6
#define DEBUG_MENU_ENTRY_DECK_EDIT 7
#define DEBUG_MENU_ENTRY_FREE_DUEL 8
#define DEBUG_MENU_ENTRY_TITLE 9
#define DEBUG_MENU_ENTRY_NAME 10
#define DEBUG_MENU_ENTRY_PASSWORD 11
#define DEBUG_MENU_ENTRY_MOVIE 12
#define DEBUG_MENU_ENTRY_LOAD 13
#define DEBUG_MENU_ENTRY_SAVE 14
#define DEBUG_MENU_ENTRY_TRADE 15
#define DEBUG_MENU_ENTRY_OPTION 16
#define DEBUG_MENU_ENTRY_LOSE 17
#define DEBUG_MENU_ENTRY_HIRATA 18
#define DEBUG_MENU_ENTRY_EXIT 19

/* The US-only "0001 = %d\n" diagnostic format DebugMenu_UpdateCampaignEntry
   passes to printf; the Japanese and European builds make no such call. */
extern const char gDebugMenu_szCampaignValueFormat[];
/* The six frontend debug HUD format strings and the scene index table,
   declared here so the source that defines them and the four that read
   them cannot drift apart. DebugMenu_UpdateCampaignEntry reads gDebugMenu_szMessageEditorFormat and gDebugMenu_szCampaignEditorFormat
   when switching between its message and campaign editors. */
extern u8 gDebugMenu_szSoundEditorFormat[0x28];
extern u8 gDebugMenu_szMessageEditorFormat[0x18];
extern u8 gDebugMenu_szCampaignEditorFormat[0x18];
extern u8 gDebugMenu_szMapEditorFormat[0x1C];
extern u8 gDebugMenu_szMenuEditorFormat[0x1C];
extern u8 gDebugMenu_szMovieEditorFormat[0x24];
#ifdef VERSION_EUROPE
/* The "LANGUAGE = %X" editor format of the European-only language entry
 * (DebugMenu_UpdateLanguageEntry), beside the other gDebugMenu_sz*EditorFormat strings. */
extern u8 gDebugMenu_szLanguageEditorFormat[0x1C];
#endif
extern u8 gDebugMenu_abMainModeByEntry[DEBUG_MENU_ENTRY_COUNT];

/* DebugMenu_EnterMappedMode reads it as the index into gDebugMenu_abMainModeByEntry (`s32 i =
   gDebugMenu_bCursor;` at debug_menu_leave_entries.c:21, `v = p[i];` at :25);
   DebugMenu_UpdateCursorLayout reads it as `s32 index = gDebugMenu_bCursor;`.
   DebugMenu_Update
   is the only C writer: it adds 0xA when the sum is below 0x14
   (debug_menu_update.c:44-46), subtracts 0xA when the difference is not negative
   (:48-50), steps it by -1 with wraps to 0x13 and 9 (:54-65) and by +1 with
   wraps to 0xA and 0 (:70-81), stores 0x13 (:89-90) and stores
   `gDebugMenu_bCursor + 1` into gDebugMenu_bStep (:112). s8 because all three units
   declared it s8 when they matched and :48 tests `gDebugMenu_bCursor - 0xA >= 0`;
   retail loads it lb at debug_menu_mapped_mode.s:5 and debug_menu_update.s:59/:68/:82/
   :109/:141, and every access is %gp_rel, so this is the plain declaration.
   No prototype takes &gDebugMenu_bCursor. Initial value not read. */
extern s8 gDebugMenu_bCursor;

#endif
