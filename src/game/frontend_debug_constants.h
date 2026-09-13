#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_CONSTANTS_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_CONSTANTS_H

#include "../types.h"

extern u16 D_8009AF44;
extern u16 D_8009AF46;
/* D_8009AF4C is a u16 to every consumer that reads it as a number, and
 * src/candidates/func_80030294.c copies eight bytes out of it with a Bytes8
 * assignment. The aggregate arm is a codegen input rather than a second
 * opinion about the object: at the compiler's -G8 the scalar is small data
 * and the read comes out gp-relative, where an unsized array is not small
 * data and gets cc1psx's own %hi/%lo pair, which is what that unit's target
 * has. The element type is not the lever -- `u8 []` and `u16 []` give the
 * same object under the same Bytes8 access, so the arm keeps the type the
 * scalar states. */
#ifdef D_8009AF4C_IS_AGGREGATE
extern u16 D_8009AF4C[];
#else
extern u16 D_8009AF4C;
#endif

#endif
