#ifndef MEMORIES_DECOMP_PSYQ_EXIT_H
#define MEMORIES_DECOMP_PSYQ_EXIT_H

#include "../types.h"

/* Process termination without the legacy stdlib header's include bundle. */
#ifdef PSYQ_EXIT_UNPROTOTYPED
/* The legacy stdlib.h spelling: no prototype and no noreturn, so a call
 * passes no argument and keeps its return path (the APv2 module's
 * func_80168D5C calls exit() this way). */
extern void exit();
#else
extern void exit(s32 status) __attribute__((noreturn));
#endif

#endif
