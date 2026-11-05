#include "../../types.h"

/* The European executables keep a different object (libpress data) at
 * 0x8009B004, so each region's symbols.txt binds the debug-state word
 * D_8009B004 at its own address and gives the libpress word a
 * region-suffixed name. */

#include "../model_debug_controller.c"
