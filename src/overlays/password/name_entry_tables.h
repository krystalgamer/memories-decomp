#ifndef MEMORIES_DECOMP_NAME_ENTRY_TABLES_H
#define MEMORIES_DECOMP_NAME_ENTRY_TABLES_H

#include "../../types.h"

#define NAME_ENTRY_KEYBOARD_ROW_COUNT 9
#define NAME_ENTRY_KEYBOARD_COLUMN_COUNT 15

/* Nine rows of fifteen signed cell descriptors. Negative cells redirect
 * selection; nonnegative cells carry row/command and width information.
 * Defined in name_entry_runtime.c. */
extern s8 gNameEntry_aKeyboardCells[NAME_ENTRY_KEYBOARD_ROW_COUNT]
                                   [NAME_ENTRY_KEYBOARD_COLUMN_COUNT];

#ifndef VERSION_EUROPE
/* Up/down row destinations for the keyboard's command columns, indexed by
 * row and then by 1 for down. Defined in name_entry_runtime.c. */
extern u8 gNameEntry_aRowLinks[NAME_ENTRY_KEYBOARD_ROW_COUNT][2];
#endif

#endif
