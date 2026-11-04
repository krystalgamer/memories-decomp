#ifndef GAME_PLAYER_NAME_GLYPHS_H
#define GAME_PLAYER_NAME_GLYPHS_H

#include "../types.h"

/* 0x801B125A. SaveData_ApplyRuntimeState passes it as the destination of
 * Text_SjisToGlyphCodes, from state->player_name_sjis
 * (save_data_payload.c:191); NameEntry_Init does the same from
 * gSaveData_aPlayerNameSjis and NameEntry_UpdateDialog does the same from
 * gNameEntry_pNameBuffer. The two name-entry paths live in
 * src/overlays/password/name_entry_runtime.c and call
 * the converter by address, as Text_SjisToGlyphCodes. The one reader is
 * NameEntry_UpdateDialog's byte scan in name_entry_runtime.c, which runs to
 * TEXT_STRING_TERMINATOR. Every access
 * takes the address (the resident listing SaveData_ApplyRuntimeState.s:7-8 forms it with
 * lui/addiu), so the listings do not say how wide the object is. The resident
 * unit used to declare it `u8 [16]` and the overlay paths `u8 []`. */
extern u8 gText_aCurrentPlayerNameGlyphs[];

#endif
