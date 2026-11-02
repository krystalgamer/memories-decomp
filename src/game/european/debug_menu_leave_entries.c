#include "../../types.h"

/* SLES-03947 build of src/game/debug_menu_leave_entries.c, with its European
 * handler. */

#define VERSION_EUROPE
/* European code stores gMain_bReturnMode through $at, as a .data scalar. */
#define MAIN_MODE_STATE_NEXT_IN_DATA

#include "../debug_menu_leave_entries.c"
