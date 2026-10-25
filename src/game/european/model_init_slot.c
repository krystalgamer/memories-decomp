#include "../../types.h"

/* SLES-03947 build of src/game/model_init_slot.c. VERSION_EUROPE selects the
 * European native label model_slot_row_tables.h binds the texture-primitive
 * rebase to. The US source is included unchanged. */

#define VERSION_EUROPE
#include "../model_init_slot.c"
