#ifndef MEMORIES_DECOMP_NAME_ENTRY_TABLES_H
#define MEMORIES_DECOMP_NAME_ENTRY_TABLES_H

#include "../../types.h"

/* Nine rows of fifteen signed cell descriptors. Negative cells redirect
 * selection; nonnegative cells carry row/command and width information. */
extern s8 D_8016AB38[][15];

/* Up/down row destinations for the keyboard's command columns. */
extern u8 D_8016ABC0[][2];

#endif
