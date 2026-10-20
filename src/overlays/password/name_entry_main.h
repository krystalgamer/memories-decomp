#ifndef MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_MAIN_H
#define MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_MAIN_H

#include "../../types.h"

/* Synchronous new-game setup entry, after the name-entry package is loaded.
 * Clears the workspace, runs keyboard/deck setup and establishes identity.
 * Distinct from the per-frame Main_RunNameEntry keyboard-only lifecycle. */
void NameEntry_Main(void);

#endif
