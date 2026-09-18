#ifndef YUGIOH_GAME_FRONTEND_STEP_TABLES_H
#define YUGIOH_GAME_FRONTEND_STEP_TABLES_H

#include "../types.h"

/* The two step tables at 0x80090D7C and 0x80090D84, selected by gDebugMenu_bPage
 * and indexed by `D_8009B2EB & FRONTEND_STEP_INDEX_MASK`.
 *
 * Both are deliberately left unsized. The mask permits thirty-two entries and
 * neither table has that many, so a bound here would assert something the
 * call site does not support. */
extern void (*gDebugMenu_apfnAlternatePageSteps[])(void);
extern void (*gDebugMenu_apfnPrimaryPageSteps[])(void);

#endif
