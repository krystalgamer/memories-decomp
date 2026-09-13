#ifndef MEMORIES_DECOMP_MODEL_TRANSFER_FLAGS_H
#define MEMORIES_DECOMP_MODEL_TRANSFER_FLAGS_H

#include "../types.h"

/* Two independent flag bytes, D_8009B07B and D_8009B07C, each with a getter
 * and a setter.
 *
 * The accessors are not the only route to them: later functions in
 * model_effect_state.c and two single-function sources read the bytes directly
 * rather than through the getters, which is why the bytes are declared here
 * as well.
 *
 * Six callers share one guard, spelled as
 *
 *     state = func_8005F174();
 *     if (state != 1 || func_8005F18C() != state) { ... }
 *
 * so the interesting condition is that both flags read exactly 1. The setter
 * for the first is called with values other than 0 and 1, which is why the
 * callers compare against 1 rather than testing for non-zero.
 */
/* Three more bytes of the same block, immediately before the two flags below.
 *
 *   D_8009B078  The live key count. func_8005EBF4 takes indices modulo it
 *               to walk the key ring, and its note there calls it that.
 *   D_8009B079  A flag, set to 1 in one place and cleared in two others.
 *   D_8009B07A  A signed counter: armed at -1, stepped with ++, and tested
 *               both as `< 0` and as `++ > 0`, so the sign is what the tests
 *               turn on. One reader spells the load `*(s8 *)&D_8009B07A`
 *               although the declaration is already s8; that cast is left
 *               exactly where it is.
 *
 * D_8009B074 sits in the same run. Before consolidation, the state and
 * transfer sources spelled it s32 while the evaluator source spelled it
 * u8 *, and a pointer against a word is a question about what the storage is,
 * not a spelling to pick.
 * The listings answer it: every retail read of the word is a base for a load
 * at a fixed offset (lhu 0x22 and 0x24, lbu 0x26, lh 0x6, or an addu with an
 * index and then lhu) or a beqz null test, and both writes store either the
 * address of D_800F5788 (func_8005F91C) or zero (func_8005FAE4). So it is a
 * pointer, and the two s32 spellings were casts around one. func_8005EBF4
 * walks it as bytes and func_8005F91C stores `(u8 *)D_800F5788`, so u8 *
 * is what the uses claim; func_8005E808 shares this declaration, while
 * func_8005DBA4 is still assembly. Every retail access is gp-relative, so no
 * unit needs an arm.
 */
extern u8 *D_8009B074;
extern u8 D_8009B078;
extern u8 D_8009B079;
extern s8 D_8009B07A;
extern u8 D_8009B07B;
extern u8 D_8009B07C;

s32 func_8005F174(void);
void func_8005F180(s32 value);
s32 func_8005F18C(void);
void func_8005F198(s32 value);

#endif
