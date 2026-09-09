#ifndef MEMORIES_DECOMP_SCRIPT_NOOP_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_NOOP_COMMANDS_H

#include "../types.h"

/* Two D_80090C50 entries with empty bodies. They are distinct functions at
 * distinct addresses in the target, not one entry repeated, so the table needs
 * both names -- and neither clears D_8009B27C, which means a script command
 * that lands on either never reports itself finished. */
void func_8002F430(void);
void func_8002F438(void);

#endif
