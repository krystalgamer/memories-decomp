#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_UPDATES_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_UPDATES_H

#include "../types.h"

/* Three of the seven per-list renderers in D_80090FB0. Each walks one display
 * object list from its head in D_800EFE38, runs every object's update callback,
 * and differs only in which list it takes and what it does afterwards:
 *
 *   func_80040CAC  list 0, the update-only pass -- no rendering at all
 *   func_80040BF8  list 3
 *   func_80040D14  list 6
 *
 * The two that render reach the object's second callback slot at +0x4C for
 * renderable objects. All three are reached only as entries of that table, so
 * the table file is their only consumer and declared all three itself before
 * this header. */
void func_80040BF8(void);
void func_80040CAC(void);
void func_80040D14(void);

#endif
