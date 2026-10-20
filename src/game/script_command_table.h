#ifndef MEMORIES_DECOMP_SCRIPT_COMMAND_TABLE_H
#define MEMORIES_DECOMP_SCRIPT_COMMAND_TABLE_H

#include "../ygo_types.h"

/* Keep unsized: the normal runner masks to five bits, while the text bridge
 * uses the raw current-command low byte. The NA initializer has 23 entries. */
extern ScriptCommandHandler gScript_apfnCommandHandlers[];

#endif
