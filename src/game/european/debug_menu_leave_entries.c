#include "../../types.h"

/* SLES-03947 build of src/game/debug_menu_leave_entries.c, with its European
 * handler. */

#define VERSION_EUROPE
/* European code stores D_8009B269 through $at, as a .data scalar. */
#define D_8009B269_AS_SCALAR_DATA

#include "../debug_menu_leave_entries.c"
