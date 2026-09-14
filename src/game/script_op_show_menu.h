#ifndef MEMORIES_DECOMP_SCRIPT_OP_SHOW_MENU_H
#define MEMORIES_DECOMP_SCRIPT_OP_SHOW_MENU_H

#include "../types.h"

/* D_80090C50 handler: opens one of the two D_800EB010 menu records -- bit 7 of
 * the second stream byte picks which -- tears down whatever that record was
 * showing, and stores the first byte as its content id with the low nibble of
 * the second as its mode. The chosen record is published through D_8009B274 for
 * the handlers that follow. */
void Script_OpShowMenu(void);

#endif
