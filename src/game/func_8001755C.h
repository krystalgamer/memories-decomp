#ifndef MEMORIES_DECOMP_FUNC_8001755C_H
#define MEMORIES_DECOMP_FUNC_8001755C_H

#include "../types.h"

/* Runs the two model setup steps func_800530C4 and func_800533D8, then calls
 * func_80056250 with selector 2, D_80010000[0].payload_bases[0], 0x63000,
 * and 4. The callee uses the pointer only as a non-null gate; this call
 * does not load or read the payload (notes/duel-package-unused-data.md).
 *
 * The name is kept address-based: the call sequence is legible but what the
 * selector and the trailing 4 choose is not established here, and nothing in
 * this unit names the buffer's contents.
 *
 * func_800179F4 (src/candidates/func_800179F4.c) is the only consumer and
 * reached it through a local extern that already agreed with this. */
void func_8001755C(void);

#endif
