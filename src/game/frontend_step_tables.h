#ifndef YUGIOH_GAME_FRONTEND_STEP_TABLES_H
#define YUGIOH_GAME_FRONTEND_STEP_TABLES_H

#include "../types.h"

/* The two step tables at 0x80090D7C and 0x80090D84, selected by D_8009B2F0
 * and indexed by `D_8009B2EB & 0x1F`.
 *
 * Both are deliberately left unsized. The mask permits thirty-two entries and
 * neither table has that many, so a bound here would assert something the
 * call site does not support. */
extern void (*D_80090D7C[])(void);
extern void (*D_80090D84[])(void);

#endif
