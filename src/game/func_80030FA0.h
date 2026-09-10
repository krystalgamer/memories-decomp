#ifndef MEMORIES_DECOMP_FUNC_80030FA0_H
#define MEMORIES_DECOMP_FUNC_80030FA0_H

#include "../types.h"

/* One step handler of the two tables at 0x80090D7C and 0x80090D84.
 *
 * It reads D_8009B2F1 as an index into D_80090D68 and publishes that byte as
 * the next mode in D_8009B26C, clearing gFreeDuel_bReturnFlags and D_8009B2B2
 * on the way. frontend_debug_tables.h documents both the index and the table
 * it selects from. */
void func_80030FA0(void);

#endif
