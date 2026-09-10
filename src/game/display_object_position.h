#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_POSITION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_POSITION_H

#include "../types.h"

/* The record pair display_effect_update_callbacks.c moves as a unit: a group
 * holding up to three child display objects, and the child view it writes into.
 *
 * Both are narrow views. Only the coordinate pair each one carries is named,
 * with the rest as padding to reach it, so neither describes the whole
 * display-object record. */

/* A positioned child. The coordinates sit at 0x30 and 0x32. */
typedef struct {
    u8 pad_00[0x30];
    s16 x;
    s16 y;
} DisplayPositionChild;

/* The group. Its own coordinates are at 0x34 and 0x36, and the three child
 * pointers occupy the first twelve bytes. func_8003A920 walks them from index
 * 2 down to 0 and skips null slots, so fewer than three children is normal
 * rather than an error. */
typedef struct {
    DisplayPositionChild *children[3];
    u8 pad_0C[0x28];
    s16 x;
    s16 y;
} DisplayPositionGroup;

/* Writes one coordinate pair into every non-null child, leaving the group's
 * own pair alone. */
void func_8003A920(DisplayPositionGroup *group, s16 x, s16 y);

/* Sets the group's own pair and then pushes it to the children. */
void func_8003A95C(DisplayPositionGroup *group, s32 x, s32 y);

/* The callbacks sharing the implementation file retain their individually
 * measured argument views where old GCC's allocation depends on them. */

#endif
