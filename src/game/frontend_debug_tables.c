#include "../types.h"
#include "frontend_debug_tables.h"

/* Initialized data at 0x80090CB4 through 0x80090D7C, previously a generated
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
