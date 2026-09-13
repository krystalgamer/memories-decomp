#ifndef MEMORIES_DECOMP_SCRIPT_OP_RETURN_TO_MENU_H
#define MEMORIES_DECOMP_SCRIPT_OP_RETURN_TO_MENU_H

#include "../types.h"

/* D_80090C50 handler: hands the scene to mode 8, with D_8009B268 set to 1 and
 * D_8009B26D to 5. Consumes nothing from the stream and never clears the busy
 * word. */
void Script_OpReturnToMenu(void);

#endif
