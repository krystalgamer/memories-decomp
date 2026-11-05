#ifndef MEMORIES_DECOMP_PSYQ_STARTUP_DATA_H
#define MEMORIES_DECOMP_PSYQ_STARTUP_DATA_H

#include "../types.h"

extern u32 D_800906E0;
extern u32 D_800906E4;
extern u32 D_800906E8[7];

/* The startup record's initialized-data start. It is a linker-ish CRT
 * symbol rather than a C object: the North American link_symbols.ld, the
 * only image that builds this unit, binds it to the first .data word (the
 * __main guard, D_800906E0). */
extern u32 initialized_data_start;

/* The Psy-Q CRT entry point the record's second word names. */
void entrypoint(void);

#endif
