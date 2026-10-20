#ifndef MEMORIES_DECOMP_SCRIPT_NOOP_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_NOOP_COMMANDS_H

#include "../types.h"

/* Two gScript_apfnCommandHandlers entries with empty bodies. They are distinct functions at
 * distinct addresses in the target, not one entry repeated, so the table needs
 * both names -- and neither clears gScript_wCommandWord, which means a script command
 * that lands on either never reports itself finished. */
void Script_OpUnusedHalt14(void);
void Script_OpUnusedHalt15(void);

#endif
