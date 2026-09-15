#ifndef MEMORIES_DECOMP_PSYQ_EXIT_H
#define MEMORIES_DECOMP_PSYQ_EXIT_H

#include "../types.h"

/* Process termination without the legacy stdlib header's include bundle. */
extern void exit(s32 status) __attribute__((noreturn));

#endif
