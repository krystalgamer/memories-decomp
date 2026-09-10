#ifndef MEMORIES_DECOMP_FRONTEND_SCENE_STATES_H
#define MEMORIES_DECOMP_FRONTEND_SCENE_STATES_H

#include "../types.h"

/* The frontend's scene states, the steps of the D_80090D84 table in
 * frontend_step_tables.c. Most latch bit 0x80 of D_8009B2EB on their first
 * call to do their setup, poll on later calls, and clear the byte when done.
 * func_80030F40 and func_80030F80 act once without the latch. The number after
 * each name is its D_80090D84 index.
 *
 * func_80030C10 is in no table and has no caller in C. */
void func_80030C10(void);
void func_80030CB0(void); /* 10 */
void func_80030D5C(void); /* 13 */
void func_80030E30(void); /* 14: runs the duel effect state with D_8009B254 = 3 */
void func_80030E7C(void); /* 15: the same with D_8009B254 = 4 */
void func_80030EC8(void); /* 16: polls func_8003FCD8, then selects mode 14 */
void func_80030F40(void); /* 3: arms a duel through func_80024DC8 */
void func_80030F80(void); /* 8: calls func_80033C90 */

#endif
