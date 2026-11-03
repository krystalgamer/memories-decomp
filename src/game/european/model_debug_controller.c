#include "../../types.h"

/* The European executable keeps a different object (libpress data) at
 * 0x8009B004 under the splat name D_8009B004, so this unit's debug-state word,
 * which no semantic name covers yet, keeps the splat name of its own
 * address. */
#define D_8009B004 D_8009BF8C

#include "../model_debug_controller.c"
